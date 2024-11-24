#include <SPI.h>
#include <SD.h>
#include <Sipeed_ST7789.h>
#include <Adafruit_NeoPixel.h>
#include "icon.h"
#include "Clock.h"
#include "Bot_Move.h"
#include <Sipeed_GC0328.h>
#include "MBNet_1000.h"
#include "Maix_KPU.h"

#define BUTTON_UP 1
#define BUTTON_DOWN 0
#define BUTTON_OK 4
#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define LED_PIN 8
#define NUM_LEDS 6
#define CHIP_SELECT 29 // SD card chip select


#define COLOR_BACKGROUND 0x0000    // Background color
#define COLOR_ITEM 0x7BEF          // Color for items
#define COLOR_TEXT 0xFFFF          // Text color


SPIClass spi_(SPI0);  // Use SPI0 for Maix series LCD
Sipeed_ST7789 lcd(320, 240, spi_);
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

Sipeed_GC0328 camera(FRAMESIZE_QVGA, PIXFORMAT_RGB565, &Wire);
KPUClass KPU;
MBNet_1000 mbnet(KPU, lcd, camera);

const char* kmodel_name = "m";


Clock myClock(lcd, 160, 90, 60);


const int bufferSize = 64;
uint16_t buffer[bufferSize];



//////////////////////////////////////////////////////////////////////////////////////////////////

enum appepage {Main_Page, Secand_Page, ClockMode, WeathMode, ImgMode, Stoph, Btnov, Btmod, Objectdt, camtest, ImgeeMod, BlkGug, Working};
enum appepage movepage = Btmod;

int page = 0;  
bool isInSubMenu = false;  
int subMenuselected = 0;     

int item_selected = 0;
int previous_item_selected = 0; 
bool isMenuScreen = true;

// Gauge values
int tempPercentage = 0;
int humidityPercentage = 0;
int soilMoistPercentage = 0;


bool switch1State = false;
bool switch2State = true;
bool switch3State = false;

void setup() {
  //Serial.begin(115200);
  lcd.begin(15000000, COLOR_BLACK);
  //lcd.begin();
  strip.begin();
  strip.show();  
   Bot_init();
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  lcd.setRotation(2);

  /*if (!SD.begin(CHIP_SELECT)) {
    Serial.println("SD card initialization failed!");
    while (true);
  }
  Serial.println("SD card initialized.");*/
   
  if( mbnet.begin(kmodel_name) != 0)
    {
        
        while(1);
    }
  
  
}

void loop() {
    switch (movepage) {
        case Main_Page:
            handleMainMenuInput();
            break;
        case Secand_Page:
            handleSubMenuInput();
            break;
        case ClockMode:
           displayClock();
            break;
        case WeathMode:
           displayweath();
            break;
        case ImgMode:
            displayimg();
            break;
        case Stoph:
            displayStopwatch();
            break;
            case Btnov:
            buttonMoveBot();
            break;
             case Btmod:
            botwelcome();
            break;
            case Objectdt:
            objectdect();
            break;
            case camtest:
            cameratst();
            break;
            case ImgeeMod:
           displayimge();
            break;
            case BlkGug:
           Gauge_test();
            break;
             case Working:
           Feture_function();
            break;
    }
}



