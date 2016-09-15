#ifndef CLAIRVOYANCE_REBOOT_H
#define CLAIRVOYANCE_REBOOT_H

#define MODE_NORMAL             1
#define MODE_SAFE               2
#define MODE_SAFE_NETWORK       4

namespace clairvoyance
{
    bool reboot();
}

#endif
