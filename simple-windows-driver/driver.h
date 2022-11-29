#include <ntddk.h>
#include <string.h>
//#if DBG
#define DbgPrnt(x) DbgPrintEx(0,0,x)

//#else
//#define DebugPring(x)
//#endif
#if DBG
#define WINDRVCTL_KDPRINT(_x_) \
                DbgPrint("WIN_DRIVER.SYS: ");\
                DbgPrint _x_;

#else
#define WINDRVCTL_KDPRINT(_x_)
#endif
#define LEN(x) (wcslen(x)+1)*2
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

NTSTATUS WinDrvDispatchCTL(
	PDEVICE_OBJECT DeviceObject,
	PIRP irp
);
VOID
PrintChars(
    _In_reads_(CountChars) PCHAR BufferAddress,
    _In_ size_t CountChars
)
{
    PAGED_CODE();

    if (CountChars) {

        while (CountChars--) {

            if (*BufferAddress > 31
                && *BufferAddress != 127) {

                KdPrint(("%c", *BufferAddress));

            }
            else {

                KdPrint(("."));

            }
            BufferAddress++;
        }
        KdPrint(("\n"));
    }
    return;
}
VOID
PrintIrpInfo(
    PIRP Irp)
{
    PIO_STACK_LOCATION  irpSp;
    irpSp = IoGetCurrentIrpStackLocation(Irp);


    WINDRVCTL_KDPRINT(("\tIrp->AssociatedIrp.SystemBuffer = 0x%p\n",
        Irp->AssociatedIrp.SystemBuffer));
    WINDRVCTL_KDPRINT(("\tIrp->UserBuffer = 0x%p\n", Irp->UserBuffer));
    WINDRVCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.Type3InputBuffer = 0x%p\n",
        irpSp->Parameters.DeviceIoControl.Type3InputBuffer));
    WINDRVCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.InputBufferLength = %d\n",
        irpSp->Parameters.DeviceIoControl.InputBufferLength));
    WINDRVCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.OutputBufferLength = %d\n",
        irpSp->Parameters.DeviceIoControl.OutputBufferLength));
    return;
}