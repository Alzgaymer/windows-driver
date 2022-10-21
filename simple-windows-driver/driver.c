#include "driver.h"

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\my-win-device");
UNICODE_STRING SysLinkName = RTL_CONSTANT_STRING(L"\\??\\link-my-win-device");
PDEVICE_OBJECT DeviceObject = NULL;


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
	case IRP_MJ_READ:
		DbgPrnt(("win-driver: "__FUNCTION__": request: read"));
		break;
	default:
		break;
	}

	irp->IoStatus.Information = NULL;
	irp->IoStatus.Status = status;

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;
}

