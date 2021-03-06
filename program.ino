#include <GyverMotor.h>
#include <SoftwareSerial.h>
#include <Servo.h> 
//uint32_t myTimer;


#define MOTOR_MAX 255                 // максимальный сигнал на мотор (max 255)
#define JOY_MAX 1024                    // рабочий ход джойстика (из приложения)
#define minDuty 40                     // скорость, при которой мотор должен начинать крутится (обычно 25-50)
#define RIGHT_MOTOR_DIRECTION REVERSE  // напрваление правого мотора (NORMAL или REVERSE)
#define LEFT_MOTOR_DIRECTION NORMAL   // напрваление левого мотора (NORMAL или REVERSE)
#define RIGHT_MOTOR_MODE HIGH         // смени HIGH на LOW если мотор включает тормоз
#define LEFT_MOTOR_MODE HIGH          // смени HIGH на LOW если мотор включает тормоз

/*=============== ПИНЫ ===============*/
#define RIGHT_MOTOR_D 7
#define RIGHT_MOTOR_PWM 11            // обязательно должен быть ШИМ пином!!!
#define LEFT_MOTOR_D 9
#define LEFT_MOTOR_PWM 10              // обязательно должен быть ШИМ пином!!!
#define BT_TX A1
#define BT_RX A0
#define GREEN A2
#define YELLOW A3
#define RED A4
#define SOUND A5


boolean doneParsing, startParsing, X, Y, B, doneX, doneY, doneB, turnR=true, turnL=true, on=false, turn2R=true, turn2L=true;
int dataX=512, dataY=512, nX, nY;
char key;
int angel=90;
int button=1;
String string_convert, string_for_x, string_for_y, string_for_b;
// Пин для сервопривода
int servoPin = 3;
int servoPin2 = 2;
// Создаем объект

SoftwareSerial MyBlue(BT_TX, BT_RX);
GMotor motorR(DRIVER3WIRE, 7, 8, 11, RIGHT_MOTOR_MODE);
GMotor motorL(DRIVER3WIRE, 9, 12, 10, LEFT_MOTOR_MODE);
uint32_t servoTimer;
Servo Servo1;
Servo Servo2;

void(* resetFunc)(void) = 0;

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(SOUND, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  MyBlue.begin(9600);
  motorR.setMode(AUTO);
  motorL.setMode(AUTO);
  motorR.setMinDuty(minDuty);
  motorL.setMinDuty(minDuty);
  motorR.setDirection(RIGHT_MOTOR_DIRECTION);
  motorL.setDirection(LEFT_MOTOR_DIRECTION);

  
}
void loop() {
  parsing();
 // if(doneParsing){ Serial.println(doneParsing);}
  // функция парсинга
  if(doneParsing){
  if (button == 1){
    digitalWrite(GREEN, 1);
    digitalWrite(YELLOW, 0);
    digitalWrite(RED, 0);
    if (on){on = false;
    resetFunc();
                 Servo1.detach();
              Servo2.detach();}
        doneParsing = false;
        doneX = false;
        doneY = false;
    int joystickX = map((dataX), 0, JOY_MAX, -MOTOR_MAX / 2, MOTOR_MAX / 2); // сигнал по Х
    int joystickY = map((dataY), 0, JOY_MAX, -MOTOR_MAX, MOTOR_MAX);         // сигнал по Y
    int dutyR = joystickY + joystickX; // считаем сигнал для правого мотора
    int dutyL = joystickY - joystickX; // считаем сигнал для левого мотора
        Serial.println(dutyR);
    dutyR = constrain(dutyR, -MOTOR_MAX, MOTOR_MAX); // ограничиваем значение для dutyR, на случай если dutyR > 255
    dutyL = constrain(dutyL, -MOTOR_MAX, MOTOR_MAX); // ограничиваем значение для dutyL, на случай если dutyL > 255
    motorR.setSpeed(dutyR); // плавно крутим правый мотор
    motorL.setSpeed(dutyL); // плавно крутим левый мотор
  }
     if (button == 2){
        if(not on){Servo1.attach(servoPin); on = true;
        Servo2.attach(servoPin2);}
            doneParsing = false;
        doneX = false;
        doneY = false;
            digitalWrite(GREEN, 0);
    digitalWrite(YELLOW, 1);
    digitalWrite(RED, 0);
    motorR.smoothTick(0); // плавно крутим правый мотор
    motorL.smoothTick(0);
   if (dataX != 512){
    int joystickX = map((dataX), 0, JOY_MAX, 0, 180); 
    Servo1.write(joystickX);// сигнал по Y
    Serial.println(joystickX);
    }
   if (dataY != 512){
    int joystickY = map((dataY), 0, JOY_MAX, 0, 180); 
    Servo2.write(joystickY);// сигнал по Y
    Serial.println(joystickY);
    }
   }
  if(button==3){
    digitalWrite(5, 1);
    digitalWrite(GREEN, 0);
    digitalWrite(YELLOW, 0);
    digitalWrite(RED, 1);
    tone(SOUND, 500);
    delay(400);
    noTone(SOUND);
    button=2;
    }else{digitalWrite(5, 0);}
  }
}


void parsing() {
  if (MyBlue.available() > 0) {
    char incomingChar = MyBlue.read();
    if(B){
      string_for_b += incomingChar;
      button = string_for_b.toInt();
      B = false;
      doneParsing = true;
      doneX = true;
      doneY = true;
      string_for_b = ' ';
      }
    if(Y){
      string_for_y += incomingChar;
      nY -= 1;
      if (nY == 0){
        dataY = string_for_y.toInt();
        Y = false;
        doneY = true;
        string_for_y = ' ';
        }
      }
      if(X){
      string_for_x += incomingChar;
      nX -= 1;
      if (nX == 0){
        dataX = string_for_x.toInt();
        X = false;
        doneX = true;
        string_for_x = ' ';
        }
      }
    if (incomingChar == 'B') {
        B = true;
        }
        
    else if (incomingChar == 'S') {
        Y = true;
        nY = 4;
        }
    else if (incomingChar == 'D') {
        X = true;
        nX =4;
      }
    if(doneX && doneY){
       doneParsing = true;
    }
  }
}
