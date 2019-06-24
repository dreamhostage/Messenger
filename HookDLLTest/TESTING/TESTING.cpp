#include <iostream>
#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <Windows.h>
#include <sys/types.h>
#include <string>


#pragma comment (lib, "gtestd.lib")
#pragma comment (lib, "gmockd.lib")
#pragma comment (lib, "Ws2_32.lib")

class server
{

public:

	virtual void start()
	{
		
	}
};

class client
{

public:

	virtual void start()
	{

	}
};

class mockserver : public server
{
	WSADATA WsaData;
	SOCKET sMain;
	server mockSer;
public:
	mockserver()
	{

	}

	void startengine()
	{
		int n = 5;
		start();
		WSAStartup(0x0101, &WsaData);
		sMain = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(80);
		sin.sin_addr.s_addr = INADDR_ANY;
		bind(sMain, (LPSOCKADDR)& sin, sizeof(sin));
		listen(sMain, SOMAXCONN);

		SOCKET connectsoc;
		SOCKADDR_IN newSin;

		int fromlen = sizeof(newSin);

		std::cout << "Accepting connection...";
		connectsoc = accept(sMain, (struct sockaddr*) & newSin, &fromlen);

		char message[256];
		std::string mes = "*****Server message*****";
		int i = 0;
		for (; i < mes.size(); ++i)
			message[i] = mes[i];
		message[i] = '\0';

		while (n)
		{

			send(sMain, message, 256, 0);
			std::cout << message << std::endl;
			Sleep(5000);
			--n;
		}
	}

	MOCK_CONST_METHOD0(start, void());
	
};

class mockclient : public client
{
	WSADATA WsaData;
	SOCKET s;
	client mockCl;
public:
	mockclient()
	{

	}

	void startengine()
	{
		int n = 5;
		start();
		WSAStartup(0x0101, &WsaData);
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(80);
		sin.sin_addr.S_un.S_addr = inet_addr("192.168.31.243"); // Server adres

		int c = connect(s, (struct sockaddr*) & sin, sizeof(struct sockaddr));

		if (!c)
			std::cout << "Connection successful!\n";
		else
		{
			std::cout << "Server is offline...\n";
			return;
		}

		char message[256];
		std::string mes = "*****Client message*****";
		int i = 0;
		for (; i < mes.size(); ++i)
			message[i] = mes[i];
		message[i] = '\0';

		while (n)
		{
			Sleep(3000);
			send(s, message, 256, 0);
			std::cout << message << std::endl;
			--n;
		}
	}

	MOCK_CONST_METHOD0(start, void());

};

DWORD WINAPI clientThread(PVOID p)
{
	mockclient* mClient = static_cast<mockclient*>(p);
	mClient->startengine();
	return 0;
}

TEST(Engine_TEST, startup)
{
	mockserver mServer;
	mockclient mClient;

	EXPECT_CALL(mServer, start());
	EXPECT_CALL(mClient, start());


	HANDLE hThread = CreateThread(NULL, 0, clientThread, &mClient, 0, NULL);
	mServer.startengine();
}

int main()
{
	LoadLibraryA("C:\\GitHub\\myMessengerHookDll\\x64\\Debug\\myMessengerHookDll.dll");
	system("pause");
	testing::InitGoogleMock();

	return RUN_ALL_TESTS();
}