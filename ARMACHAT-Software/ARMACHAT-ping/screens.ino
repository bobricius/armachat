void editorScreen (String etxt, byte cursor) {
tft.setFont(); // &Org_01 &FreeMono9pt7b
//  clearScreen();
  tft.setTextSize(1);
  tft.setCursor(0, 0);
//  displayTime();
//  tft.print(" ");
  tft.setTextColor(red,black); 
 switch (Layout) {
    case 0:
  tft.print("abc ");
      break;
    case 1:
  tft.print("ABC ");
      break;
    case 2:
  tft.print("!@# ");
      break;
  }
  tft.setTextColor(yellow,black);  
  tft.print(cursor);
  tft.print("/");
  tft.print(etxt.length());
  tft.print(" ");
//  tft.print(msgToEdit); 
//  tft.print(" ");

  tft.drawLine(0, 117, 160, 117, blue);  
  tft.drawLine(0, 8, 160, 8, blue);
  tft.setCursor(0, 120);
 switch (Layout) {
    case 0:
      tft.setTextColor(green,black);  
  tft.print("[ENTER] SEND           ");
      break;
    case 1:
      tft.setTextColor(green,black);  
  tft.print("[ENT] > [DEL] < [SPC] ,");
      break;
    case 2:
      tft.setTextColor(green,black);  
  tft.print("[ENT] > [DEL] < [SPC] .");
      break;
  }
      
  tft.setTextSize(2);
  tft.setCursor(0, 10);  
  tft.setTextColor(white,black); 
  
  byte l=etxt.length();
//  tft.setFont(&FreeMono9pt7b); // &Org_01 &FreeMono9pt7b
  tft.print(etxt.substring(0, cursor)+"_"+etxt.substring(cursor,l)+" ");

}
//----------------------------------------------------------------------------------------------------------------------

void memoryScreen (byte memIndex) {
int frameColor=white;
byte cc=0;
  tft.setFont(); // &Org_01 &FreeMono9pt7b
  clearScreen();
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.setTextColor(white,black);
  tft.print("#");
  tft.print(memIndex);
  tft.print(" ");
 switch (message[memIndex].type) {
    case 'n':
      message[memIndex].type='r'; //set message atribute r= read
      tft.setTextColor(green,black);
      tft.print("NEW from");
      frameColor=yellow;
      cc=message[memIndex].sender;
      break;
    case 's':
      tft.setTextColor(red,black);
      tft.print("To>");
      frameColor=red;
      cc=message[memIndex].recipient;
      break;
    case 'r':
      tft.setTextColor(green,black);
      tft.print("From<");
      frameColor=green;
      cc=message[memIndex].sender;
      break;
    case 'u':
      tft.setTextColor(yellow,black);
      tft.print("Undelivered to");
      frameColor=blue;
      cc=message[memIndex].recipient;
      break;
  }

   tft.setTextColor(yellow,black);  
   tft.print(cc);
   tft.print('.');  
   tft.println(addresBook[cc]);  
  
  tft.drawRoundRect(0, 8, 160, 90, 3, frameColor);
  tft.drawLine(0, 117, 160, 117, yellow);  
//  tft.drawLine(0, 8, 160, 8, yellow);
  
  tft.setCursor(0, 100);
      tft.setTextColor(magenta,black);  
  tft.println("Signal:");
  tft.print("RSSI:");
  tft.print(message[memIndex].rssi);
   tft.print("% SNR:");   
  tft.print(message[memIndex].snr);
  tft.print("%");
  tft.setCursor(0, 120);
      tft.setTextColor(magenta,black);  
  tft.print("[ENT]> [DEL]< [SPC] END");

      
  tft.setTextSize(1);
  tft.setCursor(0, 15);  
  tft.setFont(&FreeMono9pt7b); 
  tft.setTextColor(white);    
  tft.println(message[memIndex].text);
}

