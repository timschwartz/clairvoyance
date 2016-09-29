#include <iostream>
#include <unistd.h>
#include "../include/vnc.h"
#include "../include/radon.h"

static const int bpp=4;
static int maxx=800, maxy=600;
typedef struct ClientData {
    rfbBool oldButton;
    int oldx,oldy;
} ClientData;

static void drawline(unsigned char* buffer,int rowstride,int bpp,int x1,int y1,int x2,int y2)
{
  int i,j;
  i=x1-x2; j=y1-y2;
  if(i==0 && j==0) {
     for(i=0;i<bpp;i++)
       buffer[y1*rowstride+x1*bpp+i]=0xff;
     return;
  }
  if(i<0) i=-i;
  if(j<0) j=-j;
  if(i<j) {
    if(y1>y2) { i=y2; y2=y1; y1=i; i=x2; x2=x1; x1=i; }
    for(j=y1;j<=y2;j++)
      for(i=0;i<bpp;i++)
        buffer[j*rowstride+(x1+(j-y1)*(x2-x1)/(y2-y1))*bpp+i]=0xff;
  } else {
    if(x1>x2) { i=y2; y2=y1; y1=i; i=x2; x2=x1; x1=i; }
    for(i=x1;i<=x2;i++)
      for(j=0;j<bpp;j++)
        buffer[(y1+(i-x1)*(y2-y1)/(x2-x1))*rowstride+i*bpp+j]=0xff;
  }
}
    
/* Here the pointer events are handled */

static void doptr(int buttonMask,int x,int y,rfbClientPtr cl)
{
   ClientData *cd = (ClientData *)cl->clientData;

   if(x>=0 && y>=0 && x<maxx && y<maxy) {
      if(buttonMask) {
         int i,j,x1,x2,y1,y2;

         if(cd->oldButton==buttonMask) { /* draw a line */
            drawline((unsigned char*)cl->screen->frameBuffer,cl->screen->paddedWidthInBytes,bpp,
                     x,y,cd->oldx,cd->oldy);
            x1=x; y1=y;
            if(x1>cd->oldx) x1++; else cd->oldx++;
            if(y1>cd->oldy) y1++; else cd->oldy++;
            rfbMarkRectAsModified(cl->screen,x,y,cd->oldx,cd->oldy);
         } else { /* draw a point (diameter depends on button) */
            int w=cl->screen->paddedWidthInBytes;
            x1=x-buttonMask; if(x1<0) x1=0;
            x2=x+buttonMask; if(x2>maxx) x2=maxx;
            y1=y-buttonMask; if(y1<0) y1=0;
            y2=y+buttonMask; if(y2>maxy) y2=maxy;

            for(i=x1*bpp;i<x2*bpp;i++)
              for(j=y1;j<y2;j++)
                cl->screen->frameBuffer[j*w+i]=(char)0xff;
            rfbMarkRectAsModified(cl->screen,x1,y1,x2,y2);
         }

         /* we could get a selection like that:
          rfbGotXCutText(cl->screen,"Hallo",5);
          */
      } else
        cd->oldButton=0;

      cd->oldx=x; cd->oldy=y; cd->oldButton=buttonMask;
   }
   rfbDefaultPtrAddEvent(buttonMask,x,y,cl);
}

