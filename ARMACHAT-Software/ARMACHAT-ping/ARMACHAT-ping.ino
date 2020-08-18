#include <Keypad.h>
//#include <RTCZero.h>
//#include "Keyboard.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>              // include libraries
#include <LoRa.h>
// Include EEPROM-like API for FlashStorage
#include <FlashAsEEPROM.h>
#include <Fonts/Org_01.h>
#include <Fonts/Picopixel.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>

//#define POCKET  //discontinued, support removed
//#define DESKTOP //discontinued, support removed
#define FUSION

#define LCDTFT //Display ST7735
//#define LCDIPS //Display ST7789

#include "config.h"

/* Change these values to set the current initial time */
//__attribute__ ((section (".noinit"))) 
 byte  second;
 byte  minute;
 byte  hour;

/* Change these values to set the current initial date */
 byte  day;
 byte  month;
 int  year;
String incoming = "";
String outgoing;              // outgoing message
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 1;     // address of this device
byte destination = 255;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends
byte recipient = 1;      // destination to send to

// Create key layouts
Keypad directLayout( makeKeymap(directKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins) );
Keypad shiftLayout( makeKeymap(shiftKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins) );
Keypad symbolLayout( makeKeymap(symbolKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins) );
byte Layout = 0;
String editorText = "";   // text for editor

byte cursor = 0;

byte menu = 0; // main menu state
byte menuPage = 0; // menu page
byte refreshMenu = 1; // refresh main MENU screen
byte lastUnread = 0; //
byte alive = 0; // received power on status > resend undelivered messages
//************************************************************************************************
// Config structure ee.A2melody
// ID of the settings block
#define CONFIG_VERSION "ls1"
// Tell it where to store your config data in EEPROM
#define CONFIG_START 0
struct StoreStruct {
  // The variables of your settings
  byte hour;
  byte minute;
  byte second;

  byte day;
  byte month;
  byte year;

  byte freq;
  byte power;
  byte spread;
  byte myID;
  byte myGroup;
    
  byte sound;
  byte brightness;
  
  byte sleepTime;

  char version_of_program[4]; // it is the last variable of the struct
} 
ee = {
  // The default values
  0, // hour;
  0, // minute;
  0, // second;

  15, // day;
  4, // month;
  20, // year;

  1, // freq;
  20, // power;
  9, // spread;
  1, // myid;
  1, // myGroup;
   
  1, // sound;
  5, // brightnes;
  
  0, // sleepTime;
  CONFIG_VERSION
};

//************************************************************************************************
// struct message data MEMORY RAM
 
struct msgstruct { 
    int id;
    char type; 
    byte recipient; 
    byte sender; 
    int rssi;
    int snr;
    char text[100]; 
};
struct msgstruct message[100];  
int msgIndex=0;

char buffer[150];

char addresBook[10][9]=
{
    "All-----",
    "Alpha---",
    "Charlie-",
    "Oscar---",
    "Sierra--",
    "Yankee--",
    "Romeo---",
    "Tango---",
    "Victor--",
    "Zulu----"
};

//************************************************************************************************
void setup() {
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  directLayout.begin( makeKeymap(directKeys) );
  shiftLayout.begin( makeKeymap(shiftKeys) );
  symbolLayout.begin( makeKeymap(symbolKeys) );
  analogWriteResolution(8);  

  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT,HIGH); // Backlight on
  pinMode(speaker, OUTPUT);
  digitalWrite(speaker, LOW);
  playTone( 10, 1000);  
  #ifdef LCDTFT
  tft.initR(INITR_BLACKTAB);        // Initialize ST7735R screen
  tft.setRotation(1);
#endif

#ifdef LCDIPS
  tft.init(240, 240, SPI_MODE2);   // Init ST7789 240x240
  tft.setRotation(3);
#endif
  
  tft.enableDisplay(true);
  clearScreen();
  tft.setFont();
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.setTextColor(yellow);
  tft.println("Loading:");
  tft.setTextColor(white);
    
  if (!EEPROM.isValid()) {
 tft.setTextColor(red);
 tft.println("EEPROM is empty.");
  } else {
  tft.setTextColor(green);
  tft.println("EEPROM OK.");
  }
