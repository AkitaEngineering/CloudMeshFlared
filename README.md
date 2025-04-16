# CloudMeshFlared

## Overview

CloudMeshFlared is a project that demonstrates how to securely send data from an ESP32 microcontroller to the internet using a Cloudflare Worker. This approach avoids the need to open inbound ports on your network, enhancing security.

## ESP32 Setup (esp32/CloudMeshFlared.ino)

### Prerequisites

* ESP32 development board
* Arduino IDE or PlatformIO
* Wi-Fi network

### Installation

1.  **Install Libraries:**
    * In the Arduino IDE, go to Sketch -> Include Library -> Manage Libraries...
    * Search for and install:
        * `WiFi` (Built-in)
        * `WiFiClientSecure` (Built-in)
        * `HTTPClient` by ESP32
        * `ArduinoJson` by Benoit Blanchon
2.  **Clone the Repository:**
    ```bash
    git clone [https://github.com/your-username/CloudMeshFlared.git](https://github.com/your-username/CloudMeshFlared.git)
    cd CloudMeshFlared/esp32
    ```
3.  **Configure:**
    * Open `CloudMeshFlared.ino` in the Arduino IDE.
    * Replace the following placeholders with your actual credentials:
        * `YOUR_WIFI_SSID`: Your Wi-Fi network name.
        * `YOUR_WIFI_PASSWORD`: Your Wi-Fi password.
        * `https://your-worker-name.your-subdomain.workers.dev`: Your Cloudflare Worker URL (see "Cloudflare Worker Setup" below).
4.  **Upload:**
    * Connect your ESP32 board to your computer.
    * Select your board and port in the Arduino IDE.
    * Upload the sketch.

### PlatformIO (Optional)

If you are using PlatformIO, you can use the provided `platformio.ini` file.  Ensure you have PlatformIO installed.  The `platformio.ini` file already specifies the required libraries.  You will still need to configure the WiFi credentials and Cloudflare Worker URL in the `CloudMeshFlared.ino` file.

## Cloudflare Worker Setup (worker/index.js)

### Prerequisites

* Cloudflare account
* Cloudflare Workers enabled

### Installation

1.  **Clone the Repository:**
    ```bash
    git clone [https://github.com/your-username/CloudMeshFlared.git](https://github.com/your-username/CloudMeshFlared.git)
    cd CloudMeshFlared/worker
    ```
2.  **Create a Cloudflare Worker:**
    * Log in to your Cloudflare account.
    * Go to the Workers section and create a new Worker.
3.  **Deploy Code:**
    * Copy the code from `worker/index.js` and paste it into the Cloudflare Worker editor.
    * Deploy the Worker.
4.  **Set up a Route:**
    * Configure a route (a URL) that will trigger your Worker.  This is the URL that your ESP32 will send data to.  Note this URL down.
5.  **(Optional) Configure KV:**
     * If you want to use Cloudflare KV, create a namespace and bind it to your worker.  The worker code assumes a KV namespace called `MY_KV_NAMESPACE`.

## Usage

1.  **ESP32:** Once the ESP32 is powered on and connected to Wi-Fi, it will start sending data to your Cloudflare Worker every 5 seconds.
2.  **Cloudflare Worker:** The Worker will receive the data, log it, and optionally store it in Cloudflare KV.  The Worker will then send a response back to the ESP32.
3.  **Monitor:**
    * You can monitor the data being sent from the ESP32 via the Serial Monitor in the Arduino IDE.
    * You can view the Worker logs in the Cloudflare dashboard to see the data being received and processed.  You can also check your KV namespace (if used) to see the stored data.

## Security Considerations

* **HTTPS:** The ESP32 code uses HTTPS to encrypt the communication with the Cloudflare Worker.  It is *CRITICAL* that you replace  `client.setInsecure();` with proper certificate handling for production use.
* **Worker Security:** Secure your Cloudflare Worker if you are handling sensitive data.  Consider adding authentication (e.g., API keys) to the Worker to verify the requests are coming from your ESP32.
* **Data Validation:** Implement data validation on both the ESP32 and the Cloudflare Worker to ensure the data being sent is in the expected format.

## Troubleshooting

* **ESP32 Wi-Fi Connection Issues:**
    * Double-check your Wi-Fi credentials in the `CloudMeshFlared.ino` file.
    * Ensure your Wi-Fi network is working correctly.
    * Check the Serial Monitor for any error messages.
* **Cloudflare Worker Errors:**
    * Check the Worker logs in the Cloudflare dashboard for any error messages.
    * Ensure your Worker code is deployed correctly.
    * Verify that the Worker URL in the ESP32 code is correct.
* **Data Not Being Received:**
     * Check the ESP32 serial monitor to see if data is being sent.
     * Check the Cloudflare Worker logs to see if requests are arriving.
     * Check the Content-Type header in the ESP32 code.
* **HTTPS Errors:**
     * If you get HTTPS errors, it is almost certainly due to certificate validation.  You *must* replace the `client.setInsecure()` line with code that loads the correct root certificates.  This is complex on ESP32.

## Further Improvements

* **Error Handling:** Implement more robust error handling on both the ESP32 and the Cloudflare Worker.
* **Data Storage:** Explore other data storage options, such as databases or other cloud services.
* **Real-time Data:** For real-time data, consider using WebSockets or Server-Sent Events (SSE) instead of polling.  This would require changes to both the ESP32 code and the Cloudflare Worker.  (Note: Cloudflare Workers have limits on connection duration)
* **Authentication:** Add authentication between the ESP32 and the Cloudflare Worker.
* **Over-the-Air (OTA) Updates:** Implement OTA updates for the ESP32 firmware.


