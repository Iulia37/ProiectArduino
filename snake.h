#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

extern LiquidCrystal lcd;
extern LedControl lc;

extern bool board[8][8];

struct led_matrice {
  int lin, col;
};

extern led_matrice snake[70];
extern led_matrice secventa[101];

extern int head;
extern int top;
extern int sunet;
extern int viteza;
extern bool viteza_selectata;
extern int pozitie_viteza;

extern int highScore;

extern const int SW_pin;
extern const int X_pin;
extern const int Y_pin;

extern unsigned long ultimaMiscare;
extern int directie;
extern int dl[4];
extern int dc[4];

extern bool este_mar;
extern led_matrice mar;
extern led_matrice mutare;

extern bool game_over;
extern int score;

extern int joc;
extern bool joc_ales;
extern bool repetare;
extern int cnt_miscari;
extern bool initializare_joc;

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

void startGame()
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

void miscare()
{
    ultimaMiscare = millis();

    int nouLin = (snake[head].lin + dl[directie] + 8) % 8;
    int nouCol = (snake[head].col + dc[directie] + 8) % 8;

    if(!validare(nouLin, nouCol))
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
    if(analogRead(X_pin) >= 1000)
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
    else if(analogRead(X_pin) <= 50) 
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
      startGame();
    }

    if(!game_over)
    {
      if(!este_mar)
      {
        mar_spawn();
      }

      afisare();

      if(analogRead(Y_pin) >= 1000 && directie != 3)
      {
        directie = 1; //sus
      }

      if(analogRead(Y_pin) <= 40 && directie != 1)
      {
        directie = 3; //jos
      }

      if(analogRead(X_pin) <= 40 && directie != 2)
      {
        directie = 0; //dreapta
      }

      if(analogRead(X_pin) >= 1000 && directie != 0)
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