void handleMainMenuInput() {
    bool Draw_First_Pag = true;
    uint32_t loopStartMs;

    bool btn_Up_WasDown = false;
    bool btn_Down_WasDown = false;
    bool btn_Left_WasDown = false;
    bool btn_Right_WasDown = false;
    bool btn_Ok_WasDown = false;

    while (true) {
        loopStartMs = millis();

        if (Draw_First_Pag) {
            Draw_First_Pag = false;
            me_box();
            drawCursor(item_selected, true);
        }

       
        if (binIsDown(BUTTON_UP)) btn_Up_WasDown = true;
        if (binIsDown(BUTTON_DOWN)) btn_Down_WasDown = true;
        if (binIsDown(BUTTON_LEFT)) btn_Left_WasDown = true;
        if (binIsDown(BUTTON_RIGHT)) btn_Right_WasDown = true;
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

        // Button functions
        if (btn_Up_WasDown && binIsUp(BUTTON_UP)) {
            moveCursor(-3);
            btn_Up_WasDown = false;
        } else if (btn_Down_WasDown && binIsUp(BUTTON_DOWN)) {
            moveCursor(3);
            btn_Down_WasDown = false;
        } else if (btn_Left_WasDown && binIsUp(BUTTON_LEFT)) {
            moveCursor(-1);
            btn_Left_WasDown = false;
        } else if (btn_Right_WasDown && binIsUp(BUTTON_RIGHT)) {
            moveCursor(1);
            btn_Right_WasDown = false;
        } else if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
                  btn_Ok_WasDown = false;
            if (subMenuCounts[item_selected] > 0) {
                lcd.fillScreen(colors[item_selected]);
                fadeLEDColor(colors[item_selected]);
                subMenuselected = 0;
                page = 0;
                movepage = Secand_Page; 
                break;
            }
        }

        while (millis() - loopStartMs < 20) {
            delay(2);
        }
    }
}



// ***************************  SUB SECAND MANU DISPLAY ************************* // 

void handleSubMenuInput() {
    bool Draw_Secand_Pag = true;
    uint32_t loopStartMs;

    bool btn_Up_WasDown = false;
    bool btn_Down_WasDown = false;
    bool btn_Left_WasDown = false;
    bool btn_Right_WasDown = false;
     bool btn_Ok_WasDown = false;

    while (true) {
        loopStartMs = millis();

        if (Draw_Secand_Pag) {
            Draw_Secand_Pag = false;
            drawSubMenuPage();
        }

        // Button activity detection
        if (binIsDown(BUTTON_UP)) btn_Up_WasDown = true;
        if (binIsDown(BUTTON_DOWN)) btn_Down_WasDown = true;
        if (binIsDown(BUTTON_LEFT)) btn_Left_WasDown = true;
        if (binIsDown(BUTTON_RIGHT)) btn_Right_WasDown = true;
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

        
        if (btn_Up_WasDown && binIsUp(BUTTON_UP)) {
          btn_Up_WasDown = false;
            movepage = Main_Page; 
            break;
        } else if (btn_Right_WasDown && binIsUp(BUTTON_RIGHT))  {
          btn_Right_WasDown = false;
            if (subMenuselected < subMenuCounts[item_selected] - 1) {
                subMenuselected++;
                if (subMenuselected % 4 == 0 && subMenuselected > 0) {
                    page++;
                }
            }
            Draw_Secand_Pag = true;
           // btn_Down_WasDown = false;
        } else if (btn_Left_WasDown && binIsUp(BUTTON_LEFT)) {
           btn_Left_WasDown = false;
            if (subMenuselected > 0) {
                subMenuselected--;
            } else if (page > 0) {
                page--;
                subMenuselected = 3;
            }
            Draw_Secand_Pag = true;
            btn_Left_WasDown = false;
        }  else if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
          btn_Ok_WasDown = false;

          if (item_selected == 1 && subMenuselected == 0) {
        movepage = Working;
        break;
    } else if (item_selected == 3 && subMenuselected == 0) {
        movepage = ClockMode; 
        break;
            
    }  else if (item_selected == 6 && subMenuselected == 0) {
        movepage = WeathMode; 
        break;
              
    }  else if (item_selected == 3 && subMenuselected == 3) {
        movepage = ImgMode; 
        break;
            
    } else if (item_selected == 3 && subMenuselected == 1) {
        movepage =  Stoph; 
        break;
    } else if (item_selected == 0 && subMenuselected == 1) {
        movepage =  Btnov; 
        break;
    } else if (item_selected == 0 && subMenuselected == 0) {
        movepage = Btmod; 
        break;
    } else if (item_selected == 2 && subMenuselected == 0) {
        movepage = Objectdt; 
        break;
    } else if (item_selected == 2 && subMenuselected == 1) {
        movepage = camtest; 
        break;
    } else if (item_selected == 5 && subMenuselected == 1) {
        movepage = ImgeeMod; 
        break;
    } else if (item_selected == 4 && subMenuselected == 0) {
        movepage = BlkGug; 
        break;
    }


    ///////===========----------++++
        }

        while (millis() - loopStartMs < 20) {
            delay(2);
        }
    }
}




