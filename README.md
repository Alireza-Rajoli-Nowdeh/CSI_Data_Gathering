# CSI_Data_Gathering

In this repository, we look at a variety of common solutions for CSI data gathering.

## Channel State Information (CSI)

Channel State Information (CSI) is a property of a communication link that attracts a lot of attention in recent years. CSI data shows how the signal propagates from transmitter to receiver and based on the propagation patterns, researchers achieve lots of information about the propagation environment. For example, the human body is one of the objects in the environment that can reflect the received signal (propagation). So, based on the difference of propagated data, we can decide which activities or gestures the user had done. You can learn more about this property from its [page on Wikipedia](https://en.wikipedia.org/wiki/Channel_state_information).

# Data Gathering

As you probably know, for gathering this data we need at least two devices: one transmitter and one receiver. The transmitter is the device that sends the Wi-Fi signal and the receiver is the device that receives the sent signal (we can use transceivers instead of each of them; transceivers are devices that can transmit and receive at the same time). These devices could be commercial devices like laptops, personal computers, mobile phones, routers, and so on, or any chipsets with a Wi-Fi antenna.

# Tools

There are many tools developed for gathering CSI data using different hardware. Each tool only works on some specific hardware, not all of them. Some popular ones:

1. **Intel 5300:** For using this tool you need to have the Intel 5300 NIC on at least one of your devices (transmitter or receiver). I didn't find a step-by-step installation guide for this tool.
2. **Atheros:** For using this tool, you should have an Atheros NIC on your device. The authors believe it works on all Atheros NICs, but only some of them are tested. There is a step-by-step installation guide for this tool.
3. **Wi-ESP:** This tool was presented in 2020. You don't need a specific network card on your laptop or PC, only at least one ESP32 chipset (around $15). Although there is a paper and a GitHub repo for this tool, I couldn't find a step-by-step installation guide. There is some code in their GitHub (udp_server, udp_client, and so on) but no detailed guideline on how it should be installed.
4. **ESP32 CSI Toolkit:** Like the previous one, you need at least one ESP32 chipset. This toolkit contains a detailed installation guide and is easy to use.

| Tool | Hardware | Step-by-step guide |
| --- | --- | --- |
| Intel 5300 | Device with specific NIC | ✗ |
| Atheros | Device with specific NIC | ✓ |
| Wi-ESP | ESP32 microcontroller | ✗ |
| ESP32 CSI Toolkit | ESP32 microcontroller | ✓ |

![ESP32 Chipset](ESP32%20Chipset.jpg)

I installed three of them (all except Intel 5300) and only got results with the last one. I used its `active_ap` project to create an access point with the ESP32. After configuring the project with `idf.py menuconfig`, I flashed it to the ESP32 with `idf.py flash` and watched the output with `idf.py monitor`. The ESP32 creates an access point (AP) with the preset SSID and password. When another device (e.g. a mobile phone) connects to it, CSI data appears on the AP console and keeps arriving for as long as the device stays connected. You can then save this data to a file and do your analysis (machine learning, etc.) on it.

![CSI Data](CSI%20Data.png)

# ESP32 CSI Toolkit

This section is an updated, step-by-step guide for setting up the ESP32 CSI Toolkit on Windows, including the problems I ran into and how I fixed them. It builds on the guidance in [MajidGhosian/ESP32-CSI-Tool](https://github.com/MajidGhosian/ESP32-CSI-Tool) and [MajidGhosian/CSI_Data_Gathering](https://github.com/MajidGhosian/CSI_Data_Gathering), with valuable help from my colleagues Majid, Gowthami, and Nimasha.

> **Note:** The original instructions used `make menuconfig` and `make flash monitor`. This project does **not** support the `make` workflow; use `idf.py` as described below.

## Step 1: Download and Install ESP

1. Download the ESP toolchain zip file from the [ESP-IDF v3.3.1 Windows setup page](https://docs.espressif.com/projects/esp-idf/en/v3.3.1/get-started/windows-setup.html).
2. Create a folder named `esp` on the C: drive (`C:\esp`), and inside it create another folder named `esp` (`C:\esp\esp`).
3. Extract the downloaded zip file into `C:\esp\esp`.

## Step 2: Install ESP-IDF v3.3.1

1. Open Command Prompt or the MinGW32 terminal and clone ESP-IDF v3.3.1:

   ```bash
   cd C:\esp
   git clone -b v3.3.1 --recursive https://github.com/espressif/esp-idf.git
   ```

2. Confirm that the repository was cloned into `C:\esp\esp-idf`.

## Step 3: Download the ESP32 CSI Toolkit

1. Download the toolkit from the [ESP32 CSI Tool page](https://stevenmhernandez.github.io/ESP32-CSI-Tool/).
2. Extract the zip file into the `examples` folder of ESP-IDF:

   ```
   C:\esp\esp-idf\examples\ESP32-CSI-Tool-master
   ```

> **Important:** Do **not** extract it into `C:\esp\esp`. That causes errors in the next step.

## Step 4: Set Up the Build Environment

Before configuring the project, make sure your environment has the right Python, CMake, and `idf.py` available. These were the issues I hit, in order.

### 4.1 Python version (Python 2.7 → Python 3)

**Problem:** The default Python in MinGW32 was Python 2.7, which isn't compatible with `idf.py`.

**Fix:**

1. Install Python 3 (I used Python 3.13.1).
2. Install CMake **v3.13** from [cmake.org/files/v3.13](https://cmake.org/files/v3.13/). Newer versions don't work with this setup.
3. Add Python, CMake, and the ESP-IDF tools folder to your `PATH` by adding this line to `~/.bashrc` (one combined line so you don't overwrite other paths; adjust the Python path to match your username/install location):

   ```bash
   export PATH=/c/Users/Lenovo/Python:/c/Program\ Files/CMake/bin:/c/esp/esp-idf/tools:$PATH
   ```

4. Apply the changes and verify:

   ```bash
   source ~/.bashrc
   python --version
   cmake --version
   ```

5. Install the ESP-IDF Python dependencies:

   ```bash
   python -m pip install -r /c/esp/esp-idf/requirements.txt
   ```

### 4.2 `idf.py` command not found

**Problem:** `idf.py` wasn't recognized on the command line.

**Fix:** Make sure `/c/esp/esp-idf/tools` is on your `PATH` (see 4.1), or run it with its full path. Check that it works:

```bash
python /c/esp/esp-idf/tools/idf.py --version
```

## Step 5: Connect the ESP32 and Configure the Project

1. Connect the ESP32 board to your computer with a micro USB to USB cable.
2. Open **Device Manager** and find the COM port assigned to the ESP32 (e.g. `COM3`).
3. Open `mingw32.exe` from `C:\esp\esp\msys32` and go to the `active_ap` project:

   ```bash
   cd /c/esp/esp-idf/examples/ESP32-CSI-Tool-master/active_ap
   ```

4. Open the configuration menu:

   ```bash
   python /c/esp/esp-idf/tools/idf.py menuconfig
   ```

5. Configure the project settings as described in the [ESP-IDF v3.3.1 configuration docs](https://docs.espressif.com/projects/esp-idf/en/v3.3.1/get-started/index.html#configure) (serial port, SSID, password, etc.).

### 5.1 Fix: `idf_component_register` not recognized

**Problem:** The `idf_component_register` command in `CMakeLists.txt` caused an error when running `idf.py menuconfig`.

**Fix:**

1. Replace the `CMakeLists.txt` in the project's `main` folder with the corrected version (provided by Nimasha and Gowthami), available [here](https://drive.google.com/drive/folders/1NXOC2-Kxnw_jHQsJQr0jrXp51Le9iY4I?usp=drive_link). The key change is the include path:

   ```cmake
   # Original
   set(INCLUDE_DIRS "." "../_components")

   # Updated
   set(INCLUDE_DIRS "." "../../_components")
   ```

2. Clean and reconfigure:

   ```bash
   rm -rf build
   python /c/esp/esp-idf/tools/idf.py menuconfig
   ```

### 5.2 Enable CSI in menuconfig (don't skip this)

In `menuconfig`, go to:

```
Component config > Wi-Fi > CSI
```

and press Enter to enable it. If CSI is not enabled here, you won't be able to collect CSI data.

## Step 6: Flash and Monitor

1. Flash the firmware to the ESP32:

   ```bash
   python /c/esp/esp-idf/tools/idf.py flash
   ```

2. Start the serial monitor:

   ```bash
   python /c/esp/esp-idf/tools/idf.py monitor
   ```

3. With another device (laptop, smartphone, etc.), connect to the access point configured in the project (default SSID: `myssid`; the password is the one you set in menuconfig).
4. CSI data should start appearing on the console, roughly once per second.

## Troubleshooting

### Wi-Fi network appears only intermittently / can't connect

**Problem:** After flashing, the AP appeared only briefly every 1–2 minutes and devices couldn't connect to it (the board connected fine before flashing).

**Suspected cause:** The `main.cc` file or the menuconfig settings.

**What I tried:** Updated `main.cc` to work with ESP-IDF v3.3.1, but the behavior persisted. The `main.cc` I used is available [here](https://drive.google.com/drive/folders/1NXOC2-Kxnw_jHQsJQr0jrXp51Le9iY4I). If you've hit and solved this issue, please open an issue or PR.

### Wi-Fi network not visible on some devices

**Problem:** The AP wasn't visible on some devices.

**Fix:** Use the updated `csi_component` file available [here](https://drive.google.com/drive/folders/1NXOC2-Kxnw_jHQsJQr0jrXp51Le9iY4I). Also double-check that CSI is enabled under `Component config > Wi-Fi > CSI`.

## Summary

Setting up the ESP32 CSI Toolkit required working through Python version mismatches, a CMake version incompatibility, `idf.py` path issues, and a `CMakeLists.txt` path fix. With those resolved and CSI enabled in menuconfig, the `active_ap` project builds, flashes, and streams CSI data from connected devices.
