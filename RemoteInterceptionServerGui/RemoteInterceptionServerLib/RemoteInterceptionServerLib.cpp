#include "pch.h"
#include "framework.h"
#include <string>
#include <boost/asio.hpp>
#include "RemoteInterceptionServerLib.h"

using namespace boost::asio;

server::server(int port)
{
	io_s = new io_service;
	endpoint = new ip::tcp::endpoint(ip::tcp::v4(), port);
	acceptor = new ip::tcp::acceptor(*io_s, *endpoint);

	connection = new bool;
	ip = new std::string;
	this->port = new int;

	*this->port = port;
	*connection = FALSE;
}
server::~server()
{
	clear();
}
void server::clear()
{
	if (ip)
	{
		delete ip;
		ip = nullptr;
	}
	if (connection)
	{
		delete connection;
		connection = nullptr;
	}
	if (endpoint)
	{
		delete endpoint;
		endpoint = nullptr;
	}
	if (acceptor)
	{
		delete acceptor;
		acceptor = nullptr;
	}
	if (socket)
	{
		socket->close();
		delete socket;
		socket = nullptr;
	}
	if (io_s)
	{
		delete io_s;
		io_s = nullptr;
	}
}
void server::reset()
{
	if (socket)
	{
		//socket->close();
		delete socket;
		socket = nullptr;
	}
}
boost::system::error_code server::accept()
{
	boost::system::error_code ec;
	socket = new ip::tcp::socket(*io_s);
	char cname[256];
	acceptor->accept(*socket, ec);
	return ec;
}
std::string server::read()
{
	std::string message;
	char cmessage[256];

	socket->wait(ip::tcp::socket::wait_read);

	if (socket->is_open() && socket->available())
	{
		socket->read_some(boost::asio::buffer(cmessage, 256));
		message = "| " + socket->remote_endpoint().address().to_string() + " | ";
		message += cmessage;
		return message;
	}
	else
	{
		std::string ret = "**********EXIT**********";
		return ret;
	}

}
void server::write(std::string message)                          //entered message
{
	char cmessage[256];
	int i = 0;
	for (; i < message.size(); ++i)
		cmessage[i] = message[i];
	cmessage[i] = '\0';
	socket->write_some(boost::asio::buffer(cmessage, 256));
}
std::string server::loop()
{
	std::string message;
	this->accept();
	message = this->read();
	this->reset();
	return message;
}