void drawSubMenuPage() {
  lcd.fillScreen(COLOR_BACKGROUND);

  for (int i = 0; i < 4; i++) {
    int itemselected = page * 4 + i;
    if (itemselected < subMenuCounts[item_selected]) {
      int y = 8 + i * 58;
      lcd.fillRect(10, y, 300, 50, COLOR_ITEM);
      lcd.setCursor(90, y + 10);
      lcd.setTextColor(COLOR_TEXT);
      lcd.setTextSize(2);
      lcd.println(subItemNames[item_selected][itemselected]);
      if (i == subMenuselected % 4) {
        lcd.drawRect(10, y, 304, 54,  0xF800);
        lcd.drawRect(11, y + 1, 302, 52,  0xF800);
        lcd.drawRect(12, y + 2, 300, 50,  0xF800);
      }
    }
  }
}


bool binIsDown(int btn) { 
  return digitalRead(btn) == LOW && digitalRead(btn) == LOW; 
}

bool binIsUp(int btn) { 
  return digitalRead(btn) == HIGH && digitalRead(btn) == HIGH;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void me_box() {
  lcd.fillScreen(COLOR_BLACK);
  for (int i = 0; i < NUM_ITEMS; i++) {
    lcd.fillRect(menu_items[i][0], menu_items[i][1], 100, 65, colors[i]);
  }

  // Display icons
  lcd.drawImage(36, 177, 39, 45, (uint16_t*)gImage_face_t);
  lcd.drawImage(232, 26, 63, 59, (uint16_t*)gImage_weather_ice);
  lcd.drawImage(237, 102, 51, 51, (uint16_t*)gImage_set_ice);
  lcd.drawImage(27, 31, 55, 54, (uint16_t*)gImage_bot_ice);
  lcd.drawImage(33, 101, 45, 51, (uint16_t*)gImage_api_ice);
  lcd.drawImage(119, 92, 70, 64, (uint16_t*)gImage_chat_ice);
  lcd.drawImage(129, 27, 62, 63, (uint16_t*)gImage_clk_ice);
  lcd.drawImage(136, 175, 49, 45, (uint16_t*)gImage_image_ice);
  lcd.drawImage(234, 175, 55, 47, (uint16_t*)gImage_music_ice);
}

void drawCursor(int selected, bool draw) {
  int x = menu_items[selected][1];
  int y = menu_items[selected][0];
  uint16_t color = draw ? COLOR_RED : colors[selected]; // Red for cursor, original color to clear

  lcd.drawRect(y, x, 101, 66, color);
  lcd.drawRect(y + 1, x + 1, 101, 66, color);
  lcd.drawRect(y + 2, x + 2, 101, 66, color);
}





void moveCursor(int delta) {
  drawCursor(previous_item_selected, false);

  item_selected = (item_selected + delta + NUM_ITEMS) % NUM_ITEMS;

  drawCursor(item_selected, true);

  previous_item_selected = item_selected;

  updateLEDColor(colors[item_selected]);
}

void updateLEDColor(uint16_t color) {
  uint8_t r = (color >> 11) & 0x1F;
  uint8_t g = (color >> 5) & 0x3F;
  uint8_t b = color & 0x1F;

  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color((r * 255) / 31, (g * 255) / 63, (b * 255) / 31));
  }
  strip.show();
}

void fadeLEDColor(uint16_t color) {
  uint8_t r = (color >> 11) & 0x1F;
  uint8_t g = (color >> 5) & 0x3F;
  uint8_t b = color & 0x1F;

  for (int j = 0; j < 256; j += 5) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color((r * j) / 255, (g * j) / 255, (b * j) / 255));
    }
    strip.show();
    delay(20);
  }
}

