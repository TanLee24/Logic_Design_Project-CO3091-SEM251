#include "mainserver.h"

WebServer WiFiserver(80);

// State variables for webpage
bool led_state = false;
bool neo_state = false;
bool isAPMode = true;
bool isAPConnected = false;

// NeoPixel object (1 LED)
Adafruit_NeoPixel pixels(1, NEO_PIN, NEO_GRB + NEO_KHZ800);

int blinkInterval = 1000;
bool blinkMode = false;

String WiFiSTA_ID = "";
String WiFiSTA_PASS = "";

/* ========== HTML PAGES ========== */
String mainPage() {
    float temperature = 0, humidity = 0;
    if (dht_mutex && xSemaphoreTake(dht_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        temperature = dht_data.temp;
        humidity    = dht_data.humi;
        xSemaphoreGive(dht_mutex);
    }

    String currentIP = isAPMode ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
    String modeText  = isAPMode ? "Access Point" : "Dual AP + Station";
    return R"rawliteral(
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1">
                <title>ESP32 Dashboard</title>
            
                <style>
                    body {
                        margin: 0;
                        font-family: Poppins, Arial;
                        background: linear-gradient(130deg, var(--bg1), var(--bg2));
                        color: var(--text);
                        text-align: center;
                    }

                    html, body {
                        height: 100%;
                        margin: 0;
                        padding: 0;
                        background-attachment: fixed;
                    }

                    :root {
                        --bg1: #d9e4ff;
                        --bg2: #c7d6ff;
                        --text: #0e111a;
                        --card-bg: rgba(255,255,255,0.55);
                        --glass-border: rgba(255,255,255,0.35);
                        --accent: #5b8bff;
                        --accent2: #00d4ff;
                        --ok: #2ecc71;
                        --bad: #e74c3c;
                        --radius: 16px;
                        --shadow: 0 6px 26px rgba(0,0,0,0.18);
                    }
                        
                    .page-title {
                        font-size: 32px;
                        font-weight: 800;
                        margin: 10px 0 18px;
                        letter-spacing: 0.5px;
                        background: linear-gradient(90deg, #5b8bff, #00d4ff);
                        -webkit-background-clip: text;
                        -webkit-text-fill-color: transparent;
                        position: relative;
                    }
                        
                    .page-title::after {
                        content: "";
                        display: block;
                        width: 300px;
                        height: 4px;
                        margin: 6px auto 0;
                        border-radius: 10px;
                        background: linear-gradient(90deg, #5b8bff, #00d4ff);
                        opacity: 0.75;
                    }

                    .navbar {
                        display: flex;
                        justify-content: flex-start;
                        gap: 12px;
                    }
                        
                    .nav-btn {
                        padding: 10px 18px;
                        background: var(--accent2);
                        color: white;
                        border-radius: 12px;
                        font-weight: 600;
                        border: none;
                        cursor: pointer;
                    }

                    .card {
                        width: min(90vw, 380px);
                        margin: 16px auto;
                        padding: 16px;
                        background: var(--card-bg);
                        border: 1px solid var(--glass-border);
                        border-radius: var(--radius);
                        box-shadow: var(--shadow);
                        backdrop-filter: blur(12px);
                    }

                    .card-title {
                        display: flex;
                        align-items: center;
                        gap: 10px;
                        font-size: 19px;
                        font-weight: 700;
                        margin-bottom: 8px;
                    }

                    .flex-row {
                        display: flex;
                        justify-content: center;
                        gap: 12px;
                        flex-wrap: wrap;
                    }
                       
                    .device-card {
                        max-width: 300px;
                        width: 100%;
                        flex: none;
                        margin: 10px;
                    }

                    .icon {
                        width: 22px;
                        height: 22px;
                        opacity: 0.85;
                    }

                    .flex-row {
                        display: flex;
                        justify-content: center;
                        gap: 12px;
                        flex-wrap: wrap;
                    }

                    .info-chip {
                        padding: 12px;
                        background: rgba(255,255,255,0.20);
                        border-radius: var(--radius);
                        border: 1px solid var(--glass-border);
                        width: 160px;
                    }

                    .sensor-box {
                        background: rgba(255,255,255,0.20);
                        padding: 14px;
                        border-radius: var(--radius);
                        width: 160px;
                        border: 1px solid var(--glass-border);
                    }

                    .sensor-box .value {
                        font-size: 24px;
                        font-weight: 700;
                    }

                    button {
                        border: 0;
                        cursor: pointer;
                        border-radius: 12px;
                        padding: 12px 18px; /* compact */
                        font-size: 16px;
                        color: white;
                        margin: 5px;
                    }

                    .btn-on  { background: linear-gradient(180deg, #32e07d, #1fb862); }
                    .btn-off { background: linear-gradient(180deg, #ff5c5c, #d13939); }
                    .btn-blue { background: linear-gradient(180deg, #5b8bff, #3a63d8); }

                    input[type=range] {
                        width: 90%;
                        margin-top: 8px;
                    }

                    input[type=color] {
                        width: 55%;
                        height: 38px;
                        border-radius: 12px;
                        border: 2px solid var(--glass-border);
                    }

                    .toggle {
                        position: fixed;
                        top: 14px;
                        right: 14px;
                    }
                </style>
            </head>
            
            <body>
                <div class="navbar">
                    <button class="btn-blue" onclick="location.href='/'">Control Panel</button>
                    <button class="btn-blue" onclick="location.href='/settings'">Settings</button>
                </div>
                
                <h2 class="page-title">ESP32 Dashboard</h2>

                <!-- IP + MODE DISPLAY -->
                <div class="flex-row">
                    <div class="info-chip">
                        <strong>Current IP:</strong><br>
                        )rawliteral" + currentIP + R"rawliteral(
                    </div>

                    <div class="info-chip">
                        <strong>Current mode:</strong><br>
                        )rawliteral" + modeText + R"rawliteral(
                    </div>

                </div>

                <!-- WIFI DISPLAY -->
                <div class="card" style="max-width:360px;">
                    <strong>Current WiFi:</strong><br>
                    )rawliteral" + (isAPMode ? "ESP32 Access Point" : WiFiSTA_ID) + R"rawliteral(
                </div>

                <!-- SENSOR -->
                <div class="flex-row">
                    <div class="sensor-box">
                        <svg class="icon" viewBox="0 0 24 24"><path d="M12 3v10a4 4 0 1 1-4 4" stroke="currentColor" fill="none"/></svg>
                        <div class="value" id="temp">-- °C</div>
                    </div>
                
                    <div class="sensor-box">
                        <svg class="icon" viewBox="0 0 24 24"><path d="M12 2s7 7.5 7 12a7 7 0 1 1-14 0c0-4.7 7-12 7-12z" stroke="currentColor" fill="none"/></svg>
                        <div class="value" id="humi">-- %</div>
                    </div>
                </div>

                <!-- LED + NEO -->
                <div class="flex-row">
                    <!-- LED D13 -->
                    <div class="card device-card">
                        <div class="card-title">
                            <svg class="icon" viewBox="0 0 24 24"><circle cx="12" cy="14" r="5" stroke="currentColor" fill="none"/><path d="M12 2v6" stroke="currentColor"/></svg>
                            LED (D13)
                        </div>
                        
                        <p id="led_state" style="margin:6px 0;">State: --</p>
                        <button class="btn-on" onclick="send('/led/on')">ON</button>
                        <button class="btn-off" onclick="send('/led/off')">OFF</button>

                        <h4 style="margin:10px 0 6px;">Blink</h4>
                        <button class="btn-on" onclick="send('/blink/start')">Start</button>
                        <button class="btn-off" onclick="send('/blink/stop')">Stop</button>
                        <p style="margin:8px 0;">Speed: <span id="speed_label">1000 ms</span></p>
                        <input type="range" min="50" max="3000" value="1000" id="blink_slider" oninput="changeSpeed(this.value)">
                    </div>
                    
                    <!-- NEO PIXEL -->
                    <div class="card device-card">
                        <div class="card-title">
                            <svg class="icon" viewBox="0 0 24 24"><circle cx="12" cy="12" r="7" stroke="currentColor" fill="none"/></svg>
                            NeoPixel
                        </div>
                        <p id="neo_state" style="margin:6px 0;">State: --</p>
                        <button class="btn-off" onclick="send('/neo/off')">Turn Off</button>
                        <h4 style="margin:10px 0 6px;">Color</h4>
                        <input type="color" id="colorpicker" onchange="setColor(this.value)">
                    </div>
                </div>
            
                <script>
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
                        
                    // ------------ SENSOR UPDATE -----------
                    function updateSensors() {
                        fetch("/sensors").then(r => r.json()).then(d => {
                            document.getElementById("temp").innerText = d.temp.toFixed(1) + " °C";
                            document.getElementById("humi").innerText = d.humi.toFixed(0) + " %";
                        });
                    }
                    setInterval(updateSensors, 2000);
                    updateSensors();

                    // ------------ PERIODIC POLLING -------
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

String settingsPage() {
    return R"rawliteral(
        <!DOCTYPE html>
        <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1">
                <title>WiFi Settings</title>
                <style>
                    :root {
                        --bg1: #d9e4ff;
                        --bg2: #c7d6ff;
                        --text: #0e111a;
                        --card-bg: rgba(255,255,255,0.55);
                        --glass-border: rgba(255,255,255,0.35);
                        --accent: #5b8bff;
                        --accent2: #00d4ff;
                        --radius: 16px;
                        --shadow: 0 6px 26px rgba(0,0,0,0.18);
                    }

                    body {
                        margin: 0;
                        font-family: Poppins, Arial;
                        background: linear-gradient(130deg, var(--bg1), var(--bg2));
                        color: var(--text);
                        text-align: center;
                    }
                        
                    .card {
                        width: min(90vw, 380px);
                        margin: 16px auto;
                        padding: 16px;
                        background: var(--card-bg);
                        border: 1px solid var(--glass-border);
                        border-radius: var(--radius);
                        box-shadow: var(--shadow);
                        backdrop-filter: blur(12px);
                    }
                        
                    input {
                        width: 90%;
                        padding: 10px;
                        margin: 5px 0;
                        border: 1px solid #ccc;
                        border-radius: 10px;
                    }
                        
                    button {
                        border: 0;
                        cursor: pointer;
                        border-radius: 12px;
                        padding: 12px 18px;
                        font-size: 16px;
                        color: white;
                        margin: 5px;
                    }

                    .toggle {
                        position: fixed;
                        top: 14px;
                        right: 14px;
                    }

                    .btn-on  { background: linear-gradient(180deg, #32e07d, #1fb862); }
                    .btn-off { background: linear-gradient(180deg, #ff5c5c, #d13939); }
                    .btn-blue { background: linear-gradient(180deg, #5b8bff, #3a63d8); }
                </style>
            </head>
            <body>
                <div class="card">
                    <h2>WiFi Settings</h2>
                    <form action="/connect" method="GET">
                        <input type="text" name="ssid" placeholder="WiFi SSID" required>
                        <input type="password" name="pass" placeholder="Password" required>
                        <button type="submit" class="btn-on">Connect</button>
                    </form>
                    <a href="/"><button class="btn-blue">Back to Dashboard</button></a>
                </div>
            </body>
        </html>
    )rawliteral";
}

String connectingPage(String ssid) {
    return R"rawliteral(
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1">
                <title>Connecting...</title>
                <style>
                    :root {
                        --bg1: #d9e4ff;
                        --bg2: #c7d6ff;
                        --text: #0e111a;
                        --card-bg: rgba(255,255,255,0.55);
                        --glass-border: rgba(255,255,255,0.35);
                        --accent2: #00d4ff;
                        --radius: 16px;
                        --shadow: 0 6px 26px rgba(0,0,0,0.18);
                    }

                    body {
                        margin: 0;
                        font-family: Poppins, Arial;
                        background: linear-gradient(130deg, var(--bg1), var(--bg2));
                        color: var(--text);
                        text-align: center;
                    }

                    .card {
                        width: min(90vw, 380px);
                        margin: 80px auto;
                        padding: 20px;
                        background: var(--card-bg);
                        border: 1px solid var(--glass-border);
                        border-radius: var(--radius);
                        box-shadow: var(--shadow);
                        backdrop-filter: blur(12px);
                    }

                    h2 {
                        margin-top: 0;
                        font-size: 26px;
                        font-weight: 700;
                    }

                    /* Spinner */
                    .loader {
                        margin: 20px auto;
                        border: 6px solid rgba(255,255,255,0.4);
                        border-top: 6px solid var(--accent2);
                        border-radius: 50%;
                        width: 44px;
                        height: 44px;
                        animation: spin 1s linear infinite;
                    }

                    @keyframes spin {
                        0% { transform: rotate(0deg); }
                        100% { transform: rotate(360deg); }
                    }

                    .btn-blue {
                        background: linear-gradient(180deg, #5b8bff, #3a63d8);
                        border: none;
                        padding: 12px 18px;
                        border-radius: 12px;
                        font-size: 16px;
                        color: white;
                        cursor: pointer;
                        margin-top: 16px;
                        display: none;
                    }

                    .toggle {
                        position: fixed;
                        top: 14px;
                        right: 14px;
                    }
                </style>
            </head>
            
            <body>
                <div class="card">
                    <h2 id="statusTitle">Connecting to:<br><b>)rawliteral" + ssid + R"rawliteral(</b></h2>

                    <div id ="spinner" class="loader"></div>

                    <p id="statusText" style="margin-top:10px;">Attempting connection...<br>Please wait a moment.</p>

                    <button id="backButton" class="btn-blue" onclick="location.href='/'">Back to Dashboard</button>
                </div>

                <script>
                    function checkConnection() {
                        fetch('/state')
                        .then(r => r.json())
                        .then(data => {
                            if (data.connected) {
                                document.getElementById('statusTitle').innerHTML = "Connected!";
                                document.getElementById('statusText').innerHTML = "ESP32 is now connected to WiFi.<br>You may return to the dashboard.";
                                document.getElementById('spinner').style.display = "none";
                                document.getElementById('backButton').style.display = "inline-block";
                            }
                        });
                    }
                        
                    setInterval(checkConnection, 1000);
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
    bool connected = false;
    
    while (millis() - start < 8000) {
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            break;
        }
        delay(1000);
        Serial.print(".");
    }

    if (connected) {
        Serial.println("\n[WiFi] STA Connected!");
        Serial.print("[WiFi] STA IP: ");
        Serial.println(WiFi.localIP());
        isAPMode = false;
    } 
    else {
        Serial.println("\n[WiFi] STA Failed.");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_AP);
        isAPMode = true;
    }
}

void startAPMode()
{
    if (!isAPConnected) {
        IPAddress apIP(192,168,10,1);
        IPAddress gateway(192,168,10,1);
        IPAddress subnet(255,255,255,0);
        WiFi.mode(WIFI_AP);
        Serial.println("[WiFi] Starting AP...");
        WiFi.softAP("ESP32 Access Point", "bruhh12345");
        WiFi.softAPConfig(apIP, gateway, subnet);
        Serial.print("[WiFi] AP IP: ");
        Serial.println(WiFi.softAPIP());
        isAPConnected = true;
    }
    isAPMode = true;
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
        json += "\"neo\":" + String(neo_state ? "true" : "false") + ",";
        json += "\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false");
        json += "}";
        WiFiserver.send(200, "application/json", json);
    });

    WiFiserver.on("/sensors", [](){
        float t = 0, h = 0;
        if (dht_mutex && xSemaphoreTake(dht_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            t = dht_data.temp;
            h = dht_data.humi;
            xSemaphoreGive(dht_mutex);
        }
        String json = "{\"temp\":" + String(t, 1) + ",\"humi\":" + String(h, 0) + "}";
        WiFiserver.send(200, "application/json", json);
    });

    WiFiserver.on("/settings", []() {
        WiFiserver.send(200, "text/html", settingsPage());
    });
    
    WiFiserver.on("/connect", []() {
        if (WiFiserver.hasArg("ssid") && WiFiserver.hasArg("pass")) {
            WiFiSTA_ID = WiFiserver.arg("ssid");
            WiFiSTA_PASS = WiFiserver.arg("pass");
            Serial.printf("Connecting to new WiFi: %s\n", WiFiSTA_ID.c_str());
            WiFi.mode(WIFI_AP_STA);
            WiFi.begin(WiFiSTA_ID.c_str(), WiFiSTA_PASS.c_str());
            isAPMode = false;
        }
        WiFiserver.send(200, "text/html", connectingPage(WiFiSTA_ID));
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

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println("[Event] Connected to WiFi! IP: ");
            Serial.println(WiFi.localIP());
            isAPMode = false;
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[Event] WiFi disconnected! AP mode enabled.");
            isAPMode = true;
            WiFi.disconnect(true, false);
            WiFi.mode(WIFI_AP);
            break;

        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("[Event] AP Started");
            isAPMode = true;
            break;

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
    xTaskCreate(blink_mode, "Blink Mode", 4096, NULL, 1, NULL);
    xTaskCreate(DHT, "Temp & Humid Sensor", 3072, NULL, 1, NULL);

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