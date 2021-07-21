
void setup()
{

  pinMode(sensor, INPUT);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(ms1, OUTPUT);
  pinMode(ms2, OUTPUT);
  pinMode(ms3, OUTPUT);
  pinMode(chaveLimite0, INPUT_PULLUP);
  pinMode(chaveLimite90, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(eletroIma, OUTPUT);
  Serial.begin(9600);
  bt.begin(9600);
  lcd.begin(20, 4);
  inicioLcd(); //TITULO, NOME, EMPRESA, ETC
  Serial.println("\n-----------------------------------------------------------");
  digitalWrite(enaPin, HIGH);
  digitalWrite(ms1, HIGH);
  digitalWrite(ms2, HIGH);
  digitalWrite(ms3, HIGH);
}

void loop()
{
  char decisao = decisaoControle(); //PEGA CARACTER PARA TOMADA DE DECISAO DO SWITCH E IMPRIME AS OPCOES, A, B e C
  Serial.println(decisao);          //VERIFCACAO DE CHEGADA DE CARACTER, CONFERIR SE A FUNCAO ESTA RETORNANDO O CARACTER CORRETAMENTE
  switch (decisao)
  {
  case '1': //CONTROLE VIA APP
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("--------------------");
    lcd.setCursor(4, 1);
    lcd.print("CONTROLE VIA");
    lcd.setCursor(9, 2);
    lcd.print("APP");
    lcd.setCursor(0, 3);
    lcd.print("--------------------");
    app();
    break;

  case '2': //CONTROLE FISICO
    Serial.println("TECLADO");
    testeTeste();
    break;

  case '3': //DEVMODE
    String resultado = "null";
    while (resultado != "certo")
    {                         //SAI DO WHILE QUANDO A FUNCAO "senha()" RETORNA "certo"
      resultado = senha('T'); //AO PASSAR O ARGUMENTO 'T' A FUNCAO "senha()" ENTENDERA QUE ESTA NO MODO DE TESTE E VERIFICARA SUA RESPECTIOVA SENHA
    }
    developerMode(); //
    break;

  case 'D': //RESET DO ARDUINO
    resetarArduino();
    break;

  default:
    break;
  }
}
void testeTeste()
{
  char decisao2 = decisaoExperimento(); //ESCOLHA DO EXPERIMENTO DESEJADO - PLANO = 'A', QUEDA LIVRE = 'B', ENERGIA CINETICA = 'C'
  switch (decisao2)
  {
  case '1':
    planoInclinado("teclado"); //FUNCOES PLANO INCLINADO
    break;

  case '2':
    quedaLivre("teclado"); //FUNCOES QUEDA LIVRE
    break;

  case '3':
    energiaCinetica("teclado"); //FUNCOES ENERGIA CINETICA
    break;

  case 'D': //RESET DO ARDUINO
    resetarArduino();
    break;

  default:
    break;
  }
}
void inicioLcd()
{
}

char decisaoControle()
{ //CHAMA TECLADO PARA SABER SE O USUARIO VAI CONTROLAR FISICAMENTE OU VIA PP. 'A' - app, 'B' - experimento
  char decisao = '@';
  int i = 0;
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("TIPO DE CONTROLE");
  lcd.setCursor(0, 1);
  lcd.print("'1' - VIA APP");
  lcd.setCursor(0, 2);
  lcd.print("'2' - TECLADO");
  lcd.setCursor(0, 3);
  lcd.print("'3' - TESTE");
  Serial.println("TIPO DE CONTROLE");
  Serial.println("'1' - VIA APP");
  Serial.println("'2' - TECLADO");
  Serial.println("'3' - TESTE");

  while (decisao != '1' && decisao != '2' && decisao != '3' && decisao != 'D')
  {
    decisao = teclado.getKey();
  }

  return decisao;
}

char decisaoExperimento()
{ //DECISAO DE EXPERIMENTO (QUANDO CONTROLADO FISICAMENTE). 1 - PLANO, 2 - QUEDA, 3 - ENERGIA
  char decisao;
  int i = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EXPERIMENTO:");
  lcd.setCursor(0, 1);
  lcd.print("1 - Plano Inclinado");
  lcd.setCursor(0, 2);
  lcd.print("2 - Queda Livre");
  lcd.setCursor(0, 3);
  lcd.print("3 - Energia Cinetica");
  while (decisao != '1' && decisao != '2' && decisao != '3' && decisao != 'D')
  {
    decisao = teclado.getKey();
  }
  if (decisao == 'D')
    resetarArduino();
  return decisao;
}

void app()
{
  char decisao; //CARACTER ALEATORIO, TANTO FAZ, SO NAO PODE SER OS USADOS NO SWITCH
  Serial.println("AGUARDANDO INFO DE EXPERIMENTO");
  int i = 0;
  while (i == 0)
  {
    switch (bt.read())
    { //CASO DE PROBLEMA, ZERAR VARIAVEL APOS CHAMADA DO EXPERIMENTO
    case 'P':
      planoInclinado("app"); //FUNCOES PLANO INCLINADO
      i++;                   //ADC 1 AO I E SAI DO LOOP, POR CONSEGUINTE O ARDUINO PARA DE RECEBER DADOS DO BT
      break;

    case 'Q':
      quedaLivre("app"); //FUNCOES QUEDA LIVRE
      i++;
      break;

    case 'E':
      energiaCinetica("app"); //FUNCOES ENERGIA CINETICA
      i++;
      break;
    }
  }
}

float movPlano(int ang)
{
  int menorPeriodo = 2000, //O MENOR DELAY DO FOR DO MOTOR SERA ESSE - LOGO, QUANTO MENOR, MAIS RAPIDO O MOTOR GIRARA - POIS O FOR SERA MAIS RAPIDO - Dá pra usar uns 1800 ou menos
      maiorPeriodo = 2500; //O MAIOR DELAY DO FOR SERA ESSE - LOGO, QUANTO MAIOR, MAIS LENTO O MOTOR GIRARA - POIS O FOR SERA MAIS LENTO
  //x = 0;

  if (ang == 0 || ang == 90)
  {
    switch (ang)
    {
    case 0:
      digitalWrite(dirPin, HIGH); //SE dirPin FOR IGUAL A LOW, O MOTOR GIRARA NO SENTIDO HORARIO
      digitalWrite(enaPin, LOW);  //ESSA LINHA HABILITA A FUNÇÃO HALFSTEP, ENT DA UM PASSO DE 0.9º
      while (digitalRead(chaveLimite0) != LOW)
      { //ENQUANTO A  CHAVE FIM DE CURSO N ESTIVER ACIONADA, O MOTOR RECEBE OS PULSOS E SE MOVIMENTA. QUANDO PRESSIONADA, CHAVE RETORNA LOW

        digitalWrite(stepPin, HIGH);
        delayMicroseconds(800); //GERALMENTE AS PESSOAS POEM UM DELAY MENOR, MAS COMO QUERO ALGO MAIS LENTO EU AUMENTEI O DELAY
        digitalWrite(stepPin, LOW);
        delayMicroseconds(800);
      }
      digitalWrite(enaPin, HIGH); //ESSA LINHA HABILITA A FUNÇÃO HALFSTEP, ENT DA UM PASSO DE 0.9º
      break;

    case 90:
      int i = 0;
      digitalWrite(dirPin, LOW); //SE dirPin FOR IGUAL A HIGH, O MOTOR GIRARA NO SENTIDO ANTIHORARIO
      digitalWrite(enaPin, LOW); //ESSA LINHA HABILITA A FUNÇÃO HALFSTEP, ENT DA UM PASSO DE 0.9º
      while (i != 8000)
      { //ENQUANTO A  CHAVE FIM DE CURSO N ESTIVER ACIONADA, O MOTOR RECEBE OS PULSOS E SE MOVIMENTA. QUANDO PRESSIONADA, CHAVE RETORNA LOW
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(800); //GERALMENTE AS PESSOAS POE UM DELAY MENOR, MAS COMO QUERO ALGO MAIS LENTO EU AUMENTEI O DELAY
        digitalWrite(stepPin, LOW);
        delayMicroseconds(800);
        i++;
      }
      digitalWrite(enaPin, HIGH); //ESSA LINHA HABILITA A FUNÇÃO HALFSTEP, ENT DA UM PASSO DE 0.9º
      break;
    }
  }
  else if (ang != 0 && ang != 90)
  {
    digitalWrite(enaPin, LOW);
    digitalWrite(dirPin, LOW);
    int ppr = (ang / 0.01125); //O STEP DO MOTOR, INICIALMENTE 0.9, PORÉM COM REDUCAO 1:10, 0.09
    digitalWrite(enaPin, LOW);
    for (int x = 0; x <= ppr; x++)
    {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(800);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(800);
    }
    digitalWrite(enaPin, HIGH);
    return ppr * 0.01125;
  }
}

boolean verificaEletro(String controle)
{ //VERIFICA SE O MOVEL ESTA NO ELETROIMA E RECEBE SE O AVISO DEVE SER PELO APP OU LCD
  boolean posicao = LOW;
  posicao = digitalRead(sensor);
  while (digitalRead(sensor) != LOW)
  {
    if (bt.available())
      if (bt.read() == 'Y')
        resetarArduino();
    lcd.clear();
    //posicao = digitalRead(sensor);
    delay(300);
    posicao = !digitalRead(sensor);
    lcd.setCursor(0, 0);
    lcd.print("--------------------");
    lcd.setCursor(6, 1);
    lcd.print("PRENDA O");
    lcd.setCursor(7, 2);
    lcd.print("OBJETO");
    lcd.setCursor(0, 3);
    lcd.print("--------------------");
    delay(300);
  }
  if (controle == "teclado" && posicao == LOW)
  {
    lcd.clear();
    return HIGH;
  }
  else if (controle == "app")
  {
    bt.print("OK"); //FALA PRO APP QUE O MOVEL ESTA PRESO NO ELETROIMA E A ATIVIDADE PODE SER CONTINUADA
    return HIGH;
  }
}

int decisaoNum(char experimento)
{
  int num,
      i = 0,
      tecla[6],
      j;
  boolean ENTER = LOW;
  switch (experimento)
  {
  case 'P':
    j = 3;
    Serial.println("\n'#' - Confirmar Angulo");
    Serial.println("'*' - Voltar");
    Serial.print("Angulo Desejado:");
    lcd.setCursor(0, 1);
    lcd.print("Angulo Desejado:");
    lcd.setCursor(16, 1);
    lcd.print("    ");
    lcd.setCursor(3, 3);
    lcd.print("'#' - Confirmar ");
    lcd.setCursor(17, 1);
    break;
  case 'E':
    j = 4;
    lcd.setCursor(0, 2);
    lcd.print("Massa (g):");
    lcd.setCursor(3, 3);
    lcd.print("'#' - Confirmar ");
    Serial.println("\n'#' - Confirmar Massa");
    Serial.println("'*' - Voltar");
    Serial.print("Massa Desejada:");
    lcd.setCursor(11, 2);
    break;
  }
  //lcd.setCursor(17, 1);
  while (i != j && ENTER != HIGH)
  {
    char teclaPressionada = teclado.getKey();
    switch (teclaPressionada)
    {
    case '1':
      tecla[i] = 1;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '2':
      tecla[i] = 2;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '3':
      tecla[i] = 3;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '4':
      tecla[i] = 4;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '5':
      tecla[i] = 5;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '6':
      tecla[i] = 6;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '7':
      tecla[i] = 7;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '8':
      tecla[i] = 8;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '9':
      tecla[i] = 9;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '0':
      tecla[i] = 0;
      lcd.print(tecla[i]);
      Serial.print(tecla[i]);
      i++;
      break;
    case '*': //VOLTAR
      return 2089;
      break;
    case '#':
      ENTER = HIGH;
      tecla[i] = 100; //VALOR ALEATORIO, RELEVANTE PARA O PROXIMO CONDICIONAL
      break;
    }
  }
  if (i == 1 && tecla[1] == 100)
  { //SE O USUARIO DIGITAR APENAS UM NUMERO E CONFIRMAR COM #, NESTE CASO, # EH IGUAL A 100
    num = tecla[0];
    return num;
  }
  else if (i == 2 && tecla[2] == 100)
  {
    num = (tecla[0] * 10) + tecla[1];
    return num;
  }
  else if (i == 3 && tecla[3] == 100)
  {
    num = (tecla[0] * 100) + (tecla[1] * 10) + tecla[1];
    return num;
  }
  else
    return 0;
}

void lcdContagem()
{ //3, 2, 1 PARA LIBERACAO DO MOVEL
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONTAGEM REGRESSIVA:");
  int i = 1;
  for (int i = 1; i <= 3; i++)
  {
    lcd.setCursor(9, i);
    lcd.print(4 - i);
    delay(1000);
  }
}

int marcaInstantes(char experimento, String controle, float angReal)
{
  Serial.println("MARCANDO TEMPO");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(6, 1);
  lcd.print("MARCANDO");
  lcd.setCursor(8, 2);
  lcd.print("TEMPO");
  lcd.setCursor(0, 3);
  lcd.print("--------------------");

  int i = 0,
      tempo[4] = {0, 0, 0, 0};
  unsigned long temp;

  while (i <= 4)
  {
    if ((i == 0) && digitalRead(sensor1) == LOW)
    { //SENSOR UTILIZADO RETORNA LOW SEMPRE QUE IDENTIFICA UM OBJETO
      temp = millis();
      i += 1;
    }
    if ((i == 1) && (digitalRead(sensor2) == LOW))
    {
      tempo[0] = millis() - temp;
      i += 1;
    }
    if ((i == 2) && (digitalRead(sensor3) == LOW))
    {
      tempo[1] = millis() - temp;
      i += 2;
    }
    if ((i == 4) && (digitalRead(sensor5) == LOW))
    {
      tempo[3] = millis() - temp;
      i += 1;
      //VARIAVEIS GLOBAIS PARA USO NA FUNCAO DE CALC
      instantes[0] = tempo[0];
      instantes[1] = tempo[1];
      instantes[2] = tempo[2];
      instantes[3] = tempo[3];
    }
  }

  //----------------------------------- EXIBICAO VIA APP -----------------------------------\\

  if (controle == "app")
  {
    int i = 0;
    bt.print("\t\tT1: ");
    bt.print(tempo[0]);
    bt.println(" ms");
    bt.print("\t\tT2: ");
    bt.print(tempo[1]);
    bt.println(" ms");
    bt.print("\t\tT3: ");
    bt.print(tempo[2]);
    bt.println(" ms");
    bt.print("\t\tT4: ");
    bt.print(tempo[3]);
    bt.println(" ms");
    lcd.clear();
    for (i = 0; i < 4; i++)
    {
      lcd.setCursor(0, i);
      lcd.print("T");
      lcd.setCursor(1, i);
      lcd.print(i);
      lcd.setCursor(2, i);
      lcd.print("=");
      lcd.setCursor(3, i);
      lcd.print(tempo[i]);
      lcd.print("ms");
      delay(130);
    }
    if (experimento == 'P')
    {
      bt.print("\n\t\t\tINCLINAÇÃO REAL: ");
      bt.print(angReal);
      bt.println("º");
      lcd.setCursor(11, 0);
      lcd.print("Ang=");
      lcd.print(angReal);
      for (int i = 0; i <= 3; i++)
      {
        lcd.setCursor(10, i);
        lcd.print("|");
      }
    }

    //----------------------------------- EXIBICAO VIA LCD -----------------------------------\\

  }
  else if (controle == "lcd")
  {
    lcd.clear();
    for (i = 0; i < 4; i++)
    {
      lcd.setCursor(0, i);
      lcd.print("T");
      lcd.setCursor(1, i);
      lcd.print(i);
      lcd.setCursor(2, i);
      lcd.print("=");
      lcd.setCursor(3, i);
      lcd.print(tempo[i]);
      lcd.print("ms");
      delay(130);
    }
  }
  Serial.print("\n\n\tMedições");
  Serial.print("\nInstante 1 = ");
  Serial.print(tempo[0]);
  Serial.print(" ms");
  Serial.print("\nInstante 2 = ");
  Serial.print(tempo[1]);
  Serial.print(" ms");
  Serial.print("\nInstante 3 = ");
  Serial.print(tempo[2]);
  Serial.print(" ms");
  Serial.print("\nInstante 4 = ");
  Serial.print(tempo[3]);
  Serial.println(" ms");
  return tempo[3];
}

String senha(char experimento)
{ //RECEBE O EXP QUE ESTA SENDO USADO
  char password[20],
      teclaPressionada;

  if (experimento == 'T')
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MODO DESENVOLVEDOR");
    lcd.setCursor(0, 1);
    lcd.print("SENHA: ");
    lcd.setCursor(0, 2);
    lcd.print("'#'- CONFIRMAR SENHA");
    lcd.setCursor(0, 3);
    lcd.print("PARA VOLTAR REINICIE");
    lcd.setCursor(6, 1);
    Serial.println("MODO DESENVOLVEDOR");
    Serial.println("\n'#'- CONFIRMAR SENHA");
    Serial.println("PARA VOLTAR REINICIE");
    Serial.print("\nSENHA: ");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SENHA:");
    lcd.setCursor(0, 2);
    lcd.print("'#'- CONFIRMAR SENHA");
    lcd.setCursor(0, 3);
    lcd.print("'*'- NOVAS MEDIDAS");
    lcd.setCursor(0, 1);
    Serial.println("\n\n'#'- CONFIRMAR SENHA");
    Serial.println("'*'- NOVAS MEDIDAS");
    Serial.print("\nSENHA: ");
  }
  int i = 0;
  lcd.setCursor(6, 0);
  do
  {
    teclaPressionada = teclado.getKey();
    switch (teclaPressionada)
    {
    case 'A':
      password[i] = 'A';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case 'B':
      password[i] = 'B';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case 'C':
      password[i] = 'C';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case 'D':
      password[i] = 'D';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '0':
      password[i] = '0';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '1':
      password[i] = '1';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '2':
      password[i] = '2';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '3':
      password[i] = '3';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '4':
      password[i] = '4';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '5':
      password[i] = '5';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '6':
      password[i] = '6';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '7':
      password[i] = '7';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '8':
      password[i] = '8';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '9':
      password[i] = '9';
      lcd.print("*");
      Serial.print("*");
      i++;
      break;
    case '*':
      return "new";
      break;
    }
  } while (teclaPressionada != '#' && teclaPressionada != '*');
  if (i == 5 && (password[0] == 'B' && password[1] == 'A' && password[2] == '3' && password[3] == '2' && password[4] == '1'))
  { //SENHA = BA321
    lcd.setCursor(3, 1);
    lcd.print("SENHA CORRETA!");
    delay(300);
    return "certo";
  }
  if ((password[0] != 'B' || password[1] != 'A' || password[2] != '3' || password[3] != '2' || password[4] != '1'))
  {
    lcd.setCursor(2, 1);
    lcd.print("SENHA INCORRETA!");
    return "errado";
  }
}

