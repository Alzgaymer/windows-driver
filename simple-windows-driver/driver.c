#include "driver.h"

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\my-win-device");
UNICODE_STRING SysLinkName = RTL_CONSTANT_STRING(L"\\??\\link-my-win-device");
PDEVICE_OBJECT DeviceObject = NULL;

#define DEVICE_SEND CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)
#define DEVICE_REC CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_READ_DATA)

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

	driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverCTL;
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

NTSTATUS DriverCTL(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	PIO_STACK_LOCATION pirps = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	
	ULONG returnLength = 0;
	PVOID buffer = irp->AssociatedIrp.SystemBuffer;
	ULONG inLength = pirps->Parameters.DeviceIoControl.InputBufferLength;
	ULONG outLength = pirps->Parameters.DeviceIoControl.OutputBufferLength;
	WCHAR* demo = L"text from driver";
	switch (pirps->Parameters.DeviceIoControl.IoControlCode)
	{
	case DEVICE_SEND:
		KdPrint(("send data is %ws \r\n"),buffer);
		returnLength = LEN(buffer);
		break;
	case DEVICE_REC:
		wcsncpy(buffer, demo, 511);
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