//  loadConfig();
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (//EEPROM.read(CONFIG_START + sizeof(settings) - 1) == settings.version_of_program[3] // this is '\0'
    EEPROM.read(CONFIG_START + sizeof(ee) - 2) == ee.version_of_program[2] &&
    EEPROM.read(CONFIG_START + sizeof(ee) - 3) == ee.version_of_program[1] &&
    EEPROM.read(CONFIG_START + sizeof(ee) - 4) == ee.version_of_program[0])
  { // reads settings from EEPROM
    for (unsigned int t=0; t<sizeof(ee); t++)
      *((char*)&ee + t) = EEPROM.read(CONFIG_START + t);
 tft.setTextColor(green);
 tft.println("Reading settings.");
  } 
  else {
    // settings aren't valid! will overwrite with default settings
    saveConfig();
    tft.setTextColor(blue);
    tft.println("Creating default settings.");
  }


  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
  tft.print("LoRa freq:");
  //frequency in Hz (433E6, 866E6, 915E6)
   if (!LoRa.begin( setLoraFreq())) {             // initialize ratio at 915 MHz
  tft.setTextColor(red);
  tft.println("LoRa init failed.");
 //   while (true);                       // if failed, do nothing
  }
  tft.setTextColor(green);
  tft.println("LoRa init succeeded.");
  

  // large block of text 160 character like SMS
