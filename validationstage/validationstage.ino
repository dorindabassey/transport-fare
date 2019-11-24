/*
 * this sketch only deducts user amount from value blocks
 created Aug 2019 
 by 
 Bassey Dorinda Agala
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
const char * path = "/aug10a.php";
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
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("connecting...");
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.println("CONNECTED");
  delay(1000);
}
void loop() {
  if (f3 == false) {
    Serial.setTimeout(60000L);
    Serial.println(F("Type User amount to be deducted ending with #"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("type useramount");
    lcd.setCursor(0, 1);
    lcd.println("and click OK(B)");
    delay(1000);
    while (!Serial.available());
    {
      tripAmount = Serial.parseInt();
    }
    if (tripAmount > 0) {
      Serial.println(tripAmount);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println(tripAmount);
      lcd.print(" naira");
      lcd.setCursor(0, 1);
      lcd.println("scan tag");
      delay(1000);
    }
    //scan a Transpay Tag to send userid and useramount to database
    Serial.println("scan a Transpay Tag to deduct user amount");
  }
  f3 = true;
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
  if (valAbuf1 <= 0) {
    Serial.println(F("YOUR ACCOUNT IS EMPTY PLEASE RECHARGE YOUR CARD."));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Account empty");
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
  /*// Serial.print("amount on the card ");
    Serial.print(block);
    Serial.print(" = "); Serial.println(valAbuf1);
  */

  Serial.println(F("\n**End Reading**\n"));

  delay(500); //change value if you want to read cards faster


  userId = String((char *)buffer2);
  userAmount = String(valAbuf1);
  servertripAmount = String(tripAmount);
  Serial.println(userId);
  Serial.println(userAmount);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("userId");
  lcd.setCursor(0, 1);
  lcd.println(userId);
  delay(200);
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    BearSSL::WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&userid=" + userId + "&useramount=" + userAmount + "&tripamount=" + servertripAmount + "";
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
  f3 = false;
}

