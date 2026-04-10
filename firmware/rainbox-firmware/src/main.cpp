#include <Arduino.h>
#include <FastLED.h>
#include <AsyncTCP.h>
#include <WiFi.h>

#include <ESPAsyncWebServer.h>

// neopixels
#define NEOPIXEL_BUILTIN  18
#define NUM_LEDS_BUILTIN  2

CRGB ledsB[NUM_LEDS_BUILTIN];

#define NEOPIXEL_STRIP    8
#define NUM_LEDS_STRIP    1 // change if you have one plugged in

CRGB ledsS[NUM_LEDS_STRIP];

// mist sensor
#define MIST_SENSOR       9

// water sensors
#define WATER_SENSOR_1    14
#define WATER_SENSOR_1PWR 21
#define WATER_SENSOR_2    2  // not used but convenient later if needed
#define WATER_SENSOR_2PWR 1
int water_level_1 = 0;
int water_level_2 = 0;

// water pump controller (DRV8874PWPR in PMODE)
#define WATER_EN_IN1      4
#define WATER_PH_IN2      5
#define WATER_SLEEP_N     6

// stepper controller
#define PLANE_DIR         38
#define PLANE_STEP        39
#define PLANE_ENABLE      40
#define PLANE_MOSI        10
#define PLANE_SPI_CLK     11
#define PLANE_CS          12
#define PLANE_MISO        13

boolean waterPlaneEnabled = true;

// wifi ap web server panel controller
static const char *htmlContent PROGMEM = R"(
<!DOCTYPE html>
<html>
  <head>
    <title>rainbox control panel</title>
  </head>
  <body>
    <h1>rainbox control panel</h1>

    <span>water level</span>
    <progress id="water level" value="0" max="533"> 32% </progress>

    <br />
    <br />

    <fieldset>
      <legend>Scheduling</legend>

      <div>
        <input type="checkbox" id="scheduled" name="scheduled" checked />
        <label for="scheduled">Active Schedule</label>
      </div>

      <div>
        <input
          type="time"
          id="time_start"
          name="time_start" />
        <label for="time_start">Start Time</label>
      </div>

      <div>
        <input
          type="time"
          id="time_end"
          name="time_end" />
        <label for="time_end">End Time</label>
      </div>

    </fieldset>

    <fieldset>
      <legend>Configuration</legend>

      <div>
        <input type="checkbox" id="rgb" name="rgb" checked />
        <label for="rgb">RGB Lights</label>
      </div>

      <div>
        <input type="checkbox" id="mist" name="mist" checked />
        <label for="mist">Mist</label>
      </div>

    </fieldset>

    <fieldset>
      <legend>Testing</legend>

      <div>
        <button onclick="sendMessage('tPump');">Enable Pump</button>
        <button onclick="sendMessage('fPump');">Disable Pump</button>
        <br />
        <button onclick="sendMessage('tWPlane');">Enable Water Plane</button>
        <button onclick="sendMessage('fWPlane');">Disable Water Plane</button>
        <br />
        <button onclick="sendMessage('tRGB');">Enable RGB</button>
        <button onclick="sendMessage('fRGB');">Disable RGB</button>
        <br />
        <button onclick="sendMessage('tMist');">Enable Mist</button>
        <button onclick="sendMessage('fMist');">Disable Mist</button>
      </div>

    </fieldset>
  </body>
  <script>
    var ws = new WebSocket('ws://192.168.4.1/ws');
    ws.onopen = function() {
      console.log("WebSocket connected");
    };
    ws.onclose = function() {
      console.log("WebSocket closed");
    };
    ws.onerror = function(error) {
      console.log("WebSocket error: " + error);
    };

    ws.onmessage = function(event) { // handle this <---------------------------
      console.log("WebSocket message: " + event.data);
    };

    function sendMessage(message) {
      console.log("WebSocket sent: " + message);
      ws.send(message);
    }
  </script>
</html>
)";
static const size_t htmlContentLength = strlen_P(htmlContent);

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

