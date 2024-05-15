#include <DS3231.h> //Библиотека для датчика реального времени 
#include <LCD_1602_RUS.h>   // Библиотека для монитора!
#include <Arduino.h> //Библиотека для кнопки
#include <EncButton.h> // Библиотека для кнопки
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <GyverNTC.h>
#include <EEPROM.h>



GyverNTC therm; // Температура датчиков
double lastTemp1 = 0;
double lastTemp2 = 0;
double temp1 = 0;
double temp2 = 0;

RF24 radio(48, 49); // Номер пинов для WI FI

byte address[] = "1Node"; // Адрес приёмника
bool valueToSend = false;

const int numRejims = 4;
boolean flagLoop8 = false;



#define RELAY_PIN8 33 // -> Пины реле 
#define RELAY_PIN1 35
#define RELAY_PIN2 36
#define RELAY_PIN3 37
#define RELAY_PIN4 38
#define RELAY_PIN5 39
#define RELAY_PIN6 40
#define RELAY_PIN7 41

#define NUM_RELAY_PINS 8 // Константа для количества реле


#define sensor A1 // Датчик влажности
#define wet 210 // Показатель датчика влажности
#define dry 510 // Показатель датчика влажности
int valueVlajnost = 0; // Константа для датчика
int pre = 0; // Константа для датчика
int maxHumidity = 50; // Константа с которой сравнивается значение в loop(Если влажность больше maxHimidity, то полив не пойдет)
int lastHumidity = 0; 

const int relayPins[NUM_RELAY_PINS] = {35, 36, 37, 38, 39, 40, 41, 33};



LCD_1602_RUS lcd(0x27, 16, 4);  // Инициализация экрана
DS3231 myRTC;  // Инициализация дачика реального времени 
EncButton enc(3,4,2); // Инициализация кнопки
int value = 0;
int value2 = 0;
int value3 = 0;
int value4 = 0;
int value5 = 0;
////////////
bool century = false;  // |
bool h12Flag;         //  | -> Все для настройки часов 
bool pmFlag;          //  |
bool ifDisplay1 = true; // Проверка на 1 экран
bool ifDisplay2 = false; // Проверка на 2 экран
bool ifDisplay3 = false;
bool ifDisplay4 = false;
bool ifDisplay5 = false;
bool ifDisplay6 = false;
bool ifDisplay7 = false;
bool ifDisplay8 = false;
bool ifDisplay9 = false;


// Проверка на реле
bool isReleOn1 = false;
bool isReleOn2 = false;
bool isReleOn3 = false;
bool isReleOn4 = false;
bool isReleOn5 = false;
bool isReleOn6 = false;
bool isReleOn7 = false;
bool isReleOn8 = false;

//Для функции проверки времени и включения автополива
bool* isRele[] = {&isReleOn1, &isReleOn2, &isReleOn3, &isReleOn4, &isReleOn5, &isReleOn6, &isReleOn7, &isReleOn8,};

// Название РЕЛЕ (Максим 7  смволов желательно 6)
char line1[] = "1 PEЛE";
char line2[] = "2 PEЛE";
char line3[] = "3 PEЛE";
char line4[] = "4 PEЛE";
char line5[] = "5 PEЛE";
char line6[] = "6 PEЛE";
char line7[] = "7 PEЛE";
char line8[] = "8 PEЛE";

//Выбор режимов включены/выключены
bool rejim1 = false;
bool rejim2 = false;
bool rejim3 = false;
bool rejim4 = false;
bool* isRejim[4] = {&rejim1, &rejim2, &rejim3, &rejim4};

//Выбор режимов для функции
bool thisRejim1 = false;
bool thisRejim2 = false;
bool thisRejim3 = false;
bool thisRejim4 = false;

char rejimText1[] = "PEЖUM 1";
char rejimText2[] = "PEЖUM 2";
char rejimText3[] = "PEЖUM 3";
char rejimText4[] = "PEЖUM 4";


//Переменные для реле режимов 
boolean rejim1Rele[NUM_RELAY_PINS] = {false}; 
boolean rejim2Rele[NUM_RELAY_PINS] = {false};
boolean rejim3Rele[NUM_RELAY_PINS] = {false};
boolean rejim4Rele[NUM_RELAY_PINS] = {false};

int rejim1MinOn[NUM_RELAY_PINS] = {NULL}; 
int rejim2MinOn[NUM_RELAY_PINS] = {NULL};
int rejim3MinOn[NUM_RELAY_PINS] = {NULL};
int rejim4MinOn[NUM_RELAY_PINS] = {NULL};

int rejim1MinOff[NUM_RELAY_PINS] = {NULL}; 
int rejim2MinOff[NUM_RELAY_PINS] = {NULL};
int rejim3MinOff[NUM_RELAY_PINS] = {NULL};
int rejim4MinOff[NUM_RELAY_PINS] = {NULL};

int rejim1HourOn[NUM_RELAY_PINS] = {NULL}; 
int rejim2HourOn[NUM_RELAY_PINS] = {NULL};
int rejim3HourOn[NUM_RELAY_PINS] = {NULL};
int rejim4HourOn[NUM_RELAY_PINS] = {NULL};

int rejim1HourOff[NUM_RELAY_PINS] = {NULL}; 
int rejim2HourOff[NUM_RELAY_PINS] = {NULL};
int rejim3HourOff[NUM_RELAY_PINS] = {NULL};
int rejim4HourOff[NUM_RELAY_PINS] = {NULL};

//Для работы с временем ( Раз в несколько дней -) хранить значения
int rejim1Day[NUM_RELAY_PINS] = {0,0,0,0,0,0,0,0};
int rejim2Day[NUM_RELAY_PINS] = {0,0,0,0,0,0,0,0};
int rejim3Day[NUM_RELAY_PINS] = {0,0,0,0,0,0,0,0};
int rejim4Day[NUM_RELAY_PINS] = {0,0,0,0,0,0,0,0};

//Для сравнения значений с выставленным
int rejim1znDay[NUM_RELAY_PINS] = {0,0,0,0,0,0,0,0};
int rejim2znDay[NUM_RELAY_PINS] = {0,0,0,0,0,0,0,0};
int rejim3znDay[NUM_RELAY_PINS] = {0,0,0,0,0,0,0,0};
int rejim4znDay[NUM_RELAY_PINS] = {0,0,0,0,0,0,0,0};


