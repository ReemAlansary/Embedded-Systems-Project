// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <string.h>
#include <stdlib.h>
#include <Wire.h>


// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******* UI details */
#define BUTTON_X 30
#define BUTTON_XX 130
#define BUTTON_YY 135
#define BUTTON_WW 100
#define BUTTON_Y 150
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_HH 10

#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 220
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR ILI9341_MAGENTA
// the data (phone #) we store in the textfield
#define TEXT_LEN 12
//int first=0;//1
//int second=0;//0 9
//int third=0;//0  0
//char comma=',';
//int fourth=0;//6 9
//int fifth=0;//   0
char textfield[TEXT_LEN+1] = "";
uint8_t textfield_i=0;
 char s1[20];
  char s2[20];
boolean on =true;

boolean onfirst =true;
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
// We have a status line for like, is FONA working
#define STATUS_X 60
#define STATUS_Y 160



Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;
 int first=90;
int last=90;
Adafruit_GFX_Button buttons[15];
/* create 15 buttons, in classic candybar phone style */
char buttonlabels[3][7] = {"+", "-" ,"on/off"};
uint16_t buttoncolors[3] ={ILI9341_BLUE, ILI9341_ORANGE,ILI9341_BLUE};
                             
void setup(void) {
  Wire.begin(5);
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  Wire.onRequest(requestEvent);

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.4\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.4\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();
  if(identifier==0x0101)
      identifier=0x9341;
  
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.4\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  // create buttons
  for (uint8_t row=0; row<1; row++) {
    for (uint8_t col=0; col<2; col++) {
      buttons[col + row*3].initButton(&tft,
                    BUTTON_X+row*(BUTTON_H+BUTTON_SPACING_X),BUTTON_Y+col*(BUTTON_W+BUTTON_SPACING_Y),   // x, y, w, h, outline, fill, text
                  BUTTON_H, BUTTON_W, ILI9341_WHITE, buttoncolors[col+row*2], ILI9341_WHITE,
                  buttonlabels[col + row*2], BUTTON_TEXTSIZE); 
      buttons[col + row*2].drawButton();
    }
  }

  
  for (uint8_t row=0; row<1; row++) {
    for (uint8_t col=2; col<3; col++) {
      buttons[col + row*3].initButton(&tft,
                    BUTTON_XX+row*(BUTTON_H+BUTTON_SPACING_X),BUTTON_YY+col*(BUTTON_W+BUTTON_SPACING_Y),   // x, y, w, h, outline, fill, text
                  BUTTON_WW, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row*3], ILI9341_WHITE,
                  buttonlabels[col + row*3], BUTTON_TEXTSIZE); 
      buttons[col + row*3].drawButton();
    }
  }
  tft.fillRect(STATUS_X, STATUS_Y, 240, 100, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(5);
 
  sprintf(s1,"%d",first);
  sprintf(s2,"%d",last);
  tft.print(strcat(strcat(s1,","),s2));
  // create 'text field'
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);
}


