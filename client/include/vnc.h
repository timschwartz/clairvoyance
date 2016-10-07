#ifndef _CLAIRVOYANCE_VNC_H
#define _CLAIRVOYANCE_VNC_H

#include <string>
#include <thread>
#include <rfb/rfb.h>
#include <rfb/keysym.h>

#ifdef __WIN32__
  #include "../include/mingw.thread.h"
#endif
namespace clairvoyance
{
    class vncserver
    {
      public:
        vncserver(int argc, char *argv[], std::string hostname);
        ~vncserver();
      private:
        rfbScreenInfoPtr rfbScreen;
        std::thread thread_vnc;
        void thread_func();
        bool ready;
        int height, width, bpp;
    };
}

#endif
