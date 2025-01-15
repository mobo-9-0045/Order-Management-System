#include "crow.h"
#include "src/OrderController.hpp"

int main(){

    crow::SimpleApp app;

    CROW_ROUTE(app, "/get-order-book")([](){
        OrderController orderController;
        return orderController.getOrderBook();
    });

    app.port(18080).multithreaded().run();
}