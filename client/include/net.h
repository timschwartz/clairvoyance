#ifndef _CLAIRVOYANCE_NET_H
#define _CLAIRVOYANCE_NET_H

#include <string>
#include <sstream>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <thread>
#include <vector>
#include <list>

#ifdef __WIN32__
  #define NOCRYPT
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include "../include/mingw.thread.h"
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
#endif

namespace clairvoyance
{
    bool init_ssl_lib();

    class net
    {
      public:
        net(std::string hostname, int port, bool start_thread, bool encrypted);
        ~net();
        std::string read();
        void write(std::string message);
        bool is_ready();
        uint64_t bytes_read;
        uint64_t bytes_written;
        int get_sock();
        SSL_CTX *get_ssl_ctx();

      private:
        const struct addrinfo *ip;
        const struct addrinfo *resolve(std::string hostname, int port);
        int sock;
        bool enable_ssl(int socket);
        BIO *certbio, *outbio;
        X509 *cert;
        X509_NAME *certname;
        const SSL_METHOD *method;
        SSL_CTX *ctx;
        SSL *ssl;
        std::thread thread_net;
        void thread_func();
        std::list<std::string> input_buffer;
        bool ready;
        bool certificate(std::string certfile, std::string keyfile);
    };
}

#endif
