/* this sketch send user data credentials to the web for newly registered users, it sends the user details 
 *  to a php script that inserts the data to a table accordingly,

created July 2019
by BASSEY DORINDA AGALA

*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>


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
const char * path = "/newesp.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page.
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key
String apiKeyValue = "tPmAT5Ab3j7F9";
byte buffer2[18];
int valAbuf1;
String incomingName;
String incomingEmail;
String userId;
String userAmount;

#define RST_PIN         0         // Configurable, see typical pin layout above
#define SS_PIN          15         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
boolean f2 = false;

void setup() {

  Serial.begin(74880);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card

  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  // Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to

}

//void readcard() {

void loop() {

  if (f2 == false) {
    Serial.setTimeout(60000L);
    Serial.println(F("Type User name, ending with #"));
    //while (!Serial.available()){
    incomingName = Serial.readStringUntil('#');
    Serial.println("name: ");
    Serial.println(incomingName);

    Serial.setTimeout(100000L);
    Serial.println(F("Type User email, ending with #"));
    // while (!Serial.available()){
    incomingEmail = Serial.readStringUntil('#');
    Serial.println("email: ");
    Serial.println(incomingEmail);
    Serial.println("scan a Transpay Tag to send userid and useramount to database");
  }
  f2 = true;

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  //for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xDD;
  for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
    key.keyByte[i] = 0xDD;
    // keyB.keyByte[i] = 0xDD;
  }

  //some variables we need
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
  // Show the new value of valueBlockA
  status = mfrc522.MIFARE_GetValue(block, &valAbuf1);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("mifare_GetValue() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  // Serial.print("amount on the card ");
  Serial.print(block);
  Serial.print(" = "); Serial.println(valAbuf1);

  Serial.println(F("\n**End Reading**\n"));

  delay(500); //change value if you want to read cards faster


  userId = String((char *)buffer2);
  userAmount = String(valAbuf1);
  Serial.println(userId);
  Serial.println(userAmount);
  //Check WiFi connection status

  //  READ FROM HERE!!!
  if (WiFi.status() == WL_CONNECTED) {
    BearSSL::WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&name=" + incomingName + "&email=" + incomingEmail + "&userid=" + userId + "&useramount=" + userAmount + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

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
        Serial.print("failed to GET");
      }
    } else {
      Serial.print("failed to connect to server");
    }

    https.end();
    Serial.println("connection over");
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  //Send an HTTP POST request every 10 seconds
  delay(10000);
  f2 = false;
}
