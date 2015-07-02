// sudo apt-get install libcurl4-gnutls-dev
// sudo apt-get install libboost-all-dev
// https://github.com/miloyip/rapidjson/ (copy 'rapidjason-master/inlcude/rapidjson' folder into /usr/include/)

//#include <QCoreApplication>

#include "core.h"

int main(int argc, char *argv[])
{
    //initialize the variables
    string 	pushbulleturl = "https://api.pushbullet.com/v2/users/me",pushbulletdevices = "https://api.pushbullet.com/v2/devices",pushbulletpushes ="https://api.pushbullet.com/v2/pushes",
            pushbulletwebsocket = "wss://stream.pushbullet.com/websocket/", OPO="ufeWjfqsjAiVsKnSTs",push = "{\"device_iden\":\"ufeWjfqsjAiVsKnSTs\", \"type\": \"note\", \"title\": \"NOOOICCCE\", \"body\": \"Aint that cool\"}";
    char base[]  = "https://api.pushbullet.com/v2/users/me";

    access_token = getAccessToken();

    //initialize curl MUST BE DONE ONCE
    curl_global_init( CURL_GLOBAL_ALL );

    // get a CURL handle to perform the requests on
    CURL * curl;
    CURLcode result;
    // start the CURL session
    curl = curl_easy_init();

    if (curl){


        AuthorizationHeader AuthHeader;
        AuthHeader.prefix = authorization_header;
        AuthHeader.access_token = access_token;
        //cout<<AuthHeader.authorization<<endl;
        CustomHTTPHeader header(curl, AuthHeader);

        result = header.SetCustomHeader();

        curl_easy_setopt(curl, CURLOPT_URL, GetFullURL(pb_users).c_str());

        //create a pointer to a curl_slist struct this will be holding the new header
        //struct curl_slist *chunk = NULL;

        //append the access_token and authorization_header to the slist
        //chunk = curl_slist_append(chunk,authorization_header.append(access_token).c_str());

        //chunk = curl_slist_append(chunk,JSONcontent);

        //set CURL to use the custom header with our access_token
        //result = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        struct BufferStruct output;
        output.buffer = NULL;
        output.size = 0;


        //hookup the CallbackFunction
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        //and the struct to get the data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&output);

        //curl_off_t length_of_data=push.length();

        //curl_easy_setopt(curl, CURLOPT_URL, pushbulletpushes.c_str());

        //curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,"DELETE");

        //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, length_of_data);
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, push.c_str());

        //curl_easy_setopt(curl, CURLOPT_HTTPGET,1L);

        //perform the standard request-->writes to stdout by default
        result = curl_easy_perform(curl);

        Document user;

        user.Parse(output.buffer);
        cout<<output.buffer<<endl;





    }
    return 0;
}
