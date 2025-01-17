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
    // i shoul get subaccount here
    crow::query_string querry = req.url_params;

    const char* currency = querry.get("currency");
    const char* kind = querry.get("kind");
    const char* subaccount = "64910";

    std::cout << "currency: " << currency << std::endl;
    std::cout << "currency: " << kind << std::endl;
    std::cout << "currency: " << subaccount << std::endl;
    
    if (currency == nullptr || kind == nullptr || subaccount == nullptr){
        crow::json::wvalue wval;
        wval["status"] = 400;
        wval["data"] = "Bad request";
        wval["hint"] = "Hint!";
        res.write(wval.dump());
        res.end();
        return ;
    }
    std::cout << "querry: " << querry << std::endl;
    std::string get_postions_api = "https://test.deribit.com/api/v2/private/get_positions?currency=" 
    +std::string(currency)
    +"&kind=" + std::string(kind)
    +"&subaccount_id=" + std::string(subaccount);

    cpr::Header headers{{
        "Authorization", std::string("Bearer ")+AUTH_TOKEN
    }};
    auto response = cpr::Get(
        cpr::Url{get_postions_api},
        headers
    );
    res.write(response.text);
    res.end();
}
