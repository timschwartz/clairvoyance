#include <stdio.h>
#include <iostream>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <unistd.h>

#include "../include/json.h"
#include "../include/net.h"
#include "../include/reboot.h"
#include "../include/vnc.h"

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 15
#endif

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 22
#endif

namespace clairvoyance
{
    /** Splits a string by a character.
     *  @param [in] s The string to split.
     *  @param [in] c The character to split the string on.
     *  @return Results of the split as a vector of strings.
     */
    const std::vector<std::string> explode(const std::string& s, const char& c)
    {
            std::string buff{""};
            std::vector<std::string> v;

            for(auto n:s)
            {
                    if(n != c) buff+=n; else
                    if(n == c && buff != "") { v.push_back(buff); buff = ""; }
            }
            if(buff != "") v.push_back(buff);

            return v;
    }   
}

int main(int argc, char *argv[])
{
    clairvoyance::json *config;

    if(argc < 2)
    {
        printf("Usage:\n%s config.json\n", argv[0]);
        return -1;
    }

    std::fstream config_file;
    std::string config_data;

    config_file.open(argv[1], std::fstream::in | std::fstream::out);

    if(!config_file.good())
    {
        std::cout <<  "Couldn't read config file " <<  argv[1] << std::endl;
        return -1;
    }

    /* Enlarge config_data to hold contents and read in file. */
    config_file.seekg(0, std::ios::end);
    config_data.resize(config_file.tellg());
    config_file.seekg(0, std::ios::beg);
    config_file.read(&config_data[0], config_data.size());
    config_file.close();

    try
    {
        config = new clairvoyance::json(config_data);
    } 
    catch (std::string e)
    {
        std::cout << e << std::endl;
        return -1;
    }

    clairvoyance::init_ssl_lib();

    char hostname[HOST_NAME_MAX];
#ifdef __WIN32__
    strcpy(hostname, getenv("COMPUTERNAME"));
#else
    gethostname(hostname, HOST_NAME_MAX);
#endif
    config->set("hostname", hostname);

    char username[LOGIN_NAME_MAX];
#ifdef __WIN32__
    strcpy(username, getenv("USERNAME"));
#else
    strcpy(username, getlogin());
#endif
    config->set("username", username);

    std::cout << "Server: " <<  config->get("server") << std::endl;
    std::cout << "Port: " << config->get("port") << std::endl;
    std::cout << "Shared key: " << config->get("shared-key") << std::endl;
    std::cout << "ID: " << config->get("id") << std::endl;
    std::cout << "Hostname: " << config->get("hostname") << std::endl;
    std::cout << "Username: " << config->get("username") << std::endl;

    config->save(argv[1]);
    
    clairvoyance::net *conn;

    do
    {
        try 
        {
            conn = new clairvoyance::net(config->get("server"), std::stoi(config->get("port")));
        } catch (std::string err)
        {
            std::cout << err << std::endl;
            usleep(5000000);
            conn = NULL;
            std::cout << static_cast<void*>(conn) << std::endl;
        }
    } while(conn == NULL);

    while(!conn->is_ready());

    clairvoyance::json *packet = new clairvoyance::json("");
    packet->set("shared-key", config->get("shared-key"));
    conn->write(packet->to_string());
    delete packet;

    clairvoyance::json *ping = new clairvoyance::json("");
    ping->set("method", "ping");
    
    std::string message;

    uint64_t last_ping = 0;
    uint64_t last_pong = time(0);
    clairvoyance::json *recv;

    clairvoyance::vncserver *vnc;

    try
    {
        vnc = new clairvoyance::vncserver(argc, argv, config->get("hostname"));
    } catch(std::string e)
    {
        std::cout << e << std::endl;
    }

    for(;;)
    {
        message = conn->read();
        if(message.size()) 
        {
            recv = new clairvoyance::json(message);

            if(recv->get("method") == "update")
            {
                config->set(recv->get("key"), recv->get("value"));
                config->save(argv[1]);
            }

            if(recv->get("method") == "reboot")
            {
                clairvoyance::reboot();
            }
            
            if(recv->get("method") == "pong")
            {
                last_pong = time(0);
            }

            std::cout << message << conn->bytes_read << " / " << conn->bytes_written << std::endl;
        }

        if((last_ping + 15) < time(0))
        {
            last_ping = time(0);
            ping->set("id", config->get("id"));
            ping->set("client-time", std::to_string(last_ping));
            ping->set("hostname", config->get("hostname"));
            ping->set("username", config->get("username"));
            conn->write(ping->to_string());
        }

        if((last_pong + 45) < time(0))
        {
            std::cout << "Lost connection to server. Last: " << last_pong << std::endl;
            delete conn;

            try
            {
                conn = new clairvoyance::net(config->get("server"), std::stoi(config->get("port")));
                while(!conn->is_ready());

                packet = new clairvoyance::json("");
                packet->set("shared-key", config->get("shared-key"));
                conn->write(packet->to_string());
                delete packet;

                std::cout << "Reconnected." << std::endl;
                last_ping = 0;
                last_pong = time(0);
                
            } catch (std::string err)
            {
                std::cout << err << std::endl;
            }
        }
        usleep(250000);
    }
    return 0;
}
