#include <EEPROM.h>
#include <Servo.h>
#define N 10
#define temp 5000
#define sinal_abre 45 //A DEFINIR;
#define sinal_fecha 125 //A DEFINIR;
#define trans 700      //ajustar;

int LED_R = 11;
int LED_G = 12;
int LED_B = 13;

int chave_A = 6;  //Mudar os pinos se for usar o serial.
int chave_B = 2;
int chave_C = 3;
int chave_D = 4;
int chave_E = 5;
int porta = 7;

Servo servo;
int senha[N], senha_m[N];

unsigned long ti;

int currentState;

const int senha_errada = 100;
const int estado_inicial = 0;
const int a1 = 1, a2 = 2, a3 = 3, a4 = 4, a5 = 5, a6 = 6, a7 = 7, a8 = 8, a9 = 9, aX = 10; 
const int m1 = 11, m2 = 12, m3 = 13, m4 = 14, m5 = 15, m6 = 16, m7 = 17, m8 = 18, m9 = 19, mX = 20;
const int t1 = 21, t2 = 22, t3 = 23, t4 = 24, t5 = 25, t6 = 26, t7 = 27, t8 = 28, t9 = 29, tX = 30;
const int f1 = 31, f2 = 32, f3 = 33, f4 = 34, f5 = 35, f6 = 36, f7 = 37, f8 = 38, f9 = 39, fX = 40;
const int fechado_destrancado = 50, aberto_destrancado = 60;
 

void setup()
{  
  
  Serial.begin(9600);
  
  servo.attach(9);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(chave_A, INPUT);
  pinMode(chave_B, INPUT);
  pinMode(chave_C, INPUT);
  pinMode(chave_D, INPUT);
  pinMode(chave_E, INPUT);
  pinMode(porta, INPUT);
  
  for (int k=0; k<N; k++){
    senha[k] = EEPROM.read(k);
    senha_m[k] = EEPROM.read(k+10);
  }
 
  Serial.print("porta:");
  Serial.println(digitalRead(porta));
  if(digitalRead(porta) == HIGH){
    servo.write(sinal_fecha);          
    currentState = estado_inicial;
  }
  else{
    currentState = aberto_destrancado;
  }
  //Serial.println(currentState);
}


