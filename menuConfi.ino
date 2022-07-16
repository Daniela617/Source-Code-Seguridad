#include <LiquidCrystal.h>
#include <Keypad.h>
#include "AsyncTaskLib.h"
#include <EEPROM.h>
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



///////////////////////////////////////
//Emprom
const int direccion_UTempHigh=0;
const int direccion_UTempLow=1;
const int direccion_ULuz=2;
const int direccion_centenas_Luz=3;
////////////////////////////////////////
//LIMITES DEFINIDOS
int UTempHigh=25;
int UTempLow=18;
int ULuz=500;
//////////////////////////////////////////
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
int varPosSetMenu = 0;
int varAuxMenu = 0;
//Variables - RotaryEncoder
bool clkState  = LOW;
bool clkLast  = HIGH;
bool swState  = HIGH;
bool swLast  = HIGH;

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
  asycTaskTemp.Start();
  asycTaskPhoto.Start();
  asycTaskMicro.Start();
  //guardar en EEPROM
   UTempHigh=EEPROM.read(direccion_UTempHigh);
   UTempLow=EEPROM.read(direccion_UTempLow);
   ULuz=EEPROM.read(direccion_ULuz)+EEPROM.read(direccion_centenas_Luz)*100;
}
void loop()
{
  controlMenu();
 }
void controlMenu(){
   boolean varAtras=false;
  do{
    ecoderMovimiento(varAuxMenu,1);
    if(varAuxMenu==0||varAuxMenu==1){//se muestre cada opcion en la pantalla
      lcd.setCursor(1,0);
      lcd.print("Menu config");
      lcd.setCursor(1,1);
      lcd.print("Sistema ");
    }
    seleccionado(varAuxMenu);
    if(ecoderPresionado()){
      varAtras=pickMainMenu(varAuxMenu);
    }
  }while(varAtras==false);
  varAuxMenu=0;
  lcd.clear();
  }
boolean pickMainMenu(int prmvarPos){
  if(prmvarPos==0){
    configuracion();
  }
  if(prmvarPos==1){
    sistema();
  }
}
  void configuracion(){ 
    boolean varAtras=false;
  do{
    ecoderMovimiento(varPosSetMenu,3);
    if(varPosSetMenu==0||varPosSetMenu==1){
      lcd.setCursor(1,0);
      lcd.print("ConfTemp    ");
      lcd.setCursor(1,1);
      lcd.print("ConfLight   ");
    }else if(varPosSetMenu==2||varPosSetMenu==3){
      lcd.setCursor(1,0);
      lcd.print("ConfMicro   ");
      lcd.setCursor(1,1);
      lcd.print("ConfAtras    ");
    }
    seleccionado(varPosSetMenu);
    if(ecoderPresionado()){
      varAtras=opcionesMainMenu();
    }
  }while(varAtras==false);
  varPosSetMenu=0;
  lcd.clear();
}
boolean opcionesMainMenu(){
  switch(varPosSetMenu){
    case 0:
      mainTemp();
      break;
    case 1:
      mainLuz();
      break;
    case 2:
      mainMicro();
      break;
    case 3:
      return true;
  }
  return false;
}
void mainTemp(){
  boolean varAtras=false;
  do{
    ecoderMovimiento(varAuxMenu,2);
    if(varAuxMenu==0||varAuxMenu==1){
      lcd.setCursor(1,0);
      lcd.print("TempAlta");
      lcd.setCursor(1,1);
      lcd.print("TempBaja");
    }
    if(varAuxMenu==2){
      lcd.setCursor(1,0);
      lcd.print("Atras");
    }
    seleccionado(varAuxMenu);
    if(ecoderPresionado()){
      varAtras=opcionesTemp(varAuxMenu);
    }
  }while(varAtras==false);
  varAuxMenu=0;
  lcd.clear();
}
boolean opcionesTemp(int varPos){
  if(varPos==0){
    opcionConfTempH();
  }
  if(varPos==1){
    opcionConfTempL();
  }
  if(varPos==2){
    return true;
  }
  return false;
}
void mainLuz(){
  boolean varAtras=false;
  do{
    ecoderMovimiento(varAuxMenu,1);
    if(varAuxMenu==0||varAuxMenu==1){
       lcd.setCursor(1,0);
      lcd.print("Luz");
      lcd.setCursor(1,1);
      lcd.print("Atras ");
    }
    seleccionado(varAuxMenu);
    if(ecoderPresionado()){
      varAtras=opcionesLuz(varAuxMenu);
    }
  }while(varAtras==false);
  varAuxMenu==0;
  lcd.clear();
}
boolean opcionesLuz(int varPos){
  if(varPos==0){
    opcionConfLuz();
  }
  if(varPos==1){
    return true;
  }
  return false;
}
void mainMicro(){

  boolean varAtras=false;
  do{
    ecoderMovimiento(varAuxMenu,1);
    if(varAuxMenu==0||varAuxMenu==1){
      lcd.setCursor(1,0);
      lcd.print("Microfono");
      lcd.setCursor(1,1);
      lcd.print("Atras ");
    }
    seleccionado(varAuxMenu);
    if(ecoderPresionado()){
      varAtras=opcionesMicro(varAuxMenu);
    }
  }while(varAtras==false);
  varAuxMenu==0;
  lcd.clear();
}
boolean opcionesMicro(int varPos){
  if(varPos==0){
    opcionConMicro();
  }
  if(varPos==1){
    return true;
  }
  return false;
}
void opcionConMicro(){
   ingresa();

  
}
void seleccionado(int varPos){
  if(varPos%2==0){
    lcd.setCursor(0,0);
    lcd.print("*");
    lcd.setCursor(0,1);
    lcd.print(" ");
  }
  else{
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print("*");
  }
}
void sistema(){
  asycTaskTemp.Update();
  asycTaskPhoto.Update();
  asycTaskMicro.Update();

}
void ecoderMovimiento(int &rotacion, int maxOption) {
  clkState = digitalRead(clkPin);
  if ((clkLast == LOW) && (clkState == HIGH)) {
    if (digitalRead(dtPin) == HIGH) {
      rotacion--;
      if ( rotacion < 0 ) {
        rotacion = 0;
      }
    }
    else {
      rotacion++;
      if ( rotacion > maxOption ) {
        rotacion = maxOption;
      }
    }
  }
  clkLast = clkState;
}
boolean ecoderPresionado(){
  swState = digitalRead(swPin);
  if (swState == LOW && swLast == HIGH) {
    delay(500);
    swLast = swState;
    return true;
  }
  swLast = swState;
  return false;
}
//VAR CONFIGURACION
    String caden1="";
    int temConf1,varPos1=0;
    String caden2="";
    int temConf2,varPos2=0;
    String caden3="";
    int temConf3,varPos3=0;