//----------------------------------------------------------------------------------------------------------------------
byte displayMenu()
{
  tft.setFont(&FreeMono9pt7b);
  clearScreen();
//  tft.fillRoundRect(0, 0, 31, 128, 3, blue);  
//  tft.drawRoundRect(0, 0, 31, 128, 3, white);  
//  tft.fillRoundRect(0, 17, 160, 36, 3, blue); 
//  tft.fillRoundRect(0, 54, 160, 36, 3, blue);
  
  tft.drawRoundRect(0, 17, 160, 36, 3, red); 
  tft.drawRoundRect(0, 54, 160, 36, 3, green);

  
  tft.fillRoundRect(0, 0, 160, 16, 3,  cyan); 
  tft.drawRoundRect(0, 0, 160, 16, 3, white);  
  
//  tft.fillRoundRect(0, 108, 160, 20, 3,  red); 
//  tft.drawRoundRect(0, 108, 160, 20, 3, white);
//  tft.drawRoundRect(0, 0, 160, 128, 3, white); 
  tft.setTextSize(1);
  tft.setCursor(3, 12);
  tft.setTextColor(black);
  tft.print("myID:");
  tft.println(ee.myID);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  tft.setTextColor(red);
  tft.print("[N]");
  tft.setTextColor(white);
  tft.println("New message");
  tft.setTextColor(yellow);  
  tft.print("to:");
  tft.setTextColor(white);  
  tft.print(recipient); 
  tft.print(".");
     tft.println(addresBook[recipient]);
  tft.setTextColor(green);  
  tft.print("[M]");
  tft.setTextColor(white);  
  tft.print("Messages:");
  tft.println(msgIndex);
    tft.setTextColor(magenta);

   tft.setFont(); 
  tft.setCursor(5, 75);

  tft.setTextColor(yellow);    
  tft.print("NEW:");
  tft.print(countUnread());
  
  tft.print(" Udelivered:");
  tft.println(countUndelivered());

  
  tft.setCursor(0, 92);  
  tft.setFont(); //&FreeMono9pt7
  tft.setTextColor(magenta);
  tft.print("[C] ");
   tft.setTextColor(white);  
  tft.print("Contacts ");
   tft.setTextColor(magenta);   
  tft.print("[S] ");
   tft.setTextColor(white);   
  tft.println("Setup");

  tft.setTextColor(magenta);   
  tft.print("[R] ");
  tft.setTextColor(white);   
  tft.println("Resend ");
//  tft.setTextColor(magenta);   
//  tft.print("[G] ");
//  tft.setTextColor(white);   
//  tft.println("GPS");
  tft.setTextColor(magenta);   
  tft.print("[P] ");
  tft.setTextColor(white);   
  tft.print("Ping ");
//  tft.setTextColor(magenta);   
//  tft.print("[E] ");
//  tft.setTextColor(white);   
//  tft.println("Enviro");
  tft.drawLine(0, 117, 160, 117, cyan);
  tft.setTextColor(blue);   
  tft.setCursor(0, 118);
  tft.print("Armachat");
  tft.print(" Battery:");
  displayBattery();
    
//    tft.drawLine(0, 117, 160, 117, magenta);
}
//----------------------------------------------------------------------------------------------------------------------

byte displayBattery()
{
int bat=0;
bat = (hwCPUVoltage()/100);
if ( inRange(bat, 33, 34))   {tft.setTextColor(green); tft.print("GOOD");}
if ( inRange(bat, 31, 32))   {tft.setTextColor(yellow); tft.print("LOW");}
if ( inRange(bat, 20, 30))   {tft.setTextColor(red); tft.print("Critical");}
}
//----------------------------------------------------------------------------------------------------------------------


