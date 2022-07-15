#include <LiquidCrystal.h>
#include <Keypad.h>
#include "AsyncTaskLib.h"
//defino las notas musicales
#define NOTE_B0  31  
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define clkPin 6
#define dtPin 7
#define swPin 8

#define PULSADOR 10    // pulsador en pin 2
#define BUZZER_PASIVO 9   // buzzer pasivo en pin 8
//config temp var
int state_clk_old;
int count;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd (12,11,5,4,3,2);
#define analogPin A0 //the thermistor attach to
const int soundPin = A2;
#define beta 4090 //the beta of the thermistor
#define resistance 10 //the value of the pull-down resistorvoid

void sensorTemp(void);
void sensorLuz(void);
void sensorMicrofono(void);
void buzzer(void);
int melodia[] = {    // array con las notas de la melodia
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4, NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4
};
int duraciones[] = {    // array con la duracion de cada nota
  8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 2
};
//creo las tareas asincrónicas
AsyncTask asycTaskTemp(2000,true,sensorTemp);
AsyncTask asycTaskPhoto(2000,true,sensorLuz);
AsyncTask asycTaskMicro(2000,true,sensorMicrofono);
AsyncTask asycTaskBuzzer(2000,true,buzzer);

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {39,41,43,45}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {47,49,51}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const int photocellPin = A0;
int redPin=30;
int greenPin=32;
int bluePin=34;
int outputValue = 0;
//Variables - Menu
int posSetMenu = 0;
int auxPosMenu = 0;
//Variables - RotaryEncoder
bool clkState  = LOW;
bool clkLast  = HIGH;
bool swState  = HIGH;
bool swLast  = HIGH;
#define clkPin 32
#define dtPin 34
#define swPin 36

 void setup()
{
  //inicializo enconder
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH);
  //inicializo buzzer
  pinMode(PULSADOR, INPUT_PULLUP);  // pin 2 como entrada con resistencia de pull-up
  pinMode(BUZZER_PASIVO, OUTPUT); // pin 8 como salida
  //inicializo leds
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(bluePin,OUTPUT);
  //inicializo serial
  Serial.begin(115200);
  //inicializo pantalla lcd
  lcd.begin(16, 2);
  //inicializo las tareas
  //asycTaskTemp.Start();
  //asycTaskPhoto.Start();
  //asycTaskMicro.Start();
  // asycTaskBuzzer.Start();
}
void loop()
{
  controlMenu();
    // asycTaskTemp.Update();
    // asycTaskPhoto.Update();
    // asycTaskMicro.Update();
    // asycTaskBuzzer.Update();
 }
void controlMenu(){
   boolean back=false;
  do{
    readRotary(auxPosMenu,1);
    if(auxPosMenu==0||auxPosMenu==1){
      showOptions("Menu config  ","Sistema    ");
    }
    showSelected(auxPosMenu);
    if(isPressedRotary()){
      back=chooseMenuStart(auxPosMenu);
    }
  }while(back==false);
  auxPosMenu=0;
  lcd.clear();
  }
