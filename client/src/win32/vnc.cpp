#include <iostream>
#include "../include/vnc.h"
#include "../include/radon.h"

static const int bpp=4;
static int maxx=800, maxy=600;

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

typedef struct ClientData {
    rfbBool oldButton;
    int oldx,oldy;
} ClientData;

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

namespace clairvoyance
{
    vncserver::vncserver(int argc, char *argv[], std::string hostname)
    {
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
        rfbScreen->ptrAddEvent = event_handler;
        rfbScreen->kbdAddEvent = key_handler;
        rfbScreen->newClientHook = newclient;

        initBuffer((unsigned char*)rfbScreen->frameBuffer);
    }

    vncserver::~vncserver()
    {
        free(rfbScreen->frameBuffer);
        rfbScreenCleanup(rfbScreen);
    }
}