//  tft.println("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient.");

  tft.print("LoRa power:");
  tft.println(ee.power);
  tft.print("LoRa spread:");
  tft.println(ee.spread);
   
  LoRa.setTxPower(ee.power);
  LoRa.setSpreadingFactor(ee.spread);
  LoRa.enableCrc();
  LoRa.onReceive(onReceive);
  LoRa.receive();
  
  playTone( 100, 1000);
  tft.setTextColor(red);
  tft.println("Send to:ALL >I am ready.");
  destination=255;              // sendtoall
  localAddress=ee.myID;             // add sender address
  outgoing="@";                // add payload
  sendPacket();
  
  tft.setTextColor(magenta);
  tft.println("SETUP END.");

  storeMessage (0,"No message",'s',0,0,0,0,0 );
  waitKey();
  clearScreen();
}
//************************************************************************************************
void loop() {
char key = 0; //directLayout.getKey();

  switch (menu) {
case 0:
//menu standby

      key=directLayout.getKey();
      
     if ((refreshMenu==1)) {
        playTone( 100, 500); 
        displayMenu();
        refreshMenu=0;
      }
else
{
//  tft.setCursor(0, 120);
//  tft.setFont();
//  tft.setTextColor(white,black);
//  tft.setTextSize(1);
//  displayTime();
//  displayDate();
//  tft.print(" B:");
//  tft.print(hwCPUVoltage()/100);  
//  tft.println("V");
byte unread=countUnread();
if (unread>0) {
if (lastUnread!=unread) {
      refreshMenu=1;
      lastUnread=unread; 
}


long flashTime=millis()/500;
if (flashTime%2==1)
{
  tft.drawRoundRect(0, 54, 160, 36, 3, white);
}
else
{
   tft.drawRoundRect(0, 54, 160, 36, 3, red); 
   playTone( 500, 3000); 
}
}
if (alive>0) { //resend messages
  alive=0;
  resendUndelivered(alive);
  refreshMenu=1;
}

}
// main standby screen
      if (key=='s'){menu=1;} //setup
      if (key=='n'){menu=3;} //new message
      if (key=='c'){menu=4;} //contacts     
      if (key=='p'){menu=5;} //ping 
      if (key=='m'){displayMemory(); refreshMenu=1; }
      if (key=='r'){resendUndelivered(0); refreshMenu=1; }
      
      break;
case 1:
//setup
      key=displaySetup();
  switch (menuPage) {
    case 0: //radio
      if (key=='p'){ee.power=menuUp(1,20,ee.power);}
      if (key=='s'){ee.spread=menuUp(7,12,ee.spread);}  
      if (key=='f'){ee.freq=menuUp(1,3,ee.freq);}
      if (key=='i'){ee.myID=menuUp(1,10,ee.myID);localAddress=ee.myID;}
      if (key=='g'){ee.myGroup=menuUp(1,10,ee.myGroup);}
      break;
    case 1: //time
      ee.hour=hour;
      ee.minute=minute;
      ee.second=second;
      ee.day=day;
      ee.month=month;
      ee.year=year;
      if (key=='h'){ee.hour=menuUp(0,23,ee.hour);}  
      if (key=='m'){ee.minute=menuUp(0,59,ee.minute);} 
      if (key=='s'){ee.second=menuUp(0,59,ee.second);} 
      if (key=='d'){ee.day=menuUp(1,31,ee.day);}  
      if (key=='n'){ee.month=menuUp(1,12,ee.month);} 
      if (key=='y'){ee.year=menuUp(20,50,ee.year);} 
      setRTC();  
      break;
    case 2: //display  
      if (key=='b'){ee.brightness=menuUp(1,5,ee.brightness);  analogWrite(TFT_BACKLIGHT,  map(ee.brightness, 1, 5, 1, 255));} // Backlight on
      break;
    case 3://sound  
      if (key=='b') {ee.sound = (ee.sound + 1) % 2;}
      break;
  }


      if (key==1){menuPage=menuUp(0,3,menuPage);}
      if (key==0){menuPage=menuDown(0,3,menuPage);}

           
      if (key=='w'){
        saveConfig();
        LoRa.setTxPower(ee.power);
        LoRa.setSpreadingFactor(ee.spread);
        menu=0;
        } // Save configuration
      if (key==' '){menu=0;} // return to main menu without save
      
      playTone( 10, 1500);
      refreshMenu=1; 
      break;
case 2:
//config 
      break;
case 3:
//main editor
      if (menuEditor()){
        destination=recipient;              // add destination address
        localAddress=ee.myID;             // add sender address
        outgoing=editorText;                // add payload
        sendMessage();
 //       storeMessage (msgIndex,editorText,'s',0 );
        msgIndex++;
        msgCount++;                           // increment message ID
        editorText = "";
        cursor=0;
        }else{
         menu=0; 
        }
      refreshMenu=1;
      break;
case 4:
//contacts 
      key=displayContacts();
      if (key){        
      if (key=='q'){recipient=1;}  
      if (key=='w'){recipient=2;}
      if (key=='e'){recipient=3;}
      if (key=='r'){recipient=4;}  
      if (key=='t'){recipient=5;}
      if (key=='y'){recipient=6;}
      if (key=='u'){recipient=7;}  
      if (key=='i'){recipient=8;}
      if (key=='o'){recipient=9;}
      if (key=='p'){recipient=255;} 
         menu=0;
        }else{
 
        }
      refreshMenu=1;  
      break;
//default:
//      menu = 0;
//      break;
case 5:
//ping 
      key=pingTo();
      if (key){        
         menu=0;
        }else{
 
        }
      refreshMenu=1;  
      break;
//default:
//      menu = 0;
//      break;
  }

//***********************************************************

}
byte displayMemory () //String editorText
{
if (msgIndex==0) return 1;

int mi = msgIndex-1; 
byte exit = 1; 

char key = 0;
  memoryScreen(mi);
while (exit)
{
key = directLayout.getKey();;     
  if (key == ' ') {
  clearScreen(); 
  key = 0;
  return 1;
}

            
     if (key == KEY_DELETE) {
if (mi > 0) mi--;
  memoryScreen(mi);
      }
      
      if (key == KEY_RETURN) {
if (mi < msgIndex) mi++;
  memoryScreen(mi);
      }



}
return 0;

}

