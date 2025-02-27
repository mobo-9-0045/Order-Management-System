#ifndef ORDERSERVICE_HPP
#define ORDERSERVICE_HPP
#include <iostream>
#include "crow.h"
#include <cpr/cpr.h>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/common/asio_ssl.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/context.hpp>

class OrderService{
	public:
		OrderService();
		~OrderService();
		crow::response getOrderBook(const crow::request &req);
		crow::response getPositions(const crow::request &req);
		crow::response placeOrder(const crow::request &req);
		crow::response modifyOrder(const crow::request &req);

		//Ws function;
		void ft_connect(crow::websocket::connection& conn);
		void ft_message(crow::websocket::connection& conn, const std::string& data, bool is_binary);
		void ft_close(crow::websocket::connection &conn, const std::string& data);
		void broadcast(const std::string& message);

	private:
		struct ClientInfo {
			std::set<std::string> subscriptions;  // Tracks which instruments this client is subscribed to
		};
		std::string ws_url;
		std::mutex thread_mutex;
		std::string order_status;
    	std::mutex clients_mutex;
		std::string place_order_api;
		std::string modify_order_api;
		std::string get_positions_api;
		std::string get_order_book_api;
		std::set<crow::websocket::connection*> clients;
		std::map<crow::websocket::connection*, bool> client_running;
		std::map<crow::websocket::connection*, std::thread> client_threads;
		typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

    	void stop_orderbook_updates(crow::websocket::connection* conn);
		void send_websocket_message(const std::string& uri, const std::string& message);
		void start_orderbook_updates(crow::websocket::connection* conn, const std::string& data);
};

#endif