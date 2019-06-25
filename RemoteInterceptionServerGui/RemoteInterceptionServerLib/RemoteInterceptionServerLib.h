#pragma once
#include <string>
#include <boost/asio.hpp>

using namespace boost::asio;

class server
{
	io_service* io_s;
	ip::tcp::socket* socket;
	ip::tcp::endpoint* endpoint;
	ip::tcp::acceptor* acceptor;
public:
	std::string* ip;
	int* port;
	bool* connection;
	server(int port);
	~server();
	void clear();
	void reset();
	boost::system::error_code accept();
	std::string read();
	void write(std::string message);
	std::string loop();
};