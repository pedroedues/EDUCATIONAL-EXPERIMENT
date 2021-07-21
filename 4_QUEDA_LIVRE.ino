void quedaLivre(String metodoControle)
{
  boolean liberarObjeto = LOW,
          posicao = LOW;
  QUEDA_OPERACAO = HIGH;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("QUEDA LIVRE");
  Serial.println("QUEDA LIVRE");
  while (QUEDA_OPERACAO == HIGH)
  {
    digitalWrite(eletroIma, HIGH); //LIGA ELETROIMA
    movPlano(90);
    posicao = LOW;
    while (posicao == LOW)
    { //ENQUANTO NAO ESTIVER NADA PRESO NO ELETROIMA
      if (metodoControle == "teclado")
        posicao = verificaEletro("teclado"); //VERIFICA SE EXISTE ALGO PRESO NO ELETROIMA
      if (metodoControle == "app")
        posicao = verificaEletro("app"); //VERIFICA SE EXISTE ALGO PRESO NO ELETROIMA
    }

    //----------------------------------- CONTROLE VIA TECLADO -----------------------------------
    if (metodoControle == "teclado" && posicao == HIGH)
    {
      Serial.println("QUEDA LIVRE TECLADO");
      Serial.println("\n'A' - Liberar Móvel");
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("QUEDA LIVRE");
      lcd.setCursor(0, 2);
      lcd.print("--------------------");
      lcd.setCursor(3, 3);
      lcd.print("'1' - Liberar");
      while (teclado.getKey() != '1')
      {
      }
      lcdContagem();                                  //3, 2, 1, PIIIII
      digitalWrite(eletroIma, LOW);                   //SOLTA MOVEL
      int tempoFinal = marcaInstantes('Q', "lcd", 0); //MARCA O TEMPO E IMPRIME (EXPERIMENTO, LOCAL DE EXIBICAO)
      char decisao = medouMenu("teclado");            //RECEBE SE O USARIO QUER FAZER NOVAS MEDIÇÕES OU VOLTAR PARA O MENU INICIAL E PASSA SE ISSO SERA FEITO PELO TECLADO OU APP
      switch (decisao)
      {
      case '1':
        quedaLivre("teclado");
        break;
      case '2':
        resetarArduino();
        QUEDA_OPERACAO = LOW;
        break;
      case '3':
        String resultado = "";
        while (resultado != "certo" && resultado != "new")
        {
          resultado = senha('Q'); //RECEBE UM certo OU errado E INFORMA SE A SENHA VAI SER INSERIDA PELO TECLADO OU APP
        }
        if (resultado == "new")
          quedaLivre("teclado"); //USUARIO SOLICITOU NOVA MEDICAO
        else
          calculoQueda(tempoFinal, "lcd"); //SIGNIFICA QUE SAIU DO WHILE COM O VALOR DE CERTO, PORTANTO, EXIBIRA OS CALCULOs
        break;
      }

      //----------------------------------- CONTROLE VIA APP -----------------------------------\\

    }
    else if (metodoControle == "app" && posicao == HIGH)
    {
      Serial.println("QUEDA LIVRE APP");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("--------------------");
      lcd.setCursor(0, 1);
      lcd.print("AGUARDANDO LIBERACAO");
      lcd.setCursor(6, 2);
      lcd.print("VIA APP");
      lcd.setCursor(0, 3);
      lcd.print("--------------------");
      while (bt.read() != 1 && bt.read() != 89)
      {
        if (bt.available())
          if (bt.read() == 89)
            resetarArduino();
      }
      lcdContagem();                                  //3, 2, 1, PIIIII
      digitalWrite(eletroIma, LOW);                   //SOLTA MOVEL
      int tempoFinal = marcaInstantes('Q', "app", 0); //MARCA E EXIBE INSTANTES. PASSO COMO ARGUMENTO O EXPERIMNETO, LOCAL DE EXIBICAO, O ULTIMO SOJ EH UTILIZADO NO PI, PORTANTO EU PASSO 0
      char decisao = medouMenu("app");                //RECEBE SE O USARIO QUER FAZER NOVAS MEDIÇÕES OU VOLTAR PARA O MENU INICIAL E PASSA SE ISSO SERA FEITO PELO TECLADO OU APP
      switch (decisao)
      {
      case 'Z':
        quedaLivre("app");
        break;
      case 'Y':
        resetarArduino();
        QUEDA_OPERACAO = LOW;
        break;
      }
    }
  }
}

void calculoQueda(float tempo, String controle)
{
  float velocidade[5],
      altura = 0;
  char decisao = ' ';
  tempo = tempo / 1000;                       //ms - s
  altura = (gravidade * (tempo * tempo)) / 2; //H = (g*t^2)/2
  avisoUnidades();                            //EXIBE NO DISPLAY AS UNIDADES UTILIZADAS

  if (controle == "lcd")
  {
    for (int i = 0; i <= 3; i++)
    {
      velocidade[i] = gravidade * (instantes[i] / 1000); //v = g*t
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
      delay(100);
    }
    lcd.setCursor(10, 0);
    lcd.print("|1 - altu.");
    delay(100);
    lcd.setCursor(10, 1);
    lcd.print("|2 - Medir");
    delay(100);
    lcd.setCursor(10, 2);
    lcd.print("|3 - Menu");
    delay(100);
    lcd.setCursor(10, 3);
    lcd.print("|");
    Serial.println("1 - Altura\n2 - Medir\n3 - Menu");
    do
    {
      decisao = teclado.getKey();
      switch (decisao)
      {
      case '1':
        lcd.clear();
        lcd.setCursor(10, 0);
        lcd.print("|1 - vel..");
        lcd.setCursor(10, 1);
        lcd.print("|2 - Medir");
        lcd.setCursor(10, 2);
        lcd.print("|3 - Menu");
        lcd.setCursor(10, 3);
        lcd.print("|");
        lcd.setCursor(0, 0);
        lcd.print("H=");
        lcd.print(altura);
        decisao = ' ';
        while (decisao != '1' && decisao != '2' && decisao != '3')
          decisao = teclado.getKey();
        if (decisao == '1')
        {
          for (int i = 0; i <= 3; i++)
          {
            velocidade[i] = gravidade * (instantes[i] / 1000); //v=g*t
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
          lcd.setCursor(10, 0);
          lcd.print("|1 - altu.");
          decisao = ' ';
        }
        break;
      }
    } while (decisao != '2' && decisao != '3');

    if (decisao == '2')
      quedaLivre("teclado");
    else if (decisao == '3')
      resetarArduino();
    /*//SERIAL MONITOR
      char decisao = 'X';//CARACTER ALEATORIO PARA OBRIGAR A ENTRADA NO WHILE() A SEGUIR
      while (decisao != 'A' && decisao != 'B') decisao = teclado.getKey();
      switch (decisao) {
      case 'A':
        quedaLivre("teclado");
        break;
      case 'B':
        resetarArduino();
        break;
      }*/
  }
}
