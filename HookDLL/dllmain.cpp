// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#pragma warning( disable : 4996)

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

		for (int i = 0; i < lpBuffers->len; ++i)
			lpBuffers->buf[i] = ansver[i];
	}

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

		Mhook_SetHookEx(hooks, 2);
		break;

	case DLL_PROCESS_DETACH:
		Mhook_UnhookEx((PVOID * *)& hooks, 2);
		break;
	}
	return TRUE;
}

//return point 12.07.19
//////////////////////////
/*
// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#pragma warning( disable : 4996)

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

		Mhook_SetHookEx(hooks, 2);
		break;

	case DLL_PROCESS_DETACH:
		Mhook_UnhookEx((PVOID * *)& hooks, 2);
		break;
	}
	return TRUE;
}
*/