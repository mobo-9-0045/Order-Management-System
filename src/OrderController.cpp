#include"OrderController.hpp"
#include <iostream>

OrderController::OrderController(){
	std::cout << "OrderController Initialized" << std::endl;
}

crow::response OrderController::getOrderBook(){
	return this->orderService.getOrderBook();
}

OrderController::~OrderController(){
	std::cout << "Order Controller FInished" << std::endl;
}