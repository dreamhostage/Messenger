#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
#include <winsock.h>
#include <sys/types.h>

#pragma warning( disable : 4996)
#pragma comment(lib, "Ws2_32.lib")


SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;
SC_HANDLE hService;

const wchar_t* serviceName = L"myService2";
const wchar_t* servicePath;

std::string serverRequest(char* message, int port, std::string ip)
{
	// !@#ERROR#@!
	SOCKET s;
	WSADATA WsaData;
	std::string ansver;
	char temp[256];

	WSAStartup(0x0101, &WsaData);

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = inet_addr(&ip[0]);

	int c = connect(s, (struct sockaddr*) & sin, sizeof(struct sockaddr));

	if (c)
		return "!@#ERROR#@!";

	send(s, message, 256, 0);
	recv(s, temp, 256, 0);

	closesocket(s);

	ansver = temp;

	return ansver;
}

void writeLog(const char* str)
{
	std::ofstream file;
	file.open("log.txt", std::ios::app);
	file << str << '\n';
}

void ControlHandler(DWORD request);

int InstallService()
{
	//SC_HANDLE hService;
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (!hSCManager) {
		writeLog("Error: Can't open Service Control Manager");
		return -1;
	}

	hService = CreateService(
		hSCManager,
		serviceName,
		serviceName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		servicePath,
		NULL, NULL, NULL, NULL, NULL
	);

	if (!hService) {
		int err = GetLastError();
		switch (err) {
		case ERROR_ACCESS_DENIED:
			writeLog("Error: ERROR_ACCESS_DENIED");
			break;
		case ERROR_CIRCULAR_DEPENDENCY:
			writeLog("Error: ERROR_CIRCULAR_DEPENDENCY");
			break;
		case ERROR_DUPLICATE_SERVICE_NAME:
			writeLog("Error: ERROR_DUPLICATE_SERVICE_NAME");
			break;
		case ERROR_INVALID_HANDLE:
			writeLog("Error: ERROR_INVALID_HANDLE");
			break;
		case ERROR_INVALID_NAME:
			writeLog("Error: ERROR_INVALID_NAME");
			break;
		case ERROR_INVALID_PARAMETER:
			writeLog("Error: ERROR_INVALID_PARAMETER");
			break;
		case ERROR_INVALID_SERVICE_ACCOUNT:
			writeLog("Error: ERROR_INVALID_SERVICE_ACCOUNT");
			break;
		case ERROR_SERVICE_EXISTS:
			writeLog("Error: ERROR_SERVICE_EXISTS");
			break;
		default:
			writeLog("Error: Undefined");
		}
		CloseServiceHandle(hSCManager);
		return -1;
	}

	//CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	writeLog("Success install service!");
	return 0;
}

int RemoveService()
{
	//ControlHandler(SERVICE_CONTROL_SHUTDOWN);
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager) {
		writeLog("Error: Can't open Service Control Manager");
		return -1;
	}
	SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP | DELETE);
	if (!hService) {
		writeLog("Error: Can't remove service");
		CloseServiceHandle(hSCManager);
		return -1;
	}

	DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	writeLog("Success remove service!");
	return 0;
}

