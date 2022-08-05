#include "AsyncTaskLib.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Keypad.h>
#include "StateMachineLib.h"

//Initialize AsynkTask
void sensorTemp(void);
void sensorLuz(void);
void sensorMicrofono(void);

//creacion de estados
enum State{
    PosicionA = 0,
    PosicionB = 1,
    PosicionC = 2,
    PosicionD = 3
  };
  //creacion de entradas
  enum Input {
      Reset = 0,
      Forward = 1,
      Backward = 2,
      Unknown = 3,
  };
  Input input;
  //inicializo la maquina de estados
 StateMachine stateMachine(4, 9);
 void setupStateMachine()
 {
  stateMachine.AddTransition(PosicionA, PosicionB, []() { return input == Forward; });
  stateMachine.AddTransition(PosicionB, PosicionA, []() { return input == Backward; });
  stateMachine.AddTransition(PosicionB, PosicionC, []() { return input == Forward; });
  stateMachine.AddTransition(PosicionC, PosicionB, []() { return input == Backward; });
  stateMachine.AddTransition(PosicionC, PosicionD, []() { return input == Forward; });
  stateMachine.AddTransition(PosicionD, PosicionB, []() { return input == Forward; });

   // Add actions
  stateMachine.SetOnEntering(PosicionA, outputA);
  stateMachine.SetOnEntering(PosicionB, outputB);
  stateMachine.SetOnEntering(PosicionC, outputC);
  stateMachine.SetOnEntering(PosicionD, outputD);
  stateMachine.SetOnLeaving(PosicionA, []() {Serial.println("Leaving A"); });
  stateMachine.SetOnLeaving(PosicionB, []() {Serial.println("Leaving B"); });
  stateMachine.SetOnLeaving(PosicionC, []() {Serial.println("Leaving C"); });
  stateMachine.SetOnLeaving(PosicionD, []() {Serial.println("Leaving D"); });
  stateMachine.SetState(PosicionA, false, true);
}
 //TAREAS SINCRONICAS
AsyncTask asyncTaskTemp(2000,true, sensorTemp);
AsyncTask asyncTaskLight(1000,true, sensorLuz);
AsyncTask asyncTaskMicro(1000,true, sensorMicrofono);

//Initialize LiquidCrystal
LiquidCrystal lcd {12,11,5,4,3,2};//Pinouts: RS, En, D4, D5, D6, D7.

//Initialize Keypad
#define ROWS 4 //four rows
#define COLS 3 //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {23, 25, 27, 29}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {22, 24, 26}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Pin connections - RGB LED 
#define REDPIN 41
#define GREENPIN 43
#define BLUEPIN 45
//Connections - Encoder
#define CLKPIN 32
#define DTPIN 34
#define SWPIN 36
//Pin Sensors
#define TEMP A3
#define LIGHT A1
#define MICRO A2
#define DO_MICRO 26
#define LED_MICRO 13
#define BUZZER 48
//Maths
#define BETA 4090

//Variables - Menu
int posSetMenu = 0;
int auxPosMenu = 0;
//Variables - Encoder
bool clkState  = LOW;
bool clkLast  = HIGH;
bool swState  = HIGH;
bool swLast  = HIGH;
//Variables - Sensors
float tempValue =0.0;
int lightValue =0;
//Variables - Sistema seguridad
int flag=0;
int aux =0;
int pos;
int posCursor=0;
int intentos=0;
//Variables - Contraseña
char passwordReal[6] = {'3','2','4','2','3','3'}; //Se establece la contraseña para ingresar al stma
char passI[6]={}; //se crea un arreglo para guardar la contraseña q el usuario ingresa
//Variables - Read value
char enterValue[3]={'*','*','*'};
//Variables - Direccion EEPROM
int addressTempHigh=0;
int addressTempLow=0;
int addressLight=0;
int addressMicro=0;
//Variables - System 
int tempHigh=28;
int tempLow=18;
int light=850;
int micro=30;
int outputValue = 0;  
float tempC=0;
void setup() {
    //Init lcd 16x2
    lcd.begin(16, 2);
    //Init Serial USB
    Serial.begin(9600);
    //Init Rotary
    pinMode(CLKPIN,INPUT);
    pinMode(DTPIN,INPUT);
    pinMode(SWPIN,INPUT_PULLUP);
     //Initialize RGB LED
    pinMode(REDPIN, OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BLUEPIN, OUTPUT);
    //Initialize led microphone
    pinMode(LED_MICRO,OUTPUT);
    saveDefault();
    loadSystemValues();
    //AsyncTasks
    asyncTaskTemp.Start();
    asyncTaskLight.Start();
    //Inicializo maquina de estados
    setupStateMachine(); 
}

