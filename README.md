# VoiceLite
## Description
We are aware of the demand for privacy-aware voice assistants that may be achieved by offline inference, i.e., a Homepod-like artificial assistant. We may picture a household scenario where we strive to provide ease of access for responsive service (inquiry, call for aid etc.). These are often powered by designed systems, and our challenge is to at least implement the functions on a general-purpose embedded system, which gives us an advantage of being a low power device with offline input inference in light of aforementioned privacy concerns, as well as portability for ease of installation.
\
This may be powered by a lightweight embedded system architecture, which is what we proposed, **VoiceLite**, a lightweight voice recognition system powered by **Jetson Nano** from Nvidia and a STM32 development kit **B-L475E-IOT01A**.
\
\
![](https://i.imgur.com/f3xBnkk.png)
\
\
**B-L475E-IOT01A:**
\
- Using on-board audio sensor (MP34DT01) to retrieve data
- Adapt Button Service to initiate recording of audio
- Transmitting data through socket connection with Jetson Nano for further processing
\
**Jetson Nano:**
\
## How to run 
1. Build a new program in Mbed Studio.
2. Import `mbed-dsp` (url: https://os.mbed.com/teams/mbed-official/code/mbed-dsp/) for FIR filter function and `BSP_B-L475E-IOT01` (url: https://os.mbed.com/teams/ST/code/BSP_B-L475E-IOT01/) driver library for the 3D accelerator sensors.
3. Replace the file under `~\mbed-dsp\cmsis_dsp\TransformFunctions\arm_bitreversal2.S` directory and `~\main.cpp`.
4. Build and compile the program.
5. Modify the parameters in the `visualize.py` for the visualization of the output.
