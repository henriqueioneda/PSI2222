#include <EEPROM.h>
#include <Servo.h>
#include <Cofre.h> //ohhh

#define N 11    //Numero maximo de digitos da senha
#define temp 5000     //Tempo de cancelamento
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

Cofre Cofre(chave_A, chave_B, chave_C, chave_D, chave_E, LED_R, LED_G, LED_B);
Servo servo;

int senha[N], senha_m[N];    //Armazenamento da senha do usuario e da senha mestra (OBS: deve ser de conhecimento do usuario de que senha[0] != senha_m[0], ou seja,
                             //os primeiros digitos das duas senhas nao podem coincidir
                             //OBS: a senha do usuario vai ter digitos do conjunto {1,2,3,4,5} e, caso acabe antes do numero maximo, sera sinalizada com '-1', ou '255'                             
unsigned long ti;            //Variavel para armazenar tempo, que sera constantemente comparada com millis() a fim de obter (millis() - ti), e saber se essa diferenca e maior que 'temp'     

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
    senha_m[k] = EEPROM.read(k+N);   //As senhas sao armazenadas linearmente: de 0 a N-1 esta a senha do usuario e de N a 2N -1, a senha mestra
  }
 
  Serial.print("porta: ");
  Serial.println(digitalRead(porta));
  if(digitalRead(porta) == HIGH){       //Significa que a porta esta encostada (fechada) no inicio
    servo.write(sinal_fecha);     //Sinal para o servo girar para a posicao que trava a porta       
    currentState = estado_inicial;    //Nesse caso, ha um direcionamento para o estado_inicial
  }
  else{
    servo.write(sinal_abre);
    currentState = aberto_destrancado;   //No outro caso, a porta esta aberta e ha um direcionamento para o estado aberto_destrancado
  }
  //Serial.println(currentState);
}

