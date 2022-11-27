#include <ntddk.h>

//#if DBG
#define DbgPrnt(x) DbgPrintEx(0,0,x)

//#else
//#define DebugPring(x)
//#endif
#define LEN(x) (wcsnlen(x,511)+1)*2
NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);
void DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject);

NTSTATUS DispathPassThru(
	PDEVICE_OBJECT DeviceObject,
	PIRP irp
);

NTSTATUS DriverCTL(
	PDEVICE_OBJECT DeviceObject,
	PIRP irp
);