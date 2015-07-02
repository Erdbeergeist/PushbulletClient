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
#include <fstream>

using namespace std;
using std::fstream;
using namespace rapidjson;

//important Flags and Variables
extern char JSONcontent[];
extern string authorization_header,access_token,pushbulletbaseurl,pb_devices,pb_users,pb_pushes;

//Callback Function for CURL
size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

string GetFullURL(string identifier);



string getAccessToken();

//Buffer Struct to hold the Result
struct BufferStruct{
    char * buffer;
    size_t size;
};


//Holds the neccessary Information to authenticate
struct AuthorizationHeader{
    string prefix;
    string access_token;
};


class CustomHTTPHeader{
public:
    CustomHTTPHeader(CURL* curl, AuthorizationHeader AuthHeader);
    CustomHTTPHeader(CURL* curl, AuthorizationHeader AuthHeader, vector<string> additionalArguemnts);
    ~CustomHTTPHeader();
    void SetAdditionalArguments(vector<string> additionalArguemnts);
    CURLcode SetCustomHeader();

private:
    AuthorizationHeader CurrentAuthHeader;
    CURL* Currentcurl;
    CURLcode result;
    struct curl_slist* chunk;
};




#endif // CORE_H
