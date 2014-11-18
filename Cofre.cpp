blabla

#include "Arduino.h"
#include "Cofre.h"

/*
Biblioteca criada para simplificar e melhorar a visualizacao no arquivo principal
*/


Cofre::Cofre(int chave_A, int chave_B, int chave_C, int chave_D, int chave_E, int LED_R, int LED_G, int LED_B, int temp)
{
  pinMode(LED_R, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(chave_A, INPUT);
  pinMode(chave_B, INPUT);
  pinMode(chave_C, INPUT);
  pinMode(chave_D, INPUT);
  pinMode(chave_E, INPUT);
  _chave_A = chave_A;
  _chave_B = chave_B;
  _chave_C = chave_C;
  _chave_D = chave_D; 
  _chave_E = chave_E;
  _LED_R = LED_R; 
  _LED_G = LED_G; 
  _LED_B = LED_B;
  _temp = temp;
}

void Cofre::acende_led(int cor)    //Funcao que acende o LED com a cor indicada no argumento, apagando os outros
{
  if (cor == 11){   //LED vermelho
    digitalWrite(_LED_G, LOW);
    digitalWrite(_LED_B, LOW);
    digitalWrite(_LED_R, HIGH);
  }
  else{
    if (cor == 13){   //LED azul
      digitalWrite(_LED_G, LOW);
      digitalWrite(_LED_R, LOW);
      digitalWrite(_LED_B, HIGH);
    }
    else{  //LED verde;
      digitalWrite(_LED_R, LOW);
      digitalWrite(_LED_B, LOW);
      digitalWrite(_LED_G, HIGH);
    }
  }
}

void Cofre::apaga_led() //Apaga todos os LEDs
{
  digitalWrite(_LED_R, LOW);
  digitalWrite(_LED_G, LOW);
  digitalWrite(_LED_B, LOW);
}

int Cofre::senha_errada(int estado)   //Estado 'senha_errada' da maquina de estados
{
  unsigned long ti = millis();
  while (millis() - ti < _temp){
    acende_led(_LED_R);
    delay(100);
    acende_led(_LED_G);
    delay(100);
    acende_led(_LED_B);
    delay(100);
  }
  if(estado < 21) return 0;
  else return 50;
}

int Cofre::leia_chave()     //Le uma chave ativada, ou retorna 0
{
  if(digitalRead(_chave_A) == HIGH) return _chave_A;
  else{
    if(digitalRead(_chave_B) == HIGH) return _chave_B;
    else{
      if(digitalRead(_chave_C) == HIGH) return _chave_C;
      else{
        if(digitalRead(_chave_D) == HIGH) return _chave_D;
        else{
          if (digitalRead(_chave_E) == HIGH) return _chave_E;
        }
      }
    }
  }
  return 0;
}
