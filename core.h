#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <stdio.h>
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "sys/types.h"



//important Flags and Variables
extern char JSONcontent[];
extern std::string authorization_header,access_token,pushbulletbaseurl,pb_devices,pb_users,pb_pushes;

//Callback Function for CURL
size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

std::string GetFullURL(std::string identifier);

//Buffer Struct to hold the Result
struct BufferStruct{
    char * buffer;
    size_t size;
};


//Holds the neccessary Information to authenticate
struct AuthorizationHeader{
    std::string prefix;
    std::string access_token;
};


class CustomHTTPHeader{
    public:
        CustomHTTPHeader(CURL* curl, AuthorizationHeader AuthHeader);
        CustomHTTPHeader(CURL* curl, AuthorizationHeader AuthHeader, std::vector<std::string> additionalArguemnts);
        ~CustomHTTPHeader();
        void SetAdditionalArguments(std::vector<std::string> additionalArguemnts);
        CURLcode SetCustomHeader();

    private:
        AuthorizationHeader CurrentAuthHeader;
        CURL* Currentcurl;
        CURLcode result;
        struct curl_slist* chunk;
};




#endif // CORE_H
