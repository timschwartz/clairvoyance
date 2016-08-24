AM_CXXFLAGS = -std=c++11 $(SSL_CFLAGS)
AM_LDFLAGS = -pthread -static-libstdc++ -static-libgcc -static
LDADD = $(SSL_LIBS)

bin_PROGRAMS = clairvoyance
clairvoyance_SOURCES = main.cpp config.cpp net.cpp
