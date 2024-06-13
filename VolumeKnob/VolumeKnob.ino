
#include <Arduino_GFX_Library.h>
#include <SD.h>
#include <FS.h>
#include "touch.h"
#include "imgs.h"
#include "NotoSansBold15.h"
#include "largestFont.h"
#include "fatFont.h"
#include "middleFont.h"
#include <RotaryEncoder.h>
#include <TFT_eSPI.h>
#include "USB.h"
#include "USBHIDKeyboard.h" 
USBHIDKeyboard Keyboard;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

#define TFT_BLK 7
#define TFT_RES 11

#define TFT_CS 15
#define TFT_MOSI 13
#define TFT_MISO 12
#define TFT_SCLK 14
#define TFT_DC 21

#define TOUCH_INT 40
#define TOUCH_SDA 38
#define TOUCH_SCL 39
#define TOUCH_RST 16

#define PIN_IN1 10
#define PIN_IN2 9
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

#define COLOR_NUM 5
int ColorArray[COLOR_NUM] = {WHITE, BLUE, GREEN, RED, YELLOW};
unsigned short grays[13];
int chosen=0;
unsigned short color_start[6]={0x01AA,TFT_PURPLE,0xAFB8,TFT_ORANGE,0x7800,0x632C};
unsigned short color_end[6]={0xAEDE,TFT_PINK,0x0345,TFT_YELLOW,TFT_WHITE,0xF79E};

int angle=19;
int xt = 0, yt = 0;
int xs=120;
int ys=12;
bool mute=0;
int deb=0;
Arduino_ESP32SPI *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO, HSPI, true); // Constructor
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RES, 0 /* rotation */, true /* IPS */);
bool first=1;

void setup(void)
{
   
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    delay(1000);
    //USBSerial.println("start");
    gfx->begin();
    sprite.createSprite(240, 240);
    sprite.setSwapBytes(1);
    gfx->fillScreen(BLUE);

     ledcSetup(0, 10000, 8);
     ledcAttachPin(TFT_BLK, 0);
     ledcWrite(0, 40);
     Keyboard.begin();
     USB.begin();
    
      int co=220;
     for(int i=0;i<13;i++)
     {
     grays[i]=tft.color565(co, co, co);
     co=co-20;
     }
     draw();

 
}

void draw()
{
sprite.fillSprite(TFT_BLACK);
sprite.setTextDatum(4);

sprite.setTextColor(grays[0]);
sprite.loadFont(largestFont);
if(angle*2<10)
sprite.drawString("0"+String(angle*2),120,126);
else
sprite.drawString(String(angle*2),120,126);
sprite.unloadFont();

sprite.setTextColor(grays[3]);
sprite.loadFont(NotoSansBold15);
sprite.drawString("VOLUME",120,86);
sprite.unloadFont();
if(mute)
{
  sprite.setTextColor(grays[7]);
  sprite.loadFont(middleFont);
  sprite.drawString("MUTE",120,210);
  }
sprite.unloadFont();
sprite.loadFont(fatFont);
sprite.setTextColor(grays[5]);
sprite.drawString("%",120,164,4);
sprite.unloadFont();
///sprite.pushImage(96,180,48,48,logo2[0]);

for(int i=0;i<angle;i++)
{
    uint8_t interpolation = map(i, 0, angle, 0, 255);
    uint8_t red = map(interpolation, 0, 255, (color_start[chosen] >> 8) & 0xF8, (color_end[chosen] >> 8) & 0xF8);
    uint8_t green = map(interpolation, 0, 255, (color_start[chosen] >> 3) & 0xFC, (color_end[chosen] >> 3) & 0xFC);
    uint8_t blue = map(interpolation, 0, 255, (color_start[chosen] << 3) & 0xF8, (color_end[chosen] << 3) & 0xF8);
    uint16_t color = tft.color565(red, green, blue);
  
  if(i==angle)
  sprite.drawSmoothArc(120, 120, 120, 60, 29+(i*6), 29+(i*6)+3, color, TFT_BLACK);
   else if(i==angle-1 || i==angle+1)
  sprite.drawSmoothArc(120, 120, 120, 80, 30+(i*6), 30+(i*6)+1, color, TFT_BLACK);
  else if(i==angle-2 || i==angle+2)
  sprite.drawSmoothArc(120, 120, 120, 90, 30+(i*6), 30+(i*6)+1, color, TFT_BLACK);
  else if(i%10==0)
  sprite.drawSmoothArc(120, 120, 120, 100, 30+(i*6), 30+(i*6)+1, color, TFT_BLACK);
  else
  sprite.drawSmoothArc(120, 120, 120, 105, 30+(i*6), 30+(i*6)+1, color, TFT_BLACK);
}

for(int i=angle;i<51;i++)
{
    uint8_t interpolation = map(i, angle, 51, 0, 255);
    uint8_t red = map(interpolation, 0, 255, (color_end[chosen] >> 8) & 0xF8, (color_start[chosen] >> 8) & 0xF8);
    uint8_t green = map(interpolation, 0, 255, (color_end[chosen] >> 3) & 0xFC, (color_start[chosen] >> 3) & 0xFC);
    uint8_t blue = map(interpolation, 0, 255, (color_end[chosen] << 3) & 0xF8, (color_start[chosen] << 3) & 0xF8);
    uint16_t color = tft.color565(red, green, blue);
  
  if(i==angle)
  sprite.drawSmoothArc(120, 120, 120, 56, 29+(i*6), 29+(i*6)+3, color, TFT_BLACK);
   else if(i==angle-1 || i==angle+1)
  sprite.drawSmoothArc(120, 120, 120, 80, 30+(i*6), 30+(i*6)+1, color, TFT_BLACK);
  else if(i==angle-2 || i==angle+2)
  sprite.drawSmoothArc(120, 120, 120, 90, 30+(i*6), 30+(i*6)+1, color, TFT_BLACK);
  else if(i%10==0)
  sprite.drawSmoothArc(120, 120, 120, 100, 30+(i*6), 30+(i*6)+1, color, TFT_BLACK);
  else
  sprite.drawSmoothArc(120, 120, 120, 105, 30+(i*6), 30+(i*6)+1, color, TFT_BLACK);
}
gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)sprite.getPointer(),240,240);
}

void readEncoder()
 {

  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    
    if(newPos>pos && angle<50)
    {angle++;
     Keyboard.press(KEY_LEFT_CTRL);
     Keyboard.press(KEY_LEFT_SHIFT);
     Keyboard.press('8');
     Keyboard.releaseAll();
    }
    if(newPos<pos && angle>0)
    {angle--;
    Keyboard.press(KEY_LEFT_CTRL);
     Keyboard.press(KEY_LEFT_SHIFT);
     Keyboard.press('9');
     Keyboard.releaseAll();
    }
    
    pos = newPos;
    draw();
  } 

}

void callMute()
{
  mute=!mute;
  if(mute==1)
    ledcWrite(0, 8);
  else
    ledcWrite(0, 40);
   draw();
   Keyboard.press(KEY_LEFT_CTRL);
   Keyboard.press(KEY_LEFT_SHIFT);
   Keyboard.press('7');
   Keyboard.releaseAll();
}

void setColor()
{
  chosen++;
  if(chosen>5)
  chosen=0;
  draw();
}

void resetVOL()
{
   angle=19;
   Keyboard.press(KEY_LEFT_CTRL);
   Keyboard.press(KEY_LEFT_SHIFT);
   Keyboard.press('0');
   Keyboard.releaseAll();
   draw();
}

void loop()
{
  readEncoder();
  if (read_touch(&xt, &yt) == 1)
  {
    if(deb==0)
    {
      deb=1;
      if(xt<200 && yt<100)
      setColor();
      if(xt<200 && yt>100)
      callMute();
      if(xt>200)
      resetVOL();
    }
  }else{deb=0;}

  if(first)
  {
   delay(1000);
   resetVOL();
   first=0;
  }
}



