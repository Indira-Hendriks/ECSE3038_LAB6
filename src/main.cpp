#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>  // Secure client for HTTPS
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>  // LCD Library
#include <OneWire.h>
#include <DallasTemperature.h>
#include "env.h"                // Load WiFi credentials & API info

// Define if running on Wokwi
#define IS_WOKWI true

// Initialize the LCD (I2C address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClientSecure client;  // Secure client for HTTPS requests


// Setup a oneWire instance to communicate with the DS18B20 sensor
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
    Serial.begin(115200);

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Connecting...");

    // Initialize LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize DS18B20 sensor
    sensors.begin();

    // Connect to Wi-Fi with proper method for Wokwi
    if (IS_WOKWI) 
        WiFi.begin(SSID, PASS, CHANNEL);  // Channel 6 for Wokwi
    else 
        WiFi.begin(SSID, PASS);

    Serial.print("Connecting to WiFi");
    
    int retries = 20;  // Timeout after ~10 seconds
    while (WiFi.status() != WL_CONNECTED && retries > 0) {
        delay(500);
        Serial.print(".");
        retries--;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Connected!");
        delay(2000);
    } else {
        Serial.println("\nWiFi connection failed.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Failed!");
        while (true);  // Stop execution if Wi-Fi fails
    }

    // Configure WiFiClientSecure for HTTPS
    client.setInsecure();  // Disable SSL verification (for testing)
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected. Reconnecting...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Reconnecting...");
        WiFi.reconnect();
        delay(5000);
        return;
    }

    // Step 1: GET /api/light to check LED state
    HTTPClient http;
    http.begin(client, String(ENDPOINT) + "/api/light");  // Use HTTPS client
    http.addHeader("api-key", API_KEY);  // Add API key as required

    Serial.println("Sending GET request to /api/light...");
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Response: " + payload);

        // Parse JSON response
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
            Serial.print("JSON Parsing Failed: ");
            Serial.println(error.c_str());
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("JSON Error:");
            lcd.setCursor(0, 1);
            lcd.print(error.c_str());
        } else {
            // Extract LED state and control the LED
            bool lightState = doc["light"];
            digitalWrite(LED_PIN, lightState ? HIGH : LOW);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Light: ");
            lcd.print(lightState ? "ON" : "OFF");
        }
    } else {
        Serial.print("HTTP Error: ");
        Serial.println(httpCode);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("HTTP Error:");
        lcd.setCursor(0, 1);
        lcd.print(httpCode);
    }

    http.end();

    // Step 2: PUT /api/temp to send temperature data
    http.begin(client, String(ENDPOINT) + "/api/temp");  // Use HTTPS client
    http.addHeader("api-key", API_KEY);  // Add API key as required
    http.addHeader("Content-Type", "application/json");  // Add content-type

    // Read temperature from DS18B20 sensor
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    String tempJson = "{\"temp\": " + String(temperature) + "}";

    Serial.println("Sending PUT request to /api/temp...");
    httpCode = http.PUT(tempJson);

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Response: " + payload);

        // Parse JSON response
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
            Serial.print("JSON Parsing Failed: ");
            Serial.println(error.c_str());
            lcd.setCursor(0, 1);
            lcd.print("JSON Error:");
        } else {
            // Display response message
            const char* message = doc["message"];
            lcd.setCursor(0, 1);
            lcd.print("Temp: ");
            lcd.print(temperature);
            lcd.print(" ");
            lcd.print(message);
        }
    } else {
        Serial.print("HTTP Error: ");
        Serial.println(httpCode);
        lcd.setCursor(0, 1);
        lcd.print("HTTP Error:");
        lcd.print(httpCode);
    }

    http.end();

    delay(5000);  // Wait before next iteration
}