char medouMenu(String controle)
{
  digitalWrite(eletroIma, HIGH);

  char decisao; //CARACTER ALEATORIO
  int i;
  unsigned long tempo, tempo1;
  if (controle == "teclado")
  {
    lcd.setCursor(10, 0);
    lcd.print("|1 - Medir");
    lcd.setCursor(10, 1);
    lcd.print("|2 - Menu");
    lcd.setCursor(10, 2);
    lcd.print("|3 - Reso-");
    lcd.setCursor(10, 3);
    lcd.print("|");
    lcd.setCursor(15, 3);
    lcd.print("lucao");
    Serial.println("1 - Novas medidadas");
    Serial.println("2 - Menu");
    Serial.println("3 - Resolução");
    while (decisao != '1' && decisao != '2' && decisao != '3' && decisao != 'D')
    {
      tempo1 = 0;
      tempo = millis();
      tempo1 = millis() - tempo;
      decisao = teclado.getKey();
    }
  }
  else if (controle == "app")
  {
    while (decisao != 'Z' && decisao != 'Y')
    {
      digitalWrite(13, HIGH);
      if (bt.available())
        decisao = bt.read();
    }
  }
  digitalWrite(13, LOW);
  return decisao;
}

void developerMode()
{ //MODO PARA TESTE DE COMPONENTES
  boolean devMode = HIGH;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("'A' - MOTOR");
  lcd.setCursor(0, 1);
  lcd.print("'B' - SENSORES");
  lcd.setCursor(0, 2);
  lcd.print("'C' - TECLADO");
  lcd.setCursor(0, 3);
  lcd.print("D - TESTE GERAL");
  Serial.println("\n\nA - MOTOR");
  Serial.println("B - SENSORES");
  Serial.println("C - TECLADO");
  Serial.println("D - TESTE GERAL");
  Serial.println("PARA VOLTAR REINICIE");
  char teclaPressionada = '/';
  while (teclaPressionada != 'A' && teclaPressionada != 'B' && teclaPressionada != 'C' && teclaPressionada != 'D')
  {
    teclaPressionada = teclado.getKey();
  }
  switch (teclaPressionada)
  {
  case 'A':
    testeMotor();
    break;

  case 'B':
    testeSensores();
    break;

  case 'C':
    testeTeclado();
    break;

  case 'D':
    digitalWrite(13, HIGH);
    testeGeral();
    break;
  }
}
void testeMotor()
{
  int angDesejado = teclado.getKey(); //QUALQUER VALOR ALEATORIO ENTRE 1-79
  alertaMov("lcd");
  delay(500);
  for (int i = 0; i <= 2; i++)
  {
    movPlano(0);
    delay(500);
    movPlano(90);
    delay(500);
    movPlano(angDesejado);
    delay(500);
  }
  developerMode();
}

