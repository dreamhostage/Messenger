#include "pch.h"
#include "framework.h"
#include "intser.h"

#pragma warning( disable : 4996)
#pragma comment(lib, "Ws2_32.lib")

InterceptServer::InterceptServer()
{
	intercepting = false;
	sMain = NULL;
	WsaData = NULL;
	sin = NULL;
}
InterceptServer::~InterceptServer()
{
	clear();
}
std::string InterceptServer::startWork()
{
	std::string mes;

	int fromlen = sizeof(*sin);

	SOCKET s = accept(*sMain, (struct sockaddr*) sin, &fromlen);

	recv(s, message, 256, 0);

	if (intercepting)
		send(s, changedMes, 256, 0);
	else
		send(s, message, 256, 0);

	mes = message;

	return mes;
}
void InterceptServer::setIntercepting(std::string mes)
{
	int i = 0;
	while (mes[i])
		changedMes[i] = mes[i++];
	changedMes[i] = '\0';
	intercepting = true;
}
void InterceptServer::setPort(int port)
{
	sMain = new SOCKET;
	WsaData = new WSADATA;
	sin = new SOCKADDR_IN;

	WSAStartup(0x0101, WsaData);
	*sMain = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sin->sin_family = AF_INET;
	sin->sin_port = htons(port);
	sin->sin_addr.s_addr = INADDR_ANY;
	bind(*sMain, (LPSOCKADDR) sin, sizeof(*sin));
	listen(*sMain, SOMAXCONN);
}
void InterceptServer::clear()
{
	WSACleanup();
	if(sMain)
		delete sMain;
	if (WsaData)
		delete WsaData;
	if(sin)
		delete sin;
}
void InterceptServer::unsetIntercepting()
{
	intercepting = false;
}