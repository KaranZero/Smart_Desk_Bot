#ifndef CLOCK_H
#define CLOCK_H


// Clock Class to encapsulate all functionality
class Clock {
private:
    int centerX, centerY, radius;
    float prevHourAngle, prevMinuteAngle, prevSecondAngle;
    Sipeed_ST7789& lcd;
    int mode; // 0: Analog + Digital, 1: Digital Only, 2: Analog Only


    // Eye settings
int leftEyeX = 90;
int rightEyeX = 230;
int eyeY = 120;
int eyeWidth = 80;
int eyeHeight = 60;
int cornerRadius = 10;

// Colors
uint16_t eyeColor = COLOR_BLUE;
uint16_t pupilColor = COLOR_BLACK;
uint16_t whiteColor = COLOR_WHITE;
uint16_t backgroundColor = COLOR_BLACK;


public:
    // Constructor to initialize clock settings
    Clock(Sipeed_ST7789& display, int x, int y, int r)
        : centerX(x), centerY(y), radius(r), prevHourAngle(0), prevMinuteAngle(0), prevSecondAngle(0), lcd(display), mode(0) {}

    // Draw the clock face
    void drawClockFace() {
        lcd.fillScreen(COLOR_BLACK);
        lcd.drawCircle(centerX, centerY, radius, COLOR_WHITE);

        // Draw hour markers
        for (int i = 0; i < 12; i++) {
            float angle = i * 30 * PI / 180;
            int x1 = centerX + cos(angle) * (radius - 6);
            int y1 = centerY + sin(angle) * (radius - 6);
            int x2 = centerX + cos(angle) * radius;
            int y2 = centerY + sin(angle) * radius;
            lcd.drawLine(x1, y1, x2, y2, COLOR_WHITE);
        }
    }

