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

#include <SPI.h>
#include <MFRC522.h>
#include <TM1637Display.h>

#define RST_PIN  9          // Configurable, see typical pin layout above
#define SS_PIN   10         // Configurable, see typical pin layout above
#define BUZZER   8 

//TM1637 Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3

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

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);      // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
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
    display.showNumberDec(8888, false);
}
else 
{
Serial.println(F("MFRC522 Faulty - ERROR"));
Serial.println(F("-----------------------------"));
display.showNumberDec(1111, false);
delay(100000);
}
Serial.println();
}

void loop()
{
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
  /* If you want to print the full memory dump, uncomment the next line */
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  
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
    Serial.println("Card Is Successfully Collected");  
    data2 = String((char*)readBlockData);
    data2.trim();
    Serial.println(data2);
  
if (data2 == "START")
    {  
      flag = 1;
      display.showNumberDec(score, false);
      Serial.println();
      Serial.println("Master Card Scanned, Game Now Starts");
      Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  delay(730);
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(1000);
      }

if (data2 == "ADD")
    {  
      score = score + 10;
      display.showNumberDec(score, false);
      Serial.println();
      Serial.println(score);
      Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  delay(500);
      }
    else if (data2 == "SUBT")
    {  
      score = score - 10;
      display.showNumberDec(score, false);
      Serial.println();
      Serial.println(score);
      Serial.println();
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  delay(500);
      }

if (flag == 1)
{
    if (data2 == "TRUE")
    {  
      score = score + 10;
      display.showNumberDec(score, false);
      Serial.println();
      Serial.println(score);
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
      Serial.println(score);
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
  else {
    Serial.println();
    Serial.println("Card Is NOT Collected");
    delay(2000);
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
