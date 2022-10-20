#include "driver.h"


NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT driverObject,
	_In_ PUNICODE_STRING registryPath
) 
{
	DbgPrintEx(0,0,"[simple-windows-driver] Driver load");
	UNREFERENCED_PARAMETER(registryPath);
	driverObject->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}

void DriverUnload(
	_In_ PDRIVER_OBJECT driverObject)
{
	DbgPrintEx(0, 0, "[simple-windows-driver] Driver unload");

	UNREFERENCED_PARAMETER(driverObject);
}