byte pingTo()
{
byte rRSSI=0;
byte rSNR=0;
  tft.setFont(&FreeMono9pt7b);
  clearScreen();
//  tft.fillRoundRect(0, 0, 31, 128, 3, blue);  
//  tft.drawRoundRect(0, 0, 31, 128, 3, white);  
  tft.fillRoundRect(0, 0, 160, 16, 3,  yellow); 
  tft.drawRoundRect(0, 0, 160, 16, 3, blue);  

  tft.setTextSize(1);
  tft.setCursor(3, 12);
  tft.setTextColor(black);
  tft.println("Ping:");
  tft.setFont(); //&FreeMono9pt7b
//  tft.setFont(&Org_01); // &Org_01 &FreeMono9pt7b
  tft.setTextSize(1);
   tft.setCursor(0, 20); 
 for (byte pwr=20; pwr>=1; pwr=pwr-2)
  { 
   tft.setTextColor(white); 
   tft.print("Pw:"); 
   tft.print(pwr);
   LoRa.setTxPower(pwr);
  destination=recipient;              // add destination address
  localAddress=ee.myID; 
  localAddress=ee.myID;             // add sender address
  outgoing="#";                // add payload PING
  tft.fillRoundRect(0, 105, 160, 10, 3, blue);
  sendPacket();
  incoming="";
  tft.fillRoundRect(0, 105, 160, 10, 3, black);
  byte dc=0;
 for ( dc=0; dc<160; dc++)
  { 
//    tft.print(".");
    tft.fillRoundRect(0, 105, dc, 10, 3, red);
    tft.drawRoundRect(0, 105, dc, 10, 3, white);
    delay(4);
    //  tft.print(etxt.substring(0, cursor)+"_"+etxt.substring(cursor,l)+" ");   
    if (incoming.substring(0,1)=="!"){
    tft.fillRoundRect(0, 105, dc, 10, 3, green);    
    tft.setTextColor(green);
    rRSSI= incoming.charAt(1);
    rSNR= incoming.charAt(2); 
    tft.print(" RSSI:"); 
    tft.print(rRSSI);
    tft.print("% SNR:"); 
    tft.print(rSNR);
    tft.println("%"); 
    playTone( 1, 5000);
    break;  
    }
  }

if (dc>150){
      tft.setTextColor(red); 
      tft.println(" no response");
      playTone( 1, 600);
}
     
  }
  LoRa.setTxPower(ee.power);
  
  tft.drawLine(0, 117, 160, 117, red);  
  tft.setCursor(0, 120);
  tft.setTextColor(green,black);  
  tft.print("[ENT]Ping [DEL]END");
  
  return waitKey();
}
//----------------------------------------------------------------------------------------------------------------------
byte displayContacts()
{
  tft.setFont(&FreeMono9pt7b);
  clearScreen();
//  tft.fillRoundRect(0, 0, 31, 128, 3, blue);  
//  tft.drawRoundRect(0, 0, 31, 128, 3, white);  
  tft.fillRoundRect(0, 0, 160, 16, 3,  magenta); 
  tft.drawRoundRect(0, 0, 160, 16, 3, white);  
  
//  tft.fillRoundRect(0, 108, 160, 20, 3,  red); 
//  tft.drawRoundRect(0, 108, 160, 20, 3, white);
//  tft.drawRoundRect(0, 0, 160, 128, 3, white); 
  tft.setTextSize(1);
  tft.setCursor(3, 12);
  tft.setTextColor(black);
  tft.println("Contacts:");
  tft.setFont(); //&FreeMono9pt7b
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
 for (byte cc=0; cc<10; cc++)
  { 
   tft.print(cc);
   tft.print('.');  
   tft.println(addresBook[cc]);
  }
  tft.drawLine(0, 117, 160, 117, red);  
  tft.setCursor(0, 120);
  tft.setTextColor(green,black);  
  tft.print("[0-9]SEL [ENT]SCN [DEL]END");
  
  return waitKey();
}
//----------------------------------------------------------------------------------------------------------------------

byte displaySetup()
{
  tft.setFont(&FreeMono9pt7b);
  clearScreen();
  tft.fillRoundRect(0, 0, 31, 115, 3, blue);  
  tft.drawRoundRect(0, 0, 31, 115, 3, white);  
  tft.fillRoundRect(0, 0, 160, 16, 3,  cyan); 
  tft.drawRoundRect(0, 0, 160, 16, 3, white);  
  
//  tft.fillRoundRect(0, 108, 160, 20, 3,  red); 
//  tft.drawRoundRect(0, 108, 160, 20, 3, white);
  tft.drawRoundRect(0, 0, 160, 128, 3, white);  
  tft.setTextSize(1);
  tft.setCursor(3, 12);
  tft.setTextColor(black);
  tft.print("SETUP:");
  tft.setTextColor(red);
  tft.setTextSize(1);


  switch (menuPage) {
    case 0:
      tft.println("RADIO"); // menu title
      tft.setTextColor(white);
      tft.print("[F]Freq:");
      setLoraFreq();
      tft.print("[S]Spread:");
      tft.println(ee.spread);
      tft.print("[P]Power:");   
      tft.println(ee.power);
      tft.print("[I]My ID:");   
      tft.println(ee.myID);
      tft.print("[G]Group:");    
      tft.println(ee.myGroup);
      break;
    case 1:
      tft.println("time"); // menu title
      tft.setTextColor(white);
      tft.print("   ");
      displayTime();
      tft.println("");
      tft.print("   ");
      displayDate();
      tft.println("");
      tft.println("   [H][M][S]");
      tft.println("   [D][N][Y]");     
      tft.println("[-]"); 
      break; 
    case 2:
      tft.println("DISPLAY"); // menu title
      tft.setTextColor(white);
      tft.print("[B]Bright:");
      tft.println(ee.brightness); 
      tft.print("[T]Sleep:");
      tft.println(ee.sleepTime); 
      break;   
    case 3:
      tft.println("SOUND"); // menu title
      tft.setTextColor(white);
      tft.print("[B]Beep:"); 
      tft.println(ee.sound); 
      break;  

  }

  tft.setCursor(2, 124);
  tft.setTextColor(yellow);
  tft.setTextColor(black,white);  
  tft.setFont();
  tft.print("<Page> |SPC|EXIT |W|Save");
  
//  tft.println("[W]SAVE[E]EXIT");
  return waitKey();
}
//----------------------------------------------------------------------------------------------------------------------
