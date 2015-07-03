#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <stdio.h>
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
//#include "sys/types.h"
#include <fstream>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>


typedef websocketpp::client<websocketpp::config::asio_tls> client;



typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> context_ptr;


using namespace std;
using std::fstream;
using namespace rapidjson;

//important Flags and Variables
extern char JSONcontent[];
extern string authorization_header,access_token,pushbulletbaseurl,pushbulletwebsocket,pb_devices,pb_users,pb_pushes;
extern vector<string> pbmessages;


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

class Connection_Metadata {
public:
    typedef websocketpp::lib::shared_ptr<Connection_Metadata> ptr;

    Connection_Metadata(int id, websocketpp::connection_hdl hdl, string uri);

    void On_Open(client * c, websocketpp::connection_hdl hdl);

    void On_Fail(client * c, websocketpp::connection_hdl hdl);

    void On_Close(client *c, websocketpp::connection_hdl hdl);

    void On_Message(websocketpp::connection_hdl, client::message_ptr msg);

    websocketpp::connection_hdl Get_Hdl()const;

    int Get_Id()const;

    string Get_Status()const;

    void Record_Sent_Messages(string message);

    friend ostream& operator<< (ostream & out, Connection_Metadata const& data);

private:
    int m_id;
    websocketpp::connection_hdl m_hdl;
    string m_status;
    string m_uri;
    string m_server;
    string m_error_reason;
    vector<string> m_messages;
};

ostream& operator<< (ostream & out, Connection_Metadata const& data);

class Websocket_Endpoint{
public:
    Websocket_Endpoint();

    ~Websocket_Endpoint();

    int Connect(string const & uri);

    void Close(int id, websocketpp::close::status::value code, string reason);

    Connection_Metadata::ptr Get_Metadata(int id)const;

    context_ptr on_tls_init(websocketpp::connection_hdl);
private:
    typedef std::map<int,Connection_Metadata::ptr> con_list;
    client m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    con_list m_connection_list;
    int m_next_id;
};


#endif // CORE_H