byte menuEditor () //String editorText
{
char key = 0; 
byte exit = 1; 

key = getKey();
clearScreen();
editorScreen(editorText,cursor);

while (exit)
{
key = getKey();     
  if (key == KEY_RETURN) {
  clearScreen();
  tft.fillRoundRect(0, 0, 160, 128, 8, blue);
  tft.drawRoundRect(0, 0, 160, 128, 8, white);
  tft.setFont(&FreeMono9pt7b); // &Org_01 &FreeMono9pt7b

  tft.setCursor(10, 30);
  tft.setTextSize(1);
  tft.setTextColor(white); 
  tft.println(" Send ?");
  tft.println(" .");
  tft.println(" ENTER = YES");
  tft.println(" DELETE = NO");   

 if (waitKey()==1)
 {
  clearScreen();    
  return 1;
}
  clearScreen();
  key = 0; 
      }
     if ((key == KEY_DELETE) && (cursor==0)) {
return 0;
//  return 1;
      }
            
     if (key == KEY_LEFT) {
if (cursor > 0) cursor--;
          key = 0;
            editorScreen(editorText,cursor);
      }
      
      if (key == KEY_RIGHT) {
if (cursor < (editorText.length())) cursor++;
          key = 0;
          editorScreen(editorText,cursor);
      }

      if (key == KEY_DELETE) {
       if (cursor > 0) cursor--;
          key = 0;       
      editorText.remove(cursor, 1);
      editorScreen(editorText,cursor);
      }

  if (key) {

    byte l=editorText.length();
    editorText=editorText.substring(0, cursor)+key+editorText.substring(cursor,l);
    cursor++;
//      playTone( 5, 500); 
    editorScreen(editorText,cursor);
//      playTone( 5, 1000);
}
///if (key=='m'){displayMemory();

int unread = countUnread();
if ((unread)>0) {
//  tft.print("NEW:"); 
//  tft.print(unread);
  playTone( 5, 4000);
  displayMemory(); 
  clearScreen();
  editorScreen(editorText,cursor); //refresh screen
}


// if (incoming != "")
// {
//  displayMessage(incoming);
//  waitKey();
//  clearScreen();
//  editorScreen(editorText,cursor);
//  incoming = "";
// }

}
return 0;
}


//************************************************************************************************
char getKey ()
{
char key=0;  
  switch (Layout) {
    case 0:
      key = directLayout.getKey();
      break;
    case 1:

      key = shiftLayout.getKey();
      if (key>9){ Layout = 0; }
      break;
    case 2:
      key = symbolLayout.getKey();
      if (key>9){ Layout = 0; }
      break;


    default:
      Layout = 0;
      break;
  }
  
  if (key == KEY_SHIFT)
  {
    Layout++;
          if (Layout >= 3) {
            Layout = 0;
      }
    key = 0;
    editorScreen(editorText,cursor);
  }
  return key;  
}

byte waitKey ()
{
char  key = 0;
while (key == 0)
{
  key = directLayout.getKey();
      if (key == KEY_DELETE) {
      playTone( 100, 500);  
      return  0;
      }
  
      if (key == KEY_RETURN) {
      playTone( 50, 1000);  
      return  1;
      }
}
return key;
}

int menuUp(int xmin,int xmax, int value )
{
    value++;
    if (value > xmax) {
      value = xmin;
    }
    if (value < xmin) {
      value = xmax;
    }
    return (value);
}

int menuDown(int xmin,int xmax, int value )
{
    value--;
    if (value < xmin) {
      value = xmax;
    }
    if (value > xmax) {
      value = xmin;
    }
    return (value);
  }


int countUnread () {
int unread=0;
 for (byte i=0; i<msgIndex; i++)
  { 
  if (message[i].type== 'n'){
  unread++;  
 }
  }
  return unread;
}

int countUndelivered () {
int undelivered=0;
 for (byte i=0; i<msgIndex; i++)
  { 
  if (message[i].type== 'u'){
  undelivered++;  
 }
  }
  return undelivered;
}

int resendUndelivered (int a) {
int undelivered=0;
 for (byte i=0; i<msgIndex; i++)
  { 
  if (message[i].type== 'u'){
 //////---------------------------------------------------------------------
  clearScreen();
  tft.setTextSize(1);
  tft.setCursor(1, 6);
  tft.setFont(&Org_01); // &Org_01 &FreeMono9pt7b
  tft.setTextColor(red);   
  tft.println("RESENDING ..."); 
  tft.setTextColor(white); 
  tft.println("To: ");
   tft.print(message[i].recipient);
   tft.print('.');  
   tft.println(addresBook[message[i].recipient]);
  tft.setTextColor(yellow);     
//  sendPacket();

  tft.println(message[i].text);

  LoRa.beginPacket();                   // start packet
  LoRa.write(message[i].recipient);              // add destination address
  LoRa.write(message[i].sender);             // add sender address
  LoRa.write(i);                 // add message ID
  outgoing = message[i].text;
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
 
  LoRa.receive();
  tft.setTextColor(blue); 
  tft.println(" Confirmation wait..."); 
  incoming="";
  byte dc=0;
 for ( dc=0; dc<120; dc++)
  { 
   tft.print(".");
    delay(5);   
    if (incoming=="$"){
   tft.println(".");      
    tft.setTextColor(green); 
    tft.println(" Delivered");
    message[i].type='s'; // chage atribute to send=delivered
    playTone( 10, 1000);
    break;  
    }
  }

if (dc>119){
      tft.println("");
      tft.setTextColor(red); 
      tft.println(" Error");
      playTone( 10, 200);
}
  

 /////----------------------------------------------------------------------
 }
  }
//  return ;
}





