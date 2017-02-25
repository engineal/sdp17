#pragma once


#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>

#include "room.h"

typedef websocketpp::server<websocketpp::config::asio> server;


using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

class WebsocketServer {

public:
	WebsocketServer(Room& room);
	void on_open(connection_hdl hdl);
	void on_close(connection_hdl hdl);

	void begin_broadcast();
	void broadcast_targets();
	void run(UINT16 port);


private:
	thread t_broadcast;		//Thread for broadcasting target information
	thread t_server;		//Thread for managing connections
	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;
	Room& room;
	std::mutex m_mutex;
	server m_endpoint;
	con_list m_connections;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;



};