   // Weather station screen function
void displayWeatherStation() {
  lcd.fillScreen(COLOR_BLACK);
  
  lcd.setTextColor(COLOR_WHITE);

  // Placeholder weather data (you can replace with real data)
  float temperature = 22.5; // Temperature in °C
  float humidity = 65.0;    // Humidity in %
  const char* weatherCondition = "Sunny"; // Weather condition
   lcd.setTextSize(3);
  lcd.setCursor(20, 30);
  lcd.print("Weather Station");

  // Display temperature, humidity, and weather condition
  lcd.setTextSize(2);
  lcd.setCursor(80, 80);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
lcd.setTextSize(2);
  lcd.setCursor(80, 110);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");
lcd.setTextSize(2);
  lcd.setCursor(80, 140);
  lcd.print("Condition: ");
  lcd.print(weatherCondition);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drawRoundedSquareEye(int x, int y, int pupilOffsetX, int pupilOffsetY) {
    lcd.fillRect(x - eyeWidth / 2 + cornerRadius, y - eyeHeight / 2, eyeWidth - 2 * cornerRadius, eyeHeight, 0x001F); 
    lcd.fillRect(x - eyeWidth / 2, y - eyeHeight / 2 + cornerRadius, eyeWidth, eyeHeight - 2 * cornerRadius, 0x001F);
    lcd.fillCircle(x - eyeWidth / 2 + cornerRadius, y - eyeHeight / 2 + cornerRadius, cornerRadius, 0x001F);
    lcd.fillCircle(x + eyeWidth / 2 - cornerRadius, y - eyeHeight / 2 + cornerRadius, cornerRadius, 0x001F);
    lcd.fillCircle(x - eyeWidth / 2 + cornerRadius, y + eyeHeight / 2 - cornerRadius, cornerRadius, 0x001F);
    lcd.fillCircle(x + eyeWidth / 2 - cornerRadius, y + eyeHeight / 2 - cornerRadius, cornerRadius,0x001F);
    lcd.fillCircle(x + pupilOffsetX, y + pupilOffsetY, 15, 0x0000); // Pupil
}

// Function to blink eyes by covering them with black "eyelids"
void blinkEyes() {
    lcd.fillRect(leftEyeX - eyeWidth / 2, eyeY - eyeHeight / 2, eyeWidth, eyeHeight, 0x03E0);
    lcd.fillRect(rightEyeX - eyeWidth / 2, eyeY - eyeHeight / 2, eyeWidth, eyeHeight, 0x03E0);
    delay(300);  // Slow blink effect
}

// Function to draw neutral expression
void neutralExpression() {
    lcd.fillScreen(backgroundColor);
    drawRoundedSquareEye(leftEyeX, eyeY, 0, 0);  // Centered pupils
    drawRoundedSquareEye(rightEyeX, eyeY, 0, 0);
}

// Function to draw angry expression
void angryExpression() {
    lcd.fillScreen(backgroundColor);
    drawRoundedSquareEye(leftEyeX, eyeY, -10, 10);  // Pupils moved inward
    drawRoundedSquareEye(rightEyeX, eyeY, 10, 10);
    lcd.drawLine(leftEyeX - eyeWidth / 2, eyeY - eyeHeight / 2 - 10, leftEyeX + eyeWidth / 2, eyeY - eyeHeight / 2 - 15, 0xFFFF);
    lcd.drawLine(rightEyeX - eyeWidth / 2, eyeY - eyeHeight / 2 - 15, rightEyeX + eyeWidth / 2, eyeY - eyeHeight / 2 - 10, 0xFFFF);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // Draw a hand on the clock
    void drawHand(float angle, int length, uint16_t color, int thickness) {
        int x = centerX + cos(angle) * length;
        int y = centerY + sin(angle) * length;

        for (int i = -thickness / 2; i <= thickness / 2; i++) {
            lcd.drawLine(centerX, centerY + i, x, y + i, color);
        }
    }

    // Update the clock hands
    void updateHands(int hours, int minutes, int seconds) {
        float hourAngle = (hours % 12 + minutes / 60.0) * 30 * PI / 180;
        float minuteAngle = (minutes + seconds / 60.0) * 6 * PI / 180;
        float secondAngle = seconds * 6 * PI / 180;

        // Erase previous hands
        drawHand(prevHourAngle, radius - 20, COLOR_BLACK, 4);
        drawHand(prevMinuteAngle, radius - 10, COLOR_BLACK, 3);
        drawHand(prevSecondAngle, radius - 5, COLOR_BLACK, 1);

        // Draw new hands
        drawHand(hourAngle, radius - 20, COLOR_WHITE, 4);
        drawHand(minuteAngle, radius - 10, COLOR_LIGHTGREY, 3);
        drawHand(secondAngle, radius - 5, COLOR_RED, 1);

        // Update previous angles
        prevHourAngle = hourAngle;
        prevMinuteAngle = minuteAngle;
        prevSecondAngle = secondAngle;
    }

    // Display digital time
    void displayDigitalTime(int hours, int minutes, int seconds, bool largeFont = false) {
        lcd.setTextSize(largeFont ? 3 : 2);
        lcd.setTextColor(COLOR_WHITE, COLOR_BLACK);

        char timeStr[9];
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hours, minutes, seconds);

        int x = largeFont ? 80 : 100; // Position based on font size
        int y = largeFont ? 100 : 200;
        lcd.setCursor(x, y);
        lcd.print(timeStr);
    }

    // Update the clock display
    void updateClock(int hours, int minutes, int seconds) {
        if (mode == 0) { // Analog + Digital
            //drawClockFace();
            updateHands(hours, minutes, seconds);
            displayDigitalTime(hours, minutes, seconds);
        } else if (mode == 1) { // Digital Only
            lcd.fillScreen(COLOR_BLACK); // Clear screen
            displayDigitalTime(hours, minutes, seconds, true); // Larger font
        } else if (mode == 2) { // Analog Only
           // drawClockFace();
            updateHands(hours, minutes, seconds);
        }
    }

    // Toggle the display mode (forward or reverse)
    void toggleMode(bool forward = true) {
        if (forward) {
            mode = (mode + 1) % 3; // Cycle forward: 0 -> 1 -> 2 -> 0
        } else {
            mode = (mode - 1 + 3) % 3; // Cycle backward: 2 -> 1 -> 0 -> 2
        }
    }
};

#endif // CLOCK_H
