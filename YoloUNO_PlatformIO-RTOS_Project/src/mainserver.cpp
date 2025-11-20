#include "mainserver.h"

WebServer WiFiserver(80);

// State variables for webpage
bool led_state = false;
bool neo_state = false;

// NeoPixel object (1 LED)
Adafruit_NeoPixel pixels(1, NEO_PIN, NEO_GRB + NEO_KHZ800);

int blinkInterval = 500;
bool blinkMode = false;

String WiFiSTA_ID = "ACLAB";
String WiFiSTA_PASS = "ACLAB2023";

/* ========== HTML PAGE ========== */
String mainPage() {
    return R"rawliteral(
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8">
                <title>ESP32 Dashboard</title>
            
                <style>
                    body {
                        font-family: Arial;
                        transition: background 0.4s, color 0.4s;
                        background: var(--bg);
                        color: var(--fg);
                        text-align: center;
                        padding: 20px;
                    }

                    :root {
                        --bg: #eef2f3;
                        --fg: #111;
                        --card-bg: #ffffff;
                    }

                    .dark {
                        --bg: #121212;
                        --fg: #e3e3e3;
                        --card-bg: #1e1e1e;
                    }

                    .card {
                        background: var(--card-bg);
                        width: 320px;
                        padding: 20px;
                        margin: 20px auto;
                        border-radius: 12px;
                        box-shadow: 0 4px 12px rgba(0,0,0,0.25);
                    }

                    button {
                        width: 130px;
                        height: 45px;
                        margin: 8px;
                        border: none;
                        border-radius: 10px;
                        cursor: pointer;
                        font-size: 16px;
                        color: white;
                        transition: 0.2s;
                    }

                    .btn-on { background: #2ecc71; }
                    .btn-off { background: #e74c3c; }
                    .btn-blue { background: #3498db; }

                    input[type=range] {
                        width: 80%;
                        margin-top: 15px;
                    }

                    .toggle {
                        position: fixed;
                        top: 10px;
                        right: 10px;
                    }
                </style>
            </head>
            
            <body>
                <button class="toggle btn-blue" onclick="toggleDark()">Toggle Dark Mode</button>
                <h1>ESP32 Control Panel</h1>

                <!-- LED MANUAL + BLINK MODE -->
                <div class="card">
                    <h2>LED (D13)</h2>
                    <p id="led_state">State: --</p>

                    <button class="btn-on" onclick="send('/led/on')">ON</button>
                    <button class="btn-off" onclick="send('/led/off')">OFF</button>

                    <h3>Blink Mode</h3>
                    <button class="btn-on" onclick="send('/blink/start')">Start Blink</button>
                    <button class="btn-off" onclick="send('/blink/stop')">Stop Blink</button>

                    <p>Blink Speed: <span id="speed_label">500 ms</span></p>
                    <input type="range" min="50" max="2000" value="500" id="blink_slider" oninput="changeSpeed(this.value)">
                </div>
                
                <!-- NEO PIXEL -->
                <div class="card">
                    <h2>NeoPixel LED</h2>
                    <p id="neo_state">State: --</p>

                    <button class="btn-off" onclick="send('/neo/off')">Turn Off</button>

                    <h3>Pick Color</h3>
                    <input type="color" id="colorpicker" onchange="setColor(this.value)">
                </div>
            
                <script>
                    // ---------------- DARK MODE ----------------
                    function toggleDark() {
                        document.body.classList.toggle("dark");
                    }
                        
                    // ---------------- AJAX COMMAND --------------
                    function send(url) {
                        fetch(url)
                        .then(res => res.json())
                        .then(updateState);
                    }
                        
                    // ------------- BLINK SPEED SLIDER ----------
                    function changeSpeed(val) {
                        document.getElementById("speed_label").innerHTML = val + " ms";
                        fetch("/blink/speed?val=" + val);
                    }
                        
                    // ------------- COLOR PICKER -----------------
                    function setColor(hex) {
                        // Convert #RRGGBB → decimal values
                        let r = parseInt(hex.substr(1,2), 16);
                        let g = parseInt(hex.substr(3,2), 16);
                        let b = parseInt(hex.substr(5,2), 16);
                        
                        fetch(`/neo/color?r=${r}&g=${g}&b=${b}`)
                        .then(res => res.json())
                        .then(updateState);
                    }
                        
                    // ------------ STATE UPDATE -----------
                    function updateState(data) {
                        document.getElementById("led_state").innerHTML = "State: " + (data.led ? "ON" : "OFF");
                        document.getElementById("neo_state").innerHTML = "State: " + (data.neo ? "ON" : "OFF");
                    }
                        
                    // ------------ PERIODIC SENSOR POLLING -------
                    function poll() {
                        fetch('/state')
                        .then(res => res.json())
                        .then(updateState);
                    }
                        
                    setInterval(poll, 500);
                </script>
            </body>
        </html>
    )rawliteral";
}

/* ========== WIFI AP + STA SETUP ========== */

void connectToWiFi()
{
    Serial.println("[WiFi] Trying STA Mode...");

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(WiFiSTA_ID.c_str(), WiFiSTA_PASS.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
        Serial.print(".");
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] STA Connected!");
        Serial.print("[WiFi] STA IP: ");
        Serial.println(WiFi.localIP());
    } 
    else Serial.println("\n[WiFi] STA Failed.");
}

void startAPMode()
{
    Serial.println("[WiFi] Starting AP...");

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("ESP32 Access Point", "bruhh12345");

    Serial.print("[WiFi] AP IP: ");
    Serial.println(WiFi.softAPIP());
}

/* ========== WEBSERVER SETUP ========== */
void setupServer() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    pixels.begin();
    pixels.clear();
    pixels.show();

    // Main page
    WiFiserver.on("/", []() {
        WiFiserver.send(200, "text/html", mainPage());
    });

    // // LED — normal LED
    // WiFiserver.on("/led/on", []() {
    //     led_state = true;
    //     digitalWrite(LED_PIN, HIGH);
    //     WiFiserver.sendHeader("Location", "/");
    //     WiFiserver.send(303);
    // });

    // WiFiserver.on("/led/off", []() {
    //     led_state = false;
    //     digitalWrite(LED_PIN, LOW);
    //     WiFiserver.sendHeader("Location", "/");
    //     WiFiserver.send(303);
    // });

    // // NEO — NeoPixel LED
    // WiFiserver.on("/neo/on", []() {
    //     neo_state = true;
    //     pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Green
    //     pixels.show();
    //     WiFiserver.sendHeader("Location", "/");
    //     WiFiserver.send(303);
    // });

    // WiFiserver.on("/neo/off", []() {
    //     neo_state = false;
    //     pixels.clear();
    //     pixels.show();
    //     WiFiserver.sendHeader("Location", "/");
    //     WiFiserver.send(303);
    // });

    WiFiserver.on("/led/on", []() {
        led_state = true;
        digitalWrite(LED_PIN, HIGH);
        WiFiserver.send(200, "application/json", "{\"ok\":true}");
    });
    
    WiFiserver.on("/led/off", []() {
        led_state = false;
        digitalWrite(LED_PIN, LOW);
        WiFiserver.send(200, "application/json", "{\"ok\":true}");
    });

    WiFiserver.on("/blink/start", []() {
        blinkMode = true;
        WiFiserver.send(200, "application/json", "{\"ok\":true}");
    });

    WiFiserver.on("/blink/stop", []() {
        blinkMode = false;
        WiFiserver.send(200, "application/json", "{\"ok\":true}");
    });

    WiFiserver.on("/neo/off", []() {
        pixels.clear();
        pixels.show();
        neo_state = false;
        WiFiserver.send(200, "application/json", "{\"ok\":true}");
    });

    WiFiserver.on("/blink/speed", []() {
        if (WiFiserver.hasArg("val")) blinkInterval = WiFiserver.arg("val").toInt();
        WiFiserver.send(200, "application/json", "{\"ok\":true}");
    });
    
    WiFiserver.on("/neo/color", [](){
        int r = WiFiserver.arg("r").toInt();
        int g = WiFiserver.arg("g").toInt();
        int b = WiFiserver.arg("b").toInt();
        
        pixels.setPixelColor(0, pixels.Color(r, g, b));
        pixels.show();
        neo_state = !(r == 0 && g == 0 && b == 0);
        WiFiserver.send(200, "application/json", "{\"ok\":true}");
    });
    
    WiFiserver.on("/state", [](){
        String json = "{";
        json += "\"led\":" + String(led_state ? "true" : "false") + ",";
        json += "\"neo\":" + String(neo_state ? "true" : "false");
        json += "}";
        WiFiserver.send(200, "application/json", json);
    });

    WiFiserver.begin();
    Serial.println("[Server] HTTP server started!");
}

/* ========== HANDLER ==========*/

void WiFiEvent(WiFiEvent_t event)
{
    switch(event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("[Event] STA Started"); break;

        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("[Event] Connected to WiFi!"); break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[Event] WiFi disconnected! Reconnecting...");
            WiFi.reconnect();
            break;

        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("[Event] AP Started"); break;

        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("[Event] Client connected to AP"); break;

        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("[Event] Client disconnected from AP"); break;
    }
}

/* ========== TASK ========== */

void main_server_task(void *pvParameters)
{
    Serial.println("[MainServer] Task started!");

    WiFi.onEvent(WiFiEvent);
    startAPMode();
    connectToWiFi();
    setupServer();

    while (true) {
        WiFiserver.handleClient();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void blink_mode(void *pvParameters) {
    while (true) {
        if (blinkMode) {
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            vTaskDelay(pdMS_TO_TICKS(blinkInterval));
        } 
        else vTaskDelay(pdMS_TO_TICKS(100));
    }
}