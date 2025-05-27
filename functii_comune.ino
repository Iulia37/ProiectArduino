void afisare()
{
  for (int row = 0; row < 8; row++) 
  {
      for (int col = 0; col < 8; col++) 
      {
        lc.setLed(0, row, col, board[row][col]);
      }
  }
}

void sterge()
{
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      board[i][j] = 0;
    }
  }
}
  
void alege_joc()
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Choose Game");

  joc = 1;
  lcd.setCursor(1, 1);
  lcd.print(">Snake   Memory");
  delay(200);

  while(!joc_ales)
  {
    if(analogRead(X_pin) >= 1000)
    {
      if(joc != 1)
      {
        tone(sunet, 800, 50); 
        joc = 1;
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Choose Game");
        lcd.setCursor(1, 1);
        lcd.print(">Snake   Memory");
        delay(200);
      }
    }
    else if(analogRead(X_pin) <= 50) 
    {
      if(joc != 2)
      {
        tone(sunet, 800, 50);
        joc = 2;
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Choose Game");
        lcd.setCursor(2, 1);
        lcd.print("Snake  >Memory");
        delay(200);
      }
    }

    if(digitalRead(SW_pin) == LOW)
    {
      joc_ales = true;
    }
  }
}
