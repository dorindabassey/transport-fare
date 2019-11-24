
/*
 * 
 * 
 * HI TPAY  EMPLOYEES
 * This code writes user ID to block 5 and 6 in sector 1
 * this code formats block 9 as value block if not already formatted
 * this code writes user AMOUNT to block 9 
 * 
 * -----------------------------------------------------------------------------------------
 *             MFRC522                    ESP8266 
 *             Reader/PCD                 D1 MINI
 * Signal      Pin                        Pin       
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST                        0
 * SPI SS      SDA(SS)                    15
 * SPI MOSI    MOSI                       13
 * SPI MISO    MISO                       12
 * SPI SCK     SCK                        14
 *
 * Hardware required:
 * ESP8266 
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * Liquid crystal display 
 * 
 * Created June 2019
 * by Bassey Dorinda Agala
 */

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h> //Library  for LCD Display

#define RST_PIN         0          // Configurable, see typical pin layout above
#define SS_PIN          15       // Configurable, see typical pin layout above
LiquidCrystal_I2C lcd(0x27, 16, 2);

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(74880);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  lcd.backlight();
  lcd.begin();    //Initializing LCD 16x2
  mfrc522.PCD_Init();        // Init MFRC522 card
  Serial.println(F("Write personal data on MIFARE PICC "));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Write User ID ");
  lcd.setCursor(0, 1);
  lcd.println("on the serial interface");

   for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xDD;
    }

  
  Serial.println(F("BEWARE: Data will be written to the PICC, in sector 1 and 2"));
}
/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Ensure that a given block is formatted as a Value Block.
 */
