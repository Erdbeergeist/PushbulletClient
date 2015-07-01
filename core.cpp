#include "core.h"

//Set the Variables and Flags
char JSONcontent[] = "Content-Type: application/json";
std::string authorization_header = "Authorization: Bearer ",
    access_token="",
    pushbulletbaseurl = "https://api.pushbullet.com/v2/",
    pb_devices = "devices",
    pb_users = "users/me",
    pb_pushes = "pushes";

// Function to pass to CURL, fills BufferStruct
size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data){

    size_t realsize = size * nmemb;

    struct BufferStruct * mem = (struct BufferStruct *) data;

    //we need to cast this to char* since buffer is of the type char*
    mem->buffer = (char *) realloc(mem->buffer, mem->size + realsize + 1);

    if ( mem->buffer ){
        memcpy( &( mem->buffer[ mem->size ] ), ptr, realsize );
        mem->size += realsize;
        mem->buffer[ mem->size ] = 0;
    }
return realsize;
}

std::string GetAccess_Token(){
    std::fstream infile;
    std::string result;
    infile.open("access_token",std::fstream::in);

    if (infile.is_open()) infile>>result;
    else if(!infile.is_open()){
        infile.close();

        infile.open("access_token",std::fstream::out);
        if(!infile.is_open()) return "Could not create file, please check permissions";
        std::cout<<"Please enter your access_token, you can find it on https://www.pushbullet.com under Account Settings\n";
        getline(std::cin,result);
        infile<<result;
        infile.close();
    }
    return result;
}

std::string GetFullURL(std::string identifier){
    return pushbulletbaseurl.append(identifier);
}

/*CustomHTTPHeader Functions
        Use this to set custom HTTPHeaders
        setting AuthHeader via Constructor is NECCESSARY
*/
CustomHTTPHeader::CustomHTTPHeader(CURL* curl,AuthorizationHeader AuthHeader){
    Currentcurl = curl;
    CurrentAuthHeader = AuthHeader;
    chunk = NULL;
    chunk = curl_slist_append(chunk,CurrentAuthHeader.prefix.append(CurrentAuthHeader.access_token).c_str());

}

CustomHTTPHeader::CustomHTTPHeader(CURL* curl, AuthorizationHeader AuthHeader,std::vector<std::string> AdditionalArguments){
    Currentcurl = curl;
    CurrentAuthHeader = AuthHeader;
    chunk = NULL;
    chunk = curl_slist_append(chunk,CurrentAuthHeader.prefix.append(CurrentAuthHeader.access_token).c_str());
    SetAdditionalArguments(AdditionalArguments);
}

CustomHTTPHeader::~CustomHTTPHeader(){
    curl_slist_free_all(chunk);
};

void CustomHTTPHeader::SetAdditionalArguments(std::vector<std::string> AdditionalArguments){
    for (int i=0;i<AdditionalArguments.size();i++){
        chunk = curl_slist_append(chunk,AdditionalArguments[i].c_str());
    }
}

CURLcode CustomHTTPHeader::SetCustomHeader(){
    result = curl_easy_setopt(Currentcurl,CURLOPT_HTTPHEADER,chunk);
    return result;
}