void displayimg() {
  
   uint32_t loopStartMs;

  bool btn_Ok_WasDown = false;
  while (true) {
        loopStartMs = millis();
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

  static unsigned long lastMillis = 0;
  if (millis() - lastMillis >= 1000) { // Update every second
    lastMillis = millis();

    lcd.fillScreen(COLOR_BLACK);
    lcd.setTextColor(COLOR_WHITE);
    lcd.setTextSize(4);

    int hours = (millis() / 3600000) % 24;
    int minutes = (millis() / 60000) % 60;
    int seconds = (millis() / 1000) % 60;

    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);
    lcd.setCursor(50, 100);
    lcd.print(timeStr);
  }

  if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
    btn_Ok_WasDown = false;
    movepage = Secand_Page;
    break;  // Exit the loop
}

   while (millis() - loopStartMs < 20) {
            delay(2);
        }
  }
  
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void displayweath() {

   uint32_t loopStartMs;

    bool btn_Ok_WasDown = false;
  while (true) {
        loopStartMs = millis();
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

   myClock.displayWeatherStation();

  if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
    btn_Ok_WasDown = false;
    movepage = Secand_Page;
    break;  // Exit the loop
}

   while (millis() - loopStartMs < 20) {
            delay(2);
        }
  }
  
}


///////////////////////////////////////////////////////////////////////////////////////////////

  void displayClock() {
    bool Draw_Clock_Pag = true;
    uint32_t loopStartMs;
    uint32_t lastClockUpdateMs = 0; // For non-blocking clock updates

    bool btn_Left_WasDown = false;
    bool btn_Right_WasDown = false;
    bool btn_Ok_WasDown = false;

    while (true) {
        loopStartMs = millis();

        // Draw clock face initially
        if (Draw_Clock_Pag) {
            Draw_Clock_Pag = false;
            delay(20);
            myClock.drawClockFace();
        
        }

        
        if (millis() - lastClockUpdateMs >= 1000) {
            lastClockUpdateMs = millis();

            
            int hours = 10;          
            int minutes = 30;        
            int seconds = lastClockUpdateMs / 1000 % 60; 

            myClock.updateClock(hours, minutes, seconds); 
        }

        
        if (binIsDown(BUTTON_LEFT)) btn_Left_WasDown = true;
        if (binIsDown(BUTTON_RIGHT)) btn_Right_WasDown = true;
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

       
        if (btn_Left_WasDown && binIsUp(BUTTON_LEFT)) {
           Draw_Clock_Pag = true;
            btn_Left_WasDown = false; 
            myClock.toggleMode(false); 
        }

        
        if (btn_Right_WasDown && binIsUp(BUTTON_RIGHT)) {
           Draw_Clock_Pag = true;
            btn_Right_WasDown = false; 
            myClock.toggleMode(true); 
        }

       
        if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
            btn_Ok_WasDown = false; 
            movepage = Secand_Page; 
            break;  
        }

        
        while (millis() - loopStartMs < 20) {
            delay(2);
        }
    }
}

