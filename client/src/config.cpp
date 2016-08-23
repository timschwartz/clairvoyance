#include <iostream>
#include "../include/config.h"

namespace clairvoyance
{
    config::config(std::string filename)
    {
        filename = filename;
        config_file.open(filename, std::fstream::in | std::fstream::out);

        if(!config_file.good())
        {
            throw "Couldn't read config file " + filename;
        }

        config_file.seekg(0, std::ios::end);
        config_data.resize(config_file.tellg());
        config_file.seekg(0, std::ios::beg);
        config_file.read(&config_data[0], config_data.size());
        config_file.close();

        json_file = json_tokener_parse(config_data.c_str());

        if(!json_object_object_get_ex(json_file, "server", &temp))
        {
            throw "'server' not found in config file " + filename;
        }
        server = json_object_get_string(temp);

        if(!json_object_object_get_ex(json_file, "port", &temp))
        {
            throw "'port' not found in config file " +  filename;
        }
        port = json_object_get_int(temp);

        if(!json_object_object_get_ex(json_file, "url", &temp))
        {
            throw "'url' not found in config file " + filename;
        }
        url = json_object_get_string(temp);

        json_object_put(json_file);
        json_object_put(temp);
    }

    config::~config()
    {
        
    }
}