byte storeMessage (byte id,String msgToStore, char type, byte idx, byte rssi, byte snr,byte rec, byte sd)
{
 msgToStore.toCharArray(buffer, msgToStore.length()+1);
 strcpy(message[id].text, buffer);
 message[id].id=idx;
 message[id].type=type;
 message[id].recipient=rec;
 message[id].sender=sd; 
 message[id].rssi=rssi;
 message[id].snr=snr;
}

byte sendMessage() {

  clearScreen();
  tft.fillRoundRect(0, 0, 160, 128, 8, black);
  tft.drawRoundRect(0, 0, 160, 128, 8, white);
  tft.fillRoundRect(0, 0, 160, 28, 8, red);
  tft.drawRoundRect(0, 0, 160, 28, 8, white);
  tft.setTextSize(1);
  tft.setCursor(5, 15);
  tft.setFont(&FreeMono9pt7b); // &Org_01 &FreeMono9pt7b
  tft.println(" SENDING ..."); 
  tft.setFont(); // &Org_01 &FreeMono9pt7b
  tft.setCursor(5, 40);
  tft.setTextSize(1);
  tft.setTextColor(white);     
  sendPacket();
  tft.println(" Confirmation wait..."); 
//  playTone( 50, 1000); 
//  clearScreen();
//  tft.fillRoundRect(0, 0, 160, 128, 8, green);
//  tft.drawRoundRect(0, 0, 160, 128, 8, white);
//  tft.fillRoundRect(0, 0, 160, 28, 8, black);
//  tft.drawRoundRect(0, 0, 160, 28, 8, white);
//  tft.setCursor(5, 15);
//  tft.println(" Waiting ..."); 

//  tft.setTextColor(black);
  incoming="";
  byte dc=0;
 for ( dc=0; dc<120; dc++)
  { 
//    tft.print(".");
    tft.fillRoundRect(5, 60, dc, 10, 3, red);
    delay(7);   
    if (incoming=="$"){
 //   tft.println(".");      
    tft.setTextColor(green); 
    tft.fillRoundRect(5, 60, dc, 10, 3, green);
    tft.setCursor(5, 80);
    tft.println(" Delivered");

byte rssi= abs(LoRa.packetRssi());
byte snr= abs(20+LoRa.packetSnr());    
rssi= map(rssi, 121, 10, 0, 100);
snr= map(snr, 0, 32, 0, 100);     

    storeMessage (msgIndex,editorText,'s',0,rssi,snr,recipient,ee.myID );
    playTone( 100, 1000);
    break;  
    }
  }

if (dc>119){
      tft.setCursor(5, 80);
      tft.setTextColor(red); 
      tft.println(" Error");
      storeMessage (msgIndex,editorText,'u',0,0,0,recipient,ee.myID );
      playTone( 150, 200);
}
  
//  tft.setCursor(10, 50);
//  tft.setTextSize(1);
//  tft.setTextColor(black); 
//  tft.println(" Delivery");
//  tft.println(" comfirmation");
 
// waitKey();
}

byte sendPacket()
{
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  LoRa.receive();
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  incoming="";
  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

   while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
//    tft.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
//    tft.println("This message is not for me.");
    return;                             // skip rest of function
  }
 beep1 ();
