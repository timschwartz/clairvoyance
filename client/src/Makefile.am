AM_CXXFLAGS = -std=c++11 $(SSL_CFLAGS)
AM_LDFLAGS = -pthread -static-libstdc++ -static-libgcc -static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

bin_PROGRAMS = clairvoyance

if !WIN32
LDADD = $(SSL_LIBS) -ldl
clairvoyance_SOURCES = main.cpp json.cpp net.cpp linux/reboot.cpp
else
LDADD = $(SSL_LIBS) -lwbemuuid -lole32 -loleaut32
clairvoyance_SOURCES = main.cpp json.cpp net.cpp win32/reboot.cpp
endif
