// sudo apt-get install libcurl4-gnutls-dev
// sudo apt-get install libboost-all-dev
// https://github.com/miloyip/rapidjson/ (copy 'rapidjason-master/inlcude/rapidjson' folder into /usr/include/)

//#include <QCoreApplication>

#include "core.h"

int main(int argc, char *argv[]){

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

        Websocket_Endpoint endpoint;



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
        cout<<user["name"].GetString()<<endl;
        bool quit=false;
        string input;
        int msize = 0;
        int baseid = endpoint.Connect(pushbulletwebsocket.append(access_token));
        while (!quit) {
                if (pbmessages.size()>msize){
                    msize=pbmessages.size();
                    cout<<pbmessages[pbmessages.size()-1]<<endl;
                }
                /*cout << "Enter Command: ";
                //getline(cin, input);

                if (input == "quit") quit = true;
                else if (input == "help") {
                    std::cout
                        << "\nCommand List:\n"
                        << "connect <ws uri>\n"
                        << "show <connection id>\n"
                        << "help: Display this help text\n"
                        << "quit: Exit the program\n"
                        << std::endl;
                }
                else if (input.substr(0,7) == "connect") {
                    int id = endpoint.Connect(pushbulletwebsocket.append(access_token));
                    if (id != -1) {
                        std::cout << "> Created connection with id " << id << std::endl;
                    }
                }
                else if (input.substr(0,4) == "show") {
                    int id = atoi(input.substr(5).c_str());
                    cout<<id<<endl;
                    Connection_Metadata::ptr metadata = endpoint.Get_Metadata(id);
                    if (metadata) {
                        std::cout << *metadata << std::endl;
                    }
                    else {
                        std::cout << "> Unknown connection id " << id << std::endl;
                    }
                }
                else if (input.substr(0,5) == "close") {
                    std::stringstream ss(input);

                    std::string cmd;
                    int id;
                    int close_code = websocketpp::close::status::normal;
                    std::string reason = "";

                    ss >> cmd >> id >> close_code;
                    std::getline(ss,reason);

                    endpoint.Close(id, close_code,reason);
                }
               else {
                    std::cout << "> Unrecognized Command" << std::endl;
                }*/
            }

        }



    return 0;
}