boolean chooseMenuStart(int prmPos){
  if(prmPos==0){
    menuSetting();
  }
  if(prmPos==1){
    runn();
  }
}
  void menuSetting(){
  boolean back=false;
  do{
    readRotary(posSetMenu,3);
    if(posSetMenu==0||posSetMenu==1){
      showOptions("UTemp    ","ULight   ");
    }else if(posSetMenu==2||posSetMenu==3){
      showOptions("UMicro   ","UBack    ");
    }
    showSelected(posSetMenu);
    if(isPressedRotary()){
      back=chooseSetMenu();
    }
  }while(back==false);
  posSetMenu=0;
  lcd.clear();
}
boolean chooseSetMenu(){
  switch(posSetMenu){
    case 0:
      menuSetTemp();
      break;
    case 1:
      menuSetLight();
      break;
    case 2:
      menuSetMicro();
      break;
    case 3:
      return true;
  }
  return false;
}
void menuSetTemp(){
  boolean back=false;
  do{
    readRotary(auxPosMenu,2);
    if(auxPosMenu==0||auxPosMenu==1){
      showOptions("TempHigh ","TempLow  ");
    }
    if(auxPosMenu==2){
      showOptions("Back     ","         ");
    }
    showSelected(auxPosMenu);
    if(isPressedRotary()){
      back=chooseSetTemp(auxPosMenu);
    }
  }while(back==false);
  auxPosMenu==0;
  lcd.clear();
}
boolean chooseSetTemp(int pos){
  if(pos==0){
    setTempHigh();
  }
  if(pos==1){
    setTempLow();
  }
  if(pos==2){
    return true;
  }
  return false;
}
void setTempHigh(){
    successful();
    //TODO: read the value
  //EEPROM.put(addressTempHigh,value);
  //TODO: edit address
}
//Option 0.1: setTempLow
void setTempLow(){
  successful();
  //TODO: read the value
  //EEPROM.put(addressTempLow,value);
  //TODO: edit address
}
void menuSetLight(){
  boolean back=false;
  do{
    readRotary(auxPosMenu,1);
    if(auxPosMenu==0||auxPosMenu==1){
      showOptions("SetLight ","Back     ");
    }
    showSelected(auxPosMenu);
    if(isPressedRotary()){
      back=chooseSetLight(auxPosMenu);
    }
  }while(back==false);
  auxPosMenu==0;
  lcd.clear();
}
boolean chooseSetLight(int pos){
  if(pos==0){
    setLight();
  }
  if(pos==1){
    return true;
  }
  return false;
}
//Option 1.0: setLight
void setLight(){
  successful();
  //TODO: read the value
  //EEPROM.put(addressLight,value);
  //TODO: edit address
}
void menuSetMicro(){
  boolean back=false;
  do{
    readRotary(auxPosMenu,1);
    if(auxPosMenu==0||auxPosMenu==1){
      showOptions("SetMicro ","Back     ");
    }
    showSelected(auxPosMenu);
    if(isPressedRotary()){
      back=chooseSetMicro(auxPosMenu);
    }
  }while(back==false);
  auxPosMenu==0;
  lcd.clear();
}
boolean chooseSetMicro(int pos){
  if(pos==0){
    setMicro();
  }
  if(pos==1){
    return true;
  }
  return false;
}
void setMicro(){
    successful();
  //TODO: read the value
  //EEPROM.put(addressMicro,value);
  //TODO: edit address
}
//show two options in lcd
void showOptions(char highOption[], char lowOption[]){
  lcdSetPrint(1,0,highOption);
  lcdSetPrint(1,1,lowOption);
}
//show '*' in the left when is selected 
void showSelected(int pos){
  if(pos%2==0){
    lcdSetPrint(0,0,'*');
    lcdSetPrint(0,1,' ');
  }
  else{
    lcdSetPrint(0,0,' ');
    lcdSetPrint(0,1,'*');
  }
}
//run
void runn(){
  asyncTaskTemp.Update();
  asyncTaskLight.Update();
}
//rotary encoder functions
void readRotary(int &rotVal, int maxOption) {
  // gestion position
  clkState = digitalRead(clkPin);
  if ((clkLast == LOW) && (clkState == HIGH)) {//rotary moving
    Serial.print("Rotary position ");
    if (digitalRead(dtPin) == HIGH) {
      rotVal--;
      if ( rotVal < 0 ) {
        rotVal = 0;
      }
    }
    else {
      rotVal++;
      if ( rotVal > maxOption ) {
        rotVal = maxOption;
      }
    }
    Serial.println(rotVal);
  }
  clkLast = clkState;
}
boolean isPressedRotary(){//gestion bouton in rotary
  swState = digitalRead(swPin);
  if (swState == LOW && swLast == HIGH) {
    Serial.println("Rotary pressed");
    delay(500);//debounce
    swLast = swState;
    return true;
  }
  swLast = swState;
  return false;
}
  //TODO procedimientos para los submenus
  void opcionConfTempH(){
      String caden="";
      int temConf,pos=0;
      lcd.setCursor(1, 0);
      lcd.print("ConfiTemp: ");
      char key = keypad.getKey();
      if (key){
     //guarda cada caracter ingresado en el arreglo y muestra un * por cada caracter
           caden=caden+key;
           temConf=caden.toInt();
           lcd.setCursor(pos++,1);
           lcd.print(key);
      }
    }
    void opcionConfTempL(){
      lcd.clear();
       lcd.setCursor(1, 0);
      lcd.print("ConfiTemp: ");
      String caden="";
      int temConf,pos=0;
      char key = keypad.getKey();
      if (key){
     //guarda cada caracter ingresado en el arreglo y muestra un * por cada caracter
           Serial.print(key);
           caden=caden+key;
           temConf=caden.toInt();
           lcd.setCursor(pos++,1);
           lcd.print(key); 
      }
    }
    void opcionConSensorLuz(){
      }
    void opcionConSensorMicro(){
        }
  void back(){
      menu.change_screen(1);
      menu.set_focusedLine(1);
   }
 
void sensorTemp(void){

    //read thermistor value
    long a =1023 - analogRead(analogPin);
    //the calculating formula of temperature
    float tempC = beta /(log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273.0;
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    // Print a centigrade temperature to the LCD.
    lcd.print(tempC);
    // Print the unit of the centigrade temperature to the LCD.
    lcd.print("C");
    validarTemp(tempC);
    //lcd.clear();
  }
  void validarTemp(float tempC){
       if(tempC>25){
          setColor(255,0,0);
          buzzer();
        }
       else if(tempC<18){
          setColor(255,255,120);
        }
      else{
          setColor(80,255,0);
        }
    }
  void sensorLuz(void){
    outputValue = analogRead(photocellPin);
    lcd.setCursor(0, 0);
    lcd.print("Photocell:");
    lcd.setCursor(11, 0);
    lcd.print(outputValue);//print the temperature on lcd1602
    Serial.println(outputValue);
    validarLuz();
    }
    void validarLuz(){
        if(outputValue>500){
          setColor(255,51,255);
        }
      else{
          setColor(0,102,255);
        }
    }
    void sensorMicrofono(void)
    {
      int value = analogRead(soundPin);
      Serial.println(value);
      validarMicro(value);
     }
 void validarMicro(int value){
     if(value > 30){
          setColor(255,0,0);
      }
     else{
        setColor(0,0,0);
        }
    }
 void buzzer(){
       for (int i = 0; i < 25; i++) {              // bucle repite 25 veces
       int duracion = 1000 / duraciones[i];        // duracion de la nota en milisegundos
       tone(BUZZER_PASIVO, melodia[i], duracion);  // ejecuta el tono con la duracion
       int pausa = duracion * 1.30;                // calcula pausa
       delay(pausa);                               // demora con valor de pausa
       noTone(BUZZER_PASIVO);                      // detiene reproduccion de tono
      }
  }
void setColor(int red,int green,int blue){
    analogWrite(redPin,red);
    analogWrite(greenPin,green);
    analogWrite(bluePin,blue);
    }
//message in lcd succesful
void successful(){
  lcd.clear();
  lcdSetPrint(2,0,"Successful!");
  delay(2000);
  lcd.clear();
}
