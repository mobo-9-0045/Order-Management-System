#include"OrderService.hpp"
#include <cpr/cpr.h>

const std::string AUTH_TOKEN = std::getenv("AUTH_TOKEN");

OrderService::OrderService(){
    std::cout << "default constructore called" << std::endl;
    this->ws_url = "wss://test.deribit.com/ws/api/v2";
    this->get_order_book_api = "https://www.deribit.com/api/v2/public/get_order_book?";
    this->modify_order_api = "https://test.deribit.com/api/v2/private/edit";
    this->get_positions_api = "https://test.deribit.com/api/v2/private/get_positions?currency=";
    this->place_order_api = "https://test.deribit.com/api/v2/private/buy";
}

OrderService::~OrderService(){
    std::cout << "default destructore called" << std::endl;
}

crow::json::wvalue createErrorResponse(int status_code, const std::string& message, const std::string& hint = "") {
    crow::json::wvalue error_response;
    error_response["status"] = status_code;
    error_response["message"] = message;
    if (!hint.empty()) {
        error_response["hint"] = hint;
    }
    return error_response;
}

bool authorize(){
    if (AUTH_TOKEN.empty()) {
        return false;
    }
    return true;
}

crow::response OrderService::getOrderBook(const crow::request &req){
    crow::query_string query = req.url_params;
    const char* instrument = query.get("instrument");
    if (!instrument){
        crow::json::wvalue error_response = createErrorResponse(
            400,
            "please provide instrument value"
        );
        return crow::response(error_response);
    }
    std::string get_oreder_book_api = this->get_order_book_api + "instrument_name=" + instrument;
    cpr::Response response = cpr::Get(cpr::Url{get_oreder_book_api});
    if (response.status_code != 200){
        std::cout << "Error occured" << std::endl;
    }
    if (response.status_code == 200){
        crow::json::wvalue wval;
        wval["status"] = 200;
        wval["data"] = crow::json::load(response.text);
        return crow::response(wval);
    }
    crow::json::wvalue error_response = createErrorResponse(
        500,
        "Internal server error"
    );
    return crow::response(error_response);
}

crow::response OrderService::getPositions(const crow::request &req) {
    try {
        crow::query_string query = req.url_params;

        const char* currency = query.get("currency");
        const char* kind = query.get("kind");
        const char* subaccount = query.get("subaccount_id");

        if (!currency || !kind || !subaccount) {
            crow::json::wvalue error_response = createErrorResponse(
                400,
                "Bad request",
                "Please provide a query similar to: /get-positions?currency=BTC&kind=future&subaccount_id=number"
            );
            error_response["currency expected values"] = "[BTC, ETH, EURR, USDC, USDT, any]";
            error_response["kind expected values"] = "[future, future_combo, option, option_combo, spot]";
            return crow::response(error_response);
        }

        this->get_positions_api = this->get_positions_api + std::string(currency) + "&kind=" + std::string(kind) + "&subaccount_id=" + std::string(subaccount);

        if (!authorize()){
            return crow::response(createErrorResponse(
                401,
                "UnAuthorized",
                "PLease check yout token"
            ));
        }
        cpr::Header headers{{"Authorization", std::string("Bearer ")+AUTH_TOKEN}};
        cpr::Response response = cpr::Get(cpr::Url{get_positions_api}, headers);
        if (response.status_code != 200) {
            crow::json::wvalue error_response = createErrorResponse(
                response.status_code,
                "Failed to fetch positions",
                "The external API returned an error pleae check your access token"
            );
            error_response["api_response"] = response.text;
            return crow::response(response.status_code, error_response.dump());
        }
        crow::json::rvalue api_response = crow::json::load(response.text);

        crow::json::wvalue success_response;
        success_response["status"] = 200;
        success_response["positions"] = api_response;
        return crow::response(success_response);
    }
    catch (const std::exception & exception) {
        crow::json::wvalue error_response = createErrorResponse(
            500,
            "Failed to parse JSON",
            exception.what()
        );
        return crow::response(500, error_response);
    }
}

