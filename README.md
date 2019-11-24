# dorianagala
c programmer
 this sketch shows how a user can pay for his or her bus fare using a mifare PICC card. 
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

the resistors were put in place such that once a key is pressed a particular voltage is read on the A0 pin of the ESP8266

the libraries included can be gotten from github repositories, search them online and download.
