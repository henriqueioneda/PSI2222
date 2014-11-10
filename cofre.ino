#include <EEPROM.h>
#include <Servo.h>
#include <Cofre.h>

#define N 11    //Numero maximo de digitos da senha
#define temp 4000     //Tempo de cancelamento
#define sinal_abre 45    //Angulacao do servo para destravar a porta e
#define sinal_fecha 125    //para travar a porta
#define trans 700      //Tempo de transicao, com finalidade de evitar multiplas entradas sucessivas indesejadas;

int LED_R = 11;
int LED_G = 12;
int LED_B = 13;

int chave_A = 6;
int chave_B = 2;
int chave_C = 3;
int chave_D = 4;
int chave_E = 5;
int porta = 10;
int chave_ativada = 0;
bool igual = true;
int cont = 0;

Cofre Cofre(chave_A, chave_B, chave_C, chave_D, chave_E, LED_R, LED_G, LED_B, temp);
Servo servo;

int senha[N], senha_m[N];    //Armazenamento da senha do usuario e da senha mestra
                             //OBS: a senha do usuario vai ter digitos do conjunto {1,2,3,4,5} e, caso acabe antes do numero maximo, sera sinalizada com '-1', ou '255'                             
unsigned long ti;            //Variavel para armazenar tempo, que sera constantemente comparada com millis() a fim de obter (millis() - ti), e saber se essa diferenca e maior que 'temp'     

int currentState;
const int estado_inicial = 0;
const int a1 = 1, a2 = 2, a3 = 3, a4 = 4, a5 = 5, a6 = 6, a7 = 7, a8 = 8, a9 = 9, aX = 10; 
const int m1 = 11, m2 = 12, m3 = 13, m4 = 14, m5 = 15, m6 = 16, m7 = 17, m8 = 18, m9 = 19, mX = 20;
const int t1 = 21, t2 = 22, t3 = 23, t4 = 24, t5 = 25, t6 = 26, t7 = 27, t8 = 28, t9 = 29, tX = 30;
const int f1 = 31, f2 = 32, f3 = 33, f4 = 34, f5 = 35, f6 = 36, f7 = 37, f8 = 38, f9 = 39, fX = 40;
const int fechado_destrancado = 50, aberto_destrancado = 60;
 