void testeSensores()
{
  char tecla = '/';
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("'D' - VOLTAR");
  Serial.println("'D' - VOLTAR");
  while (tecla != 'D')
  {
    tecla = teclado.getKey();
    if ((digitalRead(sensor) == 0) || (digitalRead(sensor1) == 0) || (digitalRead(sensor2) == 0) || (digitalRead(sensor3) == LOW) || (digitalRead(sensor4) == LOW) || (digitalRead(sensor5) == LOW))
    {
      teste(); //ACENDE LED SEMPRE QUE ALGUM SENSOR EH ACIONADO
    }
  }
  developerMode(); //USUARIO APERTOU 'D' SAI DO WHILE VOLTA PRO MENU PRINCIPAL DE TESTES
}

void testeTeclado()
{
  char tecla1 = '/';
  lcd.clear();
  lcd.setCursor(0, 3);
  lcd.print("'D' - VOLTAR");
  Serial.println("'D' - VOLTAR");
  lcd.setCursor(0, 1);
  while (tecla1 != 'D')
  {
    tecla1 = teclado.getKey();
    lcd.print(tecla1); //IMPRIME TODAS AS TECLAS QUE FOREM PRESSIONADAS
    if (tecla1 != "")
      Serial.print(tecla1);
  }
  developerMode(); //USUARIO APERTOU 'D' SAI DO WHILE VOLTAS PRO MENU PRINCIPAL DE TESTES
}

