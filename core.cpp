#include "core.h"

//Set the Variables and Flags
char JSONcontent[] = "Content-Type: application/json";
std::string authorization_header = "Authorization: Bearer ",
access_token="",
pushbulletbaseurl = "https://api.pushbullet.com/v2/",
pushbulletwebsocket = "wss://stream.pushbullet.com/websocket/",
pb_devices = "devices",
pb_users = "users/me",
pb_pushes = "pushes";
vector<string> pbmessages;
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

/**
 * @brief Retrieve acces token from file, or ask for it if no file exists
 * @return Acces token
 */
string getAccessToken() {
    fstream infile;
    string result;
    infile.open("access_token", fstream::in);

    // If file already exists, return saved token
    if (infile.is_open()) {
        infile >> result;
        infile.close();
        return result;
    }

    fstream outfile;
    outfile.open("access_token", fstream::out);
    if (!outfile.is_open()) {
        cout << "Could not create file, please check permissions!" << endl;
        return "-1";
    }

    // Ask for token until valid input is given
    bool firstTry = true;
    do {
        if (firstTry == true) {
            cout << "Please enter your access_token, you can find it on https://www.pushbullet.com under Account Settings." << endl;
            firstTry = false;
        } else {
            cout << "Incorrect token, please try again!" << endl;
        }
        getline(cin, result);
        outfile << result;
        outfile.close();
    } while (result.length() != 32);

    return result;
}

