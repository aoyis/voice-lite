# VoiceLite
## Description
This is a prototype for a HomePod-like voice assistant aimed for low power, lightweight ML inference. This device is built up with two main development kit - **B-L475E-IOT01A** from STM and **Jetson Nano** from Nvidia, acting as audio input and inference device respectively.
</br>
**B-L475E-IOT01A: **
**Jetson Nano: **

## How to run 
1. Build a new program in Mbed Studio.
2. Import `mbed-dsp` (url: https://os.mbed.com/teams/mbed-official/code/mbed-dsp/) for FIR filter function and `BSP_B-L475E-IOT01` (url: https://os.mbed.com/teams/ST/code/BSP_B-L475E-IOT01/) driver library for the 3D accelerator sensors.
3. Replace the file under `~\mbed-dsp\cmsis_dsp\TransformFunctions\arm_bitreversal2.S` directory and `~\main.cpp`.
4. Build and compile the program.
5. Modify the parameters in the `visualize.py` for the visualization of the output.
