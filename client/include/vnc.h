#ifndef _CLAIRVOYANCE_VNC_H
#define _CLAIRVOYANCE_VNC_H

#include <string>
#include <thread>
#include <rfb/rfb.h>
#include <rfb/keysym.h>

#ifdef __WIN32__
  #include "../include/mingw.thread.h"
#endif

#include "net.h"
#include "json.h"

namespace clairvoyance
{
    class vncserver
    {
      public:
        vncserver(int argc, char *argv[], clairvoyance::json *vnc_config);
        ~vncserver();
      private:
        rfbScreenInfoPtr rfbScreen;
        std::thread thread_vnc;
        void thread_func();
        bool ready;
        int height, width, bpp;
        clairvoyance::net::client *client;
    };
}

#endif
