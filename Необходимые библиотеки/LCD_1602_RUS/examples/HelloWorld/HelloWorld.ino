#include <LCD_1602_RUS.h>



void setup()
{
  lcd.init();                      //
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print(L"Теперь можно");
  lcd.setCursor(3, 1);
  lcd.print(L"по-русски");

}

void loop()
{
}
