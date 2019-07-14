#include "stdafx.h"
#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include "chat.h"

using namespace boost::asio;


wait_for::wait_for(int port, bool client)
{
	io_s = new io_service;
	endpoint = new ip::tcp::endpoint(ip::tcp::v4(), port);
	acceptor = new ip::tcp::acceptor(*io_s, *endpoint);

	connection = new bool;
	connectedName = new std::string;
	yourName = new std::string;
	ip = new std::string;
	this->port = new int;

	*this->port = port;
	*connection = FALSE;

}
wait_for::~wait_for()
{
	clear();
}
void wait_for::clear()
{
	if (ip)
	{
		delete ip;
		ip = nullptr;
	}
	if (yourName)
	{
		delete yourName;
		yourName = nullptr;
	}
	if (connection)
	{
		delete connection;
		connection = nullptr;
	}
	if (connectedName)
	{
		delete connectedName;
		connectedName = nullptr;
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
void wait_for::reset()
{
	if (socket)
	{
		//socket->close();
		delete socket;
		socket = nullptr;
	}
}
boost::system::error_code wait_for::accept()
{
	boost::system::error_code ec;
	socket = new ip::tcp::socket(*io_s);
	char cname[256];
	acceptor->accept(*socket, ec);
	if (ec)
		return ec;
	else
	{
		*connection = TRUE;
		socket->read_some(boost::asio::buffer(cname, 256));
		*connectedName = cname;
		int i = 0;

		for (; i < yourName->size(); ++i)
			cname[i] = (*yourName)[i];
		cname[i] = '\0';
		socket->write_some(boost::asio::buffer(cname, 256));
		std::string succ = "**********123456789**********";
		i = 0;
		for (; i < succ.size(); ++i)
			cname[i] = succ[i];
		cname[i] = '\0';
		socket->write_some(boost::asio::buffer(cname, 256));
		return ec;
	}
}
boost::system::error_code wait_for::connect(std::string ip, int port)
{
	socket = new ip::tcp::socket(*io_s);
	boost::system::error_code ec;
	endpoint->port(port);
	endpoint->address(ip::address_v4::from_string(&ip[0]));
	socket->connect(*endpoint, ec);
	if (ec)
		return ec;
	else
	{
		*connection = true;
		char cname[256];
		int i = 0;
		for (; i < yourName->size(); ++i)
			cname[i] = (*yourName)[i];
		cname[i] = '\0';
		socket->write_some(boost::asio::buffer(cname, 256));
		socket->read_some(boost::asio::buffer(cname, 256));
		*connectedName = cname;

		return ec;
	}
}
std::string wait_for::read()
{
	//system("pause");
	std::string message;
	char cmessage[256];

	socket->wait(ip::tcp::socket::wait_read);

	if (socket->is_open() && socket->available())
	{

		socket->read_some(boost::asio::buffer(cmessage, 256));
		message = cmessage;
		return message;
	}
	else
	{
		std::string ret = "**********EXIT**********";
		return ret;
	}

}
void wait_for::write(std::string message)                          //entered message
{
	char cmessage[256];
	int i = 0;
	for (; i < message.size(); ++i)
		cmessage[i] = message[i];
	cmessage[i] = '\0';
	socket->write_some(boost::asio::buffer(cmessage, 256));
}











/*

wait_for::wait_for(int port, bool client)
{ 
    io_s = new io_service;
	if (!client)
	{
		endpoint = new ip::tcp::endpoint(ip::tcp::v4(), port);
		acceptor = new ip::tcp::acceptor(*io_s, *endpoint);
	}
	else
	{
		socket = new ip::tcp::socket(*io_s);
	}


    connection = new bool;
    connectedName = new std::string;
    yourName = new std::string;
	ip = new std::string;
	this->port = new int;

	*this->port = port;
    *connection = FALSE;

}
wait_for::~wait_for()
{
   clear();
}
void wait_for::clear()
{
	if (ip)
	{
		delete ip;
		ip = nullptr;
	}
    if(yourName)
    {
        delete yourName;
        yourName = nullptr;
    }
    if(connection)
    {
        delete connection;
        connection = nullptr;
    }
    if(connectedName)
    {
        delete connectedName;
        connectedName = nullptr;
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
    if(io_s)
    {
        delete io_s;
        io_s = nullptr;
    }
}
void wait_for::reset()
{
    if (socket)
    {
        socket->close();
        delete socket;
        socket = nullptr;
    }
}
void wait_for::accept()
{
    socket = new ip::tcp::socket(*io_s);
    char cname[256];
    acceptor->accept(*socket);
    *connection = TRUE;
    socket->read_some(boost::asio::buffer(cname, 256));
    *connectedName = cname;
    int i = 0;
	std::string temp;
	temp = *yourName;
    for (; i < yourName->size(); ++i)
        cname[i] = temp[i];
    cname[i] = '\0';
    socket->write_some(boost::asio::buffer(cname, 256));
}
boost::system::error_code wait_for::connect(std::string ip, int port)
{
	boost::system::error_code ec;
	endpoint = new ip::tcp::endpoint;
	endpoint->port(port);
	endpoint->address(ip::address_v4::from_string(&ip[0]));
	socket->connect(*endpoint, ec);
	if (ec)
		return ec;
	else
	{
		char cname[256];
		int i = 0;
		for (; i < yourName->size(); ++i)
			cname[i] = (*yourName)[i];
		cname[i] = '\0';
		socket->write_some(boost::asio::buffer(cname, 256));
		socket->read_some(boost::asio::buffer(cname, 256));
		*connectedName = cname;

		return ec;
	}
}
std::string wait_for::read()
{
	std::string message;
    char cmessage[256];

	socket->wait(ip::tcp::socket::wait_read);

	if (socket->is_open() && socket->available())
	{

		socket->read_some(boost::asio::buffer(cmessage, 256));
		message = cmessage;
		return message;
	}
	else
	{
		std::string ret = "**********EXIT**********";
		return ret;
	}

}
void wait_for::write(std::string message)                          //entered message
{
    char cmessage[256];
    int i = 0;
    for (; i < message.size(); ++i)
        cmessage[i] = message[i];
    cmessage[i] = '\0';
    socket->write_some(boost::asio::buffer(cmessage, 256));
}

*/