int StartServicee()
{
	//SC_HANDLE hService;
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	hService = OpenService(hSCManager, serviceName, SERVICE_START);

	if (!hService)
	{
		writeLog("Error: Can't open service");
		return -1;
	}

	if (!StartService(hService, 0, NULL))
	{
		CloseServiceHandle(hSCManager);
		int error = GetLastError();

		if (error == ERROR_ACCESS_DENIED)
			std::cout << "ERROR_ACCESS_DENIED\n";
		if (error == ERROR_INVALID_HANDLE)
			std::cout << "ERROR_INVALID_HANDLE\n";
		if (error == ERROR_PATH_NOT_FOUND)
			std::cout << "ERROR_PATH_NOT_FOUND\n";
		if (error == ERROR_SERVICE_ALREADY_RUNNING)
			std::cout << "ERROR_SERVICE_ALREADY_RUNNING\n";
		if (error == ERROR_SERVICE_DATABASE_LOCKED)
			std::cout << "ERROR_SERVICE_DATABASE_LOCKED\n";
		if (error == ERROR_SERVICE_DEPENDENCY_DELETED)
			std::cout << "ERROR_SERVICE_DEPENDENCY_DELETED\n";
		if (error == ERROR_SERVICE_DEPENDENCY_FAIL)
			std::cout << "ERROR_SERVICE_DEPENDENCY_FAIL\n";
		if (error == ERROR_SERVICE_DISABLED)
			std::cout << "ERROR_SERVICE_DISABLED\n";
		if (error == ERROR_SERVICE_LOGON_FAILED)
			std::cout << "ERROR_SERVICE_LOGON_FAILED\n";
		if (error == ERROR_SERVICE_MARKED_FOR_DELETE)
			std::cout << "ERROR_SERVICE_MARKED_FOR_DELETE\n";
		if (error == ERROR_SERVICE_NO_THREAD)
			std::cout << "ERROR_SERVICE_NO_THREAD\n";
		if (error == ERROR_SERVICE_REQUEST_TIMEOUT)
			std::cout << "ERROR_SERVICE_REQUEST_TIMEOUT\n";
		writeLog("Error: Can't start service");

		return -1;
	}

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	writeLog("Service started!");
	//CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return 0;
}

void ControlHandler(DWORD request)
{
	switch (request)
	{
	case SERVICE_CONTROL_STOP:

		writeLog("Service STOPPED");
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
		return;

	case SERVICE_CONTROL_SHUTDOWN:

		writeLog("Service SHUT DOWN");
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCurrentState = SERVICE_CONTROL_SHUTDOWN;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
		return;

	default:
		break;
	}

	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	return;
}

void WINAPI ServiceMain(int argc, char** argv)
{
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);
	if (serviceStatusHandle == 0)
	{
		writeLog("Error of registrating Service status");
		return;
	}

	SERVICE_STATUS status;

	QueryServiceStatus(hService, &status);

	if (status.dwCurrentState == SERVICE_STOPPED)
	{
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		serviceStatus.dwWin32ExitCode = -1;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
		return;
	}

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	////////////////////////////////////////////////////////////////////

	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
	// ACL is set as NULL in order to allow all access to the object.
	SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	std::string ansver;
	std::string choise;
	std::string PipeServerName = "\\\\.\\pipe\\ConsoleServer";
	std::string PipeDllName = "\\\\.\\pipe\\HookDll";
	char message[256];

	HANDLE hServer = CreateNamedPipeA(&PipeServerName[0],
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		256, 256, 5000, &sa);

	if (hServer == INVALID_HANDLE_VALUE)
		return;

	while (true)
	{
		ConnectNamedPipe(hServer, NULL);

		ReadFile(hServer, message, 256, NULL, NULL);

		ansver = serverRequest(message, 88, "192.168.31.243");

		if (ansver != "!@#ERROR#@!")
			WriteFile(hServer, &ansver[0], 256, NULL, NULL);
		else
			WriteFile(hServer, message, 256, NULL, NULL);

		DisconnectNamedPipe(hServer);
	}

	return;
}

int main(int argc, TCHAR* argv[])
{
	wchar_t dir[MAX_PATH];
	char temp[] = "muService.exe";

	GetCurrentDirectory(sizeof(dir), dir);

	int i = 0;
	while (dir[i++]);
	i -= 1;
	dir[i++] = '\\';
	int t = 0;
	while (temp[t])
		dir[i++] = temp[t++];

	servicePath = dir;

	if (argc - 1 == 0) {
		SERVICE_TABLE_ENTRY ServiceTable;
		ServiceTable.lpServiceName = (LPWSTR)serviceName;
		ServiceTable.lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

		if (!StartServiceCtrlDispatcher(&ServiceTable)) {
			writeLog("Error: StartServiceCtrlDispatcher");
		}
	}
	else if (wcscmp(argv[argc - 1], (TCHAR*)("install")) == 0) {
		InstallService();
	}
	else if (wcscmp(argv[argc - 1], (TCHAR*)("remove")) == 0) {
		RemoveService();
	}
	else if (wcscmp(argv[argc - 1], (TCHAR*)("start")) == 0) {
		StartServicee();
	}
}