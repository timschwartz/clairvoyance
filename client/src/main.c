#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <json-c/json_object.h>

typedef struct {
    char server[4096];
    uint16_t port;
    char url[4096];
} config_t;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Usage:\n%s config.json\n", argv[0]);
        return -1;
    }

    config_t config;

    json_object *json_file, *temp;
    FILE *file = fopen(argv[1], "r");

    if(!file)
    {
        printf("Couldn't open configuration file %s.\n", argv[1]);
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    uint32_t sz = ftell(file);
    rewind(file);
    char *data = (char *)malloc(sizeof(char) * sz);
    fread(data, sizeof(char), sz, file);

    json_file = json_tokener_parse(data);
    if(!json_object_object_get_ex(json_file, "server", &temp)) 
    {
        printf("server not found in config file %s.\n", argv[0]);
    }
    strcpy(config.server, json_object_get_string(temp));

    if(!json_object_object_get_ex(json_file, "port", &temp))
    {
        printf("port not found in config file %s.\n", argv[0]);
    }
    config.port = json_object_get_int(temp);

    if(!json_object_object_get_ex(json_file, "url", &temp))
    {
        printf("url not found in config file %s.\n", argv[0]);
    }
    strcpy(config.url, json_object_get_string(temp));

    free(data);
    close(file);
    json_object_put(json_file);
    json_object_put(temp);

    printf("Server: %s\n", config.server);
    printf("Port: %d\n", config.port);
    printf("URL: %s\n", config.url);
    return 0;
}