void displayStopwatch() {
    bool Draw_Stopw_Pag = true;
    uint32_t loopStartMs;
    uint32_t startTime = 0;
    uint32_t elapsedTime = 0;
    bool isRunning = false; 
    bool btn_Left_WasDown = false;
    bool btn_Right_WasDown = false;
    bool btn_Up_WasDown = false;
    bool btn_Ok_WasDown = false;

    
    if (Draw_Stopw_Pag) {
        Draw_Stopw_Pag = false;
        lcd.fillScreen(COLOR_BLACK); 
        lcd.setCursor(40, 20);
        lcd.setTextColor(COLOR_WHITE);
        lcd.setTextSize(4);
        lcd.print("Stopwatch");
    }

    while (true) {
        loopStartMs = millis();

       
        uint32_t displayTime = isRunning ? millis() - startTime + elapsedTime : elapsedTime;
        uint32_t seconds = (displayTime / 1000) % 60;
        uint32_t minutes = (displayTime / (1000 * 60)) % 60;
        uint32_t hours = (displayTime / (1000 * 60 * 60)) % 24;

        char timeStr[9];
        sprintf(timeStr, "%02u:%02u:%02u", (unsigned int)hours, (unsigned int)minutes, (unsigned int)seconds);

        
        lcd.fillRect(50, 80, 220, 70, COLOR_BLACK); // Adjust dimensions as needed

        
        lcd.drawRect(50, 80, 220, 70, COLOR_WHITE);

        
        lcd.setCursor(70, 100); 
        lcd.setTextSize(4);
        lcd.setTextColor(COLOR_WHITE);
        lcd.print(timeStr);

        
        lcd.fillRect(10, 170, 300, 30, COLOR_BLACK); 
        lcd.setCursor(100, 160); 
        lcd.setTextSize(3);
        lcd.setTextColor(COLOR_WHITE);
        lcd.print(isRunning ? "Running" : "Stopped");

        
        if (binIsDown(BUTTON_LEFT)) btn_Left_WasDown = true;
        if (binIsDown(BUTTON_RIGHT)) btn_Right_WasDown = true;
        if (binIsDown(BUTTON_UP)) btn_Up_WasDown = true;
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

        
        if (btn_Right_WasDown && binIsUp(BUTTON_RIGHT)) {
            btn_Right_WasDown = false;
            if (!isRunning) {
                isRunning = true;
                startTime = millis(); 
            }
        }

        
        if (btn_Left_WasDown && binIsUp(BUTTON_LEFT)) {
            btn_Left_WasDown = false;
            if (isRunning) {
                isRunning = false;
                elapsedTime += millis() - startTime; 
            }
        }

        
        if (btn_Up_WasDown && binIsUp(BUTTON_UP)) {
            btn_Up_WasDown = false;
            Draw_Stopw_Pag = true; 
            isRunning = false;
            startTime = 0;
            elapsedTime = 0; 
        }

        
        if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
            btn_Ok_WasDown = false;
            movepage = Secand_Page;
            break; 
        }

        
        while (millis() - loopStartMs < 10) {
            delay(2);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void randomBotMovement() {
    int moveChoice = random(0, 4);
    switch (moveChoice) {
        case 0:
            Bot_forward();
            delay(500);
            break;
        case 1:
            Bot_turnLeft();
            delay(500);
            break;
        case 2:
            Bot_turnRight();
            delay(500);
            break;
        default:
            Bot_stop();
            delay(500);
            break;
    }
    Bot_stop();
}

void botwelcome() {
    uint32_t loopStartMs;
    uint32_t lastActionTime = millis();  
    int actionState = 0;  

    bool btn_Ok_WasDown = false;

    while (true) {
        loopStartMs = millis();

        
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

        
        if (millis() - lastActionTime >= 2000) {  
            lastActionTime = millis();  

            switch (actionState) {
                case 0:
                    myClock.neutralExpression();
                    break;
                case 1:
                    myClock.blinkEyes();  
                    myClock.neutralExpression();
                    break;
                case 2:
                    randomBotMovement();  
                    break;
                case 3:
                    myClock.angryExpression();
                    break;
                case 4:
                    myClock.blinkEyes();  
                    myClock.angryExpression();
                    break;
                case 5:
                    randomBotMovement();  
                    break;
            }

            
            actionState = (actionState + 1) % 6;
        }

        
        if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
                 Bot_stop();
            btn_Ok_WasDown = false;
            movepage = Main_Page;  
            break;  
        }

        
        while (millis() - loopStartMs < 20) {
            delay(2);
        }
    }
}

void buttonMoveBot() {
    uint32_t loopStartMs;
    uint32_t actionStartTime = 0; 
    uint32_t blinkStartTime = 0;  
    bool isActionActive = false;  
    int currentAction = -1;       

    bool btn_Up_WasDown = false;
    bool btn_Down_WasDown = false;
    bool btn_Left_WasDown = false;
    bool btn_Right_WasDown = false;
    bool btn_Ok_WasDown = false;
     bool Backgroundbot = true;

    while (true) {
        loopStartMs = millis();

         if (Backgroundbot) {
            Backgroundbot = false;
           lcd.fillScreen(COLOR_BLACK);
        }


        
        if (binIsDown(BUTTON_DOWN)) btn_Up_WasDown = true;   
        if (binIsDown(BUTTON_UP)) btn_Down_WasDown = true;   
        if (binIsDown(BUTTON_RIGHT)) btn_Left_WasDown = true; 
        if (binIsDown(BUTTON_LEFT)) btn_Right_WasDown = true; 
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

        
        if (!isActionActive) {
            if (btn_Up_WasDown && binIsUp(BUTTON_DOWN)) {   
                btn_Up_WasDown = false;
                //myClock.blinkEyes();
                myClock.neutralExpression();
                Bot_turnLeft();
                isActionActive = true;
                actionStartTime = millis();
                currentAction = 0; 
            } else if (btn_Down_WasDown && binIsUp(BUTTON_UP)) { 
                btn_Down_WasDown = false;
                //myClock.blinkEyes();
                myClock.angryExpression();
                Bot_turnRight();
                isActionActive = true;
                actionStartTime = millis();
                currentAction = 1; 
            } else if (btn_Left_WasDown && binIsUp(BUTTON_RIGHT)) { 
                btn_Left_WasDown = false;
                //myClock.blinkEyes();
                myClock.neutralExpression();
                Bot_backward();
                isActionActive = true;
                actionStartTime = millis();
                currentAction = 2; 
            } else if (btn_Right_WasDown && binIsUp(BUTTON_LEFT)) { 
                btn_Right_WasDown = false;
               // myClock.blinkEyes();
                myClock.angryExpression();
                Bot_forward();
                isActionActive = true;
                actionStartTime = millis();
                currentAction = 3; 
            } else if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
                btn_Ok_WasDown = false;
                Bot_stop();
                movepage = Secand_Page;
                break; 
            }
        }

        
        if (isActionActive && millis() - actionStartTime >= 500) {
            Bot_stop();
            isActionActive = false;
            currentAction = -1;
        }

        
        if (millis() - blinkStartTime >= 2000) {
            blinkStartTime = millis(); 
           // myClock.blinkEyes();
           myClock.neutralExpression();
        }

        
        while (millis() - loopStartMs < 20) {
            delay(2);
        }
    }
}
////////////////-===----------------========================-----------------------------======================//////////////
void objectdect() {
    uint32_t loopStartMs;
    bool btn_Ok_WasDown = false;
   /* bool Sd_ineta = true;

    if (Sd_ineta) {
        Sd_ineta = false;

    // Initialize SD Card (if required in your project)
    if (!SD.begin()) {
        //lcd.setCursor(10, 80);
        //lcd.setTextColor(COLOR_WHITE);
        //lcd.setTextSize(3);
        //lcd.print("SD Card init failed!");
        while (1); // Halt the program if SD initialization fails
    }
    }*/

    
    while (true) {
        loopStartMs = millis();

   
        
         mbnet.show(); 

        // Check if OK button is pressed
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

        // Perform object detection
        //if (mbnet.detect() == 1) {
            //mbnet.show(); 
       // }

        
        if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
            btn_Ok_WasDown = false;
            movepage = Secand_Page;
            break; 
        }

        
        while (millis() - loopStartMs < 20) {
            delay(10);
        }
    }
}

