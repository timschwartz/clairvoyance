AM_CXXFLAGS = -std=c++11 $(SSL_CFLAGS) $(VNC_CFLAGS) $(JPEG_CFLAGS) $(ZLIB_CFLAGS)
AM_LDFLAGS = -pthread -static-libstdc++ -static-libgcc -static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
LDADD = $(VNC_LIBS) $(SSL_LIBS) $(JPEG_LIBS) $(ZLIB_LIBS)
bin_PROGRAMS = clairvoyance

if !WIN32
LDADD += -ldl -lresolv
clairvoyance_SOURCES = main.cpp json.cpp net.cpp linux/reboot.cpp linux/vnc.cpp
else
LDADD += -lwbemuuid -lole32 -loleaut32
clairvoyance_SOURCES = main.cpp json.cpp net.cpp win32/reboot.cpp win32/service.cpp win32/vnc.cpp
endif
