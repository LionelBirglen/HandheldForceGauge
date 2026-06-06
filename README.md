# Handheld Force Gauge
CAD and program for a custom handheld force gauge. User interface allows for changing units, zeroing display, recording min/max values. Bluetooth broadcasting of the displayed values and local Python script for realtime display and recording.

## Instructions
1. 3D print ForceGauge_BasePlate.stp, Color change for the text layer is suggested
2. purchase all components, see BOM below
3. solder protoboard according to ForceGauge_Schematic.pdf
4. assemble components on protoboard
5. upload and run ForceGauge_Program.ino on XIAO module
6. use the sensor!
7. if bluetooth communication is desired run ForceGauge_Python_BLE_Logger.py from your local machine, a window will open displaying the values and recording them.
Note: local recording of the file will overwrite previous file if it exists, rename your files if you want to keep them! 

## Bill of Materials (BOM)

| Name | # | Description 
|---|---|---|
| XIAO nrf52840 | 1 | https://www.seeedstudio.com/Seeed-XIAO-BLE-nRF52840-p-5201.html |
| `xarm6_fk.py` / `.m` | 1 |  Forward kinematics → flange + TCP pose in all conventions |
| `xarm6_ik.py` / `.m` | 1 |  Inverse kinematics — accepts T, RPY, quaternion, or axis-angle |
| `xarm6_plot.py` / `.m` | 1 |  3-D visualiser with optional TCP trajectory |
| `xarm6_kinematics_demo.py` / `.m` | 1 |  FK/IK validation + Monte-Carlo accuracy test |
| `xarm6_plot_demo.py` / `.m` | 1 |  Static pose viewer, IK verification, and animation |
| `xarm6_reload.py` | 1 |  Reload all modules after editing `xarm6_params` (Python only) |

by Lionel Birglen  <br />
Polytechnique Montreal, 2026
License: GNU GPL v3

###  DISCLAIMER:
This hardware and software project is provided "as is", and at your own risk. Under no circumstances shall any author be liable for direct, indirect, special, incidental, or consequential damages resulting from the use, misuse, or inability to use this hardware/software, even if the authors have been advised of the possibility of such damages.
