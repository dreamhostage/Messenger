#pragma once
#include <string>
#include <boost/asio.hpp>

using namespace boost::asio;

class wait_for
{
    io_service *io_s;
	ip::tcp::endpoint *endpoint;
	ip::tcp::acceptor *acceptor;
	ip::tcp::socket *socket;
public:
	std::string *ip;
	int *port;
    bool *connection;
	std::string *connectedName;
	std::string *yourName;
    wait_for(int port, bool client);
	~wait_for();
    void clear();
    void reset();
	boost::system::error_code accept();
	boost::system::error_code connect(std::string ip, int port);
	std::string read();
    void write(std::string message);
};
