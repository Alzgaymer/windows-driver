
#include <ntddk.h>


NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);
void DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject);
