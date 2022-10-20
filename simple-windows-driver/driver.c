#include "driver.h"

//32768 - 65535
#define _DEVICE_TYPE 32768 

PUNICODE_STRING name;

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT driverObject,
	_In_ PUNICODE_STRING registryPath
)
{
	DbgPrintEx(0, 0, "[simple-windows-driver] Driver load");
	UNREFERENCED_PARAMETER(registryPath);
	RtlInitUnicodeString(name, L"\Device\Simple2"); //copy  L"\Device\Simple2" to name as unicode
	driverObject->DriverUnload = DriverUnload; //pointer to unload function
	PDEVICE_OBJECT deviceObject;
	NTSTATUS status = IoCreateDevice(
		driverObject, 0,
		name,
		_DEVICE_TYPE,
		0, FALSE,
		0
	);
	if (!NT_SUCCESS(status))
		return status;
	
	return STATUS_SUCCESS;
}

void DriverUnload(
	_In_ PDRIVER_OBJECT driverObject)
{
	DbgPrintEx(0, 0, "[simple-windows-driver] Driver unload");

	UNREFERENCED_PARAMETER(driverObject);
}

