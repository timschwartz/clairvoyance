#ifndef _CLAIRVOYANCE_VNC_H
#define _CLAIRVOYANCE_VNC_H

#include <string>
#include <rfb/rfb.h>
#include <rfb/keysym.h>

namespace clairvoyance
{
    class vncserver
    {
      public:
        vncserver(int argc, char *argv[], std::string hostname);
        ~vncserver();
      private:
        rfbScreenInfoPtr rfbScreen;
    };
}

#endif