static void MakeRichCursor(rfbScreenInfoPtr rfbScreen)
{
  int i,j,w=32,h=32;
  rfbCursorPtr c = rfbScreen->cursor;
  char bitmap[]=
    "                                "
    "              xxxxxx            "
    "       xxxxxxxxxxxxxxxxx        "
    "      xxxxxxxxxxxxxxxxxxxxxx    "
    "    xxxxx  xxxxxxxx  xxxxxxxx   "
    "   xxxxxxxxxxxxxxxxxxxxxxxxxxx  "
    "  xxxxxxxxxxxxxxxxxxxxxxxxxxxxx "
    "  xxxxx   xxxxxxxxxxx   xxxxxxx "
    "  xxxx     xxxxxxxxx     xxxxxx "
    "  xxxxx   xxxxxxxxxxx   xxxxxxx "
    " xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx "
    " xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx "
    " xxxxxxxxxxxx  xxxxxxxxxxxxxxx  "
    " xxxxxxxxxxxxxxxxxxxxxxxxxxxx   "
    " xxxxxxxxxxxxxxxxxxxxxxxxxxxx   "
    " xxxxxxxxxxx   xxxxxxxxxxxxxx   "
    " xxxxxxxxxx     xxxxxxxxxxxx    "
    "  xxxxxxxxx      xxxxxxxxx      "
    "   xxxxxxxxxx   xxxxxxxxx       "
    "      xxxxxxxxxxxxxxxxxxx       "
    "       xxxxxxxxxxxxxxxxxxx      "
    "         xxxxxxxxxxxxxxxxxxx    "
    "             xxxxxxxxxxxxxxxxx  "
    "                xxxxxxxxxxxxxxx "
    "   xxxx           xxxxxxxxxxxxx "
    "  xx   x            xxxxxxxxxxx "
    "  xxx               xxxxxxxxxxx "
    "  xxxx             xxxxxxxxxxx  "
    "   xxxxxx       xxxxxxxxxxxx    "
    "    xxxxxxxxxxxxxxxxxxxxxx      "
    "      xxxxxxxxxxxxxxxx          "
    "                                ";
  c=rfbScreen->cursor = rfbMakeXCursor(w,h,bitmap,bitmap);
  c->xhot = 16; c->yhot = 24;

  c->richSource = (unsigned char*)malloc(w*h*bpp);
  c->cleanupRichSource = TRUE;
  for(j=0;j<h;j++) {
    for(i=0;i<w;i++) {
      c->richSource[j*w*bpp+i*bpp+0]=i*0xff/w;
      c->richSource[j*w*bpp+i*bpp+1]=(i+j)*0xff/(w+h);
      c->richSource[j*w*bpp+i*bpp+2]=j*0xff/h;
      c->richSource[j*w*bpp+i*bpp+3]=0;
    }
  }
}

static void initBuffer(unsigned char* buffer)
{
  int i,j;
  for(j=0;j<maxy;++j) {
    for(i=0;i<maxx;++i) {
      buffer[(j*maxx+i)*bpp+0]=(i+j)*128/(maxx+maxy); /* red */
      buffer[(j*maxx+i)*bpp+1]=i*128/maxx; /* green */
      buffer[(j*maxx+i)*bpp+2]=j*256/maxy; /* blue */
    }
    buffer[j*maxx*bpp+0]=0xff;
    buffer[j*maxx*bpp+1]=0xff;
    buffer[j*maxx*bpp+2]=0xff;
    buffer[j*maxx*bpp+3]=0xff;
  }
}

static void clientgone(rfbClientPtr cl)
{
    free(cl->clientData);
}
static enum rfbNewClientAction newclient(rfbClientPtr cl)
{
    cl->clientData = (void*)calloc(sizeof(ClientData),1);
    cl->clientGoneHook = clientgone;
    return RFB_CLIENT_ACCEPT;
}


static void event_handler(int buttonMask, int x, int y, rfbClientPtr cl)
{
}

static void key_handler(rfbBool down, rfbKeySym key, rfbClientPtr cl)
{
    
}

static void newframebuffer(rfbScreenInfoPtr screen, int width, int height)
{
  unsigned char *oldfb, *newfb;

  maxx = width;
  maxy = height;
  oldfb = (unsigned char*)screen->frameBuffer;
  newfb = (unsigned char*)malloc(maxx * maxy * bpp);
  initBuffer(newfb);
  rfbNewFramebuffer(screen, (char*)newfb, maxx, maxy, 8, 3, bpp);
  free(oldfb);

  /*** FIXME: Re-install cursor. ***/
}

