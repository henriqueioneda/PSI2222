#include <EEPROM.h>
#define N 10
#define temp 10000
#define sinal_abre 120 //A DEFINIR;
#define sinal_fecha 50 //A DEFINIR;

int s[10] = {0,1,2,3,4,5,6,7,8,9};

int LED_R = 11;
int LED_B = 12;
int LED_G = 13;

int chave_A = 1;
int chave_B = 2;
int chave_C = 3;
int chave_D = 4;
int chave_E = 5;
int porta = 7;

int servo = 9;
int senha[N], senha_m[N];

unsigned long ti;

int currentState = s[0];

void setup()
{  
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(chave_A, INPUT);
  pinMode(chave_B, INPUT);
  pinMode(chave_C, INPUT);
  pinMode(chave_D, INPUT);
  pinMode(chave_E, INPUT);
  pinMode(porta, INPUT);
  pinMode(servo, OUTPUT);
  
  for (int k=0; k<N; k++){
    senha[k] = EEPROM.read(k);
    senha_m[k] = EEPROM.read(k+10);
  }
 
}


void loop()
{
  const int s1 = 1;
  switch(currentState){
    
  case 'estado_inicial': 
    for ( int i=1; i<6; i++){    //CONFERIR ALGUMAS VEZES;
      if ((i != senha[0] ) && (i != senha_m[i]) && (digitalRead(i) == HIGH)){
        currentState = 0; 
        digitalWrite(LED_B, HIGH);
      }
      else if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        if (i == senha[i]) currentState = s[1];
        else currentState = 0;
        ti = millis();
      }
      else digitalWrite(LED_B, LOW);
    }    
    break;
    
  case s1:
    if (millis()-ti > temp){
      currentState = "estado_inicial";
      break;
    }
    if (senha[1] == -1){
      currentState = "trava_aberta";
      analogWrite(servo, sinal_abre);
      break;
    }
    for ( int i=1; i<6; i++){
      if ((i != senha[1] ) && ( digitalRead(i) == HIGH)){
        digitalWrite(LED_B, HIGH);
        currentState = "SE";

      } 
      else if (digitalRead(i) == HIGH){
        currentState = "s2";
        ti = millis();
        digitalWrite(LED_B, HIGH);
      }
    }
    break;
    
  case 'aeriufghcaiufhafgerfA':
    if (millis()-ti > temp){
      currentState = "estado_inicial";
      break;
    }
    for ( int i=1; i<6; i++){
      if ((i != senha_m[1] ) && ( digitalRead(i) == HIGH)){
        currentState = "SE";
        digitalWrite(LED_B, HIGH);
      } 
      else if (digitalRead(i) == HIGH){
        currentState = "m2";
        ti = millis();
        digitalWrite(LED_B, HIGH);
      }
    }
    break;
    
  }
}
