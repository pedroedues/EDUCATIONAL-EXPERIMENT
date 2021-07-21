void energiaCinetica(String metodoControle)
{
  float massa = 0;
  int ang = 0;
  boolean liberarObjeto = LOW,
          posicao = LOW;
  ENERGIA_OPERACAO = HIGH;

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("ENERGIA CINETICA");
  Serial.println("ENERGIA CINÉTICA");
  while (ENERGIA_OPERACAO == HIGH)
  {
    digitalWrite(eletroIma, HIGH); //LIGA ELETROIMA
    posicao = LOW;

    //----------------------------------- CONTROLE VIA TECLADO -----------------------------------\\

    if (metodoControle == "teclado")
    {
      Serial.println("\nQualquer valor Inteiro entre 1º - 90º");
      while (ang == 0 || ang > 90)
      {
        lcd.setCursor(2, 0);
        lcd.print("ENERGIA CINETICA");
        ang = decisaoNum('P'); /*PEGA O VALOR LIDO PELO TECLADO. PASSAR O 'P' EH GAMBIARRA PRA ENTENDER QUE
                                EH COMO SE FOSSE O PLANO INCLINADO, PARA PEGAR O ANGULO*/
        if (ang == 2089)
        { //VALOR RECEBIDO QUANDO USUARIO QUISER VOLTAR PARA O MENU
          resetarArduino();
        }
      }
      Serial.println("ENERGIA CINÉTICA TECLADO");
      lcd.setCursor(2, 0);
      lcd.print("ENERGIA CINETICA");
      while (massa == 0 || massa > 250)
      {
        massa = decisaoNum('E'); //PEGA O VALOR LIDO PELO TECLADO
        if (massa > 250)
        {
          lcd.setCursor(11, 2);
          lcd.print("       ");
        }
        if (massa == 2089)
        { //VALOR RECEBIDO QUANDO USUARIO QUISER VOLTAR PARA O MENU
          resetarArduino();
        }
      }

      posicao = LOW;
      while (posicao == LOW)
      {
        posicao = verificaEletro("teclado"); /*FUNCAO PEGA O ESTADO DO SENSOR PARA PRESENCA,
                                              SE RETORNAR LOW EH PORQ N TEM NADA*/
      }
      alertaMov("lcd");
      float angReal = movPlano(ang);
      //Serial.println(angReal);
      Serial.println("\n'A' - Liberar Móvel");
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("ANOTE OS DADOS");
      lcd.setCursor(0, 1);
      lcd.print("Angulo Desejado: ");
      lcd.print(ang);
      lcd.setCursor(0, 2);
      lcd.print("Massa (g): ");
      lcd.print(massa);
      lcd.setCursor(3, 3);
      lcd.print("'1' - Liberar");
      while (teclado.getKey() != '1')
      {
      }                                               //FICA AQUI ATE O USUARIO PRESSIONAR 'A' PARA LIBERAR
      lcdContagem();                                  //3, 2, 1, PIIIII
      digitalWrite(eletroIma, LOW);                   //SOLTA MOVEL
      int tempoFinal = marcaInstantes('E', "lcd", 0); //MARCA O TEMPO E IMPRIME (EXPERIMENTO, LOCAL DE EXIBICAO, ANGULO ALCANCADO PELO PLANO)
      char decisao = medouMenu("teclado");            //RECEBE SE O USARIO QUER FAZER NOVAS MEDIÇÕES OU VOLTAR PARA O MENU INICIAL E PASSA SE ISSO SERA FEITO PELO TECLADO OU APP
      switch (decisao)
      {
      case '1':
        energiaCinetica("teclado");
        break;
      case '2':
        resetarArduino();
        ENERGIA_OPERACAO = LOW;
        break;
      case '3':
        String resultado = " ";
        while (resultado != "certo" && resultado != "new")
        {
          resultado = senha('E'); //RECEBE UM certo OU errado E INFORMA SE A SENHA VAI SER INSERIDA PELO TECLADO OU APP
        }
        if (resultado == "new")
          energiaCinetica("teclado"); //USUARIO SOLICITOU NOVA MEDICAO
        else
          calculoEnergia(angReal, massa, tempoFinal, "lcd"); //SIGNIFICA QUE SAIU DO WHILE COM O VALOR DE CERTO, PORTANTO, EXIBIRA OS CALCULOs
        break;
      }
    }
    else if (metodoControle == "app")
    {
      posicao = LOW;
      while (posicao == LOW)
      {
        posicao = verificaEletro("teclado");
      }
      Serial.println("ENERGIA CINÉTICA APP");
    }
  }
}

