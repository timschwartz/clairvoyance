#include <stdio.h>
#include <iostream>
#include <json-c/json.h>
#include <json-c/json_object.h>

#include "../include/config.h"

int main(int argc, char *argv[])
{
    clairvoyance::config *config;

    if(argc < 2)
    {
        printf("Usage:\n%s config.json\n", argv[0]);
        return -1;
    }

    try
    {
        config = new clairvoyance::config(argv[1]);
    } 
    catch (std::string e)
    {
        std::cout << e << std::endl;
        return -1;
    }

    std::cout << "Server: " <<  config->server << std::endl;
    std::cout << "Port: " << config->port << std::endl;
    std::cout << "URL: " << config->url << std::endl;
    return 0;
}
