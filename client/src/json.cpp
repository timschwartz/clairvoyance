#include <iostream>
#include "../include/json.h"

namespace clairvoyance
{
    json::json(std::string json_string)
    {
        if(!json_string.size()) json_string = "{}";
        json_object *json_data;

        json_data = json_tokener_parse(json_string.c_str());

        json_object_object_foreach(json_data, key, val)
        {
            set(key, json_object_get_string(val));
        }

        json_object_put(json_data);
    }

    json::~json()
    {
        
    }

    /**
        Retrieve configuration option.
 
        @param[in]     name A string naming the option to retrieve.
        @return The value of the requested option. 
    */

    std::string json::get(std::string name)
    {
        return values[name];
    }

    /**
        Set configuration option.

        @param[in]     name A string naming the option to set.
        @param[in]     value The value of the requested option.
    */
    void json::set(std::string name, std::string value)
    {
        values[name] = value;
    }

    std::string json::to_string()
    {
       json_object *json_data = json_object_new_object();

       for (const auto& kv : values) 
       {

           json_object_object_add(json_data, kv.first.c_str(), json_object_new_string(kv.second.c_str()));
       }

       std::string data = std::string(json_object_to_json_string(json_data));
       json_object_put(json_data);
       return data;
    }

    void json::save(std::string filename)
    {
        std::fstream json_file;
        json_file.open(filename, std::fstream::in | std::fstream::out);
        json_file << to_string() << std::endl;
        json_file.close();
    }
}
