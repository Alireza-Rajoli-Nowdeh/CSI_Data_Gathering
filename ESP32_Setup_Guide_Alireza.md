## 📘 Introduction

This guide provides a detailed and corrected step-by-step procedure to set up the **ESP32 CSI Toolkit**, based on testing and documentation performed by myself and my research colleagues. It improves upon earlier instructions by resolving critical issues like:

- Python 2/3 conflicts
- Missing/incorrect CMake
- Use of outdated `make` commands
- Misconfigured paths in `CMakeLists.txt`
- How to enable CSI capture and fix device visibility issues

We use ESP-IDF v3.3.1 as required by the toolkit.
## ⚙️ Step 1: Download and Install ESP Toolkit

1. Download from the official ESP32 [Windows setup guide](https://docs.espressif.com/projects/esp-idf/en/v3.3.1/get-started/windows-setup.html)  
2. Create these folders:

C:\esp

C:\esp\esp

3. Extract the setup ZIP into:
C:\esp\esp


## 🧱 Step 2: Install ESP-IDF v3.3.1

Open `mingw32.exe` terminal (from `C:\esp\esp\msys32`) and run:

```bash
cd C:\esp
git clone -b v3.3.1 --recursive https://github.com/espressif/esp-idf.git
```
This creates: C:\esp\esp-idf

## 🧰Step 3: Download & Place ESP32 CSI Toolkit
Get the toolkit ZIP from: ESP32 CSI Tool Official Site [Link](https://stevenmhernandez.github.io/ESP32-CSI-Tool/)

Extract it to:
```bash
C:\esp\esp-idf\examples\ESP32-CSI-Tool-master
```
⚠️ Do NOT extract into C:\esp\esp — it will cause build errors.

## 🔌 Step 4: Connect and Configure Your ESP32
Use a micro USB to USB cable

Find the COM port using Device Manager (e.g., COM3)

Open mingw32.exe

Navigate to the toolkit folder:
```bash
cd /c/esp/esp-idf/examples/ESP32-CSI-Tool-master/active_ap
```
## 🧪 Step 5: Run Configuration Menu:
Open the configuration menu using:
```bash
python /c/esp/esp-idf/tools/idf.py menuconfig
```
Configure the project settings as per [ESP-IDF v3.3.1 documentation](https://docs.espressif.com/projects/esp-idf/en/v3.3.1/get-started/index.html#configure)
❌ Issue: make menuconfig no longer supported
✅ Use:
```bash
python /c/esp/esp-idf/tools/idf.py menuconfig
```

## important:
During this step, I faced multiple challenges, which I resolved through several steps. Below are the challenges and
their corresponding solutions:

## 🛠 Step 6: Fixes and Workarounds
## 🐍 Issue 1: Python Version (default is 2.7)
✅ Solution:

Install Python 3.13

Update .bashrc:
```bash
export PATH=/c/Users/Lenovo/Python:/c/Program\ Files/CMake/bin:$PATH
source ~/.bashrc
```
Verified the Python version:
```bash
python --version
```
Install dependencies:
```bash
python -m pip install -r /c/esp/esp-idf/requirements.txt
```
## 🧱 Issue 2: CMake Version Incorrect
✅ Solution:

Install CMake v3.13 [link](https://cmake.org/files/v3.13/)

Add it to PATH as above

Confirm:
```bash
cmake --version
```
## 🧩 Issue 3: idf_component_register not recognized
The idf_component_register command in the CMakeLists.txt file caused an error during the configuration step
with idf.py menuconfig.
✅ Fix:

In CMakeLists.txt, update this line:

```bash
set(INCLUDE_DIRS "." "../_components")
```
To:
```bash
set(INCLUDE_DIRS "." "../../_components")
```

📁 Corrected file is available in Repo named "CMakeLists.txt"
Replaced the content of the CMakeLists.txt file with the corrected version
Then re-run:
```bash
rm -rf build
python /c/esp/esp-idf/tools/idf.py menuconfig
```

After opening the menuconfig, navigate to:


✅ Enable CSI in the config:

Component Config → Wi-Fi → Enable CSI

## 📡 Step 7: Flash and Monitor the ESP32
```bash
python /c/esp/esp-idf/tools/idf.py flash
python /c/esp/esp-idf/tools/idf.py monitor
```
Connect a second device (like a phone) to the ESP32 AP (SSID was set in config).

CSI data will appear in the terminal console.

## ❗ Common Final Issues
⚠️ Problem: AP appears briefly then disappears
Likely caused by outdated or broken "main.c"

Use the updated main.c from the version taht exist in this Repo

⚠️ Problem: Device can’t find the AP
If you face this problem, try using the updated "csi_component" file available in this Repo

With these updates and proper configurations, ensure you enable the CSI option in menuconfig under Component
Config > Wi-Fi > CSI. Missing this step will prevent you from collecting CSI data effectively.

## 📸 CSI Data Sample
Here is an actual sample screenshot of CSI data received on the ESP32 console:

## ✅ Conclusion
This guide simplifies and fixes the ESP32 CSI setup process. It resolves all major obstacles I faced and improves on the original documentation provided in the CSI_Data_Gathering repository.

This work was conducted as part of my research in wireless sensing, with thanks to my supportive colleagues and the original repo authors.