void loop() {
  input = static_cast<Input>(readInput());
  //Actualizar el estado de la maquina
  stateMachine.Update();
}
//EEPROM
void loadSystemValues(){//guarda los valores de la configuracion
  EEPROM.get(addressTempHigh,tempHigh);
  EEPROM.get(addressTempLow,tempLow);
  EEPROM.get(addressLight,light);
  EEPROM.get(addressMicro,micro);
}
void saveDefault(){//guarda valores por defecto
  EEPROM.put(addressTempHigh,tempHigh);
  addressTempLow=addressTempHigh+sizeof(tempHigh);
  EEPROM.put(addressTempLow,tempLow);
  addressLight=addressTempLow+sizeof(tempLow);
  EEPROM.put(addressLight,light);
  addressMicro=addressLight+sizeof(light);
  EEPROM.put(addressMicro,micro);
}
//Alarma - Piratas del caribe
void Alarmabuzzer(){
    tone(BUZZER,293.66,100);
    delay(100);
    tone(BUZZER,293.66,100);
    delay(100);
    tone(BUZZER,440,100);
    delay(100);
    tone(BUZZER,523.25,100);
    delay(100);
    tone(BUZZER,587.33,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,659.25,100);
    delay(100);
    tone(BUZZER,698.45,100);
    delay(200);
    tone(BUZZER,698.45,100);
    delay(200);
    tone(BUZZER,698.45,100);
    delay(100);
    tone(BUZZER,783.99,100);
    delay(100);
    tone(BUZZER,659.25,100);
    delay(200);
    tone(BUZZER,659.25,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,523.25,100);
    delay(100);
    tone(BUZZER,523.25,100);
    delay(100);
    tone(BUZZER,587.33,100);
    delay(300);
    tone(BUZZER,440,100);
    delay(100);
    tone(BUZZER,523.25,100);
    delay(100);
    tone(BUZZER,587.33,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,659.25,100);
    delay(100);
    tone(BUZZER,698.45,100);
    delay(200);
    tone(BUZZER,698.45,100);
    delay(200);
    tone(BUZZER,698.45,100);
    delay(100);
    tone(BUZZER,783.99,100);
    delay(100);
    tone(BUZZER,659.25,100);
    delay(200);
    tone(BUZZER,659.25,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,523.25,100);
    delay(100);
    tone(BUZZER,587.33,100);
    delay(400);
    tone(BUZZER,440,100);
    delay(100);
    tone(BUZZER,523.25,100);
    delay(100);
    tone(BUZZER,587.33,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,698.45,100);
    delay(100);
    tone(BUZZER,783.99,100);
    delay(200);
    tone(BUZZER,783.99,100);
    delay(200);
    tone(BUZZER,783.99,100);
    delay(100);
    tone(BUZZER,880,100);
    delay(100);
    tone(BUZZER,932.33,100);
    delay(200);
    tone(BUZZER,932.33,100);
    delay(200);
    tone(BUZZER,880,100);
    delay(100);
    tone(BUZZER,783.99,100);
    delay(100);
    tone(BUZZER,880,100);
    delay(100);
    tone(BUZZER,587.33,100);
    delay(300);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,659.25,100);
    delay(100);
    tone(BUZZER,698.45,100);
    delay(200);
    tone(BUZZER,698.45,100);
    delay(200);
    tone(BUZZER,783.99,100);
    delay(200);
    tone(BUZZER,880,100);
    delay(100);
    tone(BUZZER,587.33,100);
    delay(300);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,698.45,100);
    delay(100);
    tone(BUZZER,659.25,100);
    delay(200);
    tone(BUZZER,659.25,100);
    delay(200);
    tone(BUZZER,698.45,100);
    delay(100);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,659.25,100);
    delay(400);
    tone(BUZZER,880,100);
    delay(100);
    tone(BUZZER,1046.50,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,1318.51,100);
    delay(100);
    tone(BUZZER,1396.91,100);
    delay(200);
    tone(BUZZER,1396.91,100);
    delay(200);
    tone(BUZZER,1396.91,100);
    delay(100);
    tone(BUZZER,1567.98,100);
    delay(100);
    tone(BUZZER,1318.51,100);
    delay(200);
    tone(BUZZER,1318.51,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,1046.50,100);
    delay(100);
    tone(BUZZER,1046.50,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(300);
    tone(BUZZER,880,100);
    delay(100);
    tone(BUZZER,1046.50,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,1318.51,100);
    delay(100);
    tone(BUZZER,1396.91,100);
    delay(200);
    tone(BUZZER,1396.91,100);
    delay(200);
    tone(BUZZER,1396.91,100);
    delay(100);
    tone(BUZZER,1567.98,100);
    delay(100);
    tone(BUZZER,1318.51,100);
    delay(200);
    tone(BUZZER,1318.51,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,1046.50,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(400);
    tone(BUZZER,880,100);
    delay(100);
    tone(BUZZER,1046.50,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,1396.91,100);
    delay(100);
    tone(BUZZER,1567.98,100);
    delay(200);
    tone(BUZZER,1567.98,100);
    delay(200);
    tone(BUZZER,1567.98,100);
    delay(100);
    tone(BUZZER,1760,100);
    delay(100);
    tone(BUZZER,1864.66,100);
    delay(200);
    tone(BUZZER,1864.66,100);
    delay(200);
    tone(BUZZER,1760,100);
    delay(100);
    tone(BUZZER,1567.98,100);
    delay(100);
    tone(BUZZER,1760,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(300);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,1318.51,100);
    delay(100);
    tone(BUZZER,1396.91,100);
    delay(200);
    tone(BUZZER,1396.91,100);
    delay(200);
    tone(BUZZER,1567.98,100);
    delay(200);
    tone(BUZZER,1760,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(300);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,1396.91,100);
    delay(100);
    tone(BUZZER,1318.51,100);
    delay(200);
    tone(BUZZER,1318.51,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,1108.73,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(200);
    tone(BUZZER,1174.66,100);
    delay(200);
    tone(BUZZER,1318.51,100);
    delay(200);
    tone(BUZZER,1396.91,100);
    delay(200);
    tone(BUZZER,1396.91,100);
    delay(100);
    tone(BUZZER,1396.91,100);
    delay(100);
    tone(BUZZER,1567.98,100);
    delay(200);
    tone(BUZZER,1760,300);
    delay(400);
    tone(BUZZER,1396.91,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,880,300);
    delay(600);
    tone(BUZZER,1864.66,300);
    delay(400);
    tone(BUZZER,1396.91,100);
    delay(100);
    tone(BUZZER,1174.66,100);
    delay(100);
    tone(BUZZER,932.33,300);
    delay(600);
    tone(BUZZER,587.33,100);
    delay(100);
    tone(BUZZER,440,100);
    delay(200);
    tone(BUZZER,587.33,100);
    delay(300);
    tone(BUZZER,554.36,100);
    delay(400);
    tone(BUZZER,1567.98,100);
    delay(100);
    tone(BUZZER,1567.98,100);
    delay(100);
    lcd.clear();
    menuStart();
}
//security system
void gestionSistemaSeguridad(){
  while(intentos<3){
    lcd.setCursor(0,0);
    lcd.print("Ing Contrasenia");
    while(posCursor<6){
        obtenerTecla();
      }
       verifyPassword();
    }
     intentos=0;
     posCursor=0;
     flag=0;
     setColor(255,0,0);
     lcd.setCursor(0,0);
     lcd.print ("Stma bloqueado!  ");
     delay(120000);  
     setColor(0,0,0);
     stateMachine.SetState(PosicionA, false, true);
  }
void obtenerTecla(){
   char key = keypad.getKey();
   if (key){
     //guarda cada caracter ingresado en el arreglo y muestra un * por cada caracter
      passI[posCursor]=key;
      lcd.setCursor(posCursor++,1);
      lcd.print("*");
    }
}
void verifyPassword(){
  //cada vez que haya una inconsistencia comparando la clave, se modifica la flag
  for(int i;i<6;i++)
    if(passI[i]!=passwordReal[i]){
        flag++;
      }
    //se hace el llamado para verificar el estado de la flag  
    impEstado();
  }
  void impEstado(){
    if(flag==0){
      //se reinicia el sistema despues de ingresar
        passI[6]={};
        posCursor=0;
        intentos=0;
      //si la bandera no sufre cambios, la clave es correcta
      lcd.setCursor(0,0);
      lcd.print (" Clave correcta!  ");
      setColor(255,255,120);
      //setColor(80,255,0);
      delay(2000);
      setColor(0,0,0);
      //delay(2000);
      lcd.clear();
      //comenzar el menu
      menuStart();    
    }else{
      //si la band sufre cambios la contraseña es incorrecta
        lcd.setCursor(0,0);
        lcd.print ("   Incorrecta!  ");
        setColor(255,0,0);
        delay(2000);
        setColor(0,0,0);
        delay(1000);
        lcd.setCursor(0,0);
        lcd.print ("   Try Again!  ");
        //se limpia el arreglo de la contraseña ing y se aumenta el numero de intentos
        passI[6]={};
        posCursor=0;
        intentos++;
        lcd.clear();
      }
    }
//Start-menu
void menuStart(){
  boolean back=false;
  do{
    readRotary(auxPosMenu,2);
    if(auxPosMenu==0||auxPosMenu==1){
      showOptions("Setting  ","Run      "); //se imprime las opciones del menu principal
    }
    if(auxPosMenu==2){
      showOptions("Back     ","         ");
    }
    showSelected(auxPosMenu);
    if(isPressedRotary()){
      back=chooseMenuStart(auxPosMenu);  //si el enconder es presionado se dirige a la opción elegida según la posición del ENCODER
    }
  }while(back==false);
  auxPosMenu=0;
  lcd.clear();
}
boolean chooseMenuStart(int pos){
  if(pos==0){
    stateMachine.SetState(PosicionB, false, true);  //Se dirige al estado de configuracion
    stateMachine.Update();
   // menuSetting();
  }
  if(pos==1){
       stateMachine.SetState(PosicionC, false, true);  //Se dirige al estado de ejecución
       stateMachine.Update();
  }
   if(pos==2){
       stateMachine.SetState(PosicionA, false, true); //Se digirige al estado de seguridad
       stateMachine.Update();
       return true;
  }
}
//gesition encoder
void readRotary(int &rotVal, int maxOption) {
  // gestion position
  clkState = digitalRead(CLKPIN);
  if ((clkLast == LOW) && (clkState == HIGH)) {//rotary moving
    Serial.print("Rotary position "); //Se imprime por serial si el enconder se esta moviendo
    if (digitalRead(DTPIN) == HIGH) {
      rotVal = rotVal - 1;
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
boolean isPressedRotary(){//gestion boton del encoder
  swState = digitalRead(SWPIN);
  if (swState == LOW && swLast == HIGH) {
    Serial.println("Rotary pressed");//Se imprime por serial si el enconder fue presionado
    delay(500);
    swLast = swState;
    return true;
  }
  swLast = swState;
  return false;
}
//Configuracion del menu-EstadoB
void menuSetting(){
  boolean back=false;
  do{
    readRotary(posSetMenu,3);
    if(posSetMenu==0||posSetMenu==1){
      showOptions("UTemp    ","ULight   "); //Imprime las opciones del menu de configuración
    }else if(posSetMenu==2||posSetMenu==3){
      showOptions("UMicro   ","UBack    ");
    }
    showSelected(posSetMenu);
    if(isPressedRotary()){
      back=chooseSetMenu(); //si el ENCODER fue presionado se dirige a una opción
    }
  }while(back==false);
  
  posSetMenu=0;
  lcd.clear();
  menuStart();
  
}
boolean chooseSetMenu(){
  if(posSetMenu==0){
    menuSetTemp(); //se dirige al menu de la temperatura
  }
  if(posSetMenu==1){
    menuSetLight(); //se dirige a configurar la luz
  }
  if(posSetMenu==2){
   menuSetMicro(); //se dirige a configurar el micro
  }
   if(posSetMenu==3){
    return true;    //se dirige al menu principal
  }
  return false;
}
//options SubMenuTemp
void menuSetTemp(){
  boolean back=false;
  do{
    readRotary(auxPosMenu,2);
    if(auxPosMenu==0||auxPosMenu==1){
      showOptions("TempHigh ","TempLow  "); //muestra las opciones si se quiere configurar la temperatura alta o baja
    }
    if(auxPosMenu==2){
      showOptions("Back     ","         ");
    }
    showSelected(auxPosMenu);
    if(isPressedRotary()){
      back=chooseSetTemp(auxPosMenu); //si el ENCODER fue presionado se dirige a una opción
    }
  }while(back==false);
  auxPosMenu==0;
  lcd.clear();
}
boolean chooseSetTemp(int pos){
  if(pos==0){
    setTempHigh(); //se dirige a configurar la temperatura alta
  }
  if(pos==1){
    setTempLow(); //se dirige a configurar la temperatura baja
  }
  if(pos==2){
    return true;   //se dirige al menu de configuracion
  }
  return false;
}
//MenuTemp: setTempHigh
void setTempHigh(){
  int auxValue=0;
  enterValueIn(auxValue);
  if(auxValue>tempLow&&auxValue<110){
    EEPROM.put(addressTempHigh,auxValue); //lee el valor del nuevo umbral y lo guarda en la memoria EPROM
    tempHigh=auxValue;
    addressTempLow=addressTempHigh+sizeof(tempHigh);
    addressLight=addressTempLow+sizeof(tempLow);
    addressMicro=addressLight+sizeof(light);
    successfulMessage();
  }
  else{
    wrongMessage();
  }
}
//MenuTemp: setTempLow
void setTempLow(){
  int auxValue=0;
  enterValueIn(auxValue);
  if(auxValue>=0&&auxValue<tempHigh){
    EEPROM.put(addressTempLow,auxValue); //lee el valor del nuevo umbral y lo guarda en la memoria EPROM
    tempLow=auxValue;
    addressLight=addressTempLow+sizeof(tempLow);
    addressMicro=addressLight+sizeof(light);
    successfulMessage();
  }
  else{
    wrongMessage(); 
  }
}
//menuSetLight
void menuSetLight(){
  boolean back=false;
  do{
    readRotary(auxPosMenu,1);
    if(auxPosMenu==0||auxPosMenu==1){
      showOptions("SetLight ","Back     "); //muestra las opciones si se quiere configurar la luz
    }
    showSelected(auxPosMenu);
    if(isPressedRotary()){  //si el ENCODER fue presionado se dirige a una opción
      back=chooseSetLight(auxPosMenu);
    }
  }while(back==false);
  auxPosMenu==0;
  lcd.clear();
}
boolean chooseSetLight(int pos){
  if(pos==0){
    setLight(); //se dirige a configurar la luz
  }
  if(pos==1){
    return true; //se dirige al menu de configuracion
  }
  return false;
}
//setLight
void setLight(){
  int auxValue=0;
  enterValueIn(auxValue);
  if(auxValue>=0&&auxValue<1000){
    EEPROM.put(addressLight,auxValue); //lee el valor del nuevo umbral y lo guarda en la memoria EPROM
    light=auxValue;
    addressMicro=addressLight+sizeof(light);
    successfulMessage();
  }
  else{
    wrongMessage();
  }
}
//Option 2: menuSetMicro
void menuSetMicro(){
  boolean back=false;
  do{
    readRotary(auxPosMenu,1);
    if(auxPosMenu==0||auxPosMenu==1){
      showOptions("SetMicro ","Back     "); //muestra las opciones si se quiere configurar el micro
    }
    showSelected(auxPosMenu);
    if(isPressedRotary()){
      back=chooseSetMicro(auxPosMenu); //si el ENCODER fue presionado se dirige a una opción
    }
  }while(back==false);
  auxPosMenu==0;
  lcd.clear();
}
boolean chooseSetMicro(int pos){
  if(pos==0){
    setMicro();  //se dirige a configurar el micro
  }
  if(pos==1){
    return true;  //se dirige al menu de configuracion
  }
  return false;
}
//setMicro
void setMicro(){
  int auxValue=0;
  enterValueIn(auxValue);
  if(auxValue>=0&&auxValue<100){
    EEPROM.put(addressMicro,auxValue); //lee el valor del nuevo umbral y lo guarda en la memoria EPROM
    micro=auxValue;
    successfulMessage();
  }
  else{
    wrongMessage();
  }
}
//muestra las opciones en el LCD
void showOptions(char highOption[], char lowOption[]){
  lcdSetPrint(1,0,highOption);
  lcdSetPrint(1,1,lowOption);
}
//Muestra un '*' en la parte izquierda de las opciones 
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
//EstadoC ejecución
void runn(){
  lcd.clear();
  
  while(!isPressedRotary()){
    asyncTaskTemp.Update();
    asyncTaskLight.Update();
    validarAsync();
  }
  lcd.clear();
  menuStart();
    
}

//validar los umbrales de temp y luz
//si alguna se pasa se activa la alarma (pasa al estado D)
void validarAsync(){
  
        if(tempC>tempHigh or outputValue>light){
          setColor(255,0,0);
          setColor(0,0,0);
          stateMachine.SetState(PosicionD, false, true);
        }
       else if(tempC<tempLow){
          setColor(255,255,120);
          setColor(0,0,0);
        }
      else{
          setColor(80,255,0);
          setColor(0,0,0);
        }
  }
  //obtiene el valor de la temperatura
void sensorTemp(void){
    //read thermistor value
    long a =1023 - analogRead(TEMP);
    //the calculating formula of temperature
     tempC = BETA /(log((1025.0 * 10 / a - 10) / 10) + BETA / 298.0) - 273.0;
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
     lcd.setCursor(6, 0);
    // Print a centigrade temperature to the LCD.
    lcd.print(tempC);
    // Print the unit of the centigrade temperature to the LCD.
    lcd.print("C");
    //lcd.clear();
  }
 //async para leer el valor de la luz por medio del photo
    void sensorLuz(void){
    
    outputValue = analogRead(LIGHT);
    lcd.setCursor(0, 1);
    lcd.print("Photocell:");
    lcd.setCursor(11, 1);
    lcd.print(outputValue);//print the temperature on lcd1602
    Serial.println(outputValue);
    //lcd.setCursor(11, 1);
    //lcd.print(" ");
    //lcd.clear();
    }
  
    void sensorMicrofono(void)
    {
      int value = analogRead(MICRO);
      Serial.println(value);
      validarMicro(value);
     }
      void validarMicro(int value){
            if(value > micro)
            {

              setColor(255,0,0);
            }
          else
           {
              setColor(0,0,0);
           }
        }
//funcion para activar los tonos del led RGB
void setColor(int red, int green, int blue){
  analogWrite(REDPIN,red);
  analogWrite(GREENPIN,green);
  analogWrite(BLUEPIN,blue);
}
void ledWith(int r, int g, int b, int time_ms){
  setColor(r,g,b);
  delay(time_ms);
}


//funcion para confirmar el valor guardado en EEPROM
void successfulMessage(){
  lcd.clear();
  lcdSetPrint(2,0,"Completado!");
  delay(2000);
  lcd.clear();
}
//avisa cuando el valor no pudo ser guardado en EEPROM
void wrongMessage(){
  lcd.clear();
  lcdSetPrint(2,0,"Error!");
  delay(2000);
  lcd.clear();
}
/*
 funciones para imprimir cada item del menu
 y/o valor ingresado
*/
void lcdSetPrint(int c, int f, char message[]){
  lcd.setCursor(c, f);
  lcd.print(message);
}
void lcdSetPrint(int c, int f, int value){
  lcd.setCursor(c, f);
  lcd.print(value);
}
void lcdSetPrint(int c, int f, float value){
  lcd.setCursor(c, f);
  lcd.print(value);
}
void lcdSetPrint(int c, int f, char value){
  lcd.setCursor(c, f);
  lcd.print(value);
}
//fn para ingresar los valores de configuracion
//por medio del teclado
void enterValueIn(int &value){
  pos=0;
  lcd.clear();
  boolean auxBack=false;
  while(auxBack==false){
    lcdSetPrint(0,0,"Ingrese un valor:");
    char key = keypad.getKey();
    if(key){
      if(key =='*'){
            deleteValue();
        }
        else if(pos<3){
        enterValue[pos]=key;
        lcd.setCursor(pos++,1);
        lcd.print(key);
      }
    }
    if(isPressedRotary()) {
        value=parseValue();
        clearEnterValue();
        lcd.clear();
        return;
      }
  }
}
void clearEnterValue(){
  enterValue[0]='*';
  enterValue[1]='*';
  enterValue[2]='*';
  pos==0;
}

void deleteValue(){
  if(pos>0){
    pos--;
    lcdSetPrint(pos,1,' ');
    enterValue[pos]='*';
  }
}
//convertir el valor a entero
int parseValue(){
    return atoi(enterValue);
}
//poner la entrada de la maquina de estados
void setInput(int value){
  input = static_cast<Input>(value);
}
//obtener la entrada de la maquina de estados
int getInput(){
  return static_cast<int>(input);
}
//----Auxiliar function that reads the user input----
int readInput(){
  Input currentInput = Input::Unknown;
  if (Serial.available())
  {
    char incomingChar = Serial.read();
    switch (incomingChar)
    {
      case 'R': currentInput = Input::Reset;  break;
      case 'A': currentInput = Input::Backward; break;
      case 'D': currentInput = Input::Forward; break;
      default: break;
    }
  }
  return currentInput;
}
//----Auxiliar output functions that show the state debug----
void outputA(){//security
  Serial.println("A   B   C   D");
  Serial.println("X            ");
  Serial.println();
  gestionSistemaSeguridad();
   
}
void outputB(){//startMenu
  Serial.println("A   B   C   D");
  Serial.println("    X        ");
  Serial.println();
  menuSetting();
}
void outputC(){//taskManager
  Serial.println("A   B   C   D");
  Serial.println("        X    ");
  Serial.println();
  runn();
}
void outputD(){//alarm
  Serial.println("A   B   C   D");
  Serial.println("            X");
  Serial.println();
  Alarmabuzzer();
}
