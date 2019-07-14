#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <Windows.h>
#include <sys/types.h>
#include <string>

#pragma comment (lib, "gtestd.lib")
#pragma comment (lib, "gmockd.lib")
#pragma comment (lib, "Ws2_32.lib")



class mockserver
{
	WSADATA WsaData;
	SOCKET sMain;

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

		WSABUF lpBuffers;
		lpBuffers.buf = message;
		lpBuffers.len = 256;

		while (n)
		{
			send();
			WSASend(sMain, &lpBuffers, 1, NULL, NULL, NULL, NULL);
			std::cout << message << std::endl;
			Sleep(5000);
			--n;
		}
	}

	MOCK_CONST_METHOD0(start, void());
	MOCK_CONST_METHOD0(send, void());

};

class mockclient
{
	WSADATA WsaData;
	SOCKET s;

public:
	mockclient()
	{

	}

	void startengine()
	{
		Sleep(1000);
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
			system("pause");
			return;
		}

		char message[256];
		std::string mes = "*****Client message*****";
		int i = 0;
		for (; i < mes.size(); ++i)
			message[i] = mes[i];
		message[i] = '\0';

		WSABUF lpBuffers;
		lpBuffers.buf = message;
		lpBuffers.len = 256;

		while (n)
		{
			Sleep(3000);
			send();
			WSASend(s, &lpBuffers, 1, NULL, NULL, NULL, NULL);
			std::cout << message << std::endl;
			--n;
		}
	}

	MOCK_CONST_METHOD0(start, void());
	MOCK_CONST_METHOD0(send, void());

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
	EXPECT_CALL(mServer, send()).Times(5);
	EXPECT_CALL(mClient, send()).Times(5);

	HANDLE hThread = CreateThread(NULL, 0, clientThread, &mClient, 0, NULL);

	mServer.startengine();
	WaitForSingleObject(hThread, INFINITE);
}

int main()
{
	if (!LoadLibraryA("C:\\GitHub\\Messenger\\x64\\Debug\\HookDLL.dll"))
	{
		return 0;
	}

	testing::InitGoogleMock();

	RUN_ALL_TESTS();

	system("pause");

	return 0;
}

