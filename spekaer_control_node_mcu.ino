#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

const char* default_ap_ssid = "ESP_Speaker_AP";
const char* default_ap_pass = "12345678";

ESP8266WebServer server(80);

// Pin mapping matching your exact order
const int btnPins[7] = {D3, D1, D2, D0, D5, D6, D7};

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Speaker Controller</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 20px; background: #f4f4f9; }
    h2, h3 { color: #333; }
    .btn-group { margin: 10px 0; background: #fff; padding: 10px; border-radius: 8px; display: inline-block; box-shadow: 0 2px 5px rgba(0,0,0,0.1); width: 280px; }
    button { background-color: #4CAF50; color: white; border: none; padding: 6px 10px; margin: 3px; font-size: 12px; cursor: pointer; border-radius: 4px; }
    button.long { background-color: #d9534f; }
    button.extra { background-color: #f0ad4e; }
    button:disabled { background-color: #cccccc !important; cursor: not-allowed; }
    .battery { font-size: 18px; font-weight: bold; color: #007bff; margin-bottom: 20px; }
    
    .settings-toggle { background-color: #6c757d; margin: 15px 0; padding: 10px 15px; font-size: 14px; border-radius: 6px; cursor: pointer; color: white; border: none; }
    .settings-box { display: none; margin-top: 10px; background: #fff; padding: 15px; border-radius: 8px; display: inline-block; box-shadow: 0 2px 5px rgba(0,0,0,0.1); width: 280px; }
    input { padding: 6px; margin: 5px 0; width: 90%; border: 1px solid #ccc; border-radius: 4px; }
    .save-btn { background-color: #008CBA; width: 95%; padding: 8px; font-size: 14px; margin-top: 5px; }
  </style>
</head>
<body>
  <h2>Bluetooth Speaker Controller</h2>
  <div class="battery" id="battery">Battery: Loading...</div>
  
  <div id="controls">
    <!-- Button 0: On/Off & Light (D1) -->
    <div class="btn-group">
      <b>On/Off & Light</b><br>
      <button onclick="pressBtn(0, 300)">0.3s</button>
      <button class="long" onclick="pressBtn(0, 2000)">2s</button>
      <button class="extra" onclick="pressBtn(0, 10000)">10s</button>
    </div><br>

    <!-- Button 1: Clock (D2) -->
    <div class="btn-group">
      <b>Clock</b><br>
      <button onclick="pressBtn(1, 300)">0.3s</button>
      <button class="long" onclick="pressBtn(1, 2000)">2s</button>
      <button class="extra" onclick="pressBtn(1, 10000)">10s</button>
    </div><br>

    <!-- Button 2: Alarm (D3) -->
    <div class="btn-group">
      <b>Alarm</b><br>
      <button onclick="pressBtn(2, 300)">0.3s</button>
      <button class="long" onclick="pressBtn(2, 2000)">2s</button>
      <button class="extra" onclick="pressBtn(2, 10000)">10s</button>
    </div><br>

    <!-- Button 3: Minus (-) (D5) -->
    <div class="btn-group">
      <b>Minus (-)</b><br>
      <button onclick="pressBtn(3, 300)">0.3s</button>
      <button class="long" onclick="pressBtn(3, 2000)">2s</button>
      <button class="extra" onclick="pressBtn(3, 10000)">10s</button>
    </div><br>

    <!-- Button 4: Plus (+) (D6) -->
    <div class="btn-group">
      <b>Plus (+)</b><br>
      <button onclick="pressBtn(4, 300)">0.3s</button>
      <button class="long" onclick="pressBtn(4, 2000)">2s</button>
      <button class="extra" onclick="pressBtn(4, 10000)">10s</button>
    </div><br>

    <!-- Button 5: Timer / BT (D7) -->
    <div class="btn-group">
      <b>Timer / BT</b><br>
      <button onclick="pressBtn(5, 300)">0.3s</button>
      <button class="long" onclick="pressBtn(5, 2000)">2s</button>
      <button class="extra" onclick="pressBtn(5, 10000)">10s</button>
    </div><br>

    <!-- Button 6: Mode / Play/Pause (D8) -->
    <div class="btn-group">
      <b>Mode / Play/Pause</b><br>
      <button onclick="pressBtn(6, 300)">0.3s</button>
      <button class="long" onclick="pressBtn(6, 2000)">2s</button>
      <button class="extra" onclick="pressBtn(6, 10000)">10s</button>
    </div><br>
  </div>

  <br>
  <button class="settings-toggle" onclick="toggleSettings()">⚙️ Wi-Fi Settings</button>
  
  <div id="settingsBox" class="settings-box">
    <h3>Wi-Fi Configuration</h3>
    <input type="text" id="ssid" placeholder="Wi-Fi SSID"><br>
    <input type="password" id="pass" placeholder="New Wi-Fi Password"><br>
    <button class="save-btn" onclick="saveWiFi()">Save & Restart</button>
  </div>

  <script>
    function toggleSettings() {
      let box = document.getElementById("settingsBox");
      if (box.style.display === "none" || box.style.display === "") {
        box.style.display = "inline-block";
        try {
          fetch('/getwifi')
            .then(response => response.text())
            .then(data => {
              document.getElementById("ssid").value = data;
            })
            .catch(err => console.error("Fetch error:", err));
        } catch (e) {
          console.error("Error:", e);
        }
      } else {
        box.style.display = "none";
      }
    }

    function pressBtn(id, duration) {
      let allButtons = document.querySelectorAll("button");
      allButtons.forEach(btn => btn.disabled = true);

      try {
        fetch(`/press?btn=${id}&dur=${duration}`)
          .then(response => response.text())
          .then(data => {
            setTimeout(() => {
              allButtons.forEach(btn => btn.disabled = false);
            }, duration);
          })
          .catch(error => {
            console.error("Error:", error);
            allButtons.forEach(btn => btn.disabled = false);
          });
      } catch (e) {
        console.error("Catch error:", e);
        allButtons.forEach(btn => btn.disabled = false);
      }
    }

    function saveWiFi() {
      let ssid = document.getElementById("ssid").value;
      let pass = document.getElementById("pass").value;
      if(!ssid) {
        alert("Please enter SSID!");
        return;
      }
      try {
        fetch(`/savewifi?ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}`)
          .then(response => response.text())
          .then(msg => {
            alert(msg);
            location.reload();
          })
          .catch(err => alert("Save failed!"));
      } catch (e) {
        console.error("Save error:", e);
      }
    }

    function updateBattery() {
      try {
        fetch('/battery')
          .then(response => response.text())
          .then(data => {
            document.getElementById("battery").innerText = "Battery Voltage: " + data + "V";
          })
          .catch(err => console.error("Battery fetch error:", err));
      } catch (e) {
        console.error("Battery catch error:", e);
      }
    }
    setInterval(updateBattery, 5000);
    updateBattery();
  </script>
</body>
</html>
)=====";

String getSavedSSID() {
  EEPROM.begin(512);
  String ssid = "";
  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(i);
    if (c == 0 || c == 255) break;
    if (isPrintable(c)) {
      ssid += c;
    }
  }
  EEPROM.end();
  return ssid;
}

String getSavedPass() {
  EEPROM.begin(512);
  String pass = "";
  for (int i = 0; i < 64; ++i) {
    char c = EEPROM.read(32 + i);
    if (c == 0 || c == 255) break;
    if (isPrintable(c)) {
      pass += c;
    }
  }
  EEPROM.end();
  return pass;
}

void saveWiFiConfig(String ssid, String pass) {
  EEPROM.begin(512);
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);
  }
  for (size_t i = 0; i < ssid.length() && i < 32; ++i) {
    EEPROM.write(i, ssid[i]);
  }
  for (size_t i = 0; i < pass.length() && i < 64; ++i) {
    EEPROM.write(32 + i, pass[i]);
  }
  EEPROM.commit();
  EEPROM.end();
}

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleGetWiFi() {
  server.send(200, "text/plain", getSavedSSID());
}

void handlePress() {
  if (server.hasArg("btn") && server.hasArg("dur")) {
    int btnIndex = server.arg("btn").toInt();
    int duration = server.arg("dur").toInt();

    if (btnIndex >= 0 && btnIndex < 7) {
      int pin = btnPins[btnIndex];
      
      Serial.print("Button index: ");
      Serial.print(btnIndex);
      Serial.print(" | Pin: ");
      Serial.print(pin);
      Serial.print(" | Duration: ");
      Serial.println(duration);

      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
      delay(duration); 
      pinMode(pin, INPUT);
    }
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void handleSaveWiFi() {
  if (server.hasArg("ssid") && server.hasArg("pass")) {
    String newSsid = server.arg("ssid");
    String newPass = server.arg("pass");
    
    saveWiFiConfig(newSsid, newPass);
    server.send(200, "text/plain", "Wi-Fi settings saved successfully! Device is restarting...");
    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Missing arguments!");
  }
}

void handleBattery() {
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (3.3 / 1023.0) * 3.2; 
  server.send(200, "text/plain", String(voltage, 2));
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 7; i++) {
    pinMode(btnPins[i], INPUT);
  }

  String ssid = getSavedSSID();
  String pass = getSavedPass();

  Serial.println("Saved SSID: " + ssid);

  if (ssid.length() > 0) {
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      yield();
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.softAP(default_ap_ssid, default_ap_pass);
    Serial.println("Access Point Created: " + String(default_ap_ssid));
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Connected to Wi-Fi successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }

  server.on("/", handleRoot);
  server.on("/getwifi", handleGetWiFi);
  server.on("/press", handlePress);
  server.on("/savewifi", handleSaveWiFi);
  server.on("/battery", handleBattery);
  
  server.begin();
}

void loop() {
  server.handleClient();
  yield();
}
