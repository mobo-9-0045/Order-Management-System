#include "crow.h"
#include "src/OrderController.hpp"

int main(){

    crow::SimpleApp app;

    CROW_ROUTE(app, "/get-order-book")([](){
        OrderController orderController;
        return orderController.getOrderBook();
    });

    CROW_ROUTE(app, "/cancel-order")([](){
        return "Cancel an order";
    });

    CROW_ROUTE(app, "/get-positions")([](){
        // i should get subaccouns first i just hardcode it from here that's it
        std::string get_postions_api = "https://test.deribit.com/api/v2/private/get_positions?currency=BTC&kind=future&subaccount_id=64910";
        auto response = cpr::Get(cpr::Url{get_postions_api});
        printf("response: %d\n", response.status_code);
        return response.text;
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