#ifndef ORDERCONTROLLER_HPP
#define ORDERCONTROLLER_HPP

#include "OrderService.hpp"

class OrderController{
	public:
		OrderController();
		~OrderController();
		crow::response getOrderBook();
	private:
		int order_id;
		std::string order_status;
		OrderService orderService;
};

#endif