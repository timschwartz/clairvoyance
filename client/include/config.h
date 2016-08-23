#ifndef _CLAIRVOYANCE_CONFIG_H
#define _CLAIRVOYANCE_CONFIG_H

#include <string>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <fstream>

namespace clairvoyance
{
    class config
    {
        public:
            std::string filename;
            std::string server;
            uint16_t port;
            std::string url;
            config(std::string);
            ~config();

        private:
            json_object *json_file, *temp;
            std::fstream config_file;
            std::string config_data;
    };
}

#endif