//TODO procedimientos para los submenus
void opcionConfTempH(){
    char key = keypad.getKey();
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ConfiTempH: ");
  if (key){
    //guarda cada caracter ingresado en el arreglo y muestra un * por cada caracter
    lcd.setCursor(varPos1++,1);
    lcd.print(key);
    caden1=caden1+key;
    UTempHigh= caden1.toInt();
  }
    EEPROM.write(direccion_UTempHigh, UTempHigh);



 
}
void opcionConfTempL(){
  char key = keypad.getKey();
  lcd.clear();
  lcd.setCursor(1, 0);   
  lcd.print("ConfiTempL: ");
  if (key){
    //guarda cada caracter ingresado en el arreglo y muestra un * por cada caracter
    lcd.setCursor(varPos2++,1);
    lcd.print(key);
    caden2=caden2+key;
    UTempLow= caden2.toInt();
           
  }
    EEPROM.write(direccion_UTempLow, UTempLow);

}
 void opcionConfLuz(){
    char key = keypad.getKey();
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ConfiLuz: ");
    if (key){
      //guarda cada caracter ingresado en el arreglo y muestra un * por cada caracter
      lcd.setCursor(varPos3++,1);
      lcd.print(key);
      caden3=caden3+key;
      ULuz= caden3.toInt();
           
    }

   if(ULuz>=100)
    {
      EEPROM.write(direccion_centenas_Luz, trunc(ULuz/100));
      EEPROM.write(direccion_ULuz, ULuz-trunc(ULuz/100)*100);
    }
    else
    {
      
      EEPROM.write(direccion_ULuz, ULuz);
      EEPROM.write(direccion_centenas_Luz, 0);
    }
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
  if(tempC>UTempHigh){
    setColor(255,0,0);
    buzzer();
  }
  else if(tempC<UTempLow){
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
        if(outputValue>ULuz){
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
void ingresa(){
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("aaaaaaa");
  delay(2000);
  lcd.clear();
}
