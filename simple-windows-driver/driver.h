#include <ntddk.h>

//#if DBG
#define DbgPrnt(x) DbgPrintEx(0,0,x)
//#else
//#define DebugPring(x)
//#endif

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);
void DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject);

NTSTATUS OnCreate(PDEVICE_OBJECT deviceObject, PIRP irp);
NTSTATUS OnClose(PDEVICE_OBJECT deviceObject, PIRP irp);