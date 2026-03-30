Pong for M5Core2 with separate server and client roles.

How to use:
1. Open this folder in PlatformIO.
2. Flash one M5Core2 with env: m5stack-core2-server
3. Flash the other M5Core2 with env: m5stack-core2-client
4. Open serial monitor at 115200 if you want BLE debug messages.

Controls on both devices:
- Touch upper half of the screen or hold BtnA to move up
- Touch lower half of the screen or hold BtnB to move down
- Touch screen or press BtnC to start / ready / continue
- BtnC also arms the speed boost ability

Role split:
- Server/Host owns the real game state, ball physics, scoring, and winner logic.
- Client sends local paddle input to the server.
- Server sends synced game state back to the client over BLE.


WiFi setup:
- Open src/WifiManager.cpp
- Replace PUT_YOUR_WIFI_NAME_HERE and PUT_YOUR_WIFI_PASSWORD_HERE

Bluetooth names:
- Host/server advertises as M5-Pong-Server
- Client uses BLE name M5-Pong-Client locally and scans for the Pong service