//Для включения выключения насоса в режимах
boolean rejim1Nasos[NUM_RELAY_PINS] = {true, true, true, true, true, true, true, true};
boolean rejim2Nasos[NUM_RELAY_PINS] = {true, true, true, true, true, true, true, true};
boolean rejim3Nasos[NUM_RELAY_PINS] = {true, true, true, true, true, true, true, true};
boolean rejim4Nasos[NUM_RELAY_PINS] = {true, true, true, true, true, true, true, true};


// Массивы для проверки влажности для каждой реле в каждом режиме On/Off
boolean rejim1Humidity[NUM_RELAY_PINS] = {false, false, false, false, false, false, false, false};
boolean rejim2Humidity[NUM_RELAY_PINS] = {false, false, false, false, false, false, false, false};
boolean rejim3Humidity[NUM_RELAY_PINS] = {false, false, false, false, false, false, false, false};
boolean rejim4Humidity[NUM_RELAY_PINS] = {false, false, false, false, false, false, false, false};




// Переменные для Display7 для проверки какая именно реле открыта и какого режима
boolean releDlyCikla[NUM_RELAY_PINS] ={NULL};

//Массивы для пробегания по циклам
bool* thisRejims[] = {&thisRejim1, &thisRejim2, &thisRejim3, &thisRejim4};
char* line[NUM_RELAY_PINS] = {line1, line2, line3, line4, line5, line6, line7, line8}; //Массив названий реле
bool* rejimRele[numRejims] = {rejim1Rele, rejim2Rele, rejim3Rele, rejim4Rele};      
int* rejimMinOn[numRejims] = {rejim1MinOn, rejim2MinOn, rejim3MinOn, rejim4MinOn}; // Массив времени включения минут
int* rejimMinOff[numRejims] = {rejim1MinOff, rejim2MinOff, rejim3MinOff, rejim4MinOff}; // Массив времени выключения минут
int* rejimHourOn[numRejims] = {rejim1HourOn, rejim2HourOn, rejim3HourOn, rejim4HourOn}; // Время включения часов
int* rejimHourOff[numRejims] = {rejim1HourOff, rejim2HourOff, rejim3HourOff, rejim4HourOff}; // Время выключения часов
int* rejimDay[numRejims] = {rejim1Day, rejim2Day, rejim3Day, rejim4Day}; // Массивы где хранятся для  моих режимов количетсво пропущенных дней
int* rejimznDay[numRejims] = {rejim1znDay, rejim2znDay, rejim3znDay, rejim4znDay}; // Пока не использую
bool* rejimNasos[numRejims] = {rejim1Nasos, rejim2Nasos, rejim3Nasos, rejim4Nasos}; // Массив булевских переменных для проверки насоса в каждом режиме
char* rejimText[numRejims] = {rejimText1, rejimText2 , rejimText3 , rejimText4}; // для вывода в Print6 какой режим выбран
bool* rejimHumidity[numRejims] = {rejim1Humidity, rejim2Humidity, rejim3Humidity, rejim4Humidity};      //Массив для влажности каждого режима



unsigned long previousMinute = 0;
unsigned long previousMinute2 = 0;
const long interval = 60000; // 60 секунд (1 минута)






void printOnOrOff(bool rele){
  if (rele){
    lcd.print("On");
  }
  else {
    lcd.print("Off");

  }
}

void printCurrentTime(){     // Для вывода корректного времени с 0! Например 01:01
  lcd.setCursor(13, 0);
  if( myRTC.getHour(h12Flag, pmFlag) < 10){
    lcd.print("0");
    lcd.setCursor(14, 0);
    lcd.print(myRTC.getHour(h12Flag, pmFlag), DEC);
  }
  else{
    lcd.print(myRTC.getHour(h12Flag, pmFlag), DEC);
  }
  lcd.setCursor(15, 0);
  lcd.print(":");
  lcd.setCursor(16, 0);
  if (myRTC.getMinute() < 10){
    lcd.print("0");
    lcd.setCursor(17, 0);
    lcd.print(myRTC.getMinute(), DEC);
  }
  else{
    lcd.print(myRTC.getMinute(), DEC);
  }
}

void printCurrentHumidity(){ // Для динамического вывода влажности на экран
  if (pre <=0){
    pre = 0;
    lcd.setCursor(0, 1);
    lcd.print("B-Tb ");
    lcd.print(pre);
    lcd.setCursor(7,1);
  }
  if (abs(lastHumidity - pre) > 2){
    lastHumidity = pre; 
    lcd.setCursor(0, 1);
    lcd.print("B-Tb ");
    lcd.print(pre);
    lcd.setCursor(7,1);
    lcd.print("%");
    if (pre < 10){
      lcd.setCursor(6,1);
      lcd.print("%");
      lcd.setCursor(7,1);
      lcd.print(" ");
    }
  }   
}

void printCurrentTemp(){
  if (abs(lastTemp1 - temp1) > 0.5){
    lastTemp1 = temp1;
    lcd.setCursor(15,1);
    lcd.print(temp1);
  }
  if (abs(lastTemp2 - temp2) > 0.5){
    lastTemp2 = temp2;
    lcd.setCursor(15,2);
    lcd.print(temp2);
  }

}



//Вывод времени только одного числа. Пример 18:39
void printTime(int hourOn, int minuteOn, int position) {
  char buffer[6]; // Буфер для временных данных
  snprintf(buffer, sizeof(buffer), "%02d:%02d", hourOn, minuteOn); // Форматированный вывод времени
  lcd.setCursor(13, position);
  lcd.print(buffer);
}

//Вывод времени двух времен. Пример 18:3
void twoPrintTime(int hourOn, int minuteOn, int hourOff, int minuteOff, int position) {
  char buffer[20]; // Буфер для временных данных
  snprintf(buffer, sizeof(buffer), "%02d:%02d-%02d:%02d", hourOn, minuteOn, hourOff, minuteOff); // Форматированный вывод времени
  lcd.setCursor(9, position);
  lcd.print(buffer);
}




// Стрелка
void drawArrow(byte col, byte row) {
  lcd.setCursor(col, row); lcd.write(126);
}