void testeGeral()
{
  boolean posicao = LOW;
  int angDesejado = 60; //  SELEÇÃO ALEATORIA ENTRE 1-79

  //----------------------------------- SCRIPT DE TESTE PLANO INCLINADO -----------------------------------\\

  Serial.println("\nTESTANDO PLANO INCLINADO");
  digitalWrite(eletroIma, HIGH);
  alertaMov("lcd");
  delay(2000);
  //movPlano(0);
  movPlano(angDesejado);
  Serial.println("PRENDA O CARRO");
  while (posicao == LOW)
  {                                      //ENQUANTO NAO ESTIVER NADA PRESO NO ELETROIMA
    posicao = verificaEletro("teclado"); //VERIFICA SE EXISTE ALGO PRESO NO ELETROIMA
  }
  lcdContagem();                                                //3, 2, 1, PIIIII
  digitalWrite(eletroIma, LOW);                                 //SOLTA MOVEL
  int tempoFinal = marcaInstantes('P', "teclado", angDesejado); //MARCA O TEMPO E IMPRIME (EXPERIMENTO, LOCAL DE EXIBICAO, ANGULO ALCANCADO PELO PLANO)
  digitalWrite(eletroIma, HIGH);                                //LIGA ELETROIMA

  //----------------------------------- SCRIPT DE TESTE QUEDA LIVRE / ENERGIA CINETICA -----------------------------------\\

  tempoFinal = 0;
  posicao = LOW;
  Serial.println("\nTESTANDO QUEDA LIVRE / ENERGIA CINÉTICA");
  alertaMov("lcd");
  delay(2000);
  //movPlano(0);
  //movPlano(90);
  Serial.println("PRENDA A ESFERA");
  while (posicao == LOW)
  {                                      //ENQUANTO NAO ESTIVER NADA PRESO NO ELETROIMA
    posicao = verificaEletro("teclado"); //VERIFICA SE EXISTE ALGO PRESO NO ELETROIMA
  }
  lcdContagem();                                  //3, 2, 1, PIIIII
  digitalWrite(eletroIma, LOW);                   //SOLTA MOVEL
  tempoFinal = marcaInstantes('Q', "teclado", 0); //MARCA O TEMPO E IMPRIME (EXPERIMENTO, LOCAL DE EXIBICAO, ANGULO ALCANCADO PELO PLANO)
  Serial.println("\n*******TESTE FINALIZADO*******\n\n");
}

void resetarArduino()
{
  wdt_enable(WDTO_2S);
  wdt_reset();
  reset();
}

void reset()
{
  while (true)
    ;
}

void teste()
{ //USADA PARA TESTAR, VER AONDE O CODIGO ESTA OU OUTROS
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
}
