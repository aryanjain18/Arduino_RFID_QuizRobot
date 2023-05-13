/*
 * ------------------------------------------------
 *             MFRC522      Arduino       NodeMCU
 *             Reader/PCD   Uno/101       
 * Signal      Pin          Pin          
 --------------------------------------------------
 * RST/Reset   RST          9              D3
 * SPI SS      SDA(SS)      10             D4
 * SPI MOSI    MOSI         11 / ICSP-4    D7
 * SPI MISO    MISO         12 / ICSP-1    D6
 * SPI SCK     SCK          13 / ICSP-3    D5

 */

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WebServer.h>
#include <TM1637Display.h>

#define RST_PIN  D3         // Configurable, see typical pin layout above
#define SS_PIN   D4         // Configurable, see typical pin layout above
#define BUZZER   D8 

//TM1637 Module connection pins (Digital Pins)
#define CLK D1
#define DIO D2

/*Put WiFi SSID & Password*/
const char* ssid = "Aryan";   // Enter SSID here
const char* password = "#aryan1809"; // Enter Password here

ESP8266WebServer server(80);

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

MFRC522::MIFARE_Key key;  
MFRC522::StatusCode status;      

TM1637Display display(CLK, DIO);

int blockNum = 2;  //Data stored in RFID Card Block 2
int score = 0;
int flag = 0;
/* Length of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];
String data2; 

String updateWebpage(uint8_t status, int score, String data2);

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  // while (!Serial);      // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  
  delay(100);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check NodeMCU is connected to Wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/on", handle_on);
  server.on("/off", handle_off);
  server.on("/addscore", handle_addscore);
  server.on("/subscore", handle_subscore);
  server.on("/update", handle_refresh);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP Server Started");

  // MFRC522
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522 module
  /* Set BUZZER as OUTPUT */
  pinMode(BUZZER, OUTPUT);
  display.setBrightness(0x0f);
// Check Firmware At Start Once
  Serial.println(F("*****************************"));
  Serial.println(F("MFRC522 Digital self test"));
  Serial.println(F("*****************************"));
  mfrc522.PCD_DumpVersionToSerial();  // Show version of PCD - MFRC522 Card Reader
  Serial.println(F("Performing test..."));
  bool result = mfrc522.PCD_PerformSelfTest(); // perform the test
  Serial.print(F("Result: "));
if (result) 
{
    Serial.println(F("OK, Game Setup Starts"));
    Serial.println(F("-----------------------------"));
    display.showNumberDec(score, false);
}
else 
{
Serial.println(F("MFRC522 Faulty - ERROR"));
Serial.println(F("-----------------------------"));
display.showNumberDec(8888, false);
delay(100000);
}
Serial.println();

}

String updateWebpage(uint8_t status, int score, String data2) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Master Control</title>\n";
  ptr += "<style>html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: inline-block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 10px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #3498db;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += ".button-add {background-color: #4CAF50;}\n";
  ptr += ".button-sub {background-color: #f44336;}\n";
  ptr += ".button-update {background-color: #979c95;}\n";

  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>RFID based IoT Quiz Game Robot</h1>\n";
  ptr += "<h3>Master Control using Station (STA) Mode</h3>\n";

  ptr += "<h2>Score: <span id=\"score\">" + String(score) + "</span></h2>\n"; // Dynamically updates the score using JavaScript
  ptr += "<h5>Last data read from Card - <span id=\"data\">" + String(data2) + "</span></h5>\n"; // Dynamically updates the data2 using JavaScript
  if(status){
    ptr +="<p>Status: Game ON!</p><a class=\"button button-off\" href=\"/off\">Sensor OFF</a>\n";
  }
  else{
    ptr +="<p>Status: Sensor OFF</p><a class=\"button button-on\" href=\"/on\">Sensor ON</a>\n";
  }
  ptr += "<a class=\"button button-add\" href=\"/addscore\">Score +10</a>\n";
  ptr += "<a class=\"button button-sub\" href=\"/subscore\">Score -10</a>\n";
  ptr += "<a class=\"button button-update\" href=\"/update\">Refresh</a>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void loop()
{
  server.handleClient();

  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  /* Read data from the same block */
  Serial.println();
  Serial.println(F("Reading last data from RFID..."));
  ReadDataFromBlock(blockNum, readBlockData);
  
  /* Print the data read from block */
  Serial.println();
  Serial.print(F("Last data in RFID:"));
  Serial.print(blockNum);
  Serial.print(F(" --> "));
  for (int j=0 ; j<16 ; j++)
  {
    Serial.write(readBlockData[j]);
  }
  
    Serial.println();
    Serial.println("Card Is Successfully Read");  
    data2 = String((char*)readBlockData);
    data2.trim();
    Serial.println(data2);
    updateWebpage(status, score, data2);

if (flag == 1)
{
    if (data2 == "TRUE")
    {  
      score = score + 10;
      display.showNumberDec(score, false);
      Serial.println();
      Serial.println("TRUE Card Is Successfully Collected");
      Serial.println(score);
      updateWebpage(status, score, data2); 
      Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  delay(200);
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  delay(4000);
      }
    else if (data2 == "FALSE")
    {  
      score = score - 10;
      display.showNumberDec(score, false);
      Serial.println();
      Serial.println("False Card Is Successfully Collected");
      Serial.println(score);
      updateWebpage(status, score, data2);
      Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(200);
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(200);
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(4000);
      }
}
else { //Flag == 0
    Serial.println();
    Serial.println("Card Is NOT Collected");
    updateWebpage(status, score, data2); 
    delay(3000);
    }
}

    

void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{ 
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  /* Authenticating the desired data block for Read access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK)
  {
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");  
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1000);
}

void handle_OnConnect() {
  Serial.println("Client Connected");
  server.send(200, "text/html", updateWebpage(flag,score," ")); 
}

void handle_on() {
  flag = 1;
  display.showNumberDec(score, false);
  Serial.println();
  Serial.println("Game Now Starts");
  Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  delay(700);
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(1000);
Serial.println("State: ON");
server.send(200, "text/html", updateWebpage(flag,score,data2)); 
}

void handle_off() {
  flag = 0;
  display.showNumberDec(score, false);
  Serial.println();
  Serial.println("Game Now Starts");
  Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(1000);
  Serial.println("State: OFF");
  server.send(200, "text/html", updateWebpage(flag,score,data2)); 
}

void handle_addscore() {
  score = score + 10;
  display.showNumberDec(score, false);
  Serial.println();
  Serial.println(score);
  Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  delay(500);
  server.send(200, "text/html", updateWebpage(flag,score,data2)); 
}
void handle_subscore() {
  score = score - 10;
  display.showNumberDec(score, false);
  Serial.println();
  Serial.println(score);
  Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  delay(500);
  server.send(200, "text/html", updateWebpage(flag,score,data2)); 
}

void handle_refresh() {
  server.send(200, "text/html", updateWebpage(flag,score,data2));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