void loop()
{
  
  switch(currentState){
  
  case estado_inicial:     //Estado estacionario; aguarda a entrada de uma das senhas
    Cofre.acende_led(LED_R);     //O LED vermelho fica aceso durante o tempo em que o cofre fica trancado
    Serial.println("Estado Inicial");
    chave_ativada = Cofre.leia_chave();
    Serial.println(chave_ativada);

    if(senha[0] != senha_m[0]){
      igual = false;
      cont = 0;
    }
    Serial.println(igual);

    if((chave_ativada == senha[0]) && (igual == false) && (cont == 0)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a1;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[0]){     //Analogamente, o processo ocorre para o caso da senha mestra
        Cofre.acende_led(LED_B);
        currentState = m1;    //Proximo estado da senha mestra
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{
        if (chave_ativada > 0){
          currentState = senha_errada;
          Serial.println("Senha Errada. Estado incial.");
          Cofre.acende_led(LED_B);
          delay(trans);
        }
      }
    }
    break;
    
  case senha_errada:      //Estado de espera ao inserir caractere errado
   // digitalWrite(LED_R, HIGH);     //Acender todos os LEDs como indicativo de erro de senha
    //digitalWrite(LED_G, HIGH);

    ti = millis();
    while (millis() - ti < temp){
      Cofre.acende_led(LED_R);
      delay(100);
      Cofre.acende_led(LED_G);
      delay(100);
      Cofre.acende_led(LED_B);
      delay(100);
    }
    cont = 0;
    igual = true;
    //delay(temp);
    currentState = estado_inicial;       //Retorna ao estado_inicial;
    break;
    
  case fechado_destrancado:     //Estado estacionario; aguarda o trancamento atraves da insercao da senha do usuario ou a abertura da porta
    cont = 0;
    igual = true;
    Cofre.acende_led(LED_G);        //O LED verde fica aceso durante o tempo em que o cofre esta aberto
    if (digitalRead(porta) == LOW){         //Transicao para o estado aberto_destrancado caso a porta seja aberta
      currentState = aberto_destrancado;
      Serial.println("Porta: ");
      Serial.print(digitalRead(porta));
      break;
    }   
    Serial.println("Estado fechado destrancado.");
    chave_ativada = Cofre.leia_chave();
    if(chave_ativada == senha[0]){      //Comeca o processo de trancamento do cofre
      Cofre.acende_led(LED_B);
      currentState = f1;    //Redirecionamento para o proximo estado de trancamento
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{
      if (chave_ativada > 0){
        currentState = senha_errada;
        Serial.println("Senha Errada. Fechado destrancado.");
        Cofre.acende_led(LED_B);
        delay(trans);
      }
    }   
    break;
  
  case aberto_destrancado:      //Estado estacionario; aguarda o inicio do processo de troca de senha ou o fechamento da porta
    Cofre.acende_led(LED_G);
    Serial.println("abriu sapoha");
    if (digitalRead(porta) == HIGH){       //Caso a porta seja fechada
      currentState = fechado_destrancado;
      Serial.println("Porta: ");
      Serial.print(digitalRead(porta));
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
    Serial.println("Estado a1.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[1] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[1]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a2;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a1");
      }
    }    
    break;
    
  case f1:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado f1.");
    if (millis()-ti > temp){     //Caso o tempo de execucao do estado seja maior que 'temp', o usuario sera redirecionado para o inicio do processo de trancamento
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[1] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);     //Sinal para o servo trancar o cofre
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[1]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = f2;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f1");
      }
    }    
    break;
     
  case m1:                 //Processo analogo aos (a1,...,aX), apenas com o processo de comparacao com o vetor senha_m
    Cofre.acende_led(LED_R);     //OBS: Como a senha mestra tem sempre o numero maximo de digitos, nao e necessario verificar o fim dela
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[1] != senha_m[1]){
      if(cont == 0) cont = 1;
      if((senha[1] == 255)&&igual) currentState = a1;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[1]) && (igual == false) && (cont == 1)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a2;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[1]){      //Continuacao do processo de abertura do cofre
        Cofre.acende_led(LED_B);
        currentState = m2;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{         //Redirecionamento para o estado 'senha_errada'
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m1");
        }
      }    
    }
    break;
  
  case t1:  
    Cofre.acende_led(LED_G);
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){    //Caso o tempo de execucao ultrapasse 'temp' ou a porta seja fechada, considera-se que
      currentState = aberto_destrancado;                            //o usuario terminou de mudar sua senha
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
    Serial.println("Estado a2.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[2] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[2]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a3;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a2");
      }
    }    
    break;

  case a3:  
    Cofre.acende_led(LED_R);
    Serial.println("Estado a3.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[3] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[3]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a4;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a3");
      }
    }    
    break;

  case a4:  
    Cofre.acende_led(LED_R);
    Serial.println("Estado a4.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[4] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[4]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a5;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a4");
      }
    }    
    break;

  case a5:  
    Cofre.acende_led(LED_R);
    Serial.println("Estado a5.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[5] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[5]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a6;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a5");
      }
    }    
    break;

  case a6:  
    Cofre.acende_led(LED_R);
    Serial.println("Estado a6.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[6] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[6]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a7;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a6");
      }
    }    
    break;

  case a7:  
    Cofre.acende_led(LED_R);
    Serial.println("Estado a7.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[7] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[7]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a8;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a7");
      }
    }    
    break;

  case a8:  
    Cofre.acende_led(LED_R);
    Serial.println("Estado a8.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[8] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[8]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = a9;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a8");
      }
    }    
    break;

  case a9:  
    Cofre.acende_led(LED_R);
    Serial.println("Estado a9.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[9] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[9]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = aX;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: a9");
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
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[2] != senha_m[2]){
      if(cont == 0) cont = 2;
      if((senha[2] == 255)&& igual) currentState = a2;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[2]) && (igual == false) && (cont == 2)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a3;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[2]){      
        Cofre.acende_led(LED_B);
        currentState = m3;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m2");
        }
      }  
    }  
    break;

  case m3:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[3] != senha_m[3]){
      if(cont == 0) cont = 3;
      if((senha[3] == 255)&& igual) currentState = a3;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[3]) && (igual == false) && (cont == 3)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a4;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[3]){      
        Cofre.acende_led(LED_B);
        currentState = m4;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m3");
        }
      }  
    }  
    break;

  case m4:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[4] != senha_m[4]){
      if(cont == 0) cont = 4;
      if((senha[4] == 255)&& igual) currentState = a4;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[4]) && (igual == false) && (cont == 4)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a5;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[4]){      
        Cofre.acende_led(LED_B);
        currentState = m5;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m4");
        }
      }  
    }  
    break;

  case m5:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[5] != senha_m[5]){
      if(cont == 0) cont = 5;
      if((senha[5] == 255)&& igual) currentState = a5;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[5]) && (igual == false) && (cont == 5)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a6;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[5]){      
        Cofre.acende_led(LED_B);
        currentState = m6;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m5");
        }
      }  
    }  
    break;

  case m6:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[6] != senha_m[6]){
      if(cont == 0) cont = 6;
      if((senha[6] == 255)&& igual) currentState = a6;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[6]) && (igual == false) && (cont == 6)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a7;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[6]){      
        Cofre.acende_led(LED_B);
        currentState = m7;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m6");
        }
      }  
    }  
    break;

  case m7:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[7] != senha_m[7]){
      if(cont == 0) cont = 7;
      if((senha[7] == 255)&& igual) currentState = a7;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[7]) && (igual == false) && (cont == 7)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a8;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[7]){      
        Cofre.acende_led(LED_B);
        currentState = m8;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m7");
        }
      }  
    }  
    break;

  case m8:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[8] != senha_m[8]){
      if(cont == 0) cont = 8;
      if((senha[8] == 255)&& igual) currentState = a8;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[8]) && (igual == false) && (cont == 8)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = a9;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[8]){      
        Cofre.acende_led(LED_B);
        currentState = m9;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m8");
        }
      }  
    }  
    break;

  case m9:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();

    if(senha[9] != senha_m[9]){
      if(cont == 0) cont = 9;
      if((senha[9] == 255)&& igual) currentState = a9;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[9]) && (igual == false) && (cont == 9)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = aX;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();      //Inicia-se um contador de tempo, a fim de testar se (millis() - ti) vai superar 'temp' no proximo estado
      break;
    }
    else{
      if(chave_ativada == senha_m[9]){      
        Cofre.acende_led(LED_B);
        currentState = mX;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: m9");
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
    Serial.println("Estado f2.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[2] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[2]){ 
      Cofre.acende_led(LED_B);
      currentState = f3;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f2");
      }
    }    
    break;

  case f3:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado f3.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[3] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[3]){ 
      Cofre.acende_led(LED_B);
      currentState = f4;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f3");
      }
    }    
    break;

  case f4:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado f4.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[4] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[4]){ 
      Cofre.acende_led(LED_B);
      currentState = f5;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f5");
      }
    }    
    break;

  case f5:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado f5.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[5] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[5]){ 
      Cofre.acende_led(LED_B);
      currentState = f6;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f5");
      }
    }    
    break;

  case f6:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado f6.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[6] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[6]){ 
      Cofre.acende_led(LED_B);
      currentState = f7;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f6");
      }
    }    
    break;

  case f7:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado f7.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[7] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[7]){ 
      Cofre.acende_led(LED_B);
      currentState = f8;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f7");
      }
    }    
    break;

  case f8:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado f8.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[8] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[8]){ 
      Cofre.acende_led(LED_B);
      currentState = f9;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f8");
      }
    }    
    break;

  case f9:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado f9.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[9] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[9]){ 
      Cofre.acende_led(LED_B);
      currentState = fX;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: f9");
      }
    }    
    break;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//                                                  
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    tK    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//  case t2: 
  case t2:
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
    Serial.println("Estado aX.");
    if (millis()-ti > temp){     //Verifica se o tempo de execucao do estado e maior que 'temp'
      currentState = estado_inicial;    //Em caso positivo, redirecionamento para estado_inicial
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[10] == 255){         //Verifica se a senha chegou ao fim (caso em que a senha possui menos digitos que o maximo permitido)
      currentState = fechado_destrancado;       //Redirecionamento para o estado em que o cofre esta destrancado e com a porta fechada
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_abre);        //O servo vai para a posicao em que destranca a porta
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[10]){      //Continuacao do processo de abertura do cofre
      Cofre.acende_led(LED_B);
      currentState = fechado_destrancado;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      break;
    }
    else{         //Redirecionamento para o estado 'senha_errada'
      if (chave_ativada > 0){
        currentState = senha_errada;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: aX");
      }
    }    
    break;
    
  case fX:  
    Cofre.acende_led(LED_G);
    Serial.println("Estado fX.");
    if (millis()-ti > temp){     
      currentState = fechado_destrancado;
      Serial.println("Deu 10 seg!");
      break;
    }
    if (senha[10] == 255){          
      currentState = estado_inicial;
      Serial.println("Senha Correta.");
      Serial.println(currentState);
      servo.write(sinal_fecha);  
      break;
    } 

    chave_ativada = Cofre.leia_chave();

    if(chave_ativada == senha[10]){ 
      Cofre.acende_led(LED_B);
      currentState = estado_inicial;
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);
      ti = millis();
      break;
    }
    else{   
      if (chave_ativada > 0){
        currentState = fechado_destrancado;
        Cofre.acende_led(LED_B);
        delay(trans);
        ti = millis();
        Serial.println("Senha errada: fX");
      }
    }    
    break;
        
  case mX:          
    Cofre.acende_led(LED_R);    
    if (millis()-ti > temp){   
      currentState = estado_inicial;
      Serial.println("Deu 10 seg!");
      break;
    }

    chave_ativada = Cofre.leia_chave();
    if(senha[10] != senha_m[10]){
      if(cont == 0) cont = 10;
      if((senha[10] == 255)&& igual) currentState = aX;
      igual = false;
    }
    Serial.println(igual);

    if((chave_ativada == senha[10]) && (igual == false) && (cont == 10)){     //Recebeu a senha do usuario
      Cofre.acende_led(LED_B);        //Acender o LED azul sempre significa que um caractere foi inserido
      currentState = fechado_destrancado;        //Direcionamento para o proximo estado de senha do usuario
      Serial.print("Foi para o Estado: ");
      Serial.println(currentState);
      delay(trans);     
      break;
    }
    else{
      if(chave_ativada == senha_m[10]){      
        Cofre.acende_led(LED_B);
        currentState = fechado_destrancado;
        Serial.print("Foi para o Estado: ");
        Serial.println(currentState);
        delay(trans);
        servo.write(sinal_abre);
        ti = millis();
        break;
      }
      else{        
        if (chave_ativada > 0){
          currentState = senha_errada;
          Cofre.acende_led(LED_B);
          delay(trans);
          ti = millis();
          Serial.println("Senha errada: mX");
        }
      }  
    }  
    break;
    
  case tX:
    Cofre.acende_led(LED_G); 
    if ( (millis()-ti > temp) || (digitalRead(porta) == HIGH) ){
      currentState = aberto_destrancado;
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
