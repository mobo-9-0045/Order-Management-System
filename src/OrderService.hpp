#ifndef ORDERSERVICE_HPP
#define ORDERSERVICE_HPP
#include <iostream>
#include "crow.h"
#include <cpr/cpr.h>

class OrderService{
	public:
		OrderService();
		~OrderService();
		crow::response getOrderBook(const crow::request &req);
		crow::response getPositions(const crow::request &req);
		crow::response placeOrder(const crow::request &req);
		void testClang();
	private:
		int order_id;
		std::string order_status;
};

#endif