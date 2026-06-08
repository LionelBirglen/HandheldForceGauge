# Handheld Force Gauge
CAD and programs for a custom handheld force gauge. User interface allows for changing units (N/kg), setting tare (zeroing), recording min/max values, and switching screen orientation. Onscreen values are broadcasted with Bluetooth and a local Python script allows for realtime display and data recording into a csv file.

![Force Gauge Video](ForceGauge_shortvideo.gif)

## Instructions
1. 3D print ForceGauge_BasePlate.stp, Color change for the text layer is suggested
2. purchase all components, see BOM below
3. solder protoboard according to ForceGauge_Schematic.pdf
4. assemble components on protoboard
5. upload and run ForceGauge_Program.ino on XIAO module (compile w/ mbed-enabled)
6. plug the usb-c connector to a suitable power supply
7. use the sensor! Tactile switches have two functions each, selectable with short or long press of the button
8. if bluetooth communication is desired run ForceGauge_Python_BLE_Logger.py from your local machine where Python is installed, a window will open displaying the values read by the sensor and the program will record them in a csv file with timestamps.<br/>
**Notes:**
- local recording will overwrite the previous file if it exists, **rename your files if you want to keep them!**
- power can be provided by a standard phone/laptop usb-c charger or a power bank for maximal portability. Careful though, certain power banks will shut down after a short while because not enough current is pulled. Use a powerbank capable of trickle charging.

## Bill of Materials (BOM)

| Name | # | Link/Description 
|---|---|---|
| XIAO nrf52840 | 1 | [SeeedStudio](https://www.seeedstudio.com/Seeed-XIAO-BLE-nRF52840-p-5201.html) |
| Qwiic Scale NAU7802 | 1 |  [Sparkfun](https://www.sparkfun.com/sparkfun-qwiic-scale-nau7802.html) |
| 0.96in OLED Display | 1 |  [Amazon kit](https://www.amazon.com/ELEGOO-Display-Compact-Self-Luminous-Projects/dp/B0D2RMQQHR) |
| Protoboard | 1 |  2.54mm pitch [Amazon kit](https://www.amazon.com/ELEGOO-Prototype-Soldering-Compatible-Arduino/dp/B072Z7Y19F) |
| Tactile switches | 2 | [Amazon kit](https://www.amazon.com/QTEATAK-Momentary-Tactile-Button-Switch/dp/B07VSNN9S2)  |
| 3k3 resistors | 2 |  any value between 1k and 10k at 1/4W should be fine, through hole |
| Load Cell | 1 |  [Robotshop](https://www.robotshop.com/products/type-s-load-cell-100-kg) or [Amazon](https://www.amazon.com/Portable-High-Precision-Pressure-Tension-Weighing/dp/B077YHNNCP) |
| M6x12 screw w/ washer| 1 |  [Amazon kit](https://www.amazon.com/MklusiveTech-141Pcs-Screw-Kit-Electronics/dp/B0DYF25FF6) |
| CHBLFSN8 mounting bracket | 1 | Baked-on finish (Silver) 43x30 footprint with 8mm holes [Misumi](https://us.misumi-ec.com/vona2/detail/110300449520/) |
| M6 hook | 1 | [Amazon](https://www.amazon.com/Faspiroty-Stainless-Question-Hanging-Connecting/dp/B0DKNBMYKB) (optional)  |
| Base plate | 1 |  3D printed from STEP file |
| Braces | 2 |  3D printed from ForceGauge_Brace_QwiicScale.step and ForceGauge_Brace_OLEDDisplay.step |
| Self-tapping screws | 16 | for plastics, [Amazon kit](https://www.amazon.com/Zmbroll-Self-Tapping-Phillips-Sheetmetal-Electronic/dp/B0F62XS2YC)  |
| Female headers | 4 | need 1x4, 1x6, 2x7 all with 2.54mm pitch, [Amazon kit](https://www.amazon.com/Ruibapa-Connector-Assortment-Straight-P-037/dp/B0B96WXT46) |

**Notes:** 
- if you use another XIAO microcontroller the Arduino program might need to be updated especially if the one you pick has no Bluetooth capability
- the CHBLFSN8 mounting bracket used here is meant for 4040 extrusions but other mounting brackets for either 4040 or 3030 profiles could also be used and might be easier/cheaper to find than Misumi's. Adjust the size of the rectangular hole in the base plate STP file accordingly. File down the tabs of the bracket you used if there are any.
- check the components you have laying around before buying the kits listed in the BOM, most components here such as tactile switches, resistors, M6 screws, etc. do not need to be the exact model from the BOM, if you have already some parts at hand you can probably use them.

by Lionel Birglen  <br />
Polytechnique Montreal, 2026
License: GNU GPL v3

###  DISCLAIMER:
This hardware and software project is provided "as is", and at your own risk. Under no circumstances shall any author be liable for direct, indirect, special, incidental, or consequential damages resulting from the use, misuse, or inability to use this hardware/software, even if the authors have been advised of the possibility of such damages.
