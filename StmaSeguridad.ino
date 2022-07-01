 // Demonstrates the simplest use of the matrix Keypad library.

#include <Keypad.h> 
#include <LiquidCrystal.h>


LiquidCrystal lcd (0,1,5,4,3,2);

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {30, 32, 34, 36}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {31, 33, 35}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int flag=0;
int aux =0;
int pos=0;
int intentos=0;
char passwordReal[6] = {'3','2','4','2','3','3'};
char passI[6]={};
int redPin=8;
int greenPin=9;
int bluePin=10;

void setup(){

  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(bluePin,OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
 // lcd.print ("   Welcome!  ");
   //setColor(255,215,0);
  //delay(2000);
  
}

void loop(){
  if(intentos<3){
   //   lcd.begin(16, 2);
      lcd.setCursor(0,0);
      lcd.print("Ing Contrasenia");
    if(pos<6)
      obtenerTecla();
      
     else{
      verifyPassword();
      }
  }
  else{
       intentos=0;
       pos=0;
       flag=0;
       setColor(255,0,0);
       delay(2000);
       lcd.setCursor(0,0);
       lcd.print ("Stma bloqueado!  ");
        delay(120000);
        //delay(1000);
        
    }
  
}
void obtenerTecla(){
   char key = keypad.getKey();
   if (key){
     //verifica caracter ingresado con el caracter del arreglo antes del incremento de pos
           passI[pos]=key;
           lcd.setCursor(pos++,1);
           lcd.print("*");
           
    }
}
void verifyPassword(){
    
      for(int i;i<6;i++)
          if(passI[i]!=passwordReal[i]){
        ////////////////////////////
            flag++;
        ////////////////////////////////
      }   
      impEstado();
  }
  void impEstado(){
    
    if(flag==0){
        lcd.setCursor(0,0);
        lcd.print (" Clave correcta!  ");
        
        setColor(80,255,0);
        delay(1000);
        setColor(0,0,0);
          delay(2000);
          sistemaDesbloqueado();
          passI[6]={};
          pos=0;
          intentos++;
          lcd.clear();
          setColor(0,0,0);
          
      }else{
        
           lcd.setCursor(0,0);
          lcd.print ("   Incorrecta!  ");
          //delay(2000);
          setColor(255,255,120);
          delay(2000);
          setColor(0,0,0);
          delay(1000);
          lcd.setCursor(0,0);
          lcd.print ("   Try Again!  ");
          passI[6]={};
          pos=0;
          intentos++;
          lcd.clear();
        
        }
    
    
    }
  void setColor(int red,int green,int blue){
    analogWrite(redPin,red);
    analogWrite(greenPin,green);
    analogWrite(bluePin,blue);
    
    }

  void sistemaDesbloqueado(){
    setColor(255,0,255);
    delay(1000);
    setColor(255,0,120);
    delay(1000);
    setColor(255,0,255);
    delay(1000);
    setColor(255,0,120);
    delay(1000);
    setColor(255,0,255);
    delay(1000);
    setColor(255,0,120);
    delay(1000);
    setColor(255,0,255);
    delay(1000);
    setColor(255,0,120);
    delay(1000);
    setColor(255,0,255);
    delay(1000);
    setColor(255,0,120);
    delay(1000);
}
