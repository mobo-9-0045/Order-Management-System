#include "crow.h"
#include "src/OrderService.hpp"



int main(){

    crow::SimpleApp app;
    OrderService orderService;

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
    });

    // more rooute must be added

    app.port(18080).multithreaded().run();
}