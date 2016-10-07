#include <iostream>
#include <unistd.h>
#include "../include/vnc.h"

static void event_handler(int buttonMask, int x, int y, rfbClientPtr cl)
{
}

static void key_handler(rfbBool down, rfbKeySym key, rfbClientPtr cl)
{
    if(!down) return;

    if((char)key == '\n') std::cout << std::endl;
    else std::cout << (char)key << std::flush;
}

static enum rfbNewClientAction new_client(rfbClientPtr cl)
{
  return RFB_CLIENT_ACCEPT;
}

namespace clairvoyance
{
    vncserver::vncserver(int argc, char *argv[], std::string hostname)
    {
        ready = false;

        height = 600;
        width = 800;
        bpp = 4;

        rfbScreen = rfbGetScreen(&argc, argv, width, height, 8, 3, bpp);
        if(!rfbScreen) 
        {
            std::string message = "rfbGetScreen(.., .., " + width;
            message += ", " + height;
            message += ", 8, 3, " + bpp;
            message += ") failed.";
            throw message;
        }

        rfbScreen->desktopName = hostname.c_str();
        rfbScreen->frameBuffer = (char *)malloc(width * height * bpp);
        rfbScreen->alwaysShared = TRUE;
        rfbScreen->ptrAddEvent = event_handler;
        rfbScreen->kbdAddEvent = key_handler;
        rfbScreen->newClientHook = new_client;

        memset(rfbScreen->frameBuffer, 0, width * height * bpp);
        rfbInitServer(rfbScreen);

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
        uint8_t color = 0;
        ready = true;

        std::cout << "Starting VNC thread..." << std::endl;
        while(ready) 
        {
            rfbProcessEvents(rfbScreen, 0);
            usleep(100000); 
        }

    }
}
