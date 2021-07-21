
void planoInclinado(String metodoControle) {
  int ang = 0,
      i = 0;
  boolean liberarObjeto = LOW,
          posicao = LOW;
  PLANO_OPERACAO = HIGH;
  float angReal;

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("PLANO INCLINADO");
  Serial.println("PLANO INCLINADO");

  while (PLANO_OPERACAO == HIGH) {
    digitalWrite(eletroIma, HIGH);//LIGA ELETROIMA
    alertaMov("lcd");//TODOS MOMENTOS QUE O PLANO SE MOVE É ALERTADO PARA QUE NAO HAJA ACIDENTES
    lcd.clear();
    //movPlano(0);
    posicao = LOW;
    while (posicao == LOW) { //ENQUANTO NAO ESTIVER NADA PRESO NO ELETROIMA
      if (metodoControle == "teclado") {
        posicao = verificaEletro("teclado"); //VERIFICA SE EXISTE ALGO PRESO NO ELETROIMA
      }
      if (metodoControle == "app") posicao = verificaEletro("app"); //VERIFICA SE EXISTE ALGO PRESO NO ELETROIMA
    }

    //----------------------------------- CONTROLE VIA TECLADO -----------------------------------\\

    if (metodoControle == "teclado" && posicao == HIGH) {
      Serial.println("PLANO INCLINADO TECLADO");
      boolean validarAng = LOW;
      while ( ang == 0 || ang > 80) {//VERIFICAR - ESTA ESTRANHO - como que ele entra no proximo if se enquanto foir maior que 80 ele rodara este if while
        lcd.setCursor(2, 0);
        lcd.print("PLANO INCLINADO");
        ang = decisaoNum('P');//PEGA O VALOR LIDO PELO TECLADO
        if (ang == 2089) { //VALOR RETORNADO AO  PRESSIONAR '*', O QUAL TEM A FUNCAO DE VOLTAR, LOGO, RESETAR ARDUINO
          resetarArduino();
        }
      }
      alertaMov("lcd");
      angReal = movPlano(ang);//PEGA DA OUTRA FUNCAO O ANGULO OBTIDO, VISTO QUE O PROCESSO NÃO É IDEAL
      Serial.println("\n'A' - Liberar Móvel");
      lcd.setCursor(0, 2);
      lcd.print("--------------------");//GAMBIARRA PARA LIMPAR SÓ A LINHA 2
      lcd.setCursor(0, 3);
      lcd.print("                    ");
      lcd.setCursor(3, 3);
      lcd.print("'1' - Liberar");
      while (teclado.getKey() != '1') {}
      lcdContagem();//3, 2, 1, PIIIII
      digitalWrite(eletroIma, LOW);//SOLTA MOVEL
      int tempoFinal = marcaInstantes('P', "lcd", angReal);//MARCA O TEMPO E IMPRIME (EXPERIMENTO, LOCAL DE EXIBICAO, ANGULO ALCANCADO PELO PLANO)
      char decisao = medouMenu("teclado");//RECEBE SE O USARIO QUER FAZER NOVAS MEDIÇÕES OU VOLTAR PARA O MENU INICIAL E PASSA SE ISSO SERA FEITO PELO TECLADO OU APP
      switch (decisao) {
        case '1':
          planoInclinado("teclado");
          break;
        case '2':
          resetarArduino();
          PLANO_OPERACAO = LOW;
          break;
        case '3':
          String resultado = "";
          while (resultado != "certo" && resultado != "new") {
            resultado = senha('P'); //RECEBE UM certo OU errado E INFORMA SE A SENHA VAI SER INSERIDA PELO TECLADO OU APP
          }
          if (resultado == "new") planoInclinado("teclado");//USUARIO SOLICITOU NOVA MEDICAO
          else {
            calculoPlanoInclinado(angReal, tempoFinal, "lcd");//SIGNIFICA QUE SAIU DO WHILE COM O VALOR DE CERTO, PORTANTO, EXIBIRA OS CALCULOS
          }
          break;
        case 'D'://RESET DO ARDUINO
          resetarArduino();
          break;
      }

      //----------------------------------- CONTROLE VIA APP -----------------------------------\\

    } else if (metodoControle == "app" && posicao == HIGH) {
      /*boolean posicao2 = LOW;
        while (posicao2 == LOW) {//ENQUANTO NAO ESTIVER NADA PRESO NO ELETROIMA
        posicao2 = verificaEletro("app");//VERIFICA SE EXISTE ALGO PRESO NO ELETROIMA
        }*/
      Serial.println("PLANO INCLINADO APP");
      int i = 0;
      ang = 0;
      while (i == 0) {
        if (bt.available()) {
          ang = bt.read();//RECEBE ANGULO DO APP
          if (ang != 1 && ang < 80) i++;
          else if (ang == 89) resetarArduino();
        }
      }
      if (ang != 1) {
        //COLOCAR AVISO DE MOVIMENTACAÇÃO
        i = 0;
        Serial.println(ang);
        angReal = movPlano(ang);
        Serial.println(angReal);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("--------------------");
        lcd.setCursor(0, 1);
        lcd.print("AGUARDANDO LIBERACAO");
        lcd.setCursor(6, 2);
        lcd.print("VIA APP");
        lcd.setCursor(0, 3);
        lcd.print("--------------------");
        int liberar = 0,
            j = 0;
        while (j == 0) {
          if (bt.available()) {
            liberar = bt.read(); //SE 1, LIBERA MOVEL
            if (liberar == 1)j++;
            if (liberar == 89) resetarArduino(); //89 eh o valor do 'Y' (ascII), o qual sera enviado sempre que clicar para voltar, ao clicar para voltar o arduino eh resetado
          }
        }
        lcdContagem();//3, 2, 1, PIIIII
        digitalWrite(eletroIma, LOW);//SOLTA MOVEL
        int tempoFinal = marcaInstantes('P', "app", angReal); //MARCA O TEMPO E IMPRIME (EXPERIMENTO, LOCAL DE EXIBICAO, ANGULO ALCANCADO PELO PLANO)
        digitalWrite(eletroIma, HIGH);//SOLTA MOVEL
        Serial.println("\nAGUARDO REQUERIMENTO DE SENHA");
        char decisao = medouMenu("app");//RECEBE SE O USARIO QUER FAZER NOVAS MEDIÇÕES OU VOLTAR PARA O MENU INICIAL E PASSA SE ISSO SERA FEITO PELO TECLADO OU APP
        Serial.println(decisao);
        switch (decisao) {
          case 'Z':
            planoInclinado("app");
            break;
          case 'Y':
            resetarArduino();
            PLANO_OPERACAO = LOW;
            break;
        }
      }
    }
  }
}

void calculoPlanoInclinado(float ang, float tempoFinal, String localExibicao) {
  lcd.clear();
  avisoUnidades();//EXIBE NO DISPLAY AS UNIDADES UTILIZADAS
  Serial.println("\nCálculos:");
  float aceleracao = 0,
        velocidade [5],
        rad = 0;
  rad = (PI * ang) / 180; //CONVERSAO PARA GRAUS RADIANOS
  tempoFinal = (tempoFinal / 1000); //TRANSFORMACAO DE MILI SEGUNDOS PARA SEGUNDOS
  aceleracao = gravidade * sin(rad); //a=g*sen(x)
  lcd.clear();
  if (localExibicao == "lcd") {
    for (int i = 0; i <= 3; i++) {
      velocidade[i] = aceleracao * (instantes[i] / 1000); //v=v0+at
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
    lcd.print("|1 - Acel.");
    delay(100);
    lcd.setCursor(10, 1);
    lcd.print("|2 - Medir");
    delay(100);
    lcd.setCursor(10, 2);
    lcd.print("|3 - Menu");
    delay(100);
    lcd.setCursor(10, 3);
    lcd.print("|");
    Serial.println("1 - Aceleração\n2 - Medir\n3 - Menu");

    char decisao = ' ';
    do {
      decisao = teclado.getKey();
      switch (decisao) {
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
          lcd.print("A=");
          lcd.print(aceleracao);
          Serial.println(aceleracao);
          Serial.println("m/s^2");
          decisao = ' ';
          while (decisao != '1' && decisao != '2' && decisao != '3') decisao = teclado.getKey();
          if (decisao == '1') {
            for (int i = 0; i <= 3; i++) {
              velocidade[i] = aceleracao * (instantes[i] / 1000); //v=v0+at
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
            lcd.print("|1 - Acel.");
            decisao = ' ';
          }
          break;
          /*case '2':
            planoInclinado("teclado");
            break;
            case '3':
            resetarArduino();
            break;*/
      }
    } while (decisao != '2' && decisao != '3');
    if (decisao == '2') planoInclinado("teclado");
    else if (decisao == '3')resetarArduino();
  }
}

void avisoUnidades() {
  char decisao = ' ';
  for ( int i = 0; i < 3; i++) {
    lcd.clear();
    delay(400);
    lcd.print(" ANOTE AS UNIDADES!");
    lcd.setCursor(1, 1);
    lcd.print("V = m/s");
    lcd.setCursor(1, 2);
    lcd.print("A = m/s^2");
    lcd.setCursor(2, 3);
    lcd.print("'1' - PROSSEGUIR");
    lcd.setCursor(11, 1);
    lcd.print("|");
    lcd.setCursor(11, 2);
    lcd.print("|");
    lcd.setCursor(12, 1);
    lcd.print("Ec = J");
    lcd.setCursor(12, 2);
    lcd.print("H = m");
    delay(600);
  }
  while (decisao != '1' ) decisao = teclado.getKey();
  lcd.clear();
}
