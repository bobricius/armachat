#define KEY_SHIFT 2
#define KEY_UP 3
#define KEY_LEFT 4
#define KEY_DOWN 5
#define KEY_RIGHT 6
#define KEY_RETURN 10
#define KEY_DELETE 8

#define PORT_IOBUS

#define white ST77XX_WHITE
#define black ST77XX_BLACK
#define red ST77XX_RED
#define blue ST77XX_BLUE
#define green ST77XX_GREEN
#define yellow ST77XX_YELLOW
#define magenta ST77XX_MAGENTA
#define cyan ST77XX_CYAN

#ifdef FUSION
// FUSION communicator version with SAMD21E18
// require samd core from https://github.com/mattairtech/ArduinoCore-samd/tree/master/variants/MT_D21E_revB
const int csPin = 23;         // LoRa radio chip select signal
const int resetPin = -1;       // LoRa radio reset
const int irqPin = 9;         // change for your board; must be a hardware interrupt pin
//******************** display
#define TFT_CS        -1 //11 // PyBadge/PyGamer display control pins: chip select
#define TFT_RST       -1 // Display reset
#define TFT_DC        27 // Display data/command select
#define TFT_BACKLIGHT 10 // Display backlight pin
#define TFT_MOSI 14  // Data out
#define TFT_SCLK 15  // Clock out

#define speaker       2 // Speaker
const byte ROWS = 6; // rows
const byte COLS = 5; // columns
byte colPins[COLS] = {30, 31, 28, 0, 1}; //pa13-d38,connect to the row pinouts of the keypad
byte rowPins[ROWS] = {3, 4, 5, 6, 7, 8}; //pb22-d30,pb23-d31,pa28-d27 connect to the column pinouts of the keypad
//define the symbols on the buttons of the keypads
char directKeys[ROWS][COLS] = {
  {KEY_RETURN, ' ', 'm', 'n', 'b',},
  {KEY_DELETE, 'l', 'k', 'j', 'h',},
  {'p', 'o', 'i', 'u', 'y',},
  {KEY_SHIFT, 'z', 'x', 'c', 'v',},
  {'a', 's', 'd', 'f', 'g',},
  {'q', 'w', 'e', 'r', 't',}  
};
char shiftKeys[ROWS][COLS] = {
  {KEY_RIGHT, ',', 'M', 'N', 'B',},
  {KEY_LEFT, 'L', 'K', 'J', 'H',},
  {'P', 'O', 'I', 'U', 'Y',},
  {KEY_SHIFT, 'Z', 'X', 'C', 'V',},
  {'A', 'S', 'D', 'F', 'G',},
  {'Q', 'W', 'E', 'R', 'T',}  
};

char symbolKeys[ROWS][COLS] = {
  {KEY_RIGHT, '.', ':', ';', '""',},
  {KEY_LEFT, ':', '*', '&', '^',},
  {'0', '9', '8', '7', '6',},
  {KEY_SHIFT, '(', ')', '?', '/',},
  {'!', '@', '#', '$', '%',},
  {'1', '2', '3', '4', '5',}  
};
#endif
//----------------------------------------------------------------------------------------------------------------------

//******************** display
#ifdef LCDTFT
Adafruit_ST7735 tft = Adafruit_ST7735(&SPI1, TFT_CS, TFT_DC, TFT_RST);
#endif

#ifdef LCDIPS
Adafruit_ST7789 tft = Adafruit_ST7789(&SPI1, TFT_CS, TFT_DC, TFT_RST);
#endif
