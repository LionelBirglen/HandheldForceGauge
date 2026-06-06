# Handheld Force Gauge
CAD and program for a custom handheld force gauge. User interface allows for changing units, zeroing display, recording min/max values. Bluetooth broadcasting of the displayed values and local Python script for realtime display and recording.

## Instructions
1. 3D print ForceGauge_BasePlate.stp, Color change for the text layer is suggested
2. purchase all components, see BOM below
3. solder protoboard according to ForceGauge_Schematic.pdf
4. assemble components on protoboard
5. upload and run ForceGauge_Program.ino on XIAO module
6. use the sensor!
7. if bluetooth communication is desired run ForceGauge_Python_BLE_Logger.py from your local machine where Python is installed, a window will open displaying the values and recording them.<br/>
**Note:** local recording will overwrite the previous file if it exists, rename your files if you want to keep them! 

## Bill of Materials (BOM)

| Name | # | Link/Description 
|---|---|---|
| XIAO nrf52840 | 1 | [SeeedStudio](https://www.seeedstudio.com/Seeed-XIAO-BLE-nRF52840-p-5201.html) |
| Qwiic Scale NAU7802 | 1 |  [Sparkfun](https://www.sparkfun.com/sparkfun-qwiic-scale-nau7802.html) |
| 0.96in OLED Display | 1 |  [Amazon kit](https://www.amazon.com/ELEGOO-Display-Compact-Self-Luminous-Projects/dp/B0D2RMQQHR) |
| Protoboard | 1 |  2.54mm pitch [Amazon kit](https://www.amazon.com/ELEGOO-Prototype-Soldering-Compatible-Arduino/dp/B072Z7Y19F) |
| Tactile switch | 2 | [Amazon kit](https://www.amazon.com/QTEATAK-Momentary-Tactile-Button-Switch/dp/B07VSNN9S2)  |
| Resistor | 2 |  any value between 1k and 10k at 1/4W should be fine, through hole |
| Load Cell | 1 |  [Robotshop](https://www.robotshop.com/products/type-s-load-cell-100-kg) or [Amazon](https://www.amazon.com/Portable-High-Precision-Pressure-Tension-Weighing/dp/B077YHNNCP) |
| M6x12 screw w/ washer| 1 |  [Amazon kit](https://www.amazon.com/MklusiveTech-141Pcs-Screw-Kit-Electronics/dp/B0DYF25FF6) |
| Mounting bracket | 1 |   |
| M6 hook | 1 | [Amazon](https://www.amazon.com/Faspiroty-Stainless-Question-Hanging-Connecting/dp/B0DKNBMYKB) (optional)  |
| Base plate | 1 |  3D printed from STEP file |

by Lionel Birglen  <br />
Polytechnique Montreal, 2026
License: GNU GPL v3

###  DISCLAIMER:
This hardware and software project is provided "as is", and at your own risk. Under no circumstances shall any author be liable for direct, indirect, special, incidental, or consequential damages resulting from the use, misuse, or inability to use this hardware/software, even if the authors have been advised of the possibility of such damages.