crow::response OrderService::placeOrder(const crow::request &req){
    try{
        crow::json::rvalue request_body = crow::json::load(req.body);
        if (!request_body){
            return crow::response(createErrorResponse(
                400,
                "Bad request",
                "Invalid data"
            ));
        }
        if (!authorize()){
            return crow::response(createErrorResponse(
                401,
                "UnAuthorized",
                "PLease check yout token"
            ));
        }
        cpr::Header headers{{"Authorization", std::string("Bearer ")+AUTH_TOKEN}};
        cpr::Response response = cpr::Post(
                cpr::Url(this->place_order_api),
                headers,
                cpr::Body(req.body)
        );
        if (response.status_code != 200){
            return crow::response(createErrorResponse(
                400,
                response.text
            ));
        }
        crow::json::wvalue success_response;
        success_response["status"] = response.status_code;
        success_response["data"] = crow::json::load(response.text);
        return crow::response(success_response);
    }
    catch(const std::exception &exception){
        return crow::response(createErrorResponse(
            500,
            "Internal server error",
            exception.what()
        ));
    }
}

crow::response OrderService::modifyOrder(const crow::request &req){
    try{
        crow::json::rvalue request_body = crow::json::load(req.body);
        if (!request_body){
            return crow::response(createErrorResponse(
                400,
                "Bad request",
                "Invalid data"
            ));
        }
        if (!authorize()){
            return crow::response(createErrorResponse(
                401,
                "UnAuthorized",
                "PLease check yout token"
            ));
        }
        cpr::Header headers{{"Authorization", std::string("Bearer ")+AUTH_TOKEN}};
        cpr::Response response = cpr::Put(
            cpr::Url(this->modify_order_api), 
            headers,
            cpr::Body(req.body)
        );
        if (response.status_code != 200){
            return crow::response(createErrorResponse(
                400,
                "Somthin went wrong",
                response.text
            ));
        }
        crow::json::wvalue success_response;
        success_response["status"] = response.status_code;
        success_response["data"] = crow::json::load(response.text);
        return crow::response(success_response);
    }
    catch(const std::exception &exception){
        return crow::response(createErrorResponse(
            500,
            "Internal server error",
            exception.what()
        ));
    }
}

void OrderService::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> _(this->clients_mutex);
    for(auto client : clients) {
        client->send_text(message);
    }
};