static void dokey(rfbBool down,rfbKeySym key,rfbClientPtr cl)
{
  if(down) {
    if(key==XK_Escape)
      rfbCloseClient(cl);
    else if(key==XK_F12)
      /* close down server, disconnecting clients */
      rfbShutdownServer(cl->screen,TRUE);
    else if(key==XK_F11)
      /* close down server, but wait for all clients to disconnect */
      rfbShutdownServer(cl->screen,FALSE);
    else if(key==XK_Page_Up) {
      initBuffer((unsigned char*)cl->screen->frameBuffer);
      rfbMarkRectAsModified(cl->screen,0,0,maxx,maxy);
    } else if (key == XK_Up) {
      if (maxx < 1024) {
        if (maxx < 800) {
          newframebuffer(cl->screen, 800, 600);
        } else {
          newframebuffer(cl->screen, 1024, 768);
        }
      }
    } else if(key==XK_Down) {
      if (maxx > 640) {
        if (maxx > 800) {
          newframebuffer(cl->screen, 800, 600);
        } else {
          newframebuffer(cl->screen, 640, 480);
        }
      }
    } else if(key>=' ' && key<0x100) {
      ClientData *cd = (ClientData *)cl->clientData;
      int x1=cd->oldx,y1=cd->oldy,x2,y2;
      cd->oldx+=rfbDrawCharWithClip(cl->screen,&radonFont,cd->oldx,cd->oldy,(char)key,0,0,cl->screen->width,cl->screen->height,0x00ffffff,0x00ffffff);
      rfbFontBBox(&radonFont,(char)key,&x1,&y1,&x2,&y2);
      rfbMarkRectAsModified(cl->screen,x1,y1,x2-1,y2-1);
    }
  }
}

namespace clairvoyance
{
    vncserver::vncserver(int argc, char *argv[], std::string hostname)
    {
        ready = false;
        rfbScreen = rfbGetScreen(&argc, argv, maxx, maxy, 8, 3, bpp);
        if(!rfbScreen) 
        {
            std::string message = "rfbGetScreen(.., .., " + maxx;
            message += ", " + maxy;
            message += ", 8, 3, " + bpp;
            message += ") failed.";
            throw message;
        }

        rfbScreen->desktopName = hostname.c_str();
        rfbScreen->frameBuffer = (char *)malloc(maxx * maxy * bpp);
        rfbScreen->alwaysShared = TRUE;
        rfbScreen->ptrAddEvent = doptr;
        rfbScreen->kbdAddEvent = dokey;
        rfbScreen->newClientHook = newclient;

        initBuffer((unsigned char*)rfbScreen->frameBuffer);
        rfbDrawString(rfbScreen,&radonFont,20,100,"Hello, World!",0xffffff);
        MakeRichCursor(rfbScreen);
        rfbInitServer(rfbScreen);

        std::cout << "Starting VNC thread" << std::endl;
        thread_vnc = std::thread(&vncserver::thread_func, this);
        thread_vnc.detach();
    }

    vncserver::~vncserver()
    {
        free(rfbScreen->frameBuffer);
        rfbScreenCleanup(rfbScreen);
    }

    void vncserver::thread_func()
    {
        ready = true;

        HDC hdc = GetDC(NULL); // get the desktop device context
        HDC hDest = CreateCompatibleDC(hdc); 
        RECT rcClient;
        BITMAP bmpScreen;
        BITMAPINFOHEADER   bi;
     
        bi.biSize = sizeof(BITMAPINFOHEADER);    
        bi.biWidth = 800;    
        bi.biHeight = 600;  
        bi.biPlanes = 1;    
        bi.biBitCount = 32;    
        bi.biCompression = BI_RGB;    
        bi.biSizeImage = 0;  
        bi.biXPelsPerMeter = 0;    
        bi.biYPelsPerMeter = 0;    
        bi.biClrUsed = 0;    
        bi.biClrImportant = 0;

        int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);

        HBITMAP hbDesktop = CreateCompatibleBitmap(hdc, width, height);
        SelectObject(hDest, hbDesktop);

        rfbRunEventLoop(rfbScreen, -1, TRUE);
        std::cout << "Running background loop..." << std::endl;
        while(ready) 
        {
            BitBlt(hDest, 0,0, width, height, hdc, 0, 0, SRCCOPY);
            GetObject(hbDesktop, sizeof(BITMAP), &bmpScreen);
         //   memcpy(rfbScreen->frameBuffer, (void *)bmpScreen, (800*600));
            GetDIBits(hdc, hbDesktop, 0, (UINT)bmpScreen.bmHeight, rfbScreen->frameBuffer, (BITMAPINFO *)&bi, DIB_RGB_COLORS);
            rfbMarkRectAsModified(rfbScreen, 0, 0, width ,height);
            rfbProcessEvents(rfbScreen,100000);
            usleep(250000); 
        }

        ReleaseDC(NULL, hdc);
        DeleteDC(hDest);
    }
}