void setup()
{    
  servo.attach(9);       //Porta com PWM
  pinMode(LED_R, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(chave_A, INPUT);
  pinMode(chave_B, INPUT);
  pinMode(chave_C, INPUT);
  pinMode(chave_D, INPUT);
  pinMode(chave_E, INPUT);
  pinMode(porta, INPUT);
  
  for (int k=0; k<N; ++k){
    senha[k] = EEPROM.read(k);       //Recebimento das senhas armazenadas na EEPROM
    senha_m[k] = EEPROM.read(k+30);   //As senhas sao armazenadas linearmente: de 0 a N-1 esta a senha do usuario e de N a 2N -1, a senha mestra
  }
 
  if(digitalRead(porta) == HIGH){       //Significa que a porta esta encostada (fechada) no inicio
    servo.write(sinal_fecha);     //Sinal para o servo girar para a posicao que trava a porta       
    currentState = estado_inicial;    //Nesse caso, ha um direcionamento para o estado_inicial
  }
  else{
    servo.write(sinal_abre);
    currentState = aberto_destrancado;   //No outro caso, a porta esta aberta e ha um direcionamento para o estado aberto_destrancado
  }
}

void loop()
{
  
  switch(currentState){
  
  case estado_inicial:     //Estado estacionario; aguarda a entrada de uma das senhas
    Cofre.acende_led(LED_R);     //O LED vermelho fica aceso durante o tempo em que o cofre fica trancado
    chave_ativada = Cofre.leia_chave();
    cont = 0;
    igual = true;

    if(senha[0] != senha_m[0]) igual = false;  //Acho que aqui é desnecessário essa verificação.

    if((chave_ativada == senha[0]) && (igual == false)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a1;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[0]){     //Analogamente, o processo ocorre para o caso da senha mestra
        Cofre.acende_led(LED_B);
        currentState = m1;    //Proximo estado da senha mestra
        delay(trans);
        ti = millis();
        break;
      }
      else{
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }
    }
    break;
    
  case fechado_destrancado:     //Estado estacionario; aguarda o trancamento atraves da insercao da senha do usuario ou a abertura da porta
    cont = 0;
    igual = true;
    Cofre.acende_led(LED_G);        //O LED verde fica aceso durante o tempo em que o cofre esta aberto
    if (digitalRead(porta) == LOW){         //Transicao para o estado aberto_destrancado caso a porta seja aberta
      currentState = aberto_destrancado;
      break;
    }   
    chave_ativada = Cofre.leia_chave();
    if(chave_ativada == senha[0]){      //Comeca o processo de trancamento do cofre
      Cofre.acende_led(LED_B);
      currentState = f1;    //Redirecionamento para o proximo estado de trancamento
      delay(trans);
      ti = millis();
      break;
    }
    else{
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }   
    break;
  
  case aberto_destrancado:      //Estado estacionario; aguarda o inicio do processo de troca de senha ou o fechamento da porta
    Cofre.apaga_led();
    if (digitalRead(porta) == HIGH){       //Caso a porta seja fechada
      currentState = fechado_destrancado;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if(chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(0, chave_ativada);
      senha[0] = chave_ativada;
      currentState = t1;
      delay(trans);
      ti = millis();
    }
    break;
 
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ESTADOS INICIAIS    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//    
  case a1:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[1] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[1]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a2;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;
    
  case f1:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     //Caso o tempo de execucao do estado seja maior que 'temp', o usuario sera redirecionado para o inicio do processo de trancamento
      currentState = fechado_destrancado;
      break;
    }
    if (senha[1] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);     //Sinal para o servo trancar o cofre
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[1]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = f2;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;
     
  case m1:                 //Processo analogo aos (a1,...,aX), apenas com o processo de comparacao com o vetor senha_m
    Cofre.acende_led(LED_R);     //OBS: Como a senha mestra tem sempre o numero maximo de digitos, nao e necessario verificar o fim dela
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[1] != senha_m[1]){
      if(cont == 0) cont = 1;
      if((senha[1] == 255)&&igual) currentState = a1;
      igual = false;
    }

    if((chave_ativada == senha[1]) && (igual == false) && (cont == 1)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a2;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[1]){      //Continuacao do processo de abertura do cofre
        Cofre.acende_led(LED_B);
        currentState = m2;
        delay(trans);
        ti = millis();
        break;
      }
      else{         //Redirecionamento para o estado 'senha_errada'
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }    
    }
    break;
  
  case t1:  
    Cofre.apaga_led();
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){    //Caso o tempo de execucao ultrapasse 'temp' ou a porta seja fechada, considera-se que
      currentState = aberto_destrancado;                            //o usuario terminou de mudar sua senha
      EEPROM.write(1,255);
      senha[1] = 255;      //Sinalizacao do fim da senha
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(1, chave_ativada);
      senha[1] = chave_ativada;
      delay(trans);
      currentState = t2;
      ti = millis();
    }
    break;
 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ESTADOS DO MEIO    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

                                                  //Processos analogos aos dos estados (a1, f1, t1 e m1), com funcao de continuidade

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//                                                  
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    aK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
  case a2:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[2] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[2]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a3;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case a3:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[3] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[3]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a4;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case a4:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[4] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[4]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a5;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case a5:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[5] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[5]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a6;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case a6:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[6] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[6]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a7;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case a7:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[7] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[7]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a8;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case a8:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[8] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[8]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a9;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case a9:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[9] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[9]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = aX;
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//                                                  
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    mK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//    
  case m2:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[2] != senha_m[2]){
      if(cont == 0) cont = 2;
      if((senha[2] == 255)&& igual) currentState = a2;
      igual = false;
    }

    if((chave_ativada == senha[2]) && (igual == false) && (cont == 2)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a3;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[2]){      
        Cofre.acende_led(LED_B);
        currentState = m3;
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;

  case m3:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[3] != senha_m[3]){
      if(cont == 0) cont = 3;
      if((senha[3] == 255)&& igual) currentState = a3;
      igual = false;
    }

    if((chave_ativada == senha[3]) && (igual == false) && (cont == 3)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a4;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[3]){      
        Cofre.acende_led(LED_B);
        currentState = m4;
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;

  case m4:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[4] != senha_m[4]){
      if(cont == 0) cont = 4;
      if((senha[4] == 255)&& igual) currentState = a4;
      igual = false;
    }

    if((chave_ativada == senha[4]) && (igual == false) && (cont == 4)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a5;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[4]){      
        Cofre.acende_led(LED_B);
        currentState = m5;
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;

  case m5:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[5] != senha_m[5]){
      if(cont == 0) cont = 5;
      if((senha[5] == 255)&& igual) currentState = a5;
      igual = false;
    }

    if((chave_ativada == senha[5]) && (igual == false) && (cont == 5)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a6;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[5]){      
        Cofre.acende_led(LED_B);
        currentState = m6;
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;

  case m6:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[6] != senha_m[6]){
      if(cont == 0) cont = 6;
      if((senha[6] == 255)&& igual) currentState = a6;
      igual = false;
    }

    if((chave_ativada == senha[6]) && (igual == false) && (cont == 6)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a7;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[6]){      
        Cofre.acende_led(LED_B);
        currentState = m7;
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;

  case m7:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[7] != senha_m[7]){
      if(cont == 0) cont = 7;
      if((senha[7] == 255)&& igual) currentState = a7;
      igual = false;
    }

    if((chave_ativada == senha[7]) && (igual == false) && (cont == 7)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a8;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[7]){      
        Cofre.acende_led(LED_B);
        currentState = m8;
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;

  case m8:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[8] != senha_m[8]){
      if(cont == 0) cont = 8;
      if((senha[8] == 255)&& igual) currentState = a8;
      igual = false;
    }

    if((chave_ativada == senha[8]) && (igual == false) && (cont == 8)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a9;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[8]){      
        Cofre.acende_led(LED_B);
        currentState = m9;
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;

  case m9:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[9] != senha_m[9]){
      if(cont == 0) cont = 9;
      if((senha[9] == 255)&& igual) currentState = a9;
      igual = false;
    }

    if((chave_ativada == senha[9]) && (igual == false) && (cont == 9)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = aX;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[9]){      
        Cofre.acende_led(LED_B);
        currentState = mX;
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//                                                  
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    fK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//  case f2:  
  case f2:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[2] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[2]){ 
      Cofre.acende_led(LED_B);
      currentState = f3;
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case f3:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[3] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[3]){ 
      Cofre.acende_led(LED_B);
      currentState = f4;
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case f4:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[4] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[4]){ 
      Cofre.acende_led(LED_B);
      currentState = f5;
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case f5:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[5] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[5]){ 
      Cofre.acende_led(LED_B);
      currentState = f6;
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case f6:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[6] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[6]){ 
      Cofre.acende_led(LED_B);
      currentState = f7;
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case f7:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[7] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[7]){ 
      Cofre.acende_led(LED_B);
      currentState = f8;
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case f8:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[8] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[8]){ 
      Cofre.acende_led(LED_B);
      currentState = f9;
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

  case f9:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[9] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[9]){ 
      Cofre.acende_led(LED_B);
      currentState = fX;
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//                                                  
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    tK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//  case t2: 
  case t2:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(2,255);
      senha[2] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(2, chave_ativada);
      senha[2] = chave_ativada;
      delay(trans);
      currentState = t3;
      ti = millis();
    }
    break;

  case t3:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(3,255);
      senha[3] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(3, chave_ativada);
      senha[3] = chave_ativada;
      delay(trans);
      currentState = t4;
      ti = millis();
    }
    break;

  case t4:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(4,255);
      senha[4] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(4, chave_ativada);
      senha[4] = chave_ativada;
      delay(trans);
      currentState = t5;
      ti = millis();
    }
    break;

  case t5:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(5,255);
      senha[5] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(5, chave_ativada);
      senha[5] = chave_ativada;
      delay(trans);
      currentState = t6;
      ti = millis();
    }
    break;

  case t6:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(6,255);
      senha[6] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(6, chave_ativada);
      senha[6] = chave_ativada;
      delay(trans);
      currentState = t7;
      ti = millis();
    }
    break;

  case t7:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(7,255);
      senha[7] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(7, chave_ativada);
      senha[7] = chave_ativada;
      delay(trans);
      currentState = t8;
      ti = millis();
    }
    break;

  case t8:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(8,255);
      senha[8] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(8, chave_ativada);
      senha[8] = chave_ativada;
      delay(trans);
      currentState = t9;
      ti = millis();
    }
    break;

  case t9:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(9,255);
      senha[9] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(9, chave_ativada);
      senha[9] = chave_ativada;
      delay(trans);
      currentState = tX;
      ti = millis();
    }
    break;
    
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ULTIMOS ESTADOS    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
                                                     //Estados finais dos processos iniciados por (a1, f1, t1 e m1) e continuados por (aK, fK, tK e mK)

  case aX:  
    Cofre.acende_led(LED_R);
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      break;
    }
    if (senha[10] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[10]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = fechado_destrancado;
      delay(trans);
      servo.write(sinal_abre);
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;
    
  case fX:  
    Cofre.acende_led(LED_G);
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      break;
    }
    if (senha[10] == 255){          
      currentState = estado_inicial;
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[10]){ 
      Cofre.acende_led(LED_B);
      currentState = estado_inicial;
      delay(trans);
      servo.write(sinal_fecha); 
      break;
    }
    else{   
      if (chave_ativada > 0){
        Cofre.acende_led(LED_B);
        delay(trans);
        currentState = Cofre.senha_errada(currentState);
      }
    }    
    break;
        
  case mX:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      break;
    }

    chave_ativada = Cofre.leia_chave();
    if(senha[10] != senha_m[10]){
      if(cont == 0) cont = 10;
      if((senha[10] == 255)&& igual) currentState = aX;
      igual = false;
    }

    if((chave_ativada == senha[10]) && (igual == false) && (cont == 10)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = fechado_destrancado;        //Direcionamento para o proximo estado de senha do usuario
      delay(trans);
      servo.write(sinal_abre);     
      break;
    }
    else{
      if(chave_ativada == senha_m[10]){      
        Cofre.acende_led(LED_B);
        currentState = fechado_destrancado;
        delay(trans);
        servo.write(sinal_abre);
        break;
      }
      else{        
        if (chave_ativada > 0){
          Cofre.acende_led(LED_B);
          delay(trans);
          currentState = Cofre.senha_errada(currentState);
        }
      }  
    }  
    break;
    
  case tX:
    Cofre.apaga_led(); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
      EEPROM.write(10,255);
      senha[10] = 255;
      break;
    }
    chave_ativada = Cofre.leia_chave();
    if (chave_ativada > 0){
      Cofre.acende_led(LED_B);
      EEPROM.write(10, chave_ativada);
      senha[10] = chave_ativada;
      delay(trans);
      currentState = aberto_destrancado;
    }
    break;
  }  //Switch
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@// 
}
