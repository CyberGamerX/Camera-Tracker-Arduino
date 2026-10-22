#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include "arduino_secrets.h"
#include <Arduino_LSM6DS3.h>
#include "Vector3.h"
#include <MadgwickAHRS.h>

Madgwick filter;

char ssid[] = WIFI_SSID;  // your WiFi network
char pass[] = WIFI_PASS;  // your WiFi password

int status = WL_IDLE_STATUS;

WiFiUDP udp;
WiFiClient client;

unsigned int udpPort = 8888;  // UDP port for listening for IP
unsigned int tcpPort = 9999;

char incomingIP[64];
IPAddress targetIP;
bool gotIP = false;

// Timing
long previousTime = 0;
long currentTime = 0;

long fixedUpdateTime = 33333;
long fixedIntervalTime = 0;

long gyroTimeElapsed = 0;
long accelTimeElapsed = 0;

Vector3 position(0, 0, 0);
Vector3 rotation(0, 0, 0);

bool calibrateGyro = true;
int gyroCalibrationCycle = 0;
float gyroCalibrationLimit = 500;

Vector3 gyroCalibration(0, 0, 0);

float gyroNullRange = 0.04;

bool calibrateAccel = true;
Vector3 speed(0, 0, 0);
Vector3 speedOffset(0, 0, 0);
Vector3 gravity(0, 0, 0);

Vector3 IMUAcceleration(0, 0, 0);

int accelCalibrationCycle = 0;
float accelCalibrationLimit = 500;
float accelNullRange = 0.0;

// Global smoothing constants (0 = no smoothing, 1 = no change)
float alphaAccel = 0.8f;  // Lower = more smoothing
float alphaGyro  = 0.8f;

float smoothedAx = 0, smoothedAy = 0, smoothedAz = 0;
float smoothedGx = 0, smoothedGy = 0, smoothedGz = 0;


float ax, ay, az;
float gx, gy, gz;

bool start = false;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module failed!");
    while (true)
      ;
  }

  // Wifi Connection System
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }

  Serial.println("Connected to WiFi");

  // Start Listener
  udp.begin(udpPort);
  Serial.println("Listening for Unity IP broadcast...");

  startIMU();
  filter.begin(104.0);
}

void loop() {
  currentTime = micros();

  TelemetryUpdate();

  if (currentTime - fixedIntervalTime >= fixedUpdateTime) {
    FixedUpdate();
    fixedIntervalTime += fixedUpdateTime;
  }

  previousTime = currentTime;
}

void FixedUpdate() {
  // Get IP Address
  if (!gotIP) {
    int packetSize = udp.parsePacket();
    if (packetSize) {
      int len = udp.read(incomingIP, 63);
      if (len > 0) {
        incomingIP[len] = '\0';
        Serial.print("Received Unity IP: ");
        Serial.println(incomingIP);
        gotIP = targetIP.fromString(incomingIP);
      }
    }
  }

  // Get TCP Connection
  if (gotIP && !client.connected()) {
    if (client.connect(targetIP, tcpPort)) {
      Serial.println("Connected to Unity via TCP");
    } else {
      Serial.println("TCP connection failed, retrying...");
    }
  }

  if (client.connected()) {

    // Handle any incoming JSON messages
    if (client.available()) {
      String incoming = client.readStringUntil('\n');  // Expect JSON line ended with '\n'

      JSONVar parsed = JSON.parse(incoming);

      if (JSON.typeof(parsed) == "undefined") {
        Serial.println("Failed to parse incoming JSON");
        start = true;
      } else {
        Serial.print("Received JSON: ");
        Serial.println(JSON.stringify(parsed));
        start = true;
        // Json Intercept
      }
    }
  }
}

void TelemetryUpdate() {
  if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable()) {
    

    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);  // Swizzle if needed for orientation match

    sendTelemetry();  // Regularly send outgoing JSON

    smoothedAx = alphaAccel * ax + (1 - alphaAccel) * smoothedAx;
    smoothedAy = alphaAccel * ay + (1 - alphaAccel) * smoothedAy;
    smoothedAz = alphaAccel * az + (1 - alphaAccel) * smoothedAz;

    smoothedGx = alphaGyro * gx + (1 - alphaGyro) * smoothedGx;
    smoothedGy = alphaGyro * gy + (1 - alphaGyro) * smoothedGy;
    smoothedGz = alphaGyro * gz + (1 - alphaGyro) * smoothedGz;

    // Feed smoothed values into Madgwick
    filter.updateIMU(smoothedGx, smoothedGy, smoothedGz, smoothedAx, smoothedAy, smoothedAz);

    // Store acceleration for velocity/position calculation
    IMUAcceleration.x = ax;
    IMUAcceleration.y = ay;
    IMUAcceleration.z = az;
  }
}

void sendTelemetry() {
  JSONVar packet;

  JSONVar accel;
  JSONVar gyro;
  accel["x"] = ax;
  accel["y"] = ay;
  accel["z"] = az;
  // Orientation in quaternion
  gyro["x"] = gx;
  gyro["y"] = gy;
  gyro["z"] = gz;

  packet["accel"] = accel;
  packet["gyro"] = gyro;
  packet["timecode"] = millis();
  packet["start"] = start;

  String jsonString = JSON.stringify(packet);

  int len = jsonString.length();
  client.write((uint8_t*)&len, 4);
  client.print(jsonString);

  if(start)
  {
    start = false;
  }

  //Serial.print("Sent: ");
  //Serial.println(jsonString);
}

void startIMU() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  Serial.print("IMU initialized at ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
}