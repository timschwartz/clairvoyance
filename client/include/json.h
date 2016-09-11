#ifndef _CLAIRVOYANCE_JSON_H
#define _CLAIRVOYANCE_JSON_H

#include <string>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <fstream>
#include <map>

namespace clairvoyance
{
    class json
    {
        public:
            std::string json_string;
            json(std::string);
            ~json();
            std::string get(std::string);
            void set(std::string, std::string);
            std::string to_string();
            void save(std::string filename);

        private:
            std::map<std::string, std::string> values;
    };
}

#endif
