//Nodemcu ESP8266 WIFI control car with the New Blynk app.

// Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Define the motor pins
#define IN1 D0
#define IN2 D1
#define IN3 D2
#define IN4 D3
#define ENA D5
#define ENB D6

// Variables for the Blynk widget values
int x = 50;
int y = 50;
int s = 200;
int sensor = 1;

char auth[] = "aK1hST72UxMOebAggcM2Ds2EdS8MA36g"; //Enter your Blynk auth token
char ssid[] = "Aryan"; //Enter your WIFI name
char pass[] = "#aryan1809"; //Enter your WIFI passowrd


void setup() {
  Serial.begin(115200);
  //Set the motor pins as output pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  // Initialize the Blynk library
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Blynk.virtualWrite(V3, 0);
}

// Get the joystick values
BLYNK_WRITE(V0) {
  x = param[0].asInt();
}
// Get the joystick values
BLYNK_WRITE(V1) {
  y = param[0].asInt();
}
// Get the speed slide values
BLYNK_WRITE(V2) {
  s = param[0].asInt();
}

// Check these values using the IF condition
void smartcar() {
  if (y > 70) {
    carForward();
    Serial.println("carForward");
  } else if (y < 30) {
    carBackward();
    Serial.println("carBackward");
  } else if (x < 30) {
    carLeft();
    Serial.println("carLeft");
  } else if (x > 70) {
    carRight();
    Serial.println("carRight");
  } else if (x < 70 && x > 30 && y < 70 && y > 30) {
    carStop();
    Serial.println("carstop");
  }
}
  
 

void loop() {
  Blynk.run();// Run the blynk function
  smartcar();// Call the main function
}

//Motor movement functions
void carForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA,s);
  analogWrite(ENB,s);
}
void carBackward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA,s);
  analogWrite(ENB,s);
}
void carLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA,s/2);
  analogWrite(ENB,s);
}
void carRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA,s);
  analogWrite(ENB,s/2);
}
void carStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA,0);
  analogWrite(ENB,0);
}
