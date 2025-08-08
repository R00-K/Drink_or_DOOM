#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <windows.h>

using namespace std;
namespace fs = std::filesystem;

// Hardcoded path to your custom EFI file
const string CUSTOM_EFI_PATH = "C:\\Users\\Godwin\\vboxuser\\Useless\\bootmgfw.efi"; 
const string ESP_MOUNT = "Z:\\"; // EFI partition will be mounted here

bool mountESP() {
    cout << "[*] Mounting EFI partition...\n";
    string cmd = "mountvol " + ESP_MOUNT + " /S";
    int result = system(cmd.c_str());
    return (result == 0);
}

bool getUserResponse(int timeoutSec) {
    cout << "[?] Press 'y' to respond positively within " << timeoutSec << " seconds...\n";

    auto start = chrono::steady_clock::now();
    while (chrono::steady_clock::now() - start < chrono::seconds(timeoutSec)) {
        if (GetAsyncKeyState('Y') & 0x8000) {
            return true;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    return false;
}

void writeFlag(bool responded) {
    string flagPath = ESP_MOUNT + string("EFI\\Boot\\flag.bin");
    ofstream flagFile(flagPath, ios::binary);
    if (!flagFile) {
        cerr << "[-] Failed to write flag file at " << flagPath << "\n";
        return;
    }
    char value = responded ? 0 : 1;
    flagFile.write(&value, 1);
    flagFile.close();
    cout << "[+] Flag file written: " << (int)value << "\n";
}

bool replaceBootloader() {
    string originalBoot = ESP_MOUNT + string("EFI\\Microsoft\\Boot\\bootmgfw.efi");
    string backupBoot   = ESP_MOUNT + string("EFI\\Microsoft\\Boot\\bootmgfw_real.efi");

    try {
        if (fs::exists(originalBoot)) {
            fs::rename(originalBoot, backupBoot);
            cout << "[+] Original bootloader renamed to bootmgfw_real.efi\n";
        } else {
            cerr << "[-] Original bootloader not found!\n";
            return false;
        }

        fs::copy(CUSTOM_EFI_PATH, originalBoot, fs::copy_options::overwrite_existing);
        cout << "[+] Custom EFI copied as bootmgfw.efi\n";
        return true;
    } catch (const fs::filesystem_error& e) {
        cerr << "[-] Filesystem error: " << e.what() << "\n";
        return false;
    }
}

int main() {
    int seconds;

    cout << "[*] Enter reminder time in seconds: ";
    cin >> seconds;

    if (!mountESP()) {
        cerr << "[-] Failed to mount EFI partition. Run as Administrator.\n";
        return 1;
    }

    cout << "[+] EFI Partition mounted successfully.\n";
    cout << "[*] Waiting for reminder...\n";
    this_thread::sleep_for(chrono::seconds(seconds));

    if (getUserResponse(10)) {
        cout << "[+] Positive response! Setting flag = 0 and extending for 1 hour...\n";
        writeFlag(true);
        this_thread::sleep_for(chrono::hours(1));
    } else {
        cout << "[!] No response! Setting flag = 1 and replacing bootloader...\n";
        writeFlag(false);
        if (!replaceBootloader()) {
            cerr << "[-] Failed to replace bootloader!\n";
        }
    }

    return 0;
}
