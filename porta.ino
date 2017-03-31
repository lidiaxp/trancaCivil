#include <SPI.h>      //ainda precisa baixar essa bibliotecas
#include <MFRC522.h>  //link: http://blog.filipeflop.com/wireless/controle-acesso-leitor-rfid-arduino.html
#include <Keypad.h>   //link: http://www.arduinoecia.com.br/2015/05/teclado-matricial-membrana-4x3-arduino.html
#include <Ethernet.h>

#define SS_PIN 10
#define RST_PIN 9

//códigos abaixo são só exemplo
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,88); //endereço para acessar a página          
IPAddress gateway(192,168,1,1);     
IPAddress subnet(255, 255, 255, 0); 

MFRC522 mfrc522(SS_PIN, RST_PIN);  

int portaRele = 7;
int portaBotao = 23; //precisa de um resistor de 10k, a maioria
int buzzer = 24;

int estadoRele = 1;
int estadiBotao = 0;

int leituraRele = 0;
String cartao;
String chaveiro;
int count;
int senha[4];

char matriz_teclas[LINHAS][COLUNAS] = 
{
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

const byte LINHAS = 4;
const byte COLUNAS = 3;

byte pinosLinhas[LINHAS] = {14, 15, 16, 17};
byte pinosColunas[COLUNAS] = {18, 19, 20};

Keypad meuteclado = Keypad( makeKeymap(matriz_teclas), pinosLinhas, 
                            pinosColunas, LINHAS, COLUNAS); 

EthernetServer server(80); //porta que vai rodar o servidor

/*
 * Nos loops abre e fecha porta pode ser ao contrario
 * preciso testar com o rele para descbrir
 * 
 * O código ta todo como debug ainda
 */

 /*
  * Esse é um programa especificamente para dusa tags
  * se for espandir precisa alterar o código
  */

void setup() {
  Serial.begin(9600);   
  
  SPI.begin();      
  mfrc522.PCD_Init();
  
  pinMode(porta_rele, OUTPUT); 
  pinMode(buzzer, OUTPUT);
  pinMode(portaBotao, INPUT);
    
  digitalWrite(portaRele, HIGH);
  
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
}

void loop() {
    estadoBotao = digitalRead(portaBotao);
    if(estadoBotao){
      lerRFID();
      cadastroCartao();
      cadastroChaveiro();
      checarRFID();
      inserirDigito();
      checarSenha();
    } else{ fechaPorta();}
}

void inserirDigito(){
  char tecla_pressionada = meuteclado.getKey();
    if (tecla_pressionada){
       putSenha(tecla_pressionada);
  }
}

void checarSenha(){
  if(count == 4){ //definir sua senha aki
    if(senha[0] == 3 && senha[1] == 5 && senha[2] == 7 && senha[3] == 0){
      abrePorta;
    }else{
      fechaPorta();
    }
    count = 0;
  }
}

void putSenha(int n){
  if(count == 0){
        senha[count] = n;
      }
      if(count == 1){
        senha[count] = n;
      }
      if(count == 2){
        senha[count] = n;
      }
      if(count == 3){
        senha[count] = n;
      }
     delay(500); 
     count++;
}

void abrePorta(){
  digitalWrite(portaRele, HIGH);
  Serial.println("Acesso liberado");
}

void fechaPorta(){
  digitalWrite(portaRele, LOW);
  Serial.println("Acesso Negado");
  digitalWrite(buzzer, HIGH);
  delay(300);
  digitalWrite(buzzer, LOW);
}

void cadastroCartao(){
  for (byte i = 0; i < mfrc522.uid.size; i++){
     Serial.print("UIDcartão: ");
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     cartao.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     cartao.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  cartao.toUpperCase(); //talvez necessite de uma aux para isso
}

void cadastroChaveiro(){
  for (byte i = 0; i < mfrc522.uid.size; i++){
     Serial.print("UIDcartão: ");
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     chaveiro.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     chaveiro.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  chaveiro.toUpperCase(); //talvez necessite de uma aux para isso
}

void checarRFID(){
  if (cartao.substring(1) == "UID que aparecer no cartao" | chaveiro.substring(1) == "UID que aparecer no chaveiro") {
    abrePorta();
  } else{
    fechaPorta();  
  }
}

void lerRFID(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
 
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }  
}
