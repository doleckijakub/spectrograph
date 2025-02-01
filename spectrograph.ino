#include <Wire.h>
#include <LiquidCrystal.h>

#define LCD_W_CHARS 16
#define LCD_H_CHARS 2

#define BOARD_W_CHARS 4
#define BOARD_H_CHARS 2

#define BOARD_W (BOARD_W_CHARS * 5)
#define BOARD_H (BOARD_H_CHARS * 8)

#define PIN_LCD_RS 8
#define PIN_LCD_E 9
#define PIN_LCD_D4 4
#define PIN_LCD_D5 5
#define PIN_LCD_D6 6
#define PIN_LCD_D7 7

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_E, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
uint8_t customChar[BOARD_W_CHARS * BOARD_H_CHARS][8] = {0};

void drawPixel(int x, int y, bool on) {
  int charX = x / 5;
  int charY = y / 8;
  int pixelX = x % 5;
  int pixelY = y % 8;
  
  if (charX < BOARD_W_CHARS && charY < BOARD_H_CHARS) {
    int charIndex = charX + charY * BOARD_W_CHARS;
    uint8_t currentState = customChar[charIndex][pixelY] & (1 << (4 - pixelX));
    
    if ((on && currentState) || (!on && !currentState)) return;
    
    if (on) customChar[charIndex][pixelY] |= (1 << (4 - pixelX));
    else customChar[charIndex][pixelY] &= ~(1 << (4 - pixelX));

    lcd.createChar(charIndex, customChar[charIndex]);
    lcd.setCursor(charX, charY);
    lcd.write(charIndex);
  }
}

void drawGraph(float values[]) {
  for (int x = 1; x < BOARD_W - 1; x++)
    for (int y = 1; y < BOARD_H - 1; y++)
      drawPixel(x, y, false);
  
  for (int x = 0; x < BOARD_W; x++) {
    drawPixel(x, 0, true);
    drawPixel(x, BOARD_H - 1, true);
    
    if (1 <= x && x <= 19 && 0.f <= values[x - 1] && values[x - 1] <= 1.f)
      drawPixel(x, 1 + int((1.f - values[x - 1]) * (BOARD_H - 2)), true);
  }

  for (int y = 0; y < BOARD_H; y++) {
    drawPixel(0, y, true);
    drawPixel(BOARD_W - 1, y, true);
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(LCD_W_CHARS, LCD_H_CHARS);
  lcd.clear();
  lcd.print("Spectrograph");
  delay(2000);
  lcd.clear();
}

void loop() {
  float values[BOARD_W - 2] = {0};

  for (int i = 0; Serial.available() && i < BOARD_W - 2; i++) {
    values[i] = Serial.parseFloat();
  }
  
  drawGraph(values);

  lcd.setCursor(5, 0);
  lcd.print(values[0]);

  delay(200);
}
