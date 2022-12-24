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
- Using on-board audio sensor (MP34DT01) to retrieve data
- Adapt Button Service to initiate recording of audio
- Transmitting data through socket connection with Jetson Nano for further processing

**Jetson Nano:**
- Converting raw string to .wav files
- Post-processing of audio input
    - Keyword spotting (KS), Automatic Speech Recognition (ASR)
- Successful detection of keyword triggers an email notification


## How to run 
Git clone this repo into your environment, build and compile.
### B-L475E-IOT01A
- Modified the parameters in `~\mbed_app.json`.
    - Change `config["hostname"]["value"]` to your host IP (you can get it using `ipconfig` in local **Jetson Nano** cmd) in `~\mbed_app.json`.
    - Change `target_overrides["*"]["nsapi.default-wifi-ssid"]` and `target_overrides["*"]["nsapi.default-wifi-password"]` to your local wifi network same as your local **Jetson Nano**.
- After initialization, press the blue `USER` button to record for about 2 seconds.

### Jetson Nano
- Use the command `docker/run.sh` to initialize the environment.
- Inside docker:
    - Keyword spotting (KS) with email notification:
        - `python3 data/hello.py --model matchboxnet --wav [processed_wav]`
    - Automatic Speech Recognition (ASR):
        - `python3 data/hello.py --wav [processed_wav]`

## Demo
Youtube: https://www.youtube.com/watch?v=jkTV97ffpUw
