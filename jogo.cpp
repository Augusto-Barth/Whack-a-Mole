#include <LiquidCrystal.h>
#include <EEPROM.h>

const int rs = 3, en = 2, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float tempoParaApertar; // Tempo para a pessoa clicar em algum botão
float tempoEntreLeds; // Tempo entre cada LED
float difApertar; // coeficiente de dificuldade, altera o tempo correspondente por uma porcentagem (0 - 1)
float difEntre;
byte pontuacao = 0; // numero de LEDs acertados, reseta quando perde / vai de 0 a 255
int difAtual; // 0 facil / 1 medio / 2 dificil


// Coloca os pinos nos modos corretos
void setup(){

  pinMode(13, OUTPUT); // led1
  pinMode(12, OUTPUT); // led2
  pinMode(11, OUTPUT); // led3
  pinMode(10, OUTPUT); // led4
  pinMode(9, OUTPUT); // led5

  pinMode(8, INPUT); // botao1
  pinMode(7, INPUT); // botao2
  pinMode(6, INPUT); // botao3
  pinMode(5, INPUT); // botao4
  pinMode(4, INPUT); // botao5

  pinMode(A0, INPUT); // seletorDeDificuldade
  pinMode(A1, INPUT); // botaoComecar

  pinMode(1, OUTPUT); // Piezo

  randomSeed(analogRead(A0));

  lcd.begin(16, 2);
  comeco();
}


// loop de jogo
void loop(){

  // define um led aleatorio e liga ele
  int ledAtual = ledAleatorio();
  ligaLed(ledAtual);

  // vai para a função loop "acertou", que verifica se o botão correto foi pressionado
  if(acertou(ledAtual)){ // botao correto pressionado a tempo
  	desligaLed(ledAtual);

    tone(1, 240, 12); // som que toca a cada acerto

    lcd.setCursor(0, 1);
    lcd.print(pontuacao);

    tempoEntreLeds = tempoEntreLeds * difEntre;
    tempoParaApertar = tempoParaApertar * difApertar;
    delay(tempoEntreLeds);

  }else{ // botao incorreto pressionado ou tempo esgotado
    lcd.clear();
    lcd.setCursor(0, 0);
	lcd.print("Perdeu!");

    lcd.setCursor(0, 1);
	lcd.print("Pontuacao:");
    lcd.print(pontuacao);

    desligaLed(ledAtual);

    musicaDerrota();

    switch(difAtual){
      case 0:
        verifPont(0);
        break;
      case 1:
        verifPont(1);
        break;
      case 2:
        verifPont(2);
        break;
    }

    comeco();
    }

}


// Verifica se a pontuação supera a maior pontuação já registrada
void verifPont(int dif){

  if(EEPROM.read(dif) < pontuacao){
    EEPROM.write(dif, pontuacao);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Maior Pontuacao!");

    lcd.setCursor(0, 1);
    lcd.print(pontuacao);
  }
    delay(2000);
}


// Loop que verifica a dificuldade selecionada e se o botão iniciar foi pressionado
void comeco(){

  bool comecar = false;
  int hs = EEPROM.read(difAtual);
  int analogDif = analogRead(A0);
  int botaoComecar = digitalRead(A1);
  pontuacao = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Highscore:  ");
  lcd.setCursor(11, 0);
  lcd.print(hs);

  lcd.setCursor(0, 1);
  lcd.print("Dificul.:");

  while(!comecar){

    analogDif = analogRead(A0);
  	botaoComecar = digitalRead(A1);

    if(analogDif <= 341){
		difAtual = 0;

      	lcd.setCursor(0, 1);
  		lcd.print("Dificul.:Facil  ");

    }else if(analogDif <= 642 && analogDif > 341){
      	difAtual = 1;

      	lcd.setCursor(0, 1);
  		lcd.print("Dificul.:Medio  ");

    }else if(analogDif > 642){
      	difAtual = 2;

      	lcd.setCursor(0, 1);
  		lcd.print("Dificul.:Dificil");
     }


    if(botaoComecar ==  HIGH){

        switch(difAtual){
          case 0:
            tempoParaApertar = 2500;
            tempoEntreLeds = 2000;
            difApertar = 0.97;
            difEntre = 0.97;
            break;
          case 1:
            tempoParaApertar = 2000;
            tempoEntreLeds = 1500;
            difApertar = 0.95;
            difEntre = 0.95;
            break;
          case 2:
            tempoParaApertar = 1500;
            tempoEntreLeds = 1250;
            difApertar = 0.92;
            difEntre = 0.92;
            break;
        }

    	break;

    }else{
      	hs = EEPROM.read(difAtual);
      	lcd.setCursor(11, 0);
      	lcd.print(hs);
      	lcd.print("  ");

    	delay(50);
    }
  }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pontuacao:");

    lcd.setCursor(0, 1);
    lcd.print(pontuacao);

}


// verifica se a pessoa acertou o led ou não
int acertou(int ledAtual){

  int botaoApertado = 0;
  int acertouLed = 0;
  int i = 0;

  while((i < tempoParaApertar) and (!botaoApertado)){

    int botao = botaoPressionado();

    if(botao == ledAtual){
      acertouLed = 1;
      botaoApertado = 1;
      pontuacao++;

    } else if(botao != 0){
        acertouLed = 0;
        botaoApertado = 1;
      }

   i++;
   delay(1);
  }
  return acertouLed;
}


// verifica qual botão foi pressionado
int botaoPressionado(){

  int botao = 0;

  if (digitalRead(8) == HIGH){
    botao = 13;
  }
  else if (digitalRead(7) == HIGH){
    botao = 12;
  }
  else if (digitalRead(6) == HIGH){
    botao = 11;
  }
  else if (digitalRead(5) == HIGH){
    botao = 10;
  }
  else if (digitalRead(4) == HIGH){
    botao = 9;
  }
  return botao;

}


// seleciona um led aleatorio
int ledAleatorio(){

    return random(9, 14);
}


// liga um led
void ligaLed(int numLed){

  switch(numLed){
    case 13:
      digitalWrite(13, HIGH);
      break;
    case 12:
      digitalWrite(12, HIGH);
      break;
    case 11:
      digitalWrite(11, HIGH);
      break;
    case 10:
      digitalWrite(10, HIGH);
      break;
    case 9:
      digitalWrite(9, HIGH);
      break;
  }
}


// desliga um led
void desligaLed(int numLed){

  switch(numLed){
    case 13:
      digitalWrite(13, LOW);
      break;
    case 12:
      digitalWrite(12, LOW);
      break;
    case 11:
      digitalWrite(11, LOW);
      break;
    case 10:
      digitalWrite(10, LOW);
      break;
    case 9:
      digitalWrite(9, LOW);
      break;
  }
}


void musicaDerrota(){
    tone(1, 311);
    delay(200);
    noTone(1);
    tone(1, 263);
    delay(200);
    noTone(1);
    tone(1, 220);
    delay(200);
    noTone(1);
}