byte rssi= abs(LoRa.packetRssi());
byte snr= abs(20+LoRa.packetSnr());    
rssi= map(rssi, 121, 10, 0, 100);
snr= map(snr, 0, 32, 0, 100); 
 
 if (incoming == "@"){
  alive=sender;
  return;
 }
 if (incoming == "#"){  // PING RESPONSE
  String  confimation="!";                // add payload
  LoRa.beginPacket();                   // start packet
  LoRa.write(sender);              // add destination address
  LoRa.write(ee.myID);             // add sender address
  LoRa.write(incomingMsgId);                 // add message ID
  LoRa.write(confimation.length());        // add payload length
  LoRa.print(confimation);                 // add payload
  LoRa.write(rssi);              // add destination address
  LoRa.write(snr);              // add destination address
  LoRa.endPacket();                     // finish packet and send it
  LoRa.receive(); 
  return;
 }


 if (incoming != "$"){
 if (recipient != 0xFF) { //||(incoming != "@")   
    storeMessage (msgIndex,incoming, 'n',incomingMsgId,rssi,snr,recipient,sender);
    msgIndex++;
 // confirm receive
String  confimation="$";                // add payload
  
  LoRa.beginPacket();                   // start packet
  LoRa.write(sender);              // add destination address
  LoRa.write(ee.myID);             // add sender address
  LoRa.write(incomingMsgId);                 // add message ID
  LoRa.write(confimation.length());        // add payload length
  LoRa.print(confimation);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  }
 }
  LoRa.receive();
// incoming="";

}

void displayMessage (String dispText)
{
  clearScreen();
  tft.setTextSize(1);
  tft.setFont(&Org_01);
  tft.setTextColor(yellow);
  tft.setCursor(0, 5); 
  tft.println(" Received:");
  tft.setCursor(0, 20);  
  tft.setTextColor(green);
  
//  tft.setFont(&FreeMono9pt7b); FreeMonoBold9pt7b
  tft.setFont(&FreeMono9pt7b);   
  tft.print(">");
  tft.setTextColor(white);  
  tft.println(dispText);

//  tft.setFont(&Org_01);
//int signal= map(LoRa.packetRssi(), -120, -30, 0, 100);
//int snr= map(LoRa.packetSnr(), -20, 10, 0, 100);  
//  tft.print("From:0x" + String(sender, HEX));
//  tft.println(">0x" + String(recipient, HEX));
//  tft.println("ID: " + String(incomingMsgId));
////  tft.println("Message length: " + String(incomingLength));
//  tft.setTextColor(red);
//  tft.print("RSSI:" + String(signal)+"%");
//  tft.println(" Snr:" + String(snr)+"%");
////  tft.println("FreqErr: " + String(LoRa.packetFrequencyError())+"Hz");
//  tft.print("Battery: ");
//  tft.println(hwCPUVoltage()/100);
//  tft.println();

}

void alarmMatch()
{ 
}

void buttonInt()
{
}


byte displayTime()
{
  second = 0;
  minute = 0;
  hour = 0;
  tft.setTextColor(yellow,black);
  if (hour<10) tft.print(0);
  tft.print(hour); 
  tft.print(":");
  if (minute<10) tft.print(0);  
  tft.print(minute);
  tft.print(":");
  if (second<10) tft.print(0);
  tft.print(second);
  tft.print(" ");
}

byte displayDate()
{
  day = 0;
  month = 0;
  year = 0;
  tft.setTextColor(yellow,black);
  tft.print(day); 
  tft.print(".");
   switch (month)
      {
      case 1: 
        tft.print("jan"); 
        break;
      case 2: 
        tft.print("Feb"); 
        break;
      case 3: 
        tft.print("mar"); 
        break;
      case 4: 
        tft.print("apr"); 
        break;
      case 5: 
        tft.print("may"); 
        break;
      case 6: 
        tft.print("jun"); 
        break;
      case 7: 
        tft.print("jun"); 
        break;
      case 8: 
        tft.print("aug"); 
        break;
      case 9: 
        tft.print("sep"); 
        break;
      case 10: 
        tft.print("oct"); 
        break;
      case 11: 
        tft.print("nov"); 
        break;
      case 12: 
        tft.print("dec"); 
        break;
      }
  tft.print(".20");
  tft.print(year);
  tft.print(" ");
}

long setLoraFreq(){
long loraFreq=868E6;
  switch (ee.freq) {
  case 1:
    tft.println("868Mhz");
    loraFreq=868E6;
    break;
  case 2:
    tft.println("433Mhz");
    loraFreq=433E6;
    break;
  case 3:
    tft.println("915Mhz");
    loraFreq=915E6;
    break;
  } 
  return loraFreq;
}


void clearScreen() {
   tft.fillScreen(black); 
}

void setRTC()
{

}


