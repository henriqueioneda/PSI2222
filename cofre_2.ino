#include <EEPROM.h>
#define N 10

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
int senha[N];

char* currentState = "estado_inicial";

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
  }
 
}


void loop()
{
  
  switch(*currentState){
    
  case 'estado_inicial': 
    for ( int i=1; i<6; i++){
      if ((i != senha[0] ) && (digitalRead(i) == HIGH)){
        currentState = "SR"; 
        digitalWrite(LED_B, HIGH);
      }
      else if (digitalRead(i) == HIGH){
        currentState = "s1";
        digitalWrite(LED_B, HIGH);
      }
    }    
    break;
  case 's1':
    for ( int i=1; i<6; i++){
      if ((i != senha[1] ) && ( digitalRead(i) == HIGH)){
        currentState = "SE";
        digitalWrite(LED_B, HIGH);
      }
      else if (digitalRead(i) == HIGH){
        currentState = "s2";
        digitalWrite(LED_B, HIGH);
      }
    }
    break;
  }
}
