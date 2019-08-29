#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <setupapi.h>

int main()
{
	const wchar_t* exeName = L"myMessenger.exe";
	const wchar_t* DriverName = L"\\\\.\\iDriver";

	// injectAPC
	while (true)
	{
		auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			std::cout << "hSnapshot == INVALID_HANDLE_VALUE\n";
			system("pause");
			return -1;
		}
		else
		{
			DWORD pid = 0;
			PROCESSENTRY32 pe = { sizeof(pe) };
			if (Process32First(hSnapshot, &pe)) {
				do
				{
					if (_wcsicmp(pe.szExeFile, exeName) == 0)
					{
						std::cout << "injectAPC" << " found...\n";
						pid = pe.th32ProcessID;
						HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
						if (!hProcess)
						{
							std::cout << "Can't open " << "myMessenger.exe" << "...\n";
							CloseHandle(hProcess);
							system("pause");
							return -1;
						}
						else
							std::cout << "myMessenger.exe" << " opened...\n";
						HANDLE hDriver = CreateFile(DriverName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
						if (hDriver == INVALID_HANDLE_VALUE)
						{
							DWORD error = GetLastError();
							std::cout << "Can't open " << "injectAPC: ERROR " << error << "...\n";
							CloseHandle(hDriver);
							system("pause");
							return -1;
						}
						else
							std::cout << "injectAPC" << " opened...\n";
						if (!WriteFile(hDriver, &pid, sizeof(DWORD), NULL, NULL))
						{
							std::cout << "Can't write to " << "injectAPC" << "...\n";
							CloseHandle(hProcess);
							CloseHandle(hDriver);
							system("pause");
							return -1;
						}
						else
							std::cout << "pid wrote to the " << "injectAPC" << "...\n";
						WaitForSingleObject(hProcess, INFINITE);
						CloseHandle(hProcess);
						CloseHandle(hDriver);
						break;
					}
				} while (::Process32Next(hSnapshot, &pe));
			}
			CloseHandle(hSnapshot);
		}

		Sleep(5000);
	}
}