void loop()
{
  
  switch(currentState){
  
  case estado_inicial:     //estado estacionario
    
    Serial.println("Estado Inicial");
    if(digitalRead(senha[0]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a1;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == estado_inicial){
      for(int i=2; i<7; i++){
          if ((i != senha[0] ) && (i != senha_m[0]) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado incial.");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case senha_errada:
  
    digitalWrite(LED_B, LOW);
    delay(temp);
    currentState = estado_inicial;
    break;
    
  case fechado_destrancado:     //estado estacionario
  
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    //digitalWrite(LED_B, LOW);

    if (digitalRead(porta) == LOW){
      currentState = aberto_destrancado;
      break;
    }
    
    Serial.println("Estado fechado destrancado.");
    if(digitalRead(senha[0]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f1;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f1){
      for(int i=2; i<7; i++){
          if ((i != senha[0] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            Serial.println("Senha Errada. Estado: fechado destrancado");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
  
  case aberto_destrancado:      //estado estacionario
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);

    Serial.println("abriu sapoha");
    if (digitalRead(porta) == HIGH){
      currentState = fechado_destrancado;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(0, i);
        senha[0] = i;
        currentState = t1;
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
 
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ESTADOS INICIAIS    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
    
  case a1:
  
    Serial.println("Estado a1.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[1] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    } 
    if(digitalRead(senha[1]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a2;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a1){
      for(int i=2; i<7; i++){
          if ((i != senha[1] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a1");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case f1:
  
    Serial.println("Estado f1.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[1] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    } 
    if(digitalRead(senha[1]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f2;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f1){
      for(int i=2; i<7; i++){
          if ((i != senha[1] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f1");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
  
  case m1:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[1] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; 
        Serial.println("senha errada - m1");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m2;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case t1:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[1] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(1, i);
        senha[1] = i;
        delay(trans);
        currentState = t2;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ESTADOS DO MEIO    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    aK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
  case a2:
  
    Serial.println("Estado a2.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[1] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[2]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a3;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a2){
      for(int i=2; i<7; i++){
          if ((i != senha[2] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a2");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case a3:
  
    Serial.println("Estado a3.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[1] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[3]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a4;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a3){
      for(int i=2; i<7; i++){
          if ((i != senha[3] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a3");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case a4:
  
    Serial.println("Estado a4.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[1] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[4]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a5;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a4){
      for(int i=2; i<7; i++){
          if ((i != senha[4] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a4");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case a5:
  
    Serial.println("Estado a5.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[5] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[5]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a6;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a5){
      for(int i=2; i<7; i++){
          if ((i != senha[5] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a5");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case a6:
  
    Serial.println("Estado a6.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[6] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[6]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a7;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a6){
      for(int i=2; i<7; i++){
          if ((i != senha[6] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a6");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case a7:
      
    Serial.println("Estado a7.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[7] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[7]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a8;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a7){
      for(int i=2; i<7; i++){
          if ((i != senha[7] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a7");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case a8:
      
    Serial.println("Estado a8.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[8] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[8]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = a9;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a8){
      for(int i=2; i<7; i++){
          if ((i != senha[8] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a8");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case a9:
      
    Serial.println("Estado a9.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[9] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[9]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = aX;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == a9){
      for(int i=2; i<7; i++){
          if ((i != senha[9] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a9");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    mK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
 
  case m2:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[2] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - m2");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m3;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case m3:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[3] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - m3");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m4;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case m4:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[4] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - m4");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m5;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case m5:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[5] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - m5");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m6;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case m6:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[6] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - m6");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m7;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case m7:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[7] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - m7");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m8;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case m8:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[8] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - m8");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = m9;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case m9:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[9] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - m9");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = mX;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    fK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
    
  case f2:
  
    Serial.println("Estado f2.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[2] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[2]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f3;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f2){
      for(int i=2; i<7; i++){
          if ((i != senha[2] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f2");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case f3:
  
    Serial.println("Estado f3.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[3] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[3]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f4;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f3){
      for(int i=2; i<7; i++){
          if ((i != senha[3] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f3");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case f4:
  
    Serial.println("Estado f4.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[4] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[4]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f5;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f4){
      for(int i=2; i<7; i++){
          if ((i != senha[4] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f4");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case f5:
  
    Serial.println("Estado f5.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[5] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[5]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f6;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f5){
      for(int i=2; i<7; i++){
          if ((i != senha[5] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f5");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case f6:
  
    Serial.println("Estado f6.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[6] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[6]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f7;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f6){
      for(int i=2; i<7; i++){
          if ((i != senha[6] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f6");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case f7:
      
    Serial.println("Estado f7.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[7] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[7]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f8;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f7){
      for(int i=2; i<7; i++){
          if ((i != senha[7] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f7");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case f8:
      
    Serial.println("Estado f8.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[8] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[8]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = f9;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f8){
      for(int i=2; i<7; i++){
          if ((i != senha[8] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f8");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case f9:
      
    Serial.println("Estado f9.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[9] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[9]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = fX;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else if(currentState == f9){
      for(int i=2; i<7; i++){
          if ((i != senha[9] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: f9");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    tK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
  
  case t2:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[2] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(2, i);
        senha[2] = i;
        delay(trans);
        currentState = t3;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case t3:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[3] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(3, i);
        senha[3] = i;
        delay(trans);
        currentState = t4;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case t4:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[4] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(4, i);
        senha[4] = i;
        delay(trans);
        currentState = t5;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case t5:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[5] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(5, i);
        senha[5] = i;
        delay(trans);
        currentState = t6;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case t6:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[6] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(6, i);
        senha[6] = i;
        delay(trans);
        currentState = t7;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case t7:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[7] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(7, i);
        senha[7] = i;
        delay(trans);
        currentState = t8;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case t8:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[8] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(8, i);
        senha[8] = i;
        delay(trans);
        currentState = t9;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  
  case t9:
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      senha[9] = 255;
      break;
    }
    for (int i=1; i<6; ++i){
      if (digitalRead(i) == HIGH){
        digitalWrite(LED_B, HIGH);
        EEPROM.write(9, i);
        senha[9] = i;
        delay(trans);
        currentState = tX;
        ti = millis();
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ULTIMOS ESTADOS    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//    
  case aX:
  
      
    Serial.println("Estado aX.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[10] == 255){         //verifica se a senha terminou
      currentState = fechado_destrancado;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);
      break;
    }
    
    if(digitalRead(senha[10]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = fechado_destrancado;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      break;
    }
    else if(currentState == a9){
      for(int i=2; i<7; i++){
          if ((i != senha[9] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = senha_errada;
            Serial.println("Senha Errada. Estado: a9");
            servo.write(sinal_abre);
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case fX:
  
    if (digitalRead(porta) == LOW){
      currentState = aberto_destrancado;
      break;
    }
      
    Serial.println("Estado fX.");
    if (millis()-ti > temp){     //verifica se esta no estado ha mais que 'temp'
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[10] == 255){         //verifica se a senha terminou
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      break;
    }
    
    if(digitalRead(senha[10]) == HIGH){
      digitalWrite(LED_B, HIGH);
      currentState = estado_inicial;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      servo.write(sinal_fecha);
      digitalWrite(LED_G, LOW);
      delay(trans);
      break;
    }
    else if(currentState == fX){
      for(int i=2; i<7; i++){
          if ((i != senha[10] ) && (digitalRead(i) == HIGH)){
            digitalWrite(LED_B, HIGH);
            currentState = fechado_destrancado;
            Serial.println("Senha Errada. Estado: fX");
            delay(trans);
            break;
          }
      }
    }
    digitalWrite(LED_B, LOW);
    break;
    
  case mX:
    if (millis()-ti > temp){
      currentState = estado_inicial;
      break;
    }
    for ( int i=2; i<7; i++){
      if ((i != senha_m[1] ) && ( digitalRead(i) == HIGH)){
        currentState = senha_errada; Serial.println("senha errada - mX");
        digitalWrite(LED_B, HIGH);
        break;
      } 
      else if (digitalRead(i) == HIGH){
        currentState = fechado_destrancado;
        digitalWrite(LED_B, HIGH);
        delay(trans);
        servo.write(sinal_abre);
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
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
        senha[10] = i;
        delay(trans);
        currentState = estado_inicial;
        break;
      }
      else{
        digitalWrite(LED_B, LOW);
        break;
      }
    }
    break;
  }  //SWITCH  
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@// 
}