void vivod1Ecran(){ //Для обновления экрана после включения реле
 ifDisplay1 = true; 
 ifDisplay2 = false; 
 ifDisplay3 = false;
 ifDisplay4 = false;
 ifDisplay5 = false;
 ifDisplay6 = false;
 ifDisplay7 = false;
 ifDisplay8 = false;
 printDisplay1();
}

//Функция для работы режимов (Включение АВТОПОЛИВА и выключение)
void turnOnRejim(){
  int currentHour = myRTC.getHour(h12Flag, pmFlag);
  int currentMinute = myRTC.getMinute();
  for (int i = 0; i < numRejims; ++i) {
        if (* isRejim[i]) {
          for (int j = 0; j < NUM_RELAY_PINS; ++j){
              if(rejimRele[i][j]){
                  if (rejimMinOn[i][j] == currentMinute && rejimHourOn[i][j] == currentHour)
                  {
                    if (rejimDay[i][j] == rejimznDay[i][j]){
                      rejimznDay[i][j] = 0;
                      if (rejimHumidity[i][j] == false)
                      { // Если выключена влажность
                      *isRele[j] = true;
                      if (rejimNasos[i][j]){
                        valueToSend = true;
                      }
                      digitalWrite(relayPins[j], 0);
                      vivod1Ecran();
                      }
                      else if( rejimHumidity[i][j]== true && pre < maxHumidity){ //Если влажность включена на определенной реле проверяем услвоия 
                        *isRele[j] = true;
                        if (rejimNasos[i][j]){
                          valueToSend = true;
                        }
                        digitalWrite(relayPins[j], 0);
                        vivod1Ecran();
                      }
                  }
                  else{
                    ++rejimznDay[i][j];
                    }
                  }

                  if (rejimMinOff[i][j] == currentMinute && rejimHourOff[i][j] == currentHour){
                    *isRele[j] = false;
                    valueToSend = false;
                    digitalWrite(relayPins[j], 1);
                    vivod1Ecran();
                  }
              }
          }
      }
  }
}


int readAddress = 0; // Глобальная переменная для адреса чтения из EEPROM
int writeAddress = 0; // Глобальная переменная для адреса записи в EEPROM

void readBooleanArray(bool* mas[]) {
    for (int i = 0; i < numRejims; i++) {
        for (int j = 0; j < NUM_RELAY_PINS; j++) {
            mas[i][j] = EEPROM.read(readAddress);
            readAddress += sizeof(bool);

        }
    }
}

// Функция для записи массива в EEPROM
void saveBooleanArray(bool* mas[]) {
    for (int i = 0; i < numRejims; i++) {
        for (int j = 0; j < NUM_RELAY_PINS; j++) {
            EEPROM.write(writeAddress, mas[i][j]);
            writeAddress += sizeof(bool);
        }
    }
}


void readIntegerArray(int* mas[]) {
    for (int i = 0; i < numRejims; i++) {
        for (int j = 0; j < NUM_RELAY_PINS; j++) {
            mas[i][j] = EEPROM.read(readAddress);
            readAddress += sizeof(int);  
        }
    }
}

// Функция для записи массива в EEPROM
void saveIntegerArray(int* mas[]) {
    for (int i = 0; i < numRejims; i++) {
        for (int j = 0; j < NUM_RELAY_PINS; j++) {
            EEPROM.write(writeAddress, mas[i][j]);
            writeAddress += sizeof(int);  
        }
    }
}




void readFromEEPROM(){
  // Чтение данных из EEPROM
  valueVlajnost = EEPROM.read(readAddress);
  readAddress += sizeof(int);  
  pre = EEPROM.read(readAddress);
  readAddress += sizeof(int);  
  maxHumidity = EEPROM.read(readAddress);
  readAddress += sizeof(int);  
  lastHumidity = EEPROM.read(readAddress);
  readAddress += sizeof(int);  

  for (int i = 0; i < 4; i++) {
    *isRejim[i] = EEPROM.read(readAddress); // для bool переменных не нужно умножение на sizeof(bool)
    readAddress += sizeof(bool);
  }
  
  readBooleanArray(rejimRele);
  readIntegerArray(rejimMinOn);
  readIntegerArray(rejimMinOff);
  readIntegerArray(rejimHourOn);
  readIntegerArray(rejimHourOff);
  readIntegerArray(rejimDay);
  readIntegerArray(rejimznDay);
  readBooleanArray(rejimNasos);
  readBooleanArray(rejimHumidity);
}

void saveToEEPROM() {
  EEPROM.write(writeAddress, valueVlajnost);
  writeAddress += sizeof(int);  
  EEPROM.write(writeAddress, pre);
  writeAddress += sizeof(int);  
  EEPROM.write(writeAddress, maxHumidity);
  writeAddress += sizeof(int);  
  EEPROM.write(writeAddress, lastHumidity);
  writeAddress += sizeof(int);  

  // Сохранение режимов в EEPROM
  for (int i = 0; i < 4; i++) {
    EEPROM.write(writeAddress, *isRejim[i]);
    writeAddress += sizeof(bool);
  }
  saveBooleanArray(rejimRele);
  saveIntegerArray(rejimMinOn);
  saveIntegerArray(rejimMinOff);
  saveIntegerArray(rejimHourOn);
  saveIntegerArray(rejimHourOff);
  saveIntegerArray(rejimDay);
  saveIntegerArray(rejimznDay);
  saveBooleanArray(rejimNasos);
  saveBooleanArray(rejimHumidity);
}




 
  


void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  therm.config(13000, 3435);
  lcd.setCursor(0, 0);
  printDisplay1();
  enc.setBtnLevel(LOW);
  enc.setClickTimeout(500);
  enc.setDebTimeout(50);
  enc.setHoldTimeout(600);
  enc.setStepTimeout(200);
  enc.setEncReverse(0);
  enc.setEncType(EB_STEP4_LOW);
  enc.setFastTimeout(30);
  //Настройка пинов 
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  pinMode(RELAY_PIN3, OUTPUT);
  pinMode(RELAY_PIN4, OUTPUT);
  pinMode(RELAY_PIN5, OUTPUT);
  pinMode(RELAY_PIN6, OUTPUT);
  pinMode(RELAY_PIN7, OUTPUT);
  pinMode(RELAY_PIN8, OUTPUT);
  digitalWrite(RELAY_PIN1, 1); //Выключить все пины
  digitalWrite(RELAY_PIN2, 1);
  digitalWrite(RELAY_PIN3, 1);
  digitalWrite(RELAY_PIN4, 1);
  digitalWrite(RELAY_PIN5, 1);
  digitalWrite(RELAY_PIN6, 1);
  digitalWrite(RELAY_PIN7,1);
  digitalWrite(RELAY_PIN8,1);
  //Настройка пинов
  //Настройка передачи
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(32);
  radio.openWritingPipe(address);
  radio.setChannel(0x60);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.powerUp();
  readFromEEPROM();
}