std::vector<std::string> extract_channels(const std::string& json_data) {
    std::vector<std::string> result;

    try {
        auto parsed_json = crow::json::load(json_data);

        if (parsed_json && parsed_json["params"].has("channels")) {
            auto channels = parsed_json["params"]["channels"];
            for (const auto& channel : channels) {
                std::string channel_str = channel.s();
                size_t pos1 = channel_str.find('.');
                size_t pos2 = channel_str.find('-');
                if (pos1 != std::string::npos) {
                    result.push_back(channel_str.substr(pos1 + 1, pos2 - pos1 - 1)); // Between the period and hyphen
                }
            }
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
    return result;
}

void OrderService::ft_connect(crow::websocket::connection& conn){
    std::cout << "New client connected" << std::endl;
    {
        std::lock_guard<std::mutex> _(this->clients_mutex);
        this->clients.insert(&conn);
    }
    {
        std::lock_guard<std::mutex> lock(thread_mutex);
        client_running[&conn] = true;
    }
    conn.send_text("Welcome! There are " + std::to_string(clients.size()) + " clients connected");
    this->broadcast("A new client has subscribed!\n");
}

void OrderService::start_orderbook_updates(crow::websocket::connection* conn, const std::string& data) {
    stop_orderbook_updates(conn);
    std::thread update_thread([this, conn, data]() {
        try {
            crow::request original_req;
            while (true) {
                {
                    std::lock_guard<std::mutex> lock(thread_mutex);
                    if (!client_running[conn]) {
                        break;
                    }
                }
                
                std::vector<std::string> channels = extract_channels(data);
                for (const auto& channel : channels) {
                    try {
                        original_req.url_params = this->get_order_book_api + "instrument=" + channel;
                        crow::response response = this->getOrderBook(original_req);
                        std::string response_text = response.body;
                        {
                            std::lock_guard<std::mutex> lock(thread_mutex);
                            if (client_running[conn]) {
                                conn->send_text("OrderBook: " + response_text);
                            } 
                            else {
                                return;
                            }
                        }
                    } 
                    catch (const std::exception& e) {
                        std::cerr << "Error processing channel " << channel << ": " << e.what() << std::endl;
                        continue;
                    }
                    
                    std::this_thread::sleep_for(std::chrono::seconds(10));
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        } 
        catch (const std::exception& e) {
            std::cerr << "Thread error: " << e.what() << std::endl;
        }
    });    
    {
        std::lock_guard<std::mutex> lock(thread_mutex);
        client_threads[conn] = std::move(update_thread);
    }
}

void OrderService::stop_orderbook_updates(crow::websocket::connection* conn) {
    std::lock_guard<std::mutex> lock(thread_mutex);
    
    auto thread_it = client_threads.find(conn);
    if (thread_it != client_threads.end()) {
        client_running[conn] = false;
        if (thread_it->second.joinable()) {
            thread_it->second.join();
        }
        client_threads.erase(thread_it);
    }
}

void OrderService::ft_message(crow::websocket::connection &conn, const std::string &data, bool is_binary){
    try{
        if (is_binary){
            std::cout << "invalid data: " << data << std::endl;
            return ;
        }
        this->send_websocket_message(this->ws_url + "/public/subscribe", data);
        conn.send_text("subscibed: "+ data);
        this->start_orderbook_updates(&conn, data);
    }
    catch(const std::exception &exception){
        std::cout << "Error: " << exception.what() << std::endl;
    }
}


void OrderService::ft_close(crow::websocket::connection &conn, const std::string &data){
    std::cout << "Client disconnected" << std::endl;
    stop_orderbook_updates(&conn);
    {
        std::lock_guard<std::mutex> _(clients_mutex);
        clients.erase(&conn);
    }
    {
        std::lock_guard<std::mutex> lock(thread_mutex);
        client_running.erase(&conn);
    }
    std::cout << "data: " << data << std::endl;
    this->broadcast("A client has disconnected!");
}

std::shared_ptr<websocketpp::lib::asio::ssl::context> on_tls_init(websocketpp::connection_hdl) {
    auto ctx = std::make_shared<websocketpp::lib::asio::ssl::context>(websocketpp::lib::asio::ssl::context::tlsv12);

    try {
        ctx->set_options(websocketpp::lib::asio::ssl::context::default_workarounds |
            websocketpp::lib::asio::ssl::context::no_sslv2 |
            websocketpp::lib::asio::ssl::context::no_sslv3 |
            websocketpp::lib::asio::ssl::context::single_dh_use);
        ctx->set_verify_mode(websocketpp::lib::asio::ssl::verify_none);
    }
    catch (std::exception& e) {
        std::cout << "TLS initialization failed: " << e.what() << std::endl;
    }
    return ctx;
}

void OrderService::send_websocket_message(const std::string& uri, const std::string& message) {
    client ws_client;

    try {
        ws_client.clear_access_channels(websocketpp::log::alevel::all);

        ws_client.init_asio();

        ws_client.set_tls_init_handler(on_tls_init);

        websocketpp::lib::error_code ec;
        client::connection_ptr con = ws_client.get_connection(uri, ec);

        if (ec) {
            std::cerr << "Error creating connection: " << ec.message() << std::endl;
            return;
        }

        ws_client.connect(con);

        con->set_open_handler([&ws_client, &con, &message](websocketpp::connection_hdl) {
            ws_client.send(con->get_handle(), message, websocketpp::frame::opcode::text);
            ws_client.stop();
        });

        ws_client.run();

    } 
    catch (const websocketpp::exception& e) {
        std::cerr << "WebSocket++ Exception: " << e.what() << std::endl;
    }
}
