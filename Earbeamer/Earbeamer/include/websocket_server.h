#pragma once


#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include <map>
#include <utility>

#include "room.h"

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;


using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

class WebsocketServer {

public:
	WebsocketServer(Room& room);
	~WebsocketServer();
	void on_open(connection_hdl hdl);
	void on_close(connection_hdl hdl);
	void on_message(server *s, websocketpp::connection_hdl, message_ptr msg);

	

	void begin_broadcast();
	void broadcast_targets();
	void run(UINT16 port);
	void stop();

private:
	BOOLEAN running;

	thread t_broadcast;		//Thread for broadcasting target information
	thread t_server;		//Thread for managing connections
	
	Room& room;
	std::mutex m_mutex;
	server m_endpoint;

	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;
	con_list m_connections;

	std::map<UINT64, BOOLEAN> parse_client_msg(std::string);	//Parses incoming messages from client on muted targets

	



};