void loop() {
  enc.tick();
  //Датчик влажности 
  valueVlajnost = analogRead(sensor);
  pre = map(valueVlajnost, wet, dry, 100, 0);
  //Датчики температуры
  therm.setPin(2);
  temp1 = therm.getTempAverage(); //Температуру 1
  therm.setPin(3);
  temp2 = therm.getTempAverage(); //Температура 2
  loopDisplay1();
  loopDisplay2();
  loopDisplay3();
  loopDisplay4();
  loopDisplay5();
  loopDisplay6();  
  loopDisplay7();
  loopDisplay8();
  loopDisplay9();
  unsigned long currentMillis = millis();
  // проверяем, прошла ли минута
  if (currentMillis - previousMinute >= interval) {
    // сохраняем время последней минуты
    previousMinute += interval;
    // вызываем функцию, которую нужно выполнять раз в минуту
    turnOnRejim();
  }
   radio.write(&valueToSend, sizeof(valueToSend));
}


void printDisplay1() {   // Главное меню
  lcd.clear();
  lcd.print("-MENU");
  lcd.setCursor(7, 0);
  lcd.print("BPEMЯ");
  printCurrentTime();
  printCurrentHumidity();
  lcd.setCursor(9, 1);
  therm.setPin(2);
  double temp = therm.getTempAverage(); // Беру температуру с 1 датчика
  lcd.print(" T.YЛ ");
  lcd.print(temp);
  lcd.setCursor(0, 2);
  lcd.print("BKЛЮЧEHO");
  lcd.setCursor(9, 2);
  therm.setPin(3);
  double temp2 = therm.getTempAverage();; // Беру температур с 2 датчика
  lcd.print(" T.TE ");
  lcd.print(temp2);
  lcd.setCursor(0, 3);
   for(int i = 0; i<NUM_RELAY_PINS; i++){
    if (*isRele[i]){
      lcd.print(line[i]);
    }
  }
}

void printDisplay2(){ //2 мини меню
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  HA3AD");
  lcd.setCursor(0,1);
  lcd.print("  PYЧHOE YПPABЛЕHUE");
  lcd.setCursor(0,2);
  lcd.print("  BЫБOP PEЖUMA");
  lcd.setCursor(0,3);
  lcd.print("  HACTPOЙKA PEЖUMOB");
  ifDisplay2 = true;
}

void printDisplay2_1(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  ВЛАЖНОСТЬ");
  lcd.setCursor(15,0);
  lcd.print(maxHumidity);
  lcd.setCursor(0,1);
  lcd.print("  СОХРАНИТЬ ДАННЫЕ");

}




void printDisplay3(){ //Ручное управление
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  HA3AD");
  lcd.setCursor(0,1);
  lcd.print("  HACOC");
  lcd.setCursor(17,1);
  printOnOrOff(valueToSend);
  lcd.setCursor(2,2);
  lcd.print(line1);
  lcd.setCursor ( 17, 2);
  printOnOrOff(isReleOn1);
  lcd.setCursor(2,3);
  lcd.print(line2);
  lcd.setCursor ( 17, 3);
  printOnOrOff(isReleOn2);
}

void printDisplay3_1(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print(line3);
  lcd.setCursor ( 17, 0);
  printOnOrOff(isReleOn3);
  lcd.setCursor(2,1);
  lcd.print(line4);
  lcd.setCursor ( 17, 1);
  printOnOrOff(isReleOn4);
  lcd.setCursor(2,2);
  lcd.print(line5);
  lcd.setCursor ( 17, 2);
  printOnOrOff(isReleOn5);
  lcd.setCursor(2,3);
  lcd.print(line6);
  lcd.setCursor ( 17, 3);
  printOnOrOff(isReleOn6);
}

void printDisplay3_2(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print(line7);
  lcd.setCursor ( 17, 0);
  printOnOrOff(isReleOn7);
  lcd.setCursor(2,1);
  lcd.print(line8);
  lcd.setCursor ( 17, 1);
  printOnOrOff(isReleOn8);
}

void printDisplay4(){ // настройка режимов 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  HA3AD");
  lcd.setCursor(0,1);
  lcd.print("  BЫБOP PEЖUMA 1");
  lcd.setCursor ( 17, 1);
  printOnOrOff(rejim1);
  lcd.setCursor(0,2);
  lcd.print("  BЫБOP PEЖUMA 2");
  lcd.setCursor ( 17, 2);
  printOnOrOff(rejim2);
  lcd.setCursor(0,3);
  lcd.print("  BЫБOP PEЖUMA 3");
  lcd.setCursor ( 17, 3);
  printOnOrOff(rejim3);
}

void printDisplay4_1(){ // настройка режимов 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  BЫБOP PEЖUMA 4");
  lcd.setCursor ( 17, 0);
  printOnOrOff(rejim4);
}

void printDisplay5(){ //Выбрп режимов
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  HA3AD");
  lcd.setCursor(0,1);
  lcd.print("  PEЖUM 1");
  lcd.setCursor(0,2);
  lcd.print("  PEЖUM 2");
  lcd.setCursor(0,3);
  lcd.print("  PEЖUM 3");
}

void printDisplay5_1(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  PEЖUM 4");

}

void printDisplay6() { // вывод режима
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  HA3AD");
  for (int i = 0; i < numRejims; ++i) {
    if (*thisRejims[i]) {
      lcd.setCursor(13, 0);
      lcd.print(rejimText[i]);
      lcd.setCursor(2, 1);
      lcd.print(line1);
      printReleTime(rejimMinOn[i][0], rejimMinOff[i][0], rejimHourOn[i][0], rejimHourOff[i][0],1, rejimRele[i][0]);
      lcd.setCursor(2, 2);
      lcd.print(line2);
      printReleTime(rejimMinOn[i][1], rejimMinOff[i][1], rejimHourOn[i][1], rejimHourOff[i][1], 2, rejimRele[i][1] );
      lcd.setCursor(2, 3);
      lcd.print(line3);
      printReleTime(rejimMinOn[i][2], rejimMinOff[i][2], rejimHourOn[i][2], rejimHourOff[i][2], 3, rejimRele[i][2]);
    }
  }
}

