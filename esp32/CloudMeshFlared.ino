#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // For JSON if you're sending structured data
#include <HardwareSerial.h>
#include <Stream.h> //for the TinyGSM library

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Cloudflare Worker URL
const char* workerURL = "https://your-worker-name.your-subdomain.workers.dev"; // Or your custom domain

// For HTTPS
WiFiClientSecure client;

// Define the UART pins for Meshtastic communication
#define MESHTASTIC_RX_PIN 16 // Example: GPIO 16
#define MESHTASTIC_TX_PIN 17 // Example: GPIO 17
#define MESHTASTIC_BAUD_RATE 115200

HardwareSerial meshtasticSerial(1); // Use Serial1 for the Meshtastic device

// Function to read data from the Meshtastic device via UART
String readMeshtasticData(Stream &serial);

// Function to send data to the Cloudflare Worker
bool sendDataToWorker(const String &data, WiFiClientSecure &client, const char* workerURL);

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    // Set the time
    configTime(0, 0, "pool.ntp.org");  // Get time from NTP server
    while (!timeAvailable()) {
        delay(1000);
        Serial.println("Waiting for NTP time...");
    }
    Serial.println("Got NTP time");

    // client.setInsecure(); // ONLY FOR TESTING, SEE BELOW - REPLACE THIS FOR PRODUCTION
      client.setCACert(
        "-----BEGIN CERTIFICATE-----\n"  // Replace with actual Cloudflare cert
        "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2i3t54wCgYIKoZIzj0EAwIwSzELMAkGA1UEBhMCVVMx\n"
        "MzIxMDArBgNVBAoTClZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZWZXJpU2lnbiBUcnVzdCBOZXR3b3Jr\n"
        "-----END CERTIFICATE-----\n");

    // Initialize the serial port for Meshtastic communication
    meshtasticSerial.begin(MESHTASTIC_BAUD_RATE, SERIAL_8N1, MESHTASTIC_RX_PIN, MESHTASTIC_TX_PIN);
    Serial.println("Meshtastic Serial Initialized");
}

void loop() {
    // Read data from Meshtastic device via UART
    String meshtasticData = readMeshtasticData(meshtasticSerial);

    // Send data to Cloudflare Worker if data was received
    if (meshtasticData.length() > 0) {
        if (sendDataToWorker(meshtasticData, client, workerURL)) {
          Serial.println("Data sent successfully");
        }
        else{
          Serial.println("Data sending failed");
        }
    }
    delay(5000); // Send data every 5 seconds (adjust as needed)
}

String readMeshtasticData(Stream &serial) {
    String data = "";
    static unsigned long previousMillis = 0;
    const long timeoutMillis = 2000; // Timeout after 2 seconds of no data

    if (serial.available()) {
        previousMillis = millis(); // Reset timeout timer
        while (serial.available()) {
            char c = serial.read();
            if (c == '\n') { // Assuming Meshtastic sends data with a newline terminator
                return data;       // Stop reading when newline is received
            }
            data += c;
        }
    }
    //check for timeout
    if (millis() - previousMillis > timeoutMillis && data.length() > 0)
    {
      Serial.println("UART read timeout");
      return data;
    }
    return "";
}

bool sendDataToWorker(const String &data, WiFiClientSecure &client, const char* workerURL) {
    HTTPClient http;
    bool retVal = false;

    Serial.print("Connecting to: ");
    Serial.println(workerURL);
    if (http.begin(client, workerURL)) { // Use https
        http.addHeader("Content-Type", "text/plain"); // Adjust Content-Type as needed

        int httpResponseCode = http.POST(data); // Send the data

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            if (httpResponseCode == HTTP_OK) {
                String response = http.getString();
                Serial.println("Response from worker:");
                Serial.println(response);
                retVal = true;
                // Parse the response if needed
            } else {
                Serial.print("Error sending data.  Code: ");
                Serial.println(httpResponseCode);
                retVal = false;
            }
        } else {
            Serial.println("Failed to connect to worker");
            retVal = false;
        }
        http.end(); // Free the resources
    } else {
        Serial.println("Failed to setup HTTP connection");
        retVal = false;
    }
    return retVal;
}
