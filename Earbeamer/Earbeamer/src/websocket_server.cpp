#pragma once


#include <mutex>
#include "websocket_server.h"
#include <sstream>
#include "room.h"


WebsocketServer::WebsocketServer(Room& the_room) : room(the_room){

	m_endpoint.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
	m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

	m_endpoint.init_asio();

	m_endpoint.set_open_handler(bind(&WebsocketServer::on_open, this, _1));
	m_endpoint.set_close_handler(bind(&WebsocketServer::on_close, this, _1));
	m_endpoint.set_message_handler(bind(&on_message, this, ::_1, ::_2));
	

}

/**
 *	Callback when a client terminates the websocket connection
 */ 
void WebsocketServer::on_close(connection_hdl hdl) {
	std::lock_guard<std::mutex> guard(m_mutex);
	cout << "Someone left!" << endl;
	m_connections.erase(hdl);
}

/**
 *	Callback when a client joins a websocket connection
 */
void WebsocketServer::on_open(connection_hdl hdl) {
	std::lock_guard<std::mutex> guard(m_mutex);
	cout << "Someone connected!" << endl;
	m_connections.insert(hdl);
}

/**
 *	Callback to handle event when server receives a message from the client
 *
 */

void WebsocketServer::on_message(server *s, websocketpp::connection_hdl, message_ptr msg) {

	std::cout << "Received message from client" << endl;

	std::map<UINT64, BOOLEAN> mute_status = this->parse_client_msg(msg->get_payload());

	



}

std::map<UINT64, BOOLEAN> WebsocketServer::parse_client_msg(std::string) {



}


/**
 *	Initiates a thread that continually publishes target locations over websockets
 */
void WebsocketServer::begin_broadcast() {
	this->t_broadcast = thread(&WebsocketServer::broadcast_targets, this);
}

void WebsocketServer::broadcast_targets() {
	
	shared_lock<shared_mutex> lck(Room::target_mutex);
	std::map<UINT64, Target*> targs;
	
	while (1) {
		
		Room::target_trigger.wait(lck);

		std::stringstream ss;
		ss.str(std::string());

		targs = room.getTargets();

		ss << "{\"targets\":[";
		for (map<UINT64, Target*>::iterator itr = targs.begin(); itr != targs.end(); itr++)
		{
			ss << *(itr->second);
		}
		ss << "]}";

		cout << ss.str() << endl;

		for (auto it : m_connections) {
			m_endpoint.send(it, ss.str(), websocketpp::frame::opcode::text);
		}
		
	}

}

void WebsocketServer::run(UINT16 port) {
	m_endpoint.listen(port);
	m_endpoint.start_accept();

	t_server = thread(&server::run, &m_endpoint);

}