void formatValueBlock(byte blockAddr) {
    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::StatusCode status;

    Serial.print(F("Reading block ")); Serial.println(blockAddr);
    status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    if (    (buffer[0] == (byte)~buffer[4])
        &&  (buffer[1] == (byte)~buffer[5])
        &&  (buffer[2] == (byte)~buffer[6])
        &&  (buffer[3] == (byte)~buffer[7])

        &&  (buffer[0] == buffer[8])
        &&  (buffer[1] == buffer[9])
        &&  (buffer[2] == buffer[10])
        &&  (buffer[3] == buffer[11])

        &&  (buffer[12] == (byte)~buffer[13])
        &&  (buffer[12] ==        buffer[14])
        &&  (buffer[12] == (byte)~buffer[15])) {
        Serial.println(F("Block has correct Value Block format."));
    }
    else {
        Serial.println(F("Formatting as Value Block..."));
        byte valueBlock[] = {
            0, 0, 0, 0,
            255, 255, 255, 255,
            0, 0, 0, 0,
            blockAddr, ~blockAddr, blockAddr, ~blockAddr };
        status = mfrc522.MIFARE_Write(blockAddr, valueBlock, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
    }
}

void loop() {

  //set trailer buffer, at delivery all buffers are set to 
//  byte trailerBuffer[] = { 255,255,255,255,255,255,0,0,0,0,255,255,255,255,255,255}; am coming
  //byte trailerBlock = 7;

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("Card UID:"));    //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print(F(" PICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  int valAbuf;
// In this sample we use the second sector,
// that is: sector #8, covering block #16 up to and including block #19
  byte sector         = 2;
  byte valueBlockA    = 9;
  byte valueBlockB    = 10;
  byte trailerBlock   = 11;
    byte size = sizeof(buffer);
//  int32_t value;  
  MFRC522::StatusCode status;
  byte len;

  Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial
  // Ask user ID
  Serial.println(F("Type User ID, ending with #"));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Type User ID ");
  lcd.setCursor(0, 1);
  lcd.println(" ending with #");
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; // read family name from serial
  for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces

  block = 4;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("failed authen");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));
        lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("authen success");

  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("failed write");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
        lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("write success");

  block = 5;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("failed authen");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("failed write");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("write success");
      
   // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();
    
    // We need a sector trailer that defines blocks 5 and 6 as Value Blocks and enables key B
    // The last block in a sector (block #3 for Mifare Classic 1K) is the Sector Trailer.
    // See http://www.nxp.com/documents/data_sheet/MF1S503x.pdf sections 8.6 and 8.7:
    //      Bytes 0-5:   Key A
    //      Bytes 6-8:   Access Bits
    //      Bytes 9:     User data
    //      Bytes 10-15: Key B (or user data)
    byte trailerBuffer[] = {
        221, 221, 221, 221, 221, 221, //eep default key A
        0, 0, 0,
        0,
        221, 221, 221, 221, 221, 221};      // Keep default key B
    // The access bits are stored in a peculiar fashion.
    // There are four groups:
    //      g[0]    Access bits for block 0 (for sectors 0-31)
    //              or blocks 0-4 (for sectors 32-39)
    //      g[1]    Access bits for block 1 (for sectors 0-31)
    //              or blocks 5-9 (for sectors 32-39)
    //      g[2]    Access bits for block 2 (for sectors 0-31)
    //              or blocks 10-14 (for sectors 32-39)
    //      g[3]    Access bits for the Sector Trailer: block 3 (for sectors 0-31)
    //              or block 15 (for sectors 32-39)
    // Each group has access bits [C1 C2 C3], in this code C1 is MSB and C3 is LSB.
    // Determine the bit pattern needed using MIFARE_SetAccessBits:
    //      g0=0    access bits for block 0 (of this sector) using [0 0 0] = 000b = 0
    //              which means key A|B have r/w for block 0 of this sector
    //              which (in this example) translates to block #4 within sector #1;
    //              this is the transport configuration (at factory delivery).
    //      g1=6    access bits for block 1 (of this sector) using [1 1 0] = 110b = 6
    //              which means block 1 (of this sector) is used as a value block,
    //              which (in this example) translates to block #5 within sector #1;
    //              where key A|B have r, key B has w, key B can increment,
    //              and key A|B can decrement, transfer, and restore.
    //      g2=6    same thing for block 2 (of this sector): set it to a value block;
    //              which (in this example) translates to block #6 within sector #1;
    //      g3=3    access bits for block 3 (of this sector): the Sector Trailer here;
    //              using [0 1 1] = 011b = 3 which means only key B has r/w access
    //              to the Sector Trailer (block 3 of this sector) from now on
    //              which (in this example) translates to block #7 within sector #1;
    mfrc522.MIFARE_SetAccessBits(&trailerBuffer[6], 0, 6, 6, 3);

    // Read the sector trailer as it is currently stored on the PICC
    Serial.println(F("Reading sector trailer..."));
    status = mfrc522.MIFARE_Read(trailerBlock, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    // Check if it matches the desired access pattern already;
    // because if it does, we don't need to write it again...
    if (    buffer[6] != trailerBuffer[6]
        ||  buffer[7] != trailerBuffer[7]
        ||  buffer[8] != trailerBuffer[8]) {
        // They don't match (yet), so write it to the PICC
        Serial.println(F("Writing new sector trailer..."));
        status = mfrc522.MIFARE_Write(trailerBlock, trailerBuffer, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
        }
    }

    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // A value block has a 32 bit signed value stored three times
    // and an 8 bit address stored 4 times. Make sure that valueBlockA
    // and valueBlockB have that format (note that it will only format
    // the block when it doesn't comply to the expected format already).
    formatValueBlock(valueBlockA);
    formatValueBlock(valueBlockB);
       // Ask personal data: Transport recharge amount in naira

    Serial.setTimeout(60000L) ;     // wait until 20 seconds for input from serial
     Serial.println(F("Type recharge amount, ending with #"));
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.println("Type amount ");
        //lcd.setCursor(0, 1);
       //lcd.println(" ending with #");
   //  len = Serial.readBytesUntil('#', (char *) buffer, 6) ; // read recharge amount from serial
     // for (byte i = len; i < 6; i++) buffer[i] = ' ';     // pad with spaces
      while (!Serial.available());
      {
        valAbuf = Serial.parseInt();
      }
      if (valAbuf > 0){
        Serial.println(valAbuf);
      }
      lcd.setCursor(0,1);
      lcd.write(valAbuf);

    // Add 2500 to the value of valueBlockA and store the result in valueBlockA.
    Serial.print("Adding recharge amount in NAIRA to value of block "); Serial.println(valueBlockA);
    status = mfrc522.MIFARE_Increment(valueBlockA, valAbuf);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Increment() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    status = mfrc522.MIFARE_Transfer(valueBlockA);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Transfer() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    // Show the new value of valueBlockA
    status = mfrc522.MIFARE_GetValue(valueBlockA, &valAbuf);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("mifare_GetValue() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    Serial.print("New value of value block "); Serial.print(valueBlockA);
    Serial.print(" = "); Serial.println(valAbuf);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("New amount: ");
    lcd.setCursor(0, 1);
    lcd.write(valAbuf);

 /*  // Decrement 50 naira from the value of valueBlockA and store the result in valueBlockA.
    Serial.print("Subtracting 50 naira from value of block "); Serial.println(valueBlockA);
    status = mfrc522.MIFARE_Decrement(valueBlockA, 2147349528);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Decrement() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    status = mfrc522.MIFARE_Transfer(valueBlockA);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Transfer() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    // Show the new value of valueBlockA
    status = mfrc522.MIFARE_GetValue(valueBlockA, &valAbuf);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("mifare_GetValue() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    Serial.print(F("New value of value block ")); Serial.print(valueBlockA);
    Serial.print(F(" = ")); Serial.println(valAbuf);
    */
    // Check some boundary...
    if (valAbuf <= -10) {
      Serial.println(F("YOUR ACCOUNT IS EMPTY PLEASE RECHARGE YOUR CARD."));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("Account empty");
    }
  Serial.println(" ");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("successful load");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

}