void status() {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 100, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(5);
  int lastplus=last+1;
  if(lastplus<=99){
    last=last+1;
  }
  else{
    last=0;
    lastplus=0;
    first=first+1;
  } 
//  Serial.println(last);
  char s1[20];
  char s2[20];
  sprintf(s1,"%d",first);
  sprintf(s2,"%d",last);

  
  
 tft.print(strcat(strcat(s1,","),s2));
}
void minus() {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 100, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(5);
  int lastplus=last-1;
  if(lastplus>=0){
    last=last-1;
  }
  else{
    last=99;
    lastplus=99;
    first=first-1;
  } 
 // Serial.println(last);
  char s1[20];
  char s2[20];
  sprintf(s1,"%d",first);
  sprintf(s2,"%d",last);

  
  
  tft.print(strcat(strcat(s1,","),s2));
}
#define MINPRESSURE 10
#define MAXPRESSURE 1000
void loop(void) {
  
//  while(on){
if(on==true&&onfirst==true){
  onfirst=false;
  tft.setCursor(TEXT_X + 45, TEXT_Y+15);
  tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
  tft.setTextSize(TEXT_TSIZE);
    tft.print("FM Radio");
    for (uint8_t row=0; row<1; row++) {
    for (uint8_t col=0; col<2; col++) {
      buttons[col + row*3].initButton(&tft,
                    BUTTON_X+row*(BUTTON_H+BUTTON_SPACING_X),BUTTON_Y+col*(BUTTON_W+BUTTON_SPACING_Y),   // x, y, w, h, outline, fill, text
                  BUTTON_H, BUTTON_W, ILI9341_WHITE, buttoncolors[col+row*2], ILI9341_WHITE,
                  buttonlabels[col + row*2], BUTTON_TEXTSIZE); 
      buttons[col + row*2].drawButton();
    }
  }

    tft.fillRect(STATUS_X, STATUS_Y, 240, 100, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(5);
 
  sprintf(s1,"%d",first);
  sprintf(s2,"%d",last);
  tft.print(strcat(strcat(s1,","),s2));
  // create 'text field'
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);
    }
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);//////////////////////////////////

  
  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  
 // p = ts.getPoint(); 
 
  
  // Scale from ~0->4000 to tft.width using the calibration #'s
 
   if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
   }
   
  // go thru all the buttons, checking if they were pressed
  for (uint8_t b=0; b<3; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      //Serial.print("Pressing: "); //Serial.println(b);
      if(on){
         buttons[b].press(true);
       }
       else if(!on&&b==2){
         buttons[b].press(true);
        }
       // tell the button it is pressed
    } else {
      if(on){  buttons[b].press(false);}
      else if(!on &&b==2){
          buttons[b].press(false); 
        }
     // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b=0; b<3; b++) {
    if (buttons[b].justReleased()) {
      //Serial.print("Released: "); //Serial.println(b);
      if(on){
      buttons[b].drawButton();
        }
        else if(!on &&b==2){
      buttons[b].drawButton();
      }
    
    }
    
    if (buttons[b].justPressed()) {
        if (b == 0) {
          status();
         
        }
        
        if (b == 1) {
          minus();
       
        }
        if(b==2){
          
          

          
          on=!on; 
          if(on==true)
          onfirst=true;
          else 
          onfirst=false;
          Serial.println("onfirst :");
          Serial.println(onfirst);
                  ;
                    Serial.println("on:")
                  ;
                    Serial.println(on);
          if(on==false){
             tft.fillScreen(BLACK); 
            }  
        }
        
       delay(100);  // UI debouncing
    }
  }
 
// while(!on){ 
//    tft.fillScreen(BLACK); 
////     for (uint8_t row=0; row<1; row++) {
////    for (uint8_t col=2; col<3; col++) {
////      buttons[col + row*3].initButton(&tft,
////                    BUTTON_XX+row*(BUTTON_H+BUTTON_SPACING_X),BUTTON_YY+col*(BUTTON_W+BUTTON_SPACING_Y),   // x, y, w, h, outline, fill, text
////                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row*4], ILI9341_WHITE,
////                  buttonlabels[col + row*4], BUTTON_TEXTSIZE); 
////      buttons[col + row*4].drawButton();
////    }
////  }
//  
//  
//  digitalWrite(13, HIGH);
//  TSPoint p = ts.getPoint();
//  digitalWrite(13, LOW);
//  pinMode(XM, OUTPUT);
//  pinMode(YP, OUTPUT);
//  //pinMode(YM, OUTPUT);
//
//  
// // p = ts.getPoint(); 
//  
//  // Scale from ~0->4000 to tft.width using the calibration #'s
// 
//   if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
//    // scale from 0->1023 to tft.width
//    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
//    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
//   }
//   
//  // go thru all the buttons, checking if they were pressed
//  for (uint8_t b=2; b<3; b++) {
//    if (buttons[b].contains(p.x, p.y)) {
//      Serial.print("Pressing: "); Serial.println(b);
//      buttons[b].press(true);  // tell the button it is pressed
//    } else {
//      buttons[b].press(false);  // tell the button it is NOT pressed
//    }
//  }
//
//  // now we can ask the buttons if their state has changed
//  for (uint8_t b=2; b<3; b++) {
//    if (buttons[b].justReleased()) {
//      Serial.print("Released: "); Serial.println(b);
//      buttons[b].drawButton();
//      // draw normal
//    }
//    
//    if (buttons[b].justPressed()) {
//        if (b == 2) {
//          on=true;
//         
//        }      
//       }
//        
//      delay(100); 
//    }
//  
// }
delay(100);
}

void requestEvent()
{
  Serial.println(Serial.available());
 // while(Serial.available()) // loop through all but the last
  //{ 
  int character= first;
  int z=last;
   Serial.println(character);
   Wire.write(character);
   Wire.write(on);
      Wire.write(z);

  //}

}
