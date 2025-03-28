# ESP32 WiFi LCD Display with Temperature Sensor and LED

## Project Overview
This project demonstrates how to connect an ESP32 to a Wi-Fi network, retrieve data from an API using HTTPS requests, and display relevant information on an I2C LCD screen. Additionally, it integrates a DS18B20 temperature sensor to read and send temperature data to an API.

## Features
- **Wi-Fi Connectivity:** Connects to a network, including support for Wokwi simulation.
- **Secure API Requests:** Uses `WiFiClientSecure` for HTTPS communication.
- **LCD Display:** Displays messages received from the API and sensor data.
- **Temperature Monitoring:** Reads temperature from a DS18B20 sensor and uploads it to an API.
- **LED Control via API:** Retrieves LED state from the API and controls an LED accordingly.
- **Error Handling:** Handles Wi-Fi disconnection, HTTP errors, and JSON parsing issues.

## Code Breakdown
### `setup()`
Initializes hardware components and establishes a Wi-Fi connection.
- Starts Serial Communication at 115200 baud.
- Initializes the LCD and displays "Connecting...".
- Initializes the LED and DS18B20 sensor.
- Connects to Wi-Fi:
  - Uses `WiFi.begin(SSID, PASS, CHANNEL);` for Wokwi.
  - Retries connection up to 20 times before failing.
  - Displays "WiFi Connected!" upon success or "WiFi Failed!" if unsuccessful.
- Configures `WiFiClientSecure` for HTTPS requests.

### `loop()`
Handles API interactions and updates the LCD.
1. **Checks Wi-Fi Status**
   - If disconnected, attempts reconnection.

2. **Retrieves LED State from API** (`GET /api/light`)
   - Parses the JSON response to extract LED status.
   - Updates the LED state based on the API response.
   - Displays the LED state on the LCD.
   - Handles HTTP and JSON parsing errors.

3. **Sends Temperature Data to API** (`PUT /api/temp`)
   - Reads temperature from the DS18B20 sensor.
   - Sends temperature data in JSON format to the API.
   - Displays the temperature and API response message on the LCD.
   - Handles HTTP and JSON parsing errors.

4. **Waits for 5 Seconds** before repeating the cycle.

## Expected Serial Monitor Output:

ets Jul 29 2019 12:21:46

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1156
load:0x40078000,len:11456
ho 0 tail 12 room 4
load:0x40080400,len:2972
entry 0x400805dc
Connecting to WiFi..
WiFi connected. IP: 10.10.0.2
Sending GET request to /api/light...
Response: {"light":true}
Sending PUT request to /api/temp...
Response: {"message":"Temperature state updated"}
Sending GET request to /api/light...
Response: {"light":false}
Sending PUT request to /api/temp...
Response: {"message":"Temperature state updated"}
Sending GET request to /api/light...
Response: {"light":true}
Sending PUT request to /api/temp...
Response: {"message":"Temperature state updated"}
Sending GET request to /api/light...
Response: {"light":true}
Sending PUT request to /api/temp...
Response: {"message":"Temperature state updated"}
Sending GET request to /api/light...
Response: {"light":true}
Sending PUT request to /api/temp...
Response: {"message":"Temperature state updated"}
Sending GET request to /api/light...
Response: {"light":true}
Sending PUT request to /api/temp...
Response: {"message":"Temperature state updated"}
Sending GET request to /api/light...

<img width="431" alt="image" src="https://github.com/user-attachments/assets/d0b88338-b3f1-48a7-b889-3685f9b9f27c" />
<img width="275" alt="image" src="https://github.com/user-attachments/assets/0bce0be9-dec1-46f5-9012-de7cf8c27e38" />


## How to Use
1. **Update `env.h` with your Wi-Fi credentials and API Key:**
    ```cpp
    #define SSID "your_wifi_ssid"
    #define PASS "your_wifi_password"
    #define API_KEY "your_api_key"
    #define ENDPOINT "https://your-api-endpoint.com"
    ```
2. **Upload the code to an ESP32.**
3. **Ensure the ESP32 is connected to Wi-Fi.**
4. **Check the LCD display for messages retrieved from the API.**

## Additional Notes
- The ESP32 uses `client.setInsecure();` to disable SSL verification (for testing purposes).
- The LED state is controlled via the API response (`true` for ON, `false` for OFF).
- The project can be simulated using Wokwi by enabling `#define IS_WOKWI true`.