void setup() {
  Serial.begin(115200);
  
  // setup devices and ip
  FastLED.addLeds<NEOPIXEL, NEOPIXEL_BUILTIN>(ledsB, NUM_LEDS_BUILTIN);
  FastLED.addLeds<NEOPIXEL, NEOPIXEL_STRIP>(ledsS, NUM_LEDS_STRIP);

  pinMode(PLANE_DIR, OUTPUT);
  pinMode(PLANE_STEP, OUTPUT);
  pinMode(PLANE_ENABLE, OUTPUT);

  pinMode(MIST_SENSOR, OUTPUT);

  pinMode(WATER_EN_IN1, OUTPUT);
  pinMode(WATER_PH_IN2, OUTPUT);
  pinMode(WATER_SLEEP_N, OUTPUT);

  delayMicroseconds(500);

  // setup wifi connection for local webpanel
  #if ASYNCWEBSERVER_WIFI_SUPPORTED
    WiFi.mode(WIFI_AP);
    WiFi.softAP("esp-captive");
  #endif

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", (uint8_t *)htmlContent, htmlContentLength);
  });

  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    (void)len;

    if (type == WS_EVT_CONNECT) {
      Serial.println("ws connect");
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.println("ws disconnect");
    } else if (type == WS_EVT_ERROR) {
      Serial.println("ws error");
    } else if (type == WS_EVT_DATA) {
      AwsFrameInfo *info = (AwsFrameInfo *)arg;
      Serial.printf(
        "index: %" PRIu64 ", len: %" PRIu64 ", final: %" PRIu8 ", opcode: %" PRIu8 ", framelen: %d\n", info->index, info->len, info->final,
        info->message_opcode, len
      );

      //todo handling messages
    }
  });


  server.addHandler(&ws);
  server.begin();

  delayMicroseconds(500);







  // test mist for half a second
  digitalWrite(MIST_SENSOR, HIGH);
  delayMicroseconds(500);
  digitalWrite(MIST_SENSOR, LOW);
  delayMicroseconds(500);

  // test water sensor and neopixels
  for (int i = 0; i < 1200; i++) { // 30 seconds
    int mappedVal = map(readWaterLevel1(), 0, 533, 0, 255);
    ledsB[0] = CRGB(mappedVal, 255, 0);
    ledsB[1] = CRGB(mappedVal, 255, 0);
    FastLED.show();
    delayMicroseconds(25);
  }

  // test water plane (always starts in its enabled pos for reference)
  setWaterPlaneEnabled(false);
  delayMicroseconds(1000);
  setWaterPlaneEnabled(true);
  delayMicroseconds(1000);

  // test water pump
  digitalWrite(WATER_SLEEP_N, HIGH); //enable device
  delayMicroseconds(100);
  analogWrite(WATER_EN_IN1, 255); // swap en and ph if pump is going backwards
  digitalWrite(WATER_PH_IN2, LOW);
  delayMicroseconds(1000);
  digitalWrite(WATER_SLEEP_N, LOW); // coast and disable everything
  digitalWrite(WATER_EN_IN1, LOW);
  digitalWrite(WATER_PH_IN2, LOW);

}

void loop() {
  delay(100);
}

//https://www.blue-pcb.com/products/9b30000-rain-water-level-sensor-module-detection-liquid-surface-depth-height-for-arduino
// reads only periodically to avoid oxidization https://www.reddit.com/r/arduino/comments/5plgrm/comment/dcs22lp
int readWaterLevel1() { // between 0-533
  digitalWrite(WATER_SENSOR_1PWR, HIGH);
  delay(10);
  water_level_1 = analogRead(WATER_SENSOR_1);
  digitalWrite(WATER_SENSOR_1PWR, LOW);
  return water_level_1;
}

int readWaterLevel2() { // between 0-533
  digitalWrite(WATER_SENSOR_2PWR, HIGH);
  delay(10);
  water_level_2 = analogRead(WATER_SENSOR_2);
  digitalWrite(WATER_SENSOR_2PWR, LOW);
  return water_level_2;
}

void setWaterPlaneEnabled(boolean on) { // todo in prod: check direction + time needed
  if (on && !waterPlaneEnabled) {
    digitalWrite(PLANE_DIR, HIGH);
    digitalWrite(PLANE_STEP, HIGH);
  } else if (!on && waterPlaneEnabled) {
    digitalWrite(PLANE_DIR, LOW);
    digitalWrite(PLANE_STEP, HIGH);
  }

  delayMicroseconds(500);

  // turn off
  digitalWrite(PLANE_STEP, LOW);

  waterPlaneEnabled = on;
}