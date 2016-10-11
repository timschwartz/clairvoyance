#include <iostream>
#include <unistd.h>
#include "../include/net.h"
#include "../include/main.h"

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

    std::string ip_tostring(const struct addrinfo *ip)
    {
        void *sa;
        struct in6_addr IPv6_addr;
        struct in_addr IPv4_addr;
        std::string output;
        std::stringstream ss;
        char *s = new char[INET6_ADDRSTRLEN];

        switch(ip->ai_family)
        {
            case AF_INET6:
                sa = (struct sockaddr_in6 *)ip->ai_addr;
                IPv6_addr = ((struct sockaddr_in6 *)sa)->sin6_addr;
                inet_ntop(AF_INET6, &IPv6_addr, s, INET6_ADDRSTRLEN);
                output = s;
                break;
            case AF_INET:
                sa = (struct sockaddr_in *)ip->ai_addr;
                IPv4_addr = ((struct sockaddr_in *)sa)->sin_addr;
                inet_ntop(AF_INET, &IPv4_addr, s, INET6_ADDRSTRLEN);
                output = s;
                break;
        }

        return output;
    }

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

    net::net(std::string hostname, int port, bool start_thread, bool encrypted)
    {
    #ifdef __WIN32__
        if(WSAStartup(MAKEWORD(2,2), &wsa_data)) throw std::string("Couldn't initialize winsock.");
    #endif
        bytes_read = bytes_written = 0;

        ip = resolve(hostname, port);

        switch(ip->ai_family)
        {
            case AF_INET6:
                std::cout << "IPv6 ";
                if((this->sock = socket(AF_INET6, SOCK_STREAM, 0)) < 0) throw std::string("Couldn't create IPv6 socket.");
                break;
            case AF_INET:
                std::cout << "IPv4 ";
                if((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) throw std::string("Couldn't create IPv4 socket.");
                break;
        }


        if((connect(sock, ip->ai_addr, ip->ai_addrlen)) < 0)
        {
            std::string err = "Error: " + std::to_string(errno) + ". Couldn't connect to " 
                            + hostname + ":" + std::to_string(port);
            throw err;
        }

        std::cout << "Connected to " << hostname << " [" << ip_tostring(ip) << "]:" << port << std::endl;

        if(encrypted)
        {
            if(!enable_ssl()) throw "Couldn't create SSL session.";

            cert = SSL_get_peer_certificate(ssl);
            if(cert == NULL) throw std::string("Error: Could not get a certificate from " + hostname);
    
            certname = X509_NAME_new();
            certname = X509_get_subject_name(cert);

            X509_NAME_print_ex(outbio, certname, 0, 0);
            std::cout << std::endl;

            if(start_thread)
            {
                std::cout << "Starting thread" << std::endl;
                thread_net = std::thread(&net::thread_func, this);
                thread_net.detach();    
            }
        }
    }

    net::~net()
    {
        ready = false;
        std::cout << "Bytes read: " << bytes_read << std::endl;
        std::cout << "Bytes written: " << bytes_written << std::endl;
        if(sock) close(sock);
    #ifdef __WIN32__
        WSACleanup();
    #endif
    }

    const struct addrinfo *net::resolve(std::string hostname, int port)
    {
        uint32_t i;

        struct addrinfo hints, *address;

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = 0;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;

        i = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &address);

        if(i)
        {
            throw std::string("getaddrinfo() failed: " + i);
        }

        return address;
    }

    bool net::enable_ssl()
    {
        outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

        method = SSLv23_client_method();

        if((ctx = SSL_CTX_new(method)) == NULL)
        {
            BIO_printf(outbio, "Unable to create a new SSL context structure.\n");
            return false;
        }

        SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        int ret;
        if(ret = SSL_connect(ssl) < 0)
        {
            std::cout << SSL_get_error(ssl, ret) << std::endl;
            return false;
        }

        std::cout << "Enabled SSL/TLS session." << std::endl;
        return true;
    }

    void net::thread_func()
    {
        char buffer[2000];
        uint32_t b_read;
        std::vector<std::string> commands;
        std::string partial_message;

        ready = true;

        while(ready)
        {            
            memset(buffer, 0, 2000);

            b_read = ::SSL_read(ssl, buffer, 2000);
            if(b_read < 0) std::cout <<  "recv failed" << std::endl;
            bytes_read += b_read;
            if(b_read) 
            {
                partial_message += std::string(buffer);
                if(partial_message.size() && (partial_message.back() == '\n'))
                {
                    commands = explode(partial_message.c_str(), '\n');
                    for(auto n:commands) input_buffer.push_back(n);

                    partial_message = "";
                }
            }
            usleep(250);
        }
    }

    std::string net::read()
    {
        std::string message = "";

        if(!this->input_buffer.size()) return message;
        message = this->input_buffer.front();
        this->input_buffer.pop_front();
        return message;
    }

    void net::write(std::string message)
    {
        message += '\n';
        ::SSL_write(ssl, message.c_str(), message.size());
        bytes_written += message.size();
    }

    bool net::is_ready()
    {
        return ready;
    }

    int net::get_sock()
    {
        return sock;
    }

    SSL_CTX *net::get_ssl_ctx()
    {
        return ctx;
    }
}
