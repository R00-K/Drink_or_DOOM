# Drink or DOOM ⚠️🍺💀

> A totally over-engineered reminder system, built with **C++** and **EDK II**, that nags you to drink water… or else.

---

## 📜 Project Description
This project has **two main components**:
At its core, Drink or DOOM is a gloriously over-engineered solution to a problem nobody really needed solved — reminding you to drink water. Sure, there are plenty of apps and timers that do this quietly and politely. But where’s the fun in that?

Instead of a gentle nudge, this project goes full drama mode: it actually messes with your computer’s boot process (in the tested, controlled environment, of course). If you ignore your hydration reminder, your machine gives you a digital guilt trip every time you reboot.

It’s a perfect blend of high-tech wizardry, harmless sabotage, and cheeky motivation. So while it’s technically useful — it’s mostly useless because nobody wants their bootloader replaced over a glass of water. But hey, if you do, here’s your chance!


1. **Custom UEFI App** – Built using the [EDK II](https://github.com/tianocore/edk2) environment.  
   - When run at boot, it checks a `flag.bin` file in the EFI System Partition (ESP) and reacts accordingly.  
   - Built as a standalone `.efi` binary for x86_64 systems.

2. **C++ Reminder App** – A Windows application that:  
   - Shows a timed reminder to the user.  
   - Records the response in `flag.bin` (0 = responded, 1 = ignored).  
   - (Safe mode) Writes to a **test ESP image** instead of modifying the real bootloader.

---

## 🚀 Features
- **Timed reminders** with keyboard detection.
- **Flag system** to communicate between the OS and EFI app.
- **EDK II UEFI app** that reads the flag and acts accordingly.
- **Safe-mode injector** to work with test ESP images.
- **Cross-environment development** – C++ for user-space, C for UEFI.

---

## 🛠️ Technical Details

### Software
- **Languages:**  
  - C++17 (Windows reminder app)  
  - C (UEFI app in EDK II)
- **Frameworks/Libraries:**  
  - Standard C++ STL  
  - Win32 API (keyboard input, file paths)  
  - EDK II BaseLib & UefiLib
- **Tools:**  
  - MinGW-w64 or MSVC (Windows app)  
  - EDK II build environment (UEFI app)  
  - QEMU + OVMF (safe testing)

### Hardware
- No special hardware required for safe mode.  
- For EFI testing:  
  - Virtual machine with UEFI firmware (OVMF)  
  - Optional: test USB or ESP disk image

---

## 📦 Installation

### Clone the Repository

git clone https://github.com/yourusername/Drink_or_DOOM.git
cd Drink_or_DOOM


# Project Demo

📽 **Video Demonstration:** Watch the complete step-by-step process in action here: [View Demo Video](https://drive.google.com/file/d/1-MPs29uwkYqv5LZR6AaqW7qhrY-8b3FB/view?usp=drive_link)





