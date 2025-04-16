#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // For JSON if you're sending structured data

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Cloudflare Worker URL
const char* workerURL = "https://your-worker-name.your-subdomain.workers.dev"; // Or your custom domain

// For HTTPS
WiFiClientSecure client;

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
    // client.setInsecure(); // ONLY FOR TESTING, SEE BELOW -  REPLACE THIS FOR PRODUCTION
    client.setCACert(
        "-----BEGIN CERTIFICATE-----\n"  // Replace with actual Cloudflare cert
        "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2i3t54wCgYIKoZIzj0EAwIwSzELMAkGA1UEBhMCVVMx\n"
        "MzIxMDArBgNVBAoTClZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZWZXJpU2lnbiBUcnVzdCBOZXR3b3Jr\n"
        "-----END CERTIFICATE-----\n");
}

void loop() {
    // Prepare data (example: a sensor reading)
    float sensorValue = random(0, 100);
    String jsonData;

    // Create JSON payload (if needed)
    StaticJsonDocument<128> doc;
    doc["sensorValue"] = sensorValue;
    serializeJson(doc, jsonData);

    // Send data to Cloudflare Worker
    sendDataToWorker(jsonData);
    delay(5000); // Send data every 5 seconds
}

void sendDataToWorker(const String& data) {
    HTTPClient http;

    Serial.print("Connecting to: ");
    Serial.println(workerURL);
    if (http.begin(client, workerURL)) { // Use https
        http.addHeader("Content-Type", "application/json"); // Important for JSON data

        int httpResponseCode = http.POST(data); // Send the JSON data

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            if (httpResponseCode == HTTP_OK) {
                String response = http.getString();
                Serial.println("Response from worker:");
                Serial.println(response);
                // Parse the response if needed
            } else {
                 Serial.print("Error sending data.  Code: ");
                 Serial.println(httpResponseCode);
            }
        } else {
            Serial.println("Failed to connect to worker");
        }
        http.end(); // Free the resources
    } else {
        Serial.println("Failed to setup HTTP connection");
    }
}
