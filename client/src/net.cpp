#include <iostream>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
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
#include "../include/net.h"

namespace clairvoyance
{
    bool SSL_init = false;

#ifdef __WIN32__
    const char* inet_ntop(int af, const void* src, char* dst, int cnt)
    {
        struct sockaddr_in srcaddr;

        memset(&srcaddr, 0, sizeof(struct sockaddr_in));
        memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));

        srcaddr.sin_family = af;
        if (WSAAddressToString((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) 
        {
            DWORD rv = WSAGetLastError();
            printf("WSAAddressToString() : %d\n",rv);
            return NULL;
        }
        return dst;
    }

    WSADATA wsa_data;
#endif

    bool init_ssl_lib()
    {
        if(SSL_init == false)
        {
            std::cout << "Initializing SSL." << std::endl;

            OpenSSL_add_all_algorithms();
            ERR_load_BIO_strings();
            ERR_load_crypto_strings();
            SSL_load_error_strings();

            if(SSL_library_init() < 0)
            {
                std::cout << "Could not initialize the OpenSSL library !" << std::endl;
            }
            else SSL_init = true;
        }

        return SSL_init;
    }
}