//**************************************************************************************************************************
//**********************-----------------------------    CONFIG  -------------------------*************************************
//**************************************************************************************************************************


void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (//EEPROM.read(CONFIG_START + sizeof(settings) - 1) == settings.version_of_program[3] // this is '\0'
    EEPROM.read(CONFIG_START + sizeof(ee) - 2) == ee.version_of_program[2] &&
    EEPROM.read(CONFIG_START + sizeof(ee) - 3) == ee.version_of_program[1] &&
    EEPROM.read(CONFIG_START + sizeof(ee) - 4) == ee.version_of_program[0])
  { // reads settings from EEPROM
    for (unsigned int t=0; t<sizeof(ee); t++)
      *((char*)&ee + t) = EEPROM.read(CONFIG_START + t);
  } 
  else {
    // settings aren't valid! will overwrite with default settings
    saveConfig();
  }
}
//**************************************************************************************************************************
void saveConfig() {
  for (unsigned int t=0; t<sizeof(ee); t++)
  { // writes to EEPROM
    EEPROM.write(CONFIG_START + t, *((char*)&ee + t));
    // and verifies the data
    if (EEPROM.read(CONFIG_START + t) != *((char*)&ee + t))
    {
      // error writing to EEPROM
    }
  }
    // commit() saves all the changes to EEPROM, it must be called
    EEPROM.commit();
}

//**************************************************************************************************************************
int hwCPUVoltage()
{
 
  // disable ADC
  while (ADC->STATUS.bit.SYNCBUSY);
  ADC->CTRLA.bit.ENABLE = 0x00;
 
  // internal 1V reference (default)
  analogReference(AR_INTERNAL1V0);
  // 12 bit resolution (default)
  analogWriteResolution(10);
  // MUXp 0x1B = SCALEDIOVCC/4 => connected to Vcc
  ADC->INPUTCTRL.bit.MUXPOS = 0x1B ;
 
  // enable ADC
  while (ADC->STATUS.bit.SYNCBUSY);
  ADC->CTRLA.bit.ENABLE = 0x01;
  // start conversion
  while (ADC->STATUS.bit.SYNCBUSY);
  ADC->SWTRIG.bit.START = 1;
  // clear the Data Ready flag
  ADC->INTFLAG.bit.RESRDY = 1;
  // start conversion again, since The first conversion after the reference is changed must not be used.
  while (ADC->STATUS.bit.SYNCBUSY);
  ADC->SWTRIG.bit.START = 1;
 
  // waiting for conversion to complete
  while (!ADC->INTFLAG.bit.RESRDY);
  const uint32_t valueRead = ADC->RESULT.reg;
 
  // disable ADC
  while (ADC->STATUS.bit.SYNCBUSY);
  ADC->CTRLA.bit.ENABLE = 0x00;
 
  return valueRead * 4;
}
void beep ()
{
  playTone( 1, 1000);
  playTone( 1, 3000);
  playTone( 1, 2000);
}

void beep1 ()
{
  playTone( 1, 6000);
}

void beep2 ()
{
beep();
}


void playTone(long duration, int freq) {
  if (ee.sound == 0) {
    return;
  }
// duration in mSecs, frequency in hertz

//  duration *= 10;
  int period = (1.0 / freq) * 1000000;
  long elapsed_time = 0;
  int amax=0;
  byte up=1;

//    while (elapsed_time < duration) {
//      analogWrite(speaker,1020);
//      delayMicroseconds(period / 2);
//      analogWrite(speaker, 0);
//      delayMicroseconds(period / 2);
//      elapsed_time += (period);  

    while (elapsed_time < duration) {

      analogWrite(speaker,amax/2);
      delayMicroseconds(period /4); 

      analogWrite(speaker,amax);
      delayMicroseconds(period /4);

      analogWrite(speaker,amax/2) ;
      delayMicroseconds(period /4);
      
      analogWrite(speaker,0);
      delayMicroseconds(period /4);

      if (up==1) {
        if (amax < 1020) {
          amax=amax+5;
        }
        else
        {
          up=0;
        }
      }
      else
      {
        if (amax > 0) amax=amax-1;
        elapsed_time += (period);
      }

  }  
      digitalWrite(speaker,LOW); 
}

bool inRange(int val, int minimum, int maximum)
{
  return ((minimum <= val) && (val <= maximum));
}