//-============================------------------==================================///////////////////

void cameratst() {
    uint32_t loopStartMs; 
    bool btn_Ok_WasDown = false;

    while (true) {
        loopStartMs = millis();

        
        uint8_t* img = camera.snapshot();
        if (img == nullptr || img == 0) {
            Serial.printf("snap fail\r\n");
        } else {
            
            const int width = camera.width();   
            const int height = camera.height(); 
            static uint16_t rotatedImage[320 * 240]; 

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    
                    int rotatedX = width - 1 - x;  
                    int rotatedY = height - 1 - y; 
                    rotatedImage[rotatedY * width + rotatedX] = ((uint16_t*)img)[y * width + x];
                }
            }

            
            lcd.drawImage(0, 0, width, height, rotatedImage);
        }

        
        if (binIsDown(BUTTON_OK)) {
            btn_Ok_WasDown = true;
        }

        if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
            btn_Ok_WasDown = false;
            movepage = Secand_Page;
            break; 
        }

        
        while (millis() - loopStartMs < 20) {
            delay(2); 
        }
    }
}


/////////////////////////--------------------------00000000000000000000////////////////////////////////////////

void displayimge() {
    const char* imageFiles[] = { "/image1.bmp", "/image2.bmp", "/image3.bmp", "/image4.bmp", "/image5.bmp" };
    const int numImages = 5;

    int xPos = 0;
    int yPos = 0;

    uint32_t loopStartMs;
    bool btn_Ok_WasDown = false;

      lcd.fillScreen(COLOR_BLACK);
       lcd.drawImage(136, 80, 49, 45, (uint16_t*)gImage_image_ice);

    for (int i = 0; i < numImages; i++) {
        
        displayBMPFromSD(imageFiles[i], xPos, yPos);

        
        for (uint32_t startTime = millis(); millis() - startTime < 5000; ) {
            loopStartMs = millis();

            
            if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

            
            if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
                btn_Ok_WasDown = false;
                movepage = Main_Page; 
                return;               
            }

            
            while (millis() - loopStartMs < 20) {
                delay(2);
            }
        }
    }
}

