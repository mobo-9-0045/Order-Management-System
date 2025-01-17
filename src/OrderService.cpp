#include"OrderService.hpp"
#include <cpr/cpr.h>

#define AUTH_TOKEN "1739643141759.1TR6H7Om.IWh0X3adwDCxg9IdHSlDKkjcTbQnrS8_qXDRgmcJeXQUim_KqSfBxFUcqvylx1tc7u1RRmI3MdB2JDLPbcCzbXMwEBGHsEMaJh1lp3hklCNFanBnh63uoXMex7YtdArBjpGvWOTTTDJ4ovChDInDEo4623xgpvQ15OCkfr6C1sDjEeaAqjRdJdQb8GKzDwVYdlWIkPXL08acfu55R5sIMFpHzB8jI3u0llM3625Of_V9SFyc6q8l8eaJfK9oKPC75LCwRVCgeR0AAcZbSRsCiRa80k17iHsEaHCny4ZCo9HIIN20uSYkU-SrPBig7NOUovC2Q548-CCoec23uE2LNjxhafpq88lWroPFGKFw8AJ1uAY05A25uAib6f2KJaO2gQ"

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

void OrderService::getPositions(const crow::request &req, crow::response &res){
    std::string get_postions_api = "https://test.deribit.com/api/v2/private/get_positions?currency=BTC&kind=future&subaccount_id=64910";

    cpr::Header headers{{
        "Authorization", std::string("Bearer ")+AUTH_TOKEN
    }};
    auto response = cpr::Get(
        cpr::Url{get_postions_api},
        headers
    );
    printf("status code: %d\n", response.status_code);
    res.write(response.text);
    res.end();
}