void printReleTime(int minOn, int minOff, int hourOn, int hourOff, int line, boolean rele) {
  lcd.setCursor(17, line);
  if (rele == false) {
    lcd.print("Off");
  } else {
    if(minOn != NULL && minOff != NULL && hourOn != NULL &&  hourOff != NULL){
      twoPrintTime(hourOn, minOn, hourOff, minOff, line);
    }
    else{
      lcd.print("Off");
    }
  }
}

void printDisplay6_1(){
  lcd.clear();
  lcd.setCursor(0,0);
  for (int i = 0; i < numRejims; ++i) {
    if (*thisRejims[i]) {
      lcd.setCursor(2, 0);
      lcd.print(line4);
      printReleTime(rejimMinOn[i][3], rejimMinOff[i][3], rejimHourOn[i][3], rejimHourOff[i][3], 0, rejimRele[i][3]);
      lcd.setCursor(2, 1);
      lcd.print(line5);
      printReleTime(rejimMinOn[i][4], rejimMinOff[i][4], rejimHourOn[i][4], rejimHourOff[i][4], 1, rejimRele[i][4]);
      lcd.setCursor(2, 2);
      lcd.print(line6);
      printReleTime(rejimMinOn[i][5], rejimMinOff[i][5], rejimHourOn[i][5], rejimHourOff[i][5], 2, rejimRele[i][5]);
      lcd.setCursor(2, 3);
      lcd.print(line7);
      printReleTime(rejimMinOn[i][6], rejimMinOff[i][6], rejimHourOn[i][6], rejimHourOff[i][6], 3, rejimRele[i][6]);
    }
  }
}

void printDisplay6_2(){
  lcd.clear();
  lcd.setCursor(0,0);
  for (int i = 0; i < numRejims; ++i) {
    if (*thisRejims[i]) {
      lcd.setCursor(2, 0);
      lcd.print(line8);
      printReleTime(rejimMinOn[i][7], rejimMinOff[i][7], rejimHourOn[i][7], rejimHourOff[i][7], 0, rejimRele[i][7]);
    }
  }
}

void printDisplay7(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  HA3AD");  
  for (int i = 0; i < numRejims; ++i) {
    if (*thisRejims[i]) {
      for (int j = 0; j < NUM_RELAY_PINS; ++j) {
        if (releDlyCikla[j]) {
          lcd.setCursor(2, 1);
          if (rejimHourOn[i][j] != NULL && rejimMinOn[i][j] != NULL){
            printOnOrOff(rejimRele[i][j]);
          }
          else{lcd.print("Off");}
          lcd.setCursor(2,2);
          lcd.print("BPEMЯ-ВКЛ");
          if (rejimHourOn[i][j] != NULL && rejimMinOn[i][j] != NULL){
            printTime(rejimHourOn[i][j], rejimMinOn[i][j], 2);
          }
          lcd.setCursor(2,3);
          lcd.print("BPEMЯ-BЫKЛ");
          if (rejimHourOff[i][j]!= NULL && rejimMinOff[i][j] != NULL){
            printTime(rejimHourOff[i][j], rejimMinOff[i][j], 3);
          }
        }
      }
    }
  }
}




void printDisplay7_1(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  HA3AD");
  for (int i = 0; i < numRejims; ++i) {
    if (*thisRejims[i]) {
      for (int j = 0; j < NUM_RELAY_PINS; ++j) {
        if (releDlyCikla[j]) {
          lcd.setCursor(2, 0);          
          lcd.print("YCЛOBUЯ:");
          lcd.setCursor(2,1);
          lcd.print("BЛAЖHOCTb");
          lcd.setCursor(17,1);
          printOnOrOff(rejimHumidity[i][j]);
          lcd.setCursor(2,2);
          lcd.print("HACOC");
          lcd.setCursor(17,2);
          printOnOrOff(rejimNasos[i][j]);
          lcd.setCursor(2,3);
          lcd.print("ПPOПYCK DHEЙ");
          lcd.setCursor(17,3);
          lcd.print(rejimDay[i][j]);
        }
      }
    }
  }
}

void printDisplay8(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("НАСТРОЙКА ВРЕМЕНИ");
  lcd.setCursor(5,1);
}

void printDisplay9(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("НАСТРОЙКА ВЛАЖНОСТИ");
  lcd.setCursor(9,1);
  lcd.print(maxHumidity);
}

void loopDisplay1(){ //Главное меню
 if (ifDisplay1){
   printCurrentTime();
   printCurrentHumidity();
   printCurrentTemp();
  }  
  if (enc.click() && ifDisplay1 == true){
    ifDisplay1 = false;
    printDisplay2();
  }
}