string GetFullURL(string identifier){
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

CustomHTTPHeader::CustomHTTPHeader(CURL* curl, AuthorizationHeader AuthHeader,vector<string> AdditionalArguments){
    Currentcurl = curl;
    CurrentAuthHeader = AuthHeader;
    chunk = NULL;
    chunk = curl_slist_append(chunk,CurrentAuthHeader.prefix.append(CurrentAuthHeader.access_token).c_str());
    SetAdditionalArguments(AdditionalArguments);
}

CustomHTTPHeader::~CustomHTTPHeader(){
    curl_slist_free_all(chunk);
};

void CustomHTTPHeader::SetAdditionalArguments(vector<string> AdditionalArguments){
    for (int i=0;i<AdditionalArguments.size();i++){
        chunk = curl_slist_append(chunk,AdditionalArguments[i].c_str());
    }
}

CURLcode CustomHTTPHeader::SetCustomHeader(){
    result = curl_easy_setopt(Currentcurl,CURLOPT_HTTPHEADER,chunk);
    return result;
}

Websocket_Endpoint::Websocket_Endpoint(): m_next_id(0){
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

    m_endpoint.init_asio();
    m_endpoint.start_perpetual();
    m_endpoint.set_tls_init_handler([this](websocketpp::connection_hdl){
        return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
    });
    //m_endpoint.set_tls_init_handler(bind(&type::on_tls_init,this,::_1));
    m_thread.reset(new websocketpp::lib::thread(&client::run,&m_endpoint));
}

context_ptr Websocket_Endpoint::on_tls_init(websocketpp::connection_hdl){
     context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
     try {
                 ctx->set_options(boost::asio::ssl::context::default_workarounds |
                                  boost::asio::ssl::context::no_sslv2 |
                                  boost::asio::ssl::context::no_sslv3 |
                                  boost::asio::ssl::context::single_dh_use);
             } catch (std::exception& e) {
                 std::cout << e.what() << std::endl;
             }
             return ctx;
}

int Websocket_Endpoint::Connect(const string &uri){

    websocketpp::lib::error_code ec;

           client::connection_ptr con = m_endpoint.get_connection(uri, ec);

           if (ec) {
               std::cout << "> Connect initialization error: " << ec.message() << std::endl;
               return -1;
           }
               int new_id = m_next_id++;
               Connection_Metadata::ptr metadata_ptr(new Connection_Metadata(new_id, con->get_handle(), uri));
               m_connection_list[new_id] = metadata_ptr;

               con->set_open_handler(websocketpp::lib::bind(&Connection_Metadata::On_Open,metadata_ptr,&m_endpoint,websocketpp::lib::placeholders::_1));
               con->set_fail_handler(websocketpp::lib::bind(&Connection_Metadata::On_Fail,metadata_ptr,&m_endpoint,websocketpp::lib::placeholders::_1));
               con->set_close_handler(websocketpp::lib::bind(&Connection_Metadata::On_Close,metadata_ptr,&m_endpoint,websocketpp::lib::placeholders::_1));
               con->set_message_handler(websocketpp::lib::bind(&Connection_Metadata::On_Message,metadata_ptr,websocketpp::lib::placeholders::_1,websocketpp::lib::placeholders::_2));
               m_endpoint.connect(con);

              return new_id;

}

void Websocket_Endpoint::Close(int id, websocketpp::close::status::value code, string reason){
    websocketpp::lib::error_code ec;

        con_list::iterator metadata_it = m_connection_list.find(id);
        if (metadata_it == m_connection_list.end()) {
            std::cout << "> No connection found with id " << id << std::endl;
            return;
        }

        m_endpoint.close(metadata_it->second->Get_Hdl(), code, "", ec);
        if (ec) {
            std::cout << "> Error initiating close: " << ec.message() << std::endl;
        }
}

Websocket_Endpoint::~Websocket_Endpoint(){
    m_endpoint.stop_perpetual();

        for (con_list::const_iterator it = m_connection_list.begin(); it != m_connection_list.end(); ++it) {
            if (it->second->Get_Status() != "Open") {
                // Only close open connections
                continue;
            }

            std::cout << "> Closing connection " << it->second->Get_Id() << std::endl;

            websocketpp::lib::error_code ec;
            m_endpoint.close(it->second->Get_Hdl(), websocketpp::close::status::going_away, "", ec);
            if (ec) {
                std::cout << "> Error closing connection " << it->second->Get_Id() << ": "
                          << ec.message() << std::endl;
            }
        }

        m_thread->join();
}

Connection_Metadata::ptr Websocket_Endpoint::Get_Metadata(int id) const {
    con_list::const_iterator metadata_it = m_connection_list.find(id);
            if (metadata_it == m_connection_list.end()) {
                return Connection_Metadata::ptr();
            } else {
                return metadata_it->second;
            }
}

Connection_Metadata::Connection_Metadata(int id, websocketpp::connection_hdl hdl, string uri):m_id(id),m_hdl(hdl),m_status("Connecting"),m_uri(uri),m_server("N/A"){

}

void Connection_Metadata::On_Open(client *c, websocketpp::connection_hdl hdl){
    m_status = "Open";

            client::connection_ptr con = c->get_con_from_hdl(hdl);
            m_server = con->get_response_header("Server");
}

void Connection_Metadata::On_Fail(client *c, websocketpp::connection_hdl hdl){
    m_status = "Failed";

            client::connection_ptr con = c->get_con_from_hdl(hdl);
            m_server = con->get_response_header("Server");
            m_error_reason = con->get_ec().message();
}

void Connection_Metadata::On_Close(client *c, websocketpp::connection_hdl hdl){
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_remote_close_code() << " (" << websocketpp::close::status::get_string(con->get_remote_close_code()) << "), close reason: " << con->get_remote_close_reason();
    m_error_reason = s.str();
}

void Connection_Metadata::On_Message(websocketpp::connection_hdl, client::message_ptr msg){
    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
                m_messages.push_back("<< " + msg->get_payload());
                pbmessages.push_back(msg->get_payload());
            }
    else {
                m_messages.push_back("<< " + websocketpp::utility::to_hex(msg->get_payload()));
            }
}

websocketpp::connection_hdl Connection_Metadata::Get_Hdl()const {
    return m_hdl;
}

int Connection_Metadata::Get_Id() const{
    return m_id;
}

string Connection_Metadata::Get_Status() const {
    return m_status;
}

void Connection_Metadata::Record_Sent_Messages(string message){
    m_messages.push_back(">> " + message);
}


ostream& operator<< (ostream & out, Connection_Metadata const& data){
    out << "> URI: " << data.m_uri << "\n"
        << "> Status: " << data.m_status << "\n"
        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);
    out << "> Messages Processed: (" << data.m_messages.size() << ") \n";

        std::vector<std::string>::const_iterator it;
        for (it = data.m_messages.begin(); it != data.m_messages.end(); ++it) {
            out << *it << "\n";
        }
        return out;

}
