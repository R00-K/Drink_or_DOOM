#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FileHandleLib.h>
#include <Library/ShellCEntryLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>

EFI_STATUS
ReadFlagFromESP(EFI_HANDLE ImageHandle, UINT8* FlagValue) {
    EFI_STATUS Status;
    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    EFI_FILE_PROTOCOL* Root;
    EFI_FILE_PROTOCOL* File;
    UINTN BufferSize = 1;

    Status = gBS->HandleProtocol(
        ImageHandle,
        &gEfiLoadedImageProtocolGuid,
        (VOID**)&LoadedImage
    );
    if (EFI_ERROR(Status)) return Status;

    Status = gBS->HandleProtocol(
        LoadedImage->DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID**)&FileSystem
    );
    if (EFI_ERROR(Status)) return Status;

    Status = FileSystem->OpenVolume(FileSystem, &Root);
    if (EFI_ERROR(Status)) return Status;

    Status = Root->Open(
        Root,
        &File,
        L"\\EFI\\Boot\\flag.bin",
        EFI_FILE_MODE_READ,
        0
    );
    if (EFI_ERROR(Status)) return Status;

    Status = File->Read(File, &BufferSize, FlagValue);
    File->Close(File);

    return Status;
}

EFI_STATUS
ChainLoadNext(EFI_HANDLE ImageHandle, CHAR16* FilePath) {
    EFI_STATUS Status;
    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    EFI_FILE_PROTOCOL* Root;
    EFI_FILE_PROTOCOL* File;
    EFI_HANDLE NewImageHandle;
    EFI_DEVICE_PATH_PROTOCOL* DevicePath;

    Status = gBS->HandleProtocol(
        ImageHandle,
        &gEfiLoadedImageProtocolGuid,
        (VOID**)&LoadedImage
    );
    if (EFI_ERROR(Status)) return Status;

    Status = gBS->HandleProtocol(
        LoadedImage->DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID**)&FileSystem
    );
    if (EFI_ERROR(Status)) return Status;

    Status = FileSystem->OpenVolume(FileSystem, &Root);
    if (EFI_ERROR(Status)) return Status;

    Status = Root->Open(
        Root,
        &File,
        FilePath,
        EFI_FILE_MODE_READ,
        0
    );
    if (EFI_ERROR(Status)) return Status;

    DevicePath = FileDevicePath(LoadedImage->DeviceHandle, FilePath);
    if (DevicePath == NULL) return EFI_OUT_OF_RESOURCES;

    Status = gBS->LoadImage(FALSE, ImageHandle, DevicePath, NULL, 0, &NewImageHandle);
    if (EFI_ERROR(Status)) return Status;

    Status = gBS->StartImage(NewImageHandle, NULL, NULL);
    return Status;
}

EFI_STATUS
EFIAPI
efi_main(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS Status;
    UINT8 Flag = 1;

    // No InitializeLib() — UefiLib works without it now
    Print(L"[+] Bootmfw.efi started\n");

    Status = ReadFlagFromESP(ImageHandle, &Flag);
    if (EFI_ERROR(Status)) {
        Print(L"[!] Could not flag.bin: %r\n", Status);
    } else {
        Print(L"[+] Flag read: %d\n", Flag);
    }

    if (Flag == 1) {
        Print(L"[*] User didnt Respond Waiting ...\n");
        gBS->Stall(3600ULL * 1000ULL * 1000ULL); // 1 hour delay in µs
    } else {
        Print(L"[*] No response. Continuing immediately...\n");
    }

    Print(L"[+] Chaining to bootx64_real.efi...\n");
    Status = ChainLoadNext(ImageHandle, L"EFI\\Microsoft\\Boot\\bootmgfw_real.efi");
    if (EFI_ERROR(Status)) {
        Print(L"[!] Failed to chainload: %r\n", Status);
    }

    return Status;
}

