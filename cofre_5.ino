#include <EEPROM.h>
#define N 10
#define temp 10000
#define sinal_abre 120 //A DEFINIR;
#define sinal_fecha 50 //A DEFINIR;
#define trans 500      //ajustar;

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

int currentState;

const int senha_errada = -1;
const int estado_inicial = 0;
const int a1 = 1, a2 = 2, a3 = 3, a4 = 4, a5 = 5, a6 = 6, a7 = 7, a8 = 8, a9 = 9, aX = 10; 
const int m1 = 11, m2 = 12, m3 = 13, m4 = 14, m5 = 15, m6 = 16, m7 = 17, m8 = 18, m9 = 19, mX = 20;
const int t1 = 21, t2 = 22, t3 = 23, t4 = 24, t5 = 25, t6 = 26, t7 = 27, t8 = 28, t9 = 29, tX = 30;
const int f1 = 31, f2 = 32, f3 = 33, f4 = 34, f5 = 35, f6 = 36, f7 = 37, f8 = 38, f9 = 39, fX = 40;
const int fechado_destrancado = 50, aberto_destrancado = 60;
 

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
  
  if(porta == HIGH){
    analogWrite(servo, sinal_fecha);
    currentState = estado_inicial;
  }
  else if (porta == LOW){
    currentState = aberto_destrancado;
  }
  
}


void loop()
{
  
  switch(currentState){
    
  case estado_inicial: 
    //digitalWrite(LED_R, HIGH);
    for ( int i=1; i<6; i++){    //CONFERIR ALGUMAS VEZES;
      if ((i != senha[0] ) && (i != senha_m[0]) && (digitalRead(i) == HIGH)){
        digitalWrite(LED_B, HIGH);
        currentState = senha_errada;       
      }
      else if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        if (i == senha[0]) currentState = a1;
        else currentState = m1;
        delay(trans);       //evitar multiplas entradas indesejadas
        ti = millis();
      }
      else digitalWrite(LED_B, LOW);
    }    
    break;
    
  case senha_errada:
    delay(temp);
    currentState = estado_inicial;
    break;
    
  case fechado_destrancado:
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    if (digitalRead(porta) == LOW){
      currentState = aberto_destrancado;
      break;
    }
    for ( int i=1; i<6; i++){       //melhorar se der tempo
      if ((i != senha[0] ) && (digitalRead(i) == HIGH)){
        digitalWrite(LED_B, HIGH);   
      }
      else if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        currentState = f1;
        delay(trans);       //evitar multiplas entradas indesejadas
        ti = millis();
      }
      else digitalWrite(LED_B, LOW);
    }
    break;
  
  case aberto_destrancado:
    if (digitalRead(porta) == HIGH){
      currentState = fechado_destrancado;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(0, i);
        currentState = t1;
        delay(trans);
        ti = millis();
      }
      else{
        digitalWrite(LED_B, LOW);
      }
    }
    break;
 
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ESTADOS INICIAIS    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
    
  case a1:
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      break;
    }
    if (senha[1] == -1){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      analogWrite(servo, sinal_abre);
      break;
    }
    for ( int i=1; i<6; i++){
      if ((i != senha[1] ) && ( digitalRead(i) == HIGH)){         //recebe a entrada, verifica se e igual a senha e redireciona para 'senha_errada'ou ao proximo estado
        digitalWrite(LED_B, HIGH);
        currentState = senha_errada;

      } 
      else if (digitalRead(i) == HIGH){
        currentState = a2;
        digitalWrite(LED_B, HIGH);       //indica ativacao da chave magnetica
        delay(trans);
        ti = millis();
      }
      else digitalWrite(LED_B, LOW);
    }
    break;
   
  case m1:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=1; i<6; i++){
      if ((i != senha_m[1] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada;
        digitalWrite(LED_B, HIGH);
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m2;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
      }
      else digitalWrite(LED_B, LOW);
    }
    break;
  
  case t1:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      break;
    }
    
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(1, i);
        delay(trans);
        currentState = t2;
        ti = millis();
      }
      else{
        digitalWrite(LED_B, LOW);
      }
    }
    break;
 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ESTADOS DO MEIO    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ULTIMOS ESTADOS    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//    
  case aX:
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      break;
    }
    if (senha[10] == -1){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      analogWrite(servo, sinal_abre);
      break;
    }
    for ( int i=1; i<6; i++){
      if ((i != senha[10] ) && ( digitalRead(i) == HIGH)){         //recebe a entrada, verifica se e igual a senha e redireciona para 'senha_errada'ou ao proximo estado
        digitalWrite(LED_B, HIGH);
        currentState = senha_errada;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = fechado_destrancado;
        digitalWrite(LED_B, HIGH);       //indica ativacao da chave magnetica
        delay(trans);
      }
      else digitalWrite(LED_B, LOW);
    }
    break;
    
  case mX:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=1; i<6; i++){
      if ((i != senha_m[1] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada;
        digitalWrite(LED_B, HIGH);
      } 
      else if (digitalRead(i) == HIGH){
        currentState = fechado_destrancado;
        digitalWrite(LED_B, HIGH);
        delay(trans);
      }
      else digitalWrite(LED_B, LOW);
    }
    break;
    
  case tX:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      break;
    }
    
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(10, i);
        delay(trans);
        currentState = estado_inicial;
      }
      else{
        digitalWrite(LED_B, LOW);
      }
    }
    break;
    



  }  //SWITCH
    
  
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@// 
}
