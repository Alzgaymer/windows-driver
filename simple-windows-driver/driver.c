#include "driver.h"

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\my-win-device");
UNICODE_STRING SysLinkName = RTL_CONSTANT_STRING(L"\\??\\link-my-win-device");
PDEVICE_OBJECT DeviceObject = NULL;


NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT driverObject,
	_In_ PUNICODE_STRING registryPath
)
{
	DbgPrnt(("[simple-windows-driver]"__FUNCTION__));
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
		
	DbgPrnt(("win-driver: driver: loaded"));
	return status;
}

void DriverUnload(
	_In_ PDRIVER_OBJECT driverObject)
{
	IoDeleteSymbolicLink(&SysLinkName);
	IoDeleteDevice(DeviceObject);
	DbgPrnt(("win-driver: driver: unloaded"));
}