void displayBMPFromSD(const char* filename, int x, int y) {
  File bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.println("Error opening BMP file!");
    return;
  }

  
  bmpFile.seek(54);  

  int width = 320;    
  int height = 240;   

  
  for (int row = height - 1; row >= 0; row--) {
    for (int col = 0; col < width; col += bufferSize) {
      int pixelsToRead = min(bufferSize, width - col);

      for (int i = 0; i < pixelsToRead; i++) {
        
        uint8_t b = bmpFile.read();
        uint8_t g = bmpFile.read();
        uint8_t r = bmpFile.read();

        
        buffer[i] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

        
        lcd.drawPixel(x + col + i, y + row, buffer[i]);
      }
    }
  }
  bmpFile.close();
}

////////////////////////////////////////-------------------------===============================================///////////////////////

void Gauge_test() {
    bool Draw_Gauge_Face = true;
    uint32_t loopStartMs;

    bool btn_Up_WasDown = false;
    bool btn_Down_WasDown = false;
    bool btn_Left_WasDown = false;
    bool btn_Right_WasDown = false;
    bool btn_Ok_WasDown = false;

    while (true) {
        loopStartMs = millis();

        if (Draw_Gauge_Face) {
           Draw_Gauge_Face = false;
           lcd.fillScreen(COLOR_BLACK);
            Gauge_title();

        }

        // Button activity detection
        if (binIsDown(BUTTON_UP)) btn_Up_WasDown = true;
        if (binIsDown(BUTTON_DOWN)) btn_Down_WasDown = true;
        if (binIsDown(BUTTON_LEFT)) btn_Left_WasDown = true;
        if (binIsDown(BUTTON_RIGHT)) btn_Right_WasDown = true;
        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

        
        if (btn_Up_WasDown && binIsUp(BUTTON_UP)) {
          
            tempPercentage = (tempPercentage + 10) % 110; 
        drawCircularGauge(60, 100, 30, tempPercentage, COLOR_YELLOW, COLOR_BLACK, "Temp");
         btn_Up_WasDown = false;

           
        } else if (btn_Down_WasDown && binIsUp(BUTTON_DOWN)) {
            
           soilMoistPercentage = (soilMoistPercentage + 10) % 110;
        drawCircularGauge(260, 100, 30, soilMoistPercentage, COLOR_GREEN, COLOR_BLACK, "Soil Moist");
         btn_Down_WasDown = false;
       
        } else if (btn_Left_WasDown && binIsUp(BUTTON_LEFT)) {
            humidityPercentage = (humidityPercentage + 10) % 110;
        drawCircularGauge(160, 100, 30, humidityPercentage, COLOR_BLUE, COLOR_BLACK, "Humidity");
            btn_Left_WasDown = false;

        } else if (btn_Right_WasDown && binIsUp(BUTTON_RIGHT)) {
           switch1State = !switch1State; 
        switch2State = !switch2State;
        switch3State = !switch3State;

        drawSwitch(20, 200, 80, 40, switch1State, "Switch 1");
        drawSwitch(120, 200, 80, 40, switch2State, "Switch 2");
        drawSwitch(220, 200, 80, 40, switch3State, "Switch 3");

            btn_Right_WasDown = false;
        } else if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
                  btn_Ok_WasDown = false;
             btn_Ok_WasDown = false;
            movepage = Secand_Page;  
            break;  // Exit the loop
        }

        while (millis() - loopStartMs < 20) {
            delay(2);
        }
    }
}

