#ifndef ORDERSERVICE_HPP
#define ORDERSERVICE_HPP
#include <iostream>
#include "crow.h"
#include <cpr/cpr.h>

class OrderService{
	public:
		OrderService();
		~OrderService();
		crow::response getOrderBook();
		crow::response getPositions(const crow::request &req);
	private:
		int order_id;
		std::string order_status;
};

#endif