#pragma once
#include "pch.h"

class InterceptServer
{
	SOCKET *sMain;
	WSADATA *WsaData;
	SOCKADDR_IN *sin;
	char message[266];
	char changedMes[256];
	bool intercepting;

public:
	InterceptServer();
	~InterceptServer();
	std::string startWork();
	void setIntercepting(std::string mes);
	void setPort(int port);
	void clear();
	void unsetIntercepting();
};