void loopDisplay2(){ //Мини меню
  
  if (ifDisplay2){
      if (value < 0){
        value = 1;
      }
      if (value > 7){
        value = 7;
      }
      if (enc.right()){
        value++;
      }
      if (enc.left()){
        value--;
      }
      if (value == 0){
        lcd.setCursor(0,0);
        lcd.print("  ");

      }
      if (value == 1){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value == 2){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      else if (value == 3){
        drawArrow(0,2);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,3);
        lcd.print("  ");
      }
      else if (value == 4){
        drawArrow(0,3);
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      if (value == 5 && enc.right()){
        printDisplay2_1();
        value++;
      }
      if (value == 5 && enc.left()){
        printDisplay2();
        value--;
      }
      if (value == 6){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value == 7){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      if (enc.click() && value == 1){
          ifDisplay2 = false;
          ifDisplay1 = true;
          value = 0;
          printDisplay1();   
      }
      if (enc.click() && value == 2){
          ifDisplay2 = false;
          value = 0;
          ifDisplay3 = true;
          printDisplay3();
      }
      
      if (enc.click() && value == 3){
          ifDisplay2 = false;
          value = 0;
          ifDisplay4 = true;
          printDisplay4();
      }
      if (enc.click() && value == 4){
          ifDisplay2 = false;
          value = 0;
          ifDisplay5 = true;
          printDisplay5();
      }
      if (enc.click() && value == 6){
          ifDisplay2 = false;
          value = 0;
          ifDisplay9 = true;
          printDisplay9();
      }
      if (enc.click() && value == 7){
        saveToEEPROM();
        lcd.setCursor(10,2);
        lcd.print("СОХРАНЕНО");
      }
  }
}

void loopDisplay3(){  // Ручное управление
  if (ifDisplay3){
    if (value2 < 0){
        value2 = 1;
      }
      if (value2 > 12){
        value2 = 12;
      }
      if (enc.right()){
        value2++;
      }
      if (enc.left()){
        value2--;
      }
      if (value2 == 0){
        lcd.setCursor(0,0);
        lcd.print("  ");

      }
      if (value2 == 1){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value2 == 2){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      else if (value2 == 3){
        drawArrow(0,2);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,3);
        lcd.print("  ");
      }
      else if (value2 == 4){
        drawArrow(0,3);
        lcd.setCursor(0,2);
        lcd.print("  ");
        lcd.setCursor(0,0);
        lcd.print("  ");
      }
      if (value2 == 5 && enc.right()){
        printDisplay3_1();
        value2++;
      }
      if (value2 == 5 && enc.left()){
        printDisplay3();
        value2--;
      }
      if (value2 == 6){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value2 == 7){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      else if (value2 == 8){
        drawArrow(0,2);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,3);
        lcd.print("  ");
      }
      else if (value2 == 9){
        drawArrow(0,3);
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      if (value2 == 10 && enc.right()){
        printDisplay3_2();
        value2++;
      }
      if (value2 == 10 && enc.left()){
        printDisplay3_1();
        value2--;
      }
      if (value2 == 11){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value2 == 12){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }

      if (enc.click() && value2 == 1){
          ifDisplay3 = false;
          ifDisplay2 = true;
          value2 = 0;
          printDisplay2();   
      }
      
      if (enc.click() && value2 == 2 && valueToSend == false){
          valueToSend = true;
          printDisplay3();
      }
      else if(enc.click() && value2 == 2){
          valueToSend = false;
          printDisplay3();
      }

      if (enc.click() && value2 == 3 && isReleOn1 == false){
          isReleOn1 = true;
          digitalWrite(RELAY_PIN1, 0);
          printDisplay3();
      }
      else if(enc.click() && value2 == 3){
          isReleOn1 = false;
          digitalWrite(RELAY_PIN1, 1);
          printDisplay3();
      }

      if (enc.click() && value2 == 4 && isReleOn2 == false){
          isReleOn2 = true;
          digitalWrite(RELAY_PIN2, 0);
          printDisplay3();
      }
      else if(enc.click() && value2 == 4){
          isReleOn2 = false;
          digitalWrite(RELAY_PIN2, 1);
          printDisplay3();
      }
      if (enc.click() && value2 == 6 && isReleOn3 == false){
          isReleOn3 = true;
          digitalWrite(RELAY_PIN3, 0);
          printDisplay3_1();
      }
      else if(enc.click() && value2 == 6){
          isReleOn3 = false;
          digitalWrite(RELAY_PIN3, 1);
          printDisplay3_1();
      }
      if (enc.click() && value2 == 7 && isReleOn4 == false){
          isReleOn4 = true;
          digitalWrite(RELAY_PIN4, 0);
          printDisplay3_1();
      }
      else if(enc.click() && value2 == 7){
          isReleOn4 = false;
          digitalWrite(RELAY_PIN4, 1);
          printDisplay3_1();
      }
      if (enc.click() && value2 == 8 && isReleOn5 == false){
          isReleOn5 = true;
          digitalWrite(RELAY_PIN5, 0);
          printDisplay3_1();
      }
      else if(enc.click() && value2 == 8){
          isReleOn5 = false;
          digitalWrite(RELAY_PIN5, 1);
          printDisplay3_1();
      }
      if (enc.click() && value2 == 9 && isReleOn6 == false){
          isReleOn6 = true;
          digitalWrite(RELAY_PIN6, 0);
          printDisplay3_1();
      }
      else if(enc.click() && value2 == 9){
          isReleOn6 = false;
          digitalWrite(RELAY_PIN6, 1);
          printDisplay3_1();
      }
      if (enc.click() && value2 == 11 && isReleOn7 == false){
          isReleOn7 = true;
          digitalWrite(RELAY_PIN7, 0);
          printDisplay3_2();
      }
      else if(enc.click() && value2 == 11){
          isReleOn7 = false;
          digitalWrite(RELAY_PIN7, 1);
          printDisplay3_2();
      }
      if (enc.click() && value2 == 12 && isReleOn8 == false){
          isReleOn8 = true;
          digitalWrite(RELAY_PIN8, 0);
          printDisplay3_2();
      }
      else if(enc.click() && value2 == 12){
          isReleOn8 = false;
          digitalWrite(RELAY_PIN8, 1);
          printDisplay3_2();
      }
  }
}

void loopDisplay4(){ // Выбор режима
  if (ifDisplay4){
    if (value3 < 0){
        value3 = 1;
      }
      if (value3 > 6){
        value3 = 6;
      }
      if (enc.right()){
        value3++;
      }
      if (enc.left()){
        value3--;
      }
      if (value3 == 0){
        lcd.setCursor(0,0);
        lcd.print("  ");
      }
      if (value3 == 1){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value3 == 2){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      else if (value3 == 3){
        drawArrow(0,2);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,3);
        lcd.print("  ");
      }
      else if (value3 == 4){
        drawArrow(0,3);
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      if (value3 == 5 && enc.right()){
        printDisplay4_1();
        value3++;
      }
      if (value3 == 5 && enc.left()){
        printDisplay4();
        value3--;
      }
      if (value3 == 6){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      if (enc.click() && value3 == 1){
          ifDisplay4 = false;
          ifDisplay2 = true;
          value3 = 0;
          printDisplay2();   
      }
      
      if (enc.click() && value3 == 2 && rejim1 == false){
        rejim1 = true;
        printDisplay4();
        saveToEEPROM();
      }
      else if(enc.click() && value3 == 2){
        rejim1 = false;
        printDisplay4();
        saveToEEPROM();
      }

      if (enc.click() && value3 == 3 && rejim2 == false){
        rejim2 = true;
        printDisplay4();
        saveToEEPROM();
      }
      else if(enc.click() && value3 == 3){
        rejim2 = false;
        printDisplay4();
        saveToEEPROM();
      }
      if (enc.click() && value3 == 4 && rejim3 == false){
        rejim3 = true;
        saveToEEPROM();
        printDisplay4();
      }
      else if(enc.click() && value3 == 4){
        rejim3 = false;
        saveToEEPROM();
        printDisplay4();
      }
      if (enc.click() && value3 == 6 && rejim4 == false){
        rejim4 = true;
        saveToEEPROM();
        printDisplay4_1();
      }
      else if(enc.click() && value3 == 6){
        rejim4 = false;
        saveToEEPROM();
        printDisplay4_1();
      }
  }
}

void loopDisplay5(){
  if (ifDisplay5){
    if (value5 < 0){
      value5 = 1;
    }
    if (value5 > 6){
      value5 = 6;
    }
    if (enc.right()){
      value5++;
    }
    if (enc.left()){
      value5--;
    }
    if (value5 == 1){
      drawArrow(0,0);
      lcd.setCursor(0,1);
      lcd.print("  ");
    }
    else if (value5 == 2){
      drawArrow(0,1);
      lcd.setCursor(0,0);
      lcd.print("  ");
      lcd.setCursor(0,2);
      lcd.print("  ");
    }
    else if (value5 == 3){
      drawArrow(0,2);
      lcd.setCursor(0,1);
      lcd.print("  ");
      lcd.setCursor(0,3);
      lcd.print("  ");
    }
    else if (value5 == 4){
      drawArrow(0,3);
      lcd.setCursor(0,2);
      lcd.print("  ");
      lcd.setCursor(0,0);
      lcd.print("  "); 
    }
    if (value5 == 5 && enc.right()){
      printDisplay5_1();
      value5++;
    }
    if (value5 == 5 && enc.left()){
      printDisplay5();
      value5--;
    }
    if (value5 == 6){
      drawArrow(0,0);
      lcd.setCursor(0,1);
      lcd.print("  ");
    }
    if (enc.click() && value5 == 1){
      ifDisplay5 = false;
      ifDisplay2 = true;
      value5 = 0;
      printDisplay2();   
    }
    if (enc.click() && value5 == 2){
      ifDisplay5 = false;
      ifDisplay6 = true;
      value5 = 0;
      thisRejim1 = true;
      saveToEEPROM();
      printDisplay6();
    }
    if (enc.click() && value5 == 3){
      ifDisplay5 = false;
      ifDisplay6 = true;
      value5 = 0;
      thisRejim2 = true;
      saveToEEPROM();
      printDisplay6();
    }
    if (enc.click() && value5 == 4){
      ifDisplay5 = false;
      ifDisplay6 = true;
      value5 = 0;
      thisRejim3 = true;
      saveToEEPROM();
      printDisplay6();
    }
    if (enc.click() && value5 == 6){
      ifDisplay5 = false;
      ifDisplay6 = true;
      value5 = 0;
      thisRejim4 = true;
      saveToEEPROM();
      printDisplay6();
    }
  }
}

void loopDisplay6(){
  if (ifDisplay6){
      if (value4 < 0){
        value4 = 1;
      }
      if (value4 > 11){
        value4 = 11;
      }
      if (enc.right()){
        value4++;
      }
      if (enc.left()){
        value4--;
      }
      if (value4 == 0){
        lcd.setCursor(0,0);
        lcd.print("  ");
      }
      if (value4 == 1){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value4 == 2){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      else if (value4 == 3){
        drawArrow(0,2);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,3);
        lcd.print("  ");
      }
      else if (value4 == 4){
        drawArrow(0,3);
        lcd.setCursor(0,2);
        lcd.print("  ");
        lcd.setCursor(0,0);
        lcd.print("  "); 
      }
      if (value4 == 5 && enc.right()){
        printDisplay6_1();
        value4++;
      }
      if (value4 == 5 && enc.left()){
        printDisplay6();
        value4--;
      }
      if (value4 == 6){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value4 == 7){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      else if (value4 == 8){
        drawArrow(0,2);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,3);
        lcd.print("  ");
      }
      else if (value4 == 9){
        drawArrow(0,3);
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      if (value4 == 10 && enc.right()){
        printDisplay6_2();
        value4++;
      }
      if (value4 == 10 && enc.left()){
        printDisplay6_1();
        value4--;
      }
      if (value4 == 11){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      if (enc.click() && value4 == 1){
        ifDisplay6 = false;
        ifDisplay5 = true;
        value4 = 0;
        thisRejim1 = false;
        thisRejim2 = false;
        thisRejim3 = false;
        thisRejim4 = false;
        printDisplay5();   
      }
      if (enc.click() && value4 == 2){
        ifDisplay6 = false;
        ifDisplay7 = true;
        value4 = 0;
        releDlyCikla[0] = !releDlyCikla[0]; 
        saveToEEPROM();
        printDisplay7();  
      }
      if (enc.click() && value4 == 3){
        ifDisplay6 = false;
        ifDisplay7 = true;
        value4 = 0;
        releDlyCikla[1] = !releDlyCikla[1]; 
        saveToEEPROM();

        printDisplay7();  
      }
      if (enc.click() && value4 == 4){
        ifDisplay6 = false;
        ifDisplay7 = true;
        value4 = 0;
        releDlyCikla[2] = !releDlyCikla[2]; 
        saveToEEPROM();

        printDisplay7();  
      }
      if (enc.click() && value4 == 6){
        ifDisplay6 = false;
        ifDisplay7 = true;
        value4 = 0;
        releDlyCikla[3] = !releDlyCikla[3]; 
        saveToEEPROM();

        printDisplay7();  
      }
      if (enc.click() && value4 == 7){
        ifDisplay6 = false;
        ifDisplay7 = true;
        value4 = 0;
        releDlyCikla[4] = !releDlyCikla[4]; 
        saveToEEPROM();
        printDisplay7();  
      }
      if (enc.click() && value4 == 8){
        ifDisplay6 = false;
        ifDisplay7 = true;
        value4 = 0;
        releDlyCikla[5] = !releDlyCikla[5]; 
        saveToEEPROM();
        printDisplay7();  
      }
      if (enc.click() && value4 == 9){
        ifDisplay6 = false;
        ifDisplay7 = true;
        value4 = 0;
        releDlyCikla[6] = !releDlyCikla[6]; 
        saveToEEPROM();
        printDisplay7();  
      }
      if (enc.click() && value4 == 11){
        ifDisplay6 = false;
        ifDisplay7 = true;
        value4 = 0;
        releDlyCikla[7] = !releDlyCikla[7]; 
        saveToEEPROM();
        printDisplay7();  
      }

  }
}

void loopDisplay7(){
  if (ifDisplay7){
    if (value4 < 0){
        value4 = 1;
      }
      if (value4 > 9){
        value4 = 9;
      }
      if (enc.right()){
        value4++;
      }
      if (enc.left()){
        value4--;
      }
      if (value4 == 0){
        lcd.setCursor(0,0);
        lcd.print("  ");
      }
      if (value4 == 1){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value4 == 2){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      else if (value4 == 3){
        drawArrow(0,2);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,3);
        lcd.print("  ");
      }
      else if (value4 == 4){
        drawArrow(0,3);
        lcd.setCursor(0,2);
        lcd.print("  ");
        lcd.setCursor(0,0);
        lcd.print("  "); 
      }
      if (value4 == 5 && enc.right()){
        printDisplay7_1();
        value4++;
      }
      if (value4 == 5 && enc.left()){
        printDisplay7();
        value4--;
      }
      if (value4 == 6){
        drawArrow(0,0);
        lcd.setCursor(0,1);
        lcd.print("  ");
      }
      else if (value4 == 7){
        drawArrow(0,1);
        lcd.setCursor(0,0);
        lcd.print("  ");
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      else if (value4 == 8){
        drawArrow(0,2);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,3);
        lcd.print("  ");
      }
      else if (value4 == 9){
        drawArrow(0,3);
        lcd.setCursor(0,2);
        lcd.print("  ");
      }
      if (enc.click() && value4 == 1){
        ifDisplay7 = false;
        ifDisplay6 = true;
        value4 = 0;
        releDlyCikla[0] = false;
        releDlyCikla[1] = false;
        releDlyCikla[2] = false;
        releDlyCikla[3] = false;
        releDlyCikla[4] = false;
        releDlyCikla[5] = false;
        releDlyCikla[6] = false;
        releDlyCikla[7] = false;
        printDisplay6();   
      }

      for (int i = 0; i < numRejims; ++i) {
        if (*thisRejims[i]) {
          for (int j = 0; j < NUM_RELAY_PINS; ++j){
            if (releDlyCikla[j]){
              if(enc.click() && value4 == 2){
                rejimRele[i][j] = !(rejimRele[i][j]);
                saveToEEPROM();
                printDisplay7();
              }
              if(enc.click() && value4 == 7){
                rejimHumidity[i][j] = !(rejimHumidity[i][j]);
                saveToEEPROM();
                printDisplay7_1();
              }
              if(enc.click() && value4 == 8){
                rejimNasos[i][j] = !(rejimNasos[i][j]);
                saveToEEPROM();
                printDisplay7_1();
              }
              if(enc.click() && value4 == 9){
                rejimDay[i][j] = ++rejimDay[i][j];
                saveToEEPROM();
                printDisplay7_1();
              }
              if(enc.hold() && value4 == 9){
                rejimDay[i][j] = --rejimDay[i][j];
                saveToEEPROM();
                printDisplay7_1();
              }
              

            }
          }
        }
      } 
      if(enc.click() && value4 == 3){
        ifDisplay7 = false;
        ifDisplay8 = true;
        value4 = 0;
        flagLoop8 = false;
        saveToEEPROM();
        printDisplay8();
      }
      
      if(enc.click() && value4 == 4){
        ifDisplay7 = false;
        ifDisplay8 = true;
        value4 = 0;
        flagLoop8 = true;
        saveToEEPROM();
        printDisplay8();
      }
  }
}

int hourOn = 0;
int minuteOn = 0;
boolean flag = true;

void loopDisplay8(){
  if (ifDisplay8){
      for (int i = 0; i < numRejims; ++i) {
        if (*thisRejims[i]) {
          for (int j = 0; j < NUM_RELAY_PINS; ++j){
            if (releDlyCikla[j]){
              char buffer[6]; 
              if (hourOn < 0 && flag){
                hourOn = 23;
              }
              if (hourOn > 23 && flag ){
                hourOn = 0;
              }
              if (minuteOn < 0 && !flag){
                minuteOn = 59;
              }
              if (minuteOn > 59 && !flag ){
                minuteOn = 0;
              }
              if (enc.right() && flag ){
                hourOn++;
                lcd.setCursor(7, 1);
                saveToEEPROM();
                lcd.print(buffer);
              }
              if (enc.left() && flag ){
                hourOn--;
                lcd.setCursor(7, 1);
                saveToEEPROM();
                lcd.print(buffer);
              }
              if (enc.right() && !flag ){
                minuteOn++;
                lcd.setCursor(7, 1);
                saveToEEPROM();
                lcd.print(buffer);
              }
              if (enc.left() && !flag ){
                minuteOn--;
                lcd.setCursor(7, 1);
                saveToEEPROM();
                lcd.print(buffer);
              }
              snprintf(buffer, sizeof(buffer), "%02d:%02d", hourOn, minuteOn); // Форматированный вывод времени
              lcd.setCursor(7, 1);
              lcd.print(buffer);
              if (enc.hold()){
                if(!flagLoop8){
                  rejimMinOn[i][j] = minuteOn;
                  rejimHourOn[i][j] =  hourOn;
                  saveToEEPROM();

                }
                else{
                  rejimMinOff[i][j] = minuteOn;
                  rejimHourOff[i][j] =  hourOn;
                  saveToEEPROM();
                }
                ifDisplay8 = false;
                ifDisplay7 = true;
                printDisplay7();
              }
              if (enc.click()){
                flag = !flag;
              }
            }
          }
        }
      }
  }
}

void loopDisplay9(){
  if (ifDisplay9){
    if (enc.right()){
      maxHumidity++;
      saveToEEPROM();

      printDisplay9();
    }
    if (enc.left() && flag ){
      maxHumidity--;
      saveToEEPROM();
      printDisplay9();
    }
    if (enc.hold()){
      ifDisplay9 = false;
      ifDisplay2 = true;
      printDisplay2();
    }
  }
}





