#include "driver.h"

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\my-win-device");
UNICODE_STRING SysLinkName = RTL_CONSTANT_STRING(L"\\??\\link-my-win-device");
PDEVICE_OBJECT DeviceObject = NULL;

#define DEVICE_SEND_BUFF CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)
#define DEVICE_SEND_DIRECT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_IN_DIRECT, FILE_WRITE_DATA)
#define DEVICE_SEND_NEITHER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_NEITHER, FILE_WRITE_DATA)

#define DEVICE_REC_BUFF CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_READ_DATA)
#define DEVICE_REC_DIRECT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_IN_DIRECT, FILE_READ_DATA)
#define DEVICE_REC_NEITHER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_NEITHER, FILE_READ_DATA)

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT driverObject,
	_In_ PUNICODE_STRING registryPath
)
{
	DbgPrnt(("[win-driver]"__FUNCTION__));
	UNREFERENCED_PARAMETER(registryPath);	
	
	driverObject->DriverUnload = DriverUnload; //pointer to unload function
	
	NTSTATUS status = IoCreateDevice(
		driverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&DeviceObject
	);

	if (!NT_SUCCESS(status)) {
		DbgPrnt(("create: device: failed\r\n"));	
		return status;
	}

	status = IoCreateSymbolicLink(&SysLinkName, &DeviceName);

	if (!NT_SUCCESS(status)) {
		DbgPrnt(("create: symbolik link: failed\r\n"));
		IoDeleteDevice(DeviceObject);
		return status;
	}
		
	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		driverObject->MajorFunction[i] = DispathPassThru;

	driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = WinDrvDispatchCTL;
	DbgPrnt(("win-driver: driver: loaded\r\n"));
	RTL_OSVERSIONINFOW os;
	RtlGetVersion(&os);
	//output windows version info 
	DbgPrintEx(0,0,"task 3: windows: %u %u.%u",
		(unsigned int)os.dwBuildNumber,
		(unsigned int)os.dwMajorVersion,
		(unsigned int)os.dwMinorVersion
	);

	return status;
}

void DriverUnload(
	_In_ PDRIVER_OBJECT driverObject)
{
	IoDeleteSymbolicLink(&SysLinkName);
	IoDeleteDevice(DeviceObject);
	DbgPrnt(("win-driver: driver: unloaded\r\n"));	
}

NTSTATUS DispathPassThru(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	PIO_STACK_LOCATION irps = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	
	switch (irps->MajorFunction)
	{
	case IRP_MJ_CREATE:
		DbgPrnt(("win-driver: "__FUNCTION__": request: create"));
		break;
	case IRP_MJ_CLOSE:
		DbgPrnt(("win-driver: "__FUNCTION__": request: close"));
		break;
	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	irp->IoStatus.Information = NULL;
	irp->IoStatus.Status = status;

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS WinDrvDispatchCTL(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	PIO_STACK_LOCATION pirps = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	
	ULONG returnLength = 0;
	PVOID buffer = irp->AssociatedIrp.SystemBuffer;
	ULONG inLength = pirps->Parameters.DeviceIoControl.InputBufferLength;
	ULONG outLength = pirps->Parameters.DeviceIoControl.OutputBufferLength;
	WCHAR* demo_buff = L"text from driver(method buffered)", demo_direct = L"text from driver(method direct)", demo_neither = L"text from driver(method neither)";
	switch (pirps->Parameters.DeviceIoControl.IoControlCode)
	{
	case DEVICE_SEND_BUFF:
		//KdPrint(("send data is %ws \r\n"),buffer);
		DbgPrnt(("win-driver: "__FUNCTION__": request: read: method: buffered"));
		returnLength = LEN(buffer);
		break;
	case DEVICE_SEND_DIRECT:
		//KdPrint(("send data is %ws \r\n"),buffer);
		DbgPrnt(("win-driver: "__FUNCTION__": request: read: method: direct"));
		returnLength = LEN(buffer);
		break;
	case DEVICE_SEND_NEITHER:
		//KdPrint(("send data is %ws \r\n"),buffer);
		DbgPrnt(("win-driver: "__FUNCTION__": request: read: method: neither"));
		returnLength = LEN(buffer);
		break;
	case DEVICE_REC_BUFF:
		wcsncpy(buffer, demo_buff, 511);
		DbgPrnt(("win-driver: "__FUNCTION__": request: write: method: buffered"));
		returnLength = LEN(buffer);
		break;
	case DEVICE_REC_DIRECT:
		wcsncpy(buffer, demo_direct, 511);
		DbgPrnt(("win-driver: "__FUNCTION__": request: write: method: direct"));
		returnLength = LEN(buffer);
		break;
	case DEVICE_REC_NEITHER:
		wcsncpy(buffer, demo_neither, 511);
		DbgPrnt(("win-driver: "__FUNCTION__": request: write: method: neither"));
		returnLength = LEN(buffer);
		break;
	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = returnLength;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}