void Gauge_title() {

   drawCircularGauge(60, 100, 30, tempPercentage, COLOR_YELLOW, COLOR_BLACK, "Temp");
    drawCircularGauge(160, 100, 30, humidityPercentage, COLOR_BLUE, COLOR_BLACK, "Humidity");
    drawCircularGauge(260, 100, 30, soilMoistPercentage, COLOR_GREEN, COLOR_BLACK, "Soil Moist");

    drawSwitch(20, 200, 80, 40, switch1State, "Switch 1");
    drawSwitch(120, 200, 80, 40, switch2State, "Switch 2");
    drawSwitch(220, 200, 80, 40, switch3State, "Switch 3");
}


void drawCircularGauge(int centerX, int centerY, int radius, int percentage, uint16_t fillColor, uint16_t emptyColor, const char* label) {
    
    lcd.fillCircle(centerX, centerY, radius, emptyColor);

    
    lcd.fillCircle(centerX, centerY, radius - 5, 0x7BEF); 

    
    int maxAngle = percentage * 360 / 100;

    
    for (int angle = 0; angle < 360; angle += 2) {
        float radian = radians(angle + 90); 
        int x = centerX + radius * cos(radian);
        int y = centerY + radius * sin(radian); 
        uint16_t color = (angle <= maxAngle) ? fillColor : emptyColor; 
        lcd.drawPixel(x, y, color);
    }

    
    lcd.setTextColor(COLOR_BLACK); 
    lcd.setTextSize(1);
    lcd.setCursor(centerX - 15, centerY - 5);
    lcd.printf("%d%%", percentage);

   
    lcd.setTextColor(COLOR_WHITE);
    lcd.setTextSize(1);
    lcd.setCursor(centerX - 25, centerY + radius + 10);
    lcd.print(label);
}


void drawSwitch(int x, int y, int width, int height, bool state, const char* label) {
    
    lcd.drawRect(x, y, width, height, COLOR_WHITE); 
    
    
    if (state) {
        lcd.fillRect(x + 2, y + 2, width - 4, height - 4, COLOR_GREEN); 
        lcd.setTextColor(COLOR_BLACK); 
        lcd.setCursor(x + (width / 2) - 10, y + (height / 2) - 5);
        lcd.print("ON");
    } else {
        lcd.fillRect(x + 2, y + 2, width - 4, height - 4, COLOR_RED); 
        lcd.setTextColor(COLOR_WHITE); 
        lcd.setCursor(x + (width / 2) - 15, y + (height / 2) - 5);
        lcd.print("OFF");
    }

    
    lcd.setTextColor(COLOR_WHITE);
    lcd.setCursor(x + (width / 2) - 15, y + height + 5);
    lcd.print(label);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Feture_function() {
  
   uint32_t loopStartMs;
   bool Draw_title = true;

    bool btn_Ok_WasDown = false;
  while (true) {
        loopStartMs = millis();

        if (Draw_title) {
           Draw_title = false;
    lcd.fillScreen(COLOR_BACKGROUND);
    lcd.setCursor(100, 100);
    lcd.setTextColor(COLOR_TEXT);
    lcd.setTextSize(2);
    lcd.println("Not Implement Yet");

        }

        if (binIsDown(BUTTON_OK)) btn_Ok_WasDown = true;

  if (btn_Ok_WasDown && binIsUp(BUTTON_OK)) {
    btn_Ok_WasDown = false;
    movepage = Secand_Page;
    break;  
}

   while (millis() - loopStartMs < 20) {
            delay(2);
        }
  }
  
}