void calculoEnergia(float angReal, float massa, int tempo, String controle)
{
  float velocidade[4],
      energia[4],
      rad = 0;
  char decisao = "";
  massa = massa / 1000;
  avisoUnidades();            //EXIBE NO DISPLAY AS UNIDADES UTILIZADAS
  rad = (PI * angReal) / 180; //CONVERSAO PARA GRAUS RADIANOS
  for (int i = 0; i <= 3; i++)
  {
    instantes[i] = instantes[i] / 1000;                    //ms - s
    velocidade[i] = (gravidade * sin(rad)) * instantes[i]; //v = (g*sen(ang))*t
    energia[i] = (massa * (velocidade[i] * velocidade[i])) / 2;
  }

  lcd.clear();

  if (controle == "lcd")
  {
    Serial.print("\n");
    for (int i = 0; i <= 3; i++)
    {
      lcd.setCursor(0, i);
      lcd.print("E");
      lcd.setCursor(1, i);
      lcd.print(i);
      lcd.setCursor(2, i);
      lcd.print("=");
      lcd.setCursor(3, i);
      lcd.print(energia[i]);
      Serial.print("E");
      Serial.print(i);
      Serial.print("=");
      Serial.println(energia[i]);
      delay(100);
    }
    lcd.setCursor(10, 0);
    lcd.print("|1 - velo.");
    delay(100);
    lcd.setCursor(10, 1);
    lcd.print("|2 - Medir");
    delay(100);
    lcd.setCursor(10, 2);
    lcd.print("|3 - Menu");
    delay(100);
    lcd.setCursor(10, 3);
    lcd.print("|");
    Serial.println("1 - Velocidade\n2 - Medir\n3 - Menu");
    char decisao = ' ';
    do
    {
      decisao = teclado.getKey();
      switch (decisao)
      {
      case '1':
        lcd.clear();
        lcd.setCursor(10, 0);
        lcd.print("|1 - Ene..");
        lcd.setCursor(10, 1);
        lcd.print("|2 - Medir");
        lcd.setCursor(10, 2);
        lcd.print("|3 - Menu");
        lcd.setCursor(10, 3);
        lcd.print("|");
        for (int i = 0; i <= 3; i++)
        {
          lcd.setCursor(0, i);
          lcd.print("V");
          lcd.setCursor(1, i);
          lcd.print(i);
          lcd.setCursor(2, i);
          lcd.print("=");
          lcd.setCursor(3, i);
          lcd.print(velocidade[i]);
          Serial.print("V");
          Serial.print(i);
          Serial.print("=");
          Serial.println(velocidade[i]);
          delay(130);
        }
        decisao = ' ';
        while (decisao != '1' && decisao != '2' && decisao != '3')
          decisao = teclado.getKey();
        if (decisao == '1')
        {
          for (int i = 0; i <= 3; i++)
          {
            lcd.setCursor(0, i);
            lcd.print("E");
            lcd.setCursor(1, i);
            lcd.print(i);
            lcd.setCursor(2, i);
            lcd.print("=");
            lcd.setCursor(3, i);
            lcd.print(energia[i]);
            Serial.print("E");
            Serial.print(i);
            Serial.print("=");
            Serial.println(energia[i]);
            delay(100);
          }
          lcd.setCursor(10, 0);
          lcd.print("|1 - velo.");
          decisao = ' ';
        }
        break;
      }
    } while (decisao != '2' && decisao != '3');
    if (decisao == '2')
      energiaCinetica("teclado");
    else if (decisao == '3')
      resetarArduino();
  }
}
