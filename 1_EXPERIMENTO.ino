//BIBLIOTECAS
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Servo.h>
#include <Keypad.h>
#include <avr/wdt.h> //RESET ARDUINO
#include <SoftwareSerial.h>

//DEFINICAO DE SENSORES/ATUADORES E PINOS
//const boolean sensor[] = {22, 24, 26, 28, 30, 32};//DEFINIR PORTAS PARA OS 5 SENSORES

#define chaveLimite90 8
#define chaveLimite0 9
#define led 13
#define eletroIma 12
#define sensor5 2
#define sensor1 7
#define sensor2 5
#define sensor3 4
#define sensor4 3
#define sensor 6
#define ms1 52
#define ms2 50
#define ms3 48

#define PI 3.1415926535897932384626433832795

LiquidCrystal lcd(53, 51, 49, 47, 45, 43);
SoftwareSerial bt(10, 11); // RX, TX

//TECLADO
byte pinosColunas[] = {36, 34, 32, 30};
byte pinosLinhas[] = {28, 26, 24, 22};

char teclas[4][4] =
    {{'1', '2', '3', 'A'},
     {'4', '5', '6', 'B'},
     {'7', '8', '9', 'C'},
     {'*', '0', '#', 'D'}};

Keypad teclado = Keypad(makeKeymap(teclas), pinosLinhas, pinosColunas, 4, 4);
int posicaoCancela, j = 0, angulo = 0;
const int stepPin = 39,
          dirPin = 37,
          enaPin = 41;

const float gravidade = 9.78;

boolean PLANO_OPERACAO,
    QUEDA_OPERACAO,
    ENERGIA_OPERACAO;
float instantes[5]; //USADO PARA CALCULO DE VELOCIDADE/ENERGIA EM DETERMINADOS INSTANTES
