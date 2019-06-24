#include <iostream>
#include <Windows.h>
#include <sys/types.h>
#include <string>

#pragma warning( disable : 4996)
#pragma comment(lib, "Ws2_32.lib")

SOCKET sMain;

DWORD WINAPI monitoring(PVOID param)
{
	do
	{
		fd_set s_set = { 1, {sMain} };
		timeval timeout = { 0, 0 };
		int select_res = select(0, &s_set, 0, 0, &timeout);
		if (select_res == SOCKET_ERROR) return -1;
		if (select_res)
		{
			char mes[256];

			SOCKADDR_IN newSin;

			int fromlen = sizeof(newSin);

			SOCKET client = accept(sMain, (struct sockaddr*) & newSin, &fromlen);

			if (client != INVALID_SOCKET)
			{
				std::string ad = inet_ntoa(newSin.sin_addr);
				recv(client, mes, 256, 0);
				std::string message = "[" + ad + "]: " + mes;
				std::cout << message << std::endl;
			}
		}

	} while (1);
}


int main()
{
	//WSA creating:
	//////////////////////////////////////////////////////////////////
	WSADATA WsaData;
	int err = WSAStartup(0x0101, &WsaData);
	if (err == SOCKET_ERROR)
	{
		printf("WSAStartup() failed: %ld\n", GetLastError());
		return 1;
	}
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	//s creating:
	//////////////////////////////////////////////////////////////////
	sMain = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sMain == -1)
		std::cout << "Error creating MAIN socket\n";
	else
		std::cout << "Main socket was created!\n";

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(90);
	sin.sin_addr.s_addr = INADDR_ANY;

	if (!bind(sMain, (LPSOCKADDR)& sin, sizeof(sin)))
		std::cout << "Successful BIND!\n";

	if (!listen(sMain, SOMAXCONN))
		std::cout << "Successful listening\n";

	std::cout << "Waiting for clients...\n\n";

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	HANDLE hMon = CreateThread(NULL, 0, monitoring, NULL, 0, NULL);
	WaitForSingleObject(hMon, INFINITE);

	CloseHandle(hMon);
	closesocket(sMain);
}