#include"OrderService.hpp"
#include <cpr/cpr.h>

const std::string AUTH_TOKEN = std::getenv("AUTH_TOKEN");

OrderService::OrderService(){
    std::cout << "default constructore called" << std::endl;
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
    std::string get_oreder_book_api = "https://www.deribit.com/api/v2/public/get_order_book";
    crow::query_string query = req.url_params;
    const char* instrument = query.get("instrument");
    if (!instrument){
        crow::json::wvalue error_response = createErrorResponse(
            400,
            "please provide instrument value"
        );
        return crow::response(error_response);
    }
    cpr::Response response = cpr::Get(cpr::Url{get_oreder_book_api}, cpr::Parameters{{"instrument_name", instrument}});
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
        std::string get_positions_api = "https://test.deribit.com/api/v2/private/get_positions?currency=" 
        + std::string(currency) 
        + "&kind=" + std::string(kind) 
        + "&subaccount_id=" + std::string(subaccount);
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
        std::cout << "body: " << request_body << std::endl;
        if (!request_body){
            return crow::response(createErrorResponse(
                400,
                "Bad request",
                "Invalid data"
            ));
        }
        const std::string place_order_api = "https://test.deribit.com/api/v2/private/buy";
        if (!authorize()){
            return crow::response(createErrorResponse(
                401,
                "UnAuthorized",
                "PLease check yout token"
            ));
        }
        cpr::Header headers{{"Authorization", std::string("Bearer ")+AUTH_TOKEN}};
        cpr::Response response = cpr::Post(
                cpr::Url(place_order_api),
                headers,
                cpr::Body(req.body)
        );
        if (response.status_code != 200){
            std::cout << "response: " << response.text << std::endl;
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
        const std::string place_order_api = "https://test.deribit.com/api/v2/private/edit";
        if (!authorize()){
            return crow::response(createErrorResponse(
                401,
                "UnAuthorized",
                "PLease check yout token"
            ));
        }
        cpr::Header headers{{"Authorization", std::string("Bearer ")+AUTH_TOKEN}};
        cpr::Response response = cpr::Put(
            cpr::Url(place_order_api), 
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