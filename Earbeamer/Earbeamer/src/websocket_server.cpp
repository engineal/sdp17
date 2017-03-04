#pragma once


#include <mutex>
#include "websocket_server.h"
#include <sstream>
#include "room.h"
#include <boost/regex.hpp>


WebsocketServer::WebsocketServer(Room& the_room) : room(the_room){

	running = true;

	m_endpoint.set_access_channels(websocketpp::log::alevel::none);
	m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

	m_endpoint.init_asio();

	m_endpoint.set_open_handler(bind(&WebsocketServer::on_open, this, _1));
	m_endpoint.set_close_handler(bind(&WebsocketServer::on_close, this, _1));
	m_endpoint.set_message_handler(bind(&WebsocketServer::on_message, this, &m_endpoint, _1, _2));
	

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

	std::cout << "Received message from client:" << endl;
	std::cout << "################" << endl;
	std::cout << msg->get_payload() << endl;
	std::cout << "################" << endl;

	std::map<UINT64, BOOLEAN> mute_status = this->parse_client_msg(msg->get_payload());

	room.muteTargets(mute_status);


}

std::map<UINT64, BOOLEAN> WebsocketServer::parse_client_msg(std::string json) {

	boost::regex r("{\"id\":[\n ]*([0-9]*)[^,]*?,[ \n]*\"muted\":[\n ]*([a-z]*)}", boost::regex::perl | boost::regex::icase);

	boost::sregex_iterator beg = boost::sregex_iterator(json.begin(), json.end(), r);
	boost::sregex_iterator end = boost::sregex_iterator();

	std::map<UINT64, BOOLEAN> out;

	//Iterate over all id matches within json
	for (boost::sregex_iterator it = beg; it != end; it++)
	{
		boost::smatch match = *it;
		string s_id = match[1];								//Gets the capture group
		cout << "The id is " << s_id << endl;
		UINT64 i_id = (_strtoui64(s_id.c_str(), NULL, 10));	//Convert string to unsigned 64-bit int

		string s_muted = match[2];
		cout << "The mute status is " << s_muted << endl;
		BOOLEAN b_muted;
		if (!s_muted.compare("true")) {
			b_muted = true;
		}
		else {
			b_muted = false;
		}

		out.insert(std::pair<UINT64, BOOLEAN>(i_id, b_muted));
	}

	return out;

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
	
	while (running) {
		
		Room::target_trigger.wait(lck);

		std::stringstream ss;
		ss.str(std::string());

		targs = room.getTargets();

		ss << "{\"targets\":[";
		for (map<UINT64, Target*>::iterator itr = targs.begin(); itr != targs.end(); ++itr)
		{
			ss << *(itr->second);
			if (!(std::next(itr, 1) == targs.end()))
			{
				ss << ",";
			}
		}
		ss << "]}";

		//cout << ss.str() << endl;

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

void WebsocketServer::stop(){
	running = false;
	if (t_broadcast.joinable()) {
		t_broadcast.join();
	}

	if (t_server.joinable()) {
		t_server.join();
	}
}


