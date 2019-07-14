#pragma once

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#pragma comment(lib, "Ws2_32.lib")
#include "mhook-lib/mhook.h"
#pragma comment(lib,"mhook.lib")
