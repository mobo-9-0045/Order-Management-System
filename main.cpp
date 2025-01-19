#include "crow.h"
#include "src/OrderService.hpp"



int main(){

    crow::SimpleApp app;
    OrderService orderService;

    std::set<crow::websocket::connection*> clients;
    std::mutex clients_mutex;

    auto broadcast = [&](const std::string& message) {
        std::lock_guard<std::mutex> _(clients_mutex);
        for(auto client : clients) {
            client->send_text(message);
        }
    };

    CROW_ROUTE(app, "/get-order-book")([&orderService](const crow::request &req){
        return orderService.getOrderBook(req);
    });
    CROW_ROUTE(app, "/cancel-order")([](){
        return "Cancel an order";
    });

    CROW_ROUTE(app, "/get-positions")([&orderService](const crow::request &req){
        return orderService.getPositions(req);
    });

    CROW_ROUTE(app, "/place-order").methods(crow::HTTPMethod::POST)([&orderService](const crow::request &req){
        return orderService.placeOrder(req);
    });

    CROW_ROUTE(app, "/modify-order").methods(crow::HTTPMethod::PUT)([&orderService](const crow::request &req){
        return orderService.modifyOrder(req);
        //std::string get_oreder_time_api = "https://www.deribit.com/api/v2/public/get_time"; 
        // std::cout << get_oreder_time_api << std::endl;
    });

    CROW_WEBSOCKET_ROUTE(app, "/public-instument-name")
    .onopen([&](crow::websocket::connection &conn){
        orderService.ft_connect(conn);
    }).
    onmessage([&](crow::websocket::connection &conn, const std::string& data, bool is_binary){
        orderService.ft_message(conn, data, is_binary);
    })
    .onclose([&](crow::websocket::connection &conn, const std::string& data){
        orderService.ft_close(conn, data);
    });

    CROW_ROUTE(app, "/broadcast").methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        broadcast("Server announcement: " + req.body);
        return crow::response(200, "Broadcast sent");
    });
    app.port(18080).multithreaded().run();
}