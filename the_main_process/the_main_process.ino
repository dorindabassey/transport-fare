/*
 * Automated bus ticketing system main sketch
 * this sketch shows how a user can pay for his or her bus fare using a mifare PICC card. 
 * this sketch enables a driver insert fare amount and a registered user scan his/her card 
 * afterwards fare deduction is being made, data is being posted to the server, connection to the server
 * is made, and php scripts has been put in place to compare data and update the database 


 * Hardware required:
 * ESP8266 
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * Liquid crystal display 
 
Hardware connections

ESP8266 ----------------MFRC522 READER-------------16*2LCD------------RESISTORS---------4*4 KEYPAD
A0---------------------------------------------------------------------1K ohm ------------R
RST                                                                 |_ 1K ohm  -----------R
GPIO16                                                              |_ 1k ohm  -----------R
GPIO14-------------------SCK                                        |_ 1k ohm-------------R
GPIO12-------------------MISO                                       |
GPIO13-------------------MOSI                                       |
GPIO15-------------------SS                                         |
3V3----------------------3V3                                        |
GPIO1                                                               |
GPIO3                                                               |
GPIO5-----------------------------------------------SCL             |
GPIO4-----------------------------------------------SDA             | |_ 4.7K ohm---------C
GPIO0--------------------RST                                        | |_ 4.7K ohm---------C
GPIO2                                                               | |_ 4.7K ohm---------C
GND----------------------GND------------------------GND------------_| |_ 4.7K ohm---------C
5V--------------------------------------------------5V---------------_|

created Sept 2019
by Bassey Dorinda Agala

*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h> //Library  for LCD Display

// Replace with your network credentials
//const char *ssid = "no nonsense";
//const char *pass = "foolishboy";
const char* ssid     = "SamsungS5 0708";
const char* pass = "dorianagala";
//WiFiClientSecure client;
//client.setInsecure();
// REPLACE with your Domain name and URL path or IP address with path
const char* host = "transpay.000webhostapp.com";
const uint16_t port = 443;
const char * path = "/transactionScript.php";
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keep this API Key value to be compatible with the PHP code provided in the project page.
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key
String apiKeyValue = "tPmAT5Ab3j7F9";
byte buffer2[18];
int valAbuf1;
String userId;
String userAmount;
int tripAmount;
String servertripAmount;
int adc_val;
int adc_va;
String key;
String keychar;
int number, num;
boolean result = false;
boolean res = false;

#define RST_PIN         0         // Configurable, see typical pin layout above
#define SS_PIN          15         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
boolean f3 = false;
boolean servee = false;

void setup() {

  Serial.begin(74880);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  lcd.begin();
  lcd.backlight();

  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("connecting...");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.println("CONNECTED");
  delay(1000);
}
void numplaceholder() {
  if (number == 0)
    number = num;//key pressed once
  else if
  (number = (number * 10) + num); //key pressed twice
  else if
  (number = (number * 100) + num); //key pressed thrice
  else
    keychar == "C";//if the ok function is pressed
  delay(300);
}
void keypadbuttons() {

  if (adc_val > 3500)
  {
    keychar = "A";//this char is the clear button
    lcd.clear();

    number = tripAmount = 0; result = false; //initialize variables
    delay(300);
  }
  if ( adc_val > 2500  && adc_val < 3500)
  {
    key = "3";
    lcd.print(key);
    num = 3;
    numplaceholder();
  }
  if ( adc_val > 1800  && adc_val < 2500)
  {
    key = "2";
    lcd.print(key);
    num = 2;
    numplaceholder();
  }
  if ( adc_val > 1400 && adc_val < 1800)
  {
    key = "1";
    lcd.print(key);
    num = 1;
    numplaceholder();
  }
  /*  if ( adc_val > 1100  && adc_val < 1300)
    { //OK is pressed
      keychar = "B";
      Serial.println("OK");
      tripAmount = number;//put the numbers gotten from the number variable into tripamt
      result = true;//exit this function
      delay(300);
    }*/
  if ( adc_val > 400  && adc_val < 452)
  {
    key = "0";
    lcd.print(key);
    num = 0;
    numplaceholder();
  }
  if ( adc_val > 865  && adc_val < 965)
  {
    key = "6";
    lcd.print(key);
    num = 6;
    numplaceholder();
  }
  if ( adc_val > 600 && adc_val < 646)
  {
    key = "5";
    lcd.print(key);
    num = 5;
    numplaceholder();
  }
  if ( adc_val > 740  && adc_val < 865)
  { //OK is pressed
    keychar = "C";
    lcd.print(" OK");
    tripAmount = number;//put the numbers gotten from the number variable into tripamt
    result = true;//exit this function
    delay(300);
  }

  if ( adc_val > 646  && adc_val < 674)
  {
    key = "9";
    lcd.print(key);
    num = 9;
    numplaceholder();
  }
  if ( adc_val > 674  && adc_val < 740)
  {
    key = "4";
    lcd.print(key);
    num = 4;
    numplaceholder();
  }
  if ( adc_val > 530 && adc_val < 600)
  {
    key = "9";
    lcd.print(key);
    num = 9;
    numplaceholder();
  }
  /*  if ( adc_val > 100  && adc_val < 105)
    {
      keychar = "D";
      Serial.println(keychar);
      delay(300);
    }
    /*if ( adc_val>95 && adc_val<100)
      {
       keychar = "#";
       Serial.println(keychar);
       delay(300);
      }*/
  if ( adc_val > 452  && adc_val < 500)
  {
    key = "7";
    lcd.print(key);
    num = 7;
    numplaceholder();
  }
  /* if ( adc_val>78  && adc_val<87)
    {
      keychar = "*";
      Serial.println(keychar);
      delay(300);
    }
  */
}
void showResult() {
  if (result == true) {
    Serial.print("deducted amount: ");
    Serial.println(tripAmount);
    lcd.setCursor(0, 0);
    lcd.print(tripAmount);
    lcd.print(" naira");
    Serial.println("scan a Transpay Tag to deduct user amount");
    lcd.setCursor(0, 1);
    lcd.print("scan tag ");
  }
}
void loop() {

  if (f3 == false) {
    Serial.println(F("Type User amount to be deducted ending with #"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("type useramount");
    lcd.setCursor(0, 1);
    lcd.println("and click OK(B)");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
  }
  adc_va = analogRead(A0); /* Read input from keypad */
  adc_val = adc_va * 4;
  if (adc_val > 100)
  {
    keypadbuttons();//allows user to type amount
  }
  if (result == true) {
    f3 = true;
  }
  showResult();// displays the result of the tripamt and useramount variable
  result = false;// stops the loop execution


  /*   if (tripAmount > 0) {
       Serial.println(tripAmount);
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.println(tripAmount);
       lcd.print(" naira");
       lcd.setCursor(0, 1);
       lcd.println("scan tag");
       delay(1000);
     }*/
  //scan a Transpay Tag to send userid and useramount to database
  // Serial.println("scan a Transpay Tag to deduct user amount");

  f3 = true;
  if (f3 == true) {
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
    //for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xDD;
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
      key.keyByte[i] = 0xDD;
      // keyB.keyByte[i] = 0xDD;
    }

    //some variables we need
    // byte block;
    byte block;
    byte len;
    MFRC522::StatusCode status;

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    Serial.println(F("**Card Detected:**"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Detected");
    Serial.print(F("User ID: "));

    block = 4;
    len = 18;

    //------------------------------------------- GET User ID
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    status = mfrc522.MIFARE_Read(block, buffer2, &len);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    //PRINT User ID
    for (uint8_t i = 0; i < 16; i++)
    {
      if (buffer2[i] != 32)
      {
        Serial.write(buffer2[i]);
      }
    }
    Serial.println(" ");

    //---------------------------------------- GET User AMOUNT

    Serial.print(F("Amount on the card: "));


    block = 9;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 8, &key, &(mfrc522.uid)); //line 834
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    // Show the old value of valueBlockA
    status = mfrc522.MIFARE_GetValue(block, &valAbuf1);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("mifare_GetValue() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("amount on card: ");
    lcd.setCursor(0, 1);
    lcd.print(valAbuf1); lcd.print(" - "); lcd.print(tripAmount);
    delay(3000);
    if (valAbuf1 <= 0) {
      Serial.println(F("YOUR ACCOUNT IS EMPTY PLEASE RECHARGE YOUR CARD."));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Account empty");
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
    // Decrement tripamount naira from the value of valueBlockA and store the result in valueBlockA.
    Serial.print("Subtracting tripAmount in naira from "); Serial.println(block);
    status = mfrc522.MIFARE_Decrement(block, tripAmount);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Decrement() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    status = mfrc522.MIFARE_Transfer(block);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Transfer() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    // Show the new value of valueBlockA
    status = mfrc522.MIFARE_GetValue(block, &valAbuf1);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("mifare_GetValue() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    Serial.print(F("New amount on card ")); Serial.print(block);
    Serial.print(F(" = ")); Serial.println(valAbuf1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("balance: "); lcd.print(valAbuf1);

    Serial.println(F("\n**End Reading**\n"));

    delay(500); //change value if you want to read cards faster


    userId = String((char *)buffer2);
    userAmount = String(valAbuf1);
    servertripAmount = String(tripAmount);
    Serial.println(userId);
    Serial.println(userAmount);

    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      BearSSL::WiFiClientSecure client;
      client.setInsecure();
      HTTPClient https;

      // Prepare your HTTP POST request data
      String httpRequestData = "api_key=" + apiKeyValue + "&cardID=" + userId + "&useramount=" + userAmount + "&tripamount=" + servertripAmount + "";
      Serial.print("httpRequestData: ");
      Serial.println(httpRequestData);
      if (servee == false) {


        Serial.println("conntecting to server..");
        if (https.begin(client, host, port, path)) {
          // Specify content-type header
          https.addHeader("Content-Type", "application/x-www-form-urlencoded");
          int httpsCode = https.POST(httpRequestData);
          if (httpsCode > 0) {
            Serial.println(httpsCode);
            if (httpsCode == HTTP_CODE_OK) {
              Serial.println(https.getString());
            }
          } else {
            Serial.print("failed to POST");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("lost conection");
            servee = false;
          }
        } else {
          Serial.print("failed to connect to server");
          servee = false;
        }
      }
      servee = true;
      https.end();
      Serial.println("connection over");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("amount deducted");
      delay(200);
    }
    if (servee == true) { //initializing the tripamount and number placeholdders back to zero
      number = 0;
      tripAmount = 0;
    }
    else {
      Serial.println("WiFi Disconnected");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("wifi disconnected");
      delay(200);
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    //Send an HTTP POST request every 10 seconds
    delay(5000);
  }
  f3 = false;
  servee = false;
}

