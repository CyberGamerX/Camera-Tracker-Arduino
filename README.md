# Camera-Tracker-Arduino
Arduino position and rotation tracker and transmitter.

This project works in parallel with Camera Tracker Unity to track and virtualize the position of a camera in 3D space using a custom UDP packet. The project has the file *arduino_secrets.h* ommitted to protect wifi passwords. This file must be added to use this project.

### arduino_secrets.h

>#define SECRETS_H
>
>#define WIFI_SSID "*Network Name*"
>
>#define WIFI_PASS "*Network Password*"
>
>#define Gravity 9.80665f

## Base Project
[Camera Tracker Unity](https://github.com/CyberGamerX/Camera-Tracker-Unity)