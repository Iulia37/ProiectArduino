#include "snake.h"
#include <Arduino.h>

void startSnakeGame()
{
    tone(sunet, 500, 100);
    delay(120);
    tone(sunet, 700, 100);

    game_over = false;
    este_mar = false;
    directie = 0;
    lcd.clear();
    score = 0;
    lcd.setCursor(0, 0);
    lcd.print("Score:");
    lcd.setCursor(7, 0);
    lcd.print(score);
    lcd.setCursor(0, 1);
    lcd.print("High Score: ");
    lcd.setCursor(12, 1);
    lcd.print(highScore);

    //resetez sarpele
    snake[0].lin = 4;
    snake[0].col = 0;  
    snake[1].lin = 4;
    snake[1].col = 1;
    head = 1;

    //curat boardul
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        board[row][col] = 0;
      }
    }
    board[4][0] = board[4][1] = 1;
}

bool validare(int lin, int col)
{
  for(int i = 0; i <= head; i++)
  {
    if(snake[i].lin == lin && snake[i].col == col)
    {
      return false;
    }
  }
  return true;
}

void mar_spawn()
{
    este_mar = true;
    mar.lin = random(0, 8);
    mar.col = random(0, 8);
    while(!validare(mar.lin, mar.col))
    {
      mar.lin = random(0, 8);
      mar.col = random(0, 8);
    }

    board[mar.lin][mar.col] = 1;
}

void snakeGameOver()
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
  viteza_selectata = false;
  if(score > highScore)
  {
    EEPROM.put(0, score);
    highScore = score;
  }
}

void miscare()
{
    ultimaMiscare = millis();

    int nouLin = (snake[head].lin + dl[directie] + 8) % 8;
    int nouCol = (snake[head].col + dc[directie] + 8) % 8;

    if(!validare(nouLin, nouCol))
    {
      snakeGameOver();
    }
    else
    {
      tone(sunet, 1000, 20);
      if(nouLin == mar.lin && nouCol == mar.col && este_mar)
      {
        board[mar.lin][mar.col] = 0;
        este_mar = false;
        head++;
        snake[head].lin = mar.lin;
        snake[head].col = mar.col;
        board[snake[head].lin][snake[head].col] = 1;

        score++;
        lcd.setCursor(7, 0);
        lcd.print(" ");
        lcd.setCursor(7, 0);
        lcd.print(score);

        tone(sunet, 900, 100);
      }
      else
      {
        board[snake[0].lin][snake[0].col] = 0;
        for (int i = 0; i < head; i++) 
        {
          snake[i] = snake[i + 1];
        }
        snake[head].lin = nouLin;
        snake[head].col = nouCol;
        board[snake[head].lin][snake[head].col] = 1;
      }
    }
}

void selectare_viteza()
{
  pozitie_viteza = 1;
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Choose speed");
  lcd.setCursor(1, 1);
  lcd.print(">Slow    Fast");
  delay(200);

  while(!viteza_selectata)
  {
    if(analogRead(X_pin) >= 900)
    {
      if(pozitie_viteza != 1)
      {
        tone(sunet, 800, 50); 
        pozitie_viteza = 1;
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Choose speed");
        lcd.setCursor(1, 1);
        lcd.print(">Slow    Fast");
        delay(200);
      }
    }
    else if(analogRead(X_pin) <= 150) 
    {
      if(pozitie_viteza != 2)
      {
        tone(sunet, 800, 50);
        pozitie_viteza = 2;
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Choose speed");
        lcd.setCursor(2, 1);
        lcd.print("Slow   >Fast");
        delay(200);
      }
    }

    if(digitalRead(SW_pin) == LOW)
    {
      viteza_selectata = true;
      if(pozitie_viteza == 1)
      {
        viteza = 700; 
      }
      else
      {
        viteza = 400; 
      }
      tone(sunet, 600, 50);
    }
  }
}

void SNAKE()
{
    if(!viteza_selectata) 
    {
      selectare_viteza();
    } 
      
    if(game_over) 
    {
      startSnakeGame();
    }

    if(!game_over)
    {
      if(!este_mar)
      {
        mar_spawn();
      }

      afisare();

      if(analogRead(Y_pin) >= 900 && directie != 3)
      {
        directie = 1; //sus
      }

      if(analogRead(Y_pin) <= 150 && directie != 1)
      {
        directie = 3; //jos
      }

      if(analogRead(X_pin) <= 150 && directie != 2)
      {
        directie = 0; //dreapta
      }

      if(analogRead(X_pin) >= 900 && directie != 0)
      {
        directie = 2; //stanga
      }

      if(millis() - ultimaMiscare >= viteza)
      {
        miscare();
        if(game_over)
        {
          while(1)
          {
            if(digitalRead(SW_pin) == LOW)
            {
              joc_ales = false;
              break;
            }
          }
        }
      }
    }
}

