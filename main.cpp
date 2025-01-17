#include "crow.h"
#include "src/OrderService.hpp"

int main(){

    crow::SimpleApp app;

    // CROW_ROUTE(app, "/get-auth-token")([](){
    //     return "geting token function";
    // });

    CROW_ROUTE(app, "/get-order-book")([](const crow::request &req){
        OrderService orderService;
        return orderService.getOrderBook(req);
    });

    CROW_ROUTE(app, "/cancel-order")([](){
        return "Cancel an order";
    });

    CROW_ROUTE(app, "/get-positions")([](const crow::request &req){
        OrderService orderService;
        return orderService.getPositions(req);
    });

    CROW_ROUTE(app, "/place-order")([](){
        std::string get_oreder_time_api = "https://www.deribit.com/api/v2/public/get_time";
        auto response = cpr::Get(cpr::Url{get_oreder_time_api});
        //i must use this time to place an order
        return response.text;
    });

    // more rooute must be added

    app.port(18080).multithreaded().run();
}