#include "memory.h"
#include <Arduino.h>

void startMemoryGame()
{
  initializare_joc = true;

  tone(sunet, 500, 100);
  delay(120);
  tone(sunet, 700, 100);

  game_over = false;
  score = 0;
  top = 0;
  repetare = false;
  cnt_miscari = 0;
  mutare.lin = mutare.col = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("High Score: ");
  lcd.setCursor(12, 1);
  lcd.print(highScore);
    
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      board[row][col] = 0;
    }
  }

}

void afisare_chenar(led_matrice led)
{
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      board[led.lin + i][led.col + j] = 1;
    }
  }
  afisare();
}

void afisare_secventa_chenare()
{
  int next = random(1, 5); // generaz urmatorul chenar
  switch(next) 
  {
    case 1: 
      secventa[top].lin = secventa[top].col = 0;
      break;
    case 2: 
      secventa[top].lin = 0; secventa[top].col = 4;
      break;
    case 3: 
      secventa[top].lin = 4; secventa[top].col = 0; 
      break;
    case 4: 
      secventa[top].lin = secventa[top].col = 4;
      break;
    }
    top++;

    for(int i = 0; i < top; i++)  // afisez secevnta de repetat
    {
      sterge();
      afisare();
      delay(300);
      afisare_chenar(secventa[i]);
      tone(sunet, 1000, 150);
      delay(1000);
    }
}

void memoryGameOver()
{
  tone(sunet, 400, 200);
  delay(250);
  tone(sunet, 200, 300);
  game_over = true;
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Game Over");
  lcd.setCursor(4, 1);
  lcd.print("Score: ");
  lcd.setCursor(11, 1);
  lcd.print(score);
  repetare = false;
  initializare_joc = false;

  if(score > highScore)
  {
    EEPROM.put(2, score);
    highScore = score;
  }

  while(1)
  {
    if(digitalRead(SW_pin) == LOW)
    {
      joc_ales = false;
      break;
    }
  }
}

void MEMORY()
{
  if(!initializare_joc)
  {
    startMemoryGame();
  }

  while(!game_over)
  { 
    if(!repetare)
    {
      afisare_secventa_chenare();

      repetare = true; // marcam ca s-a afisat secventa
      cnt_miscari = 0;
      mutare.lin = mutare.col = 0;

      tone(sunet, 500, 100);
      delay(120);
      tone(sunet, 800, 100);
    }
    else
    {
      sterge();
      afisare_chenar(mutare);
      if(analogRead(Y_pin) >= 1000)
      {
        mutare.lin = 0;
      }

      if(analogRead(Y_pin) <= 40)
      {
        mutare.lin = 4;
      }

      if(analogRead(X_pin) <= 40)
      {
        mutare.col = 4;
      }

      if(analogRead(X_pin) >= 1000)
      {
        mutare.col = 0;
      }

      if(digitalRead(SW_pin) == LOW)
      {
        delay(500);
        if(mutare.lin == secventa[cnt_miscari].lin && mutare.col == secventa[cnt_miscari].col)
        {
          tone(sunet, 1000, 150);
          cnt_miscari++;
        }
        else
        {
          memoryGameOver();
        }
      }

      if(cnt_miscari == top)
      {
        tone(sunet, 800, 100);
        delay(150);
        tone(sunet, 1000, 100);
        delay(150);
        tone(sunet, 1200, 200);

        score++;
        lcd.setCursor(7, 0);
        lcd.print(" ");
        lcd.setCursor(7, 0);
        lcd.print(score);

        repetare = false;
        delay(500);
      }
    }
  }
}
