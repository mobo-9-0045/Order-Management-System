#include"OrderService.hpp"
#include <cpr/cpr.h>

OrderService::OrderService(){
    std::cout << "default constructore called" << std::endl;
}

OrderService::~OrderService(){
    std::cout << "default destructore called" << std::endl;
}

crow::response OrderService::getOrderBook(){
    std::string get_oreder_book_api = "https://www.deribit.com/api/v2/public/get_order_book";
    std::string instrument = "BTC-PERPETUAL";
    auto response = cpr::Get(cpr::Url{get_oreder_book_api}, cpr::Parameters{{"instrument_name", instrument}});
    if (response.status_code != 200){
        std::cout << "Error occured" << std::endl;
    }
    if (response.status_code == 200){
        crow::json::wvalue wval;
        wval["status"] = 200;
        wval["data"] = crow::json::load(response.text);
        return crow::response(wval);
    }
    crow::json::wvalue error_response;
    error_response["status"] = response.status_code;
    error_response["message"] = "Error occurred while fetching data";
    return crow::response(500, error_response.dump());
}