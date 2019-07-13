#include <iostream>
#include <Windows.h>
#include <string>

bool change;
char changedMessage[256];

DWORD WINAPI ReadingThread(PVOID p)
{
	HANDLE* hServer = static_cast<HANDLE*>(p);
	char message[256];
	char temp[] = { "!@#$%^&*()" };                           // MAY BE ERROR!!!!!!
	char changed[256];
	int i = 0;
	while (temp[i])
		changed[i] = temp[i++];
	changed[i] = '\0';

	while (true)
	{
		ConnectNamedPipe(*hServer, NULL);

		ReadFile(*hServer, message, 256, NULL, NULL);
		std::cout << message << "\n";

		if (change)
			WriteFile(*hServer, changedMessage, 256, NULL, NULL);
		else
			WriteFile(*hServer, message, 256, NULL, NULL);

		DisconnectNamedPipe(*hServer);
	}

	return 0;
}

int main()
{
	change = FALSE;
	std::string choise;
	std::string PipeServerName = "\\\\.\\pipe\\ConsoleServer";
	std::string PipeDllName = "\\\\.\\pipe\\HookDll";
	std::string created = "***********************\n|Server started|\n***********************\n\n";
	char message[256];

	HANDLE hServer = CreateNamedPipeA(&PipeServerName[0],
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		256, 256, 5000, NULL);

	if (hServer == INVALID_HANDLE_VALUE)
	{
		int error = GetLastError();
		std::cout << "Error creating server...\n\n";
		return error;
	}
	else
		std::cout << created;

	std::cout << "Waiting for client connections...\n\n";

	HANDLE hReadingThread = CreateThread(NULL, 0, ReadingThread, &hServer, 0, 0);

	while (true)
	{
		std::cout << "1 - start changing messages\n";
		std::cout << "2 - stop changing messages\n\n";
		std::getline(std::cin, choise);
		if (choise[0] == '1')
		{
			change = true;
			gets_s(changedMessage);
		}
		if (choise[0] == '2')
		{
			change = FALSE;
		}
	}

	/*
	HANDLE rFile = CreateFileA(&PipeDllName[0], GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (rFile == INVALID_HANDLE_VALUE)
		std::cout << "Error DLL answer chanel!\n";
	*/

	CloseHandle(hServer);
}

/*
#include <iostream>
#include <Windows.h>
#include <string>

bool change;
char changedMessage[256];

DWORD WINAPI ReadingThread(PVOID p)
{
	HANDLE* hServer = static_cast<HANDLE*>(p);
	char message[256];
	char temp[] = { "!@#$%^&*()" };                           // MAY BE ERROR!!!!!!
	char changed[256];
	int i = 0;
	while (temp[i])
		changed[i] = temp[i++];
	changed[i] = '\0';

	while (true)
	{
		ConnectNamedPipe(*hServer, NULL);

		ReadFile(*hServer, message, 256, NULL, NULL);
		std::cout << message << "\n";

		if (change)
		{

			if(WriteFile(*hServer, changed, 256, NULL, NULL));

			if(WriteFile(*hServer, changedMessage, 256, NULL, NULL));

		}

		if (WriteFile(*hServer, changedMessage, 256, NULL, NULL));

		DisconnectNamedPipe(*hServer);
	}

	return 0;
}

int main()
{
	change = FALSE;
	std::string choise;
	std::string PipeServerName = "\\\\.\\pipe\\ConsoleServer";
	std::string PipeDllName = "\\\\.\\pipe\\HookDll";
	std::string created = "***********************\n|Server started|\n***********************\n\n";
	char message[256];

	HANDLE hServer = CreateNamedPipeA(&PipeServerName[0],
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		256, 256, 5000, NULL);

	if (hServer == INVALID_HANDLE_VALUE)
	{
		int error = GetLastError();
		std::cout << "Error creating server...\n\n";
		return error;
	}
	else
		std::cout << created;

	std::cout << "Waiting for client connections...\n\n";

	HANDLE hReadingThread = CreateThread(NULL, 0, ReadingThread, &hServer, 0, 0);

	while (true)
	{
		std::cout << "1 - start changing messages\n";
		std::cout << "2 - stop changing messages\n\n";
		std::getline(std::cin, choise);
		if (choise[0] == '1')
		{
			change = true;
			gets_s(changedMessage);
		}
		if (choise[0] == '2')
		{
			change = FALSE;
		}
	}

CloseHandle(hServer);
}
*/