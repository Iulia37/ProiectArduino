#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

///pentru lcd ul de scor
const int rs = 2, en = 8, d4 = A0, d5 = A1, d6 = 3, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int contrast = 75;

// DIN, CLK, CS, numar matrici (1 pentru o singură matrice 8x8)
LedControl lc = LedControl(11, 12, 13, 1);

bool board[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
  };

struct led_matrice
{
  int lin, col;
}snake[70], secventa[101];

int head; // ultimul element din vector, capul sarpelui
int top; // numarul de cadrane din secventa de memorat
int sunet = 7;
int viteza;
bool viteza_selectata = false;
int pozitie_viteza = 1; //1 in stanga si 2 in dreapta

int highScore = 0;

const int SW_pin = 9;
const int X_pin = A2; 
const  int Y_pin = A3; 

unsigned long ultimaMiscare = 0;
int directie = 0; // 0 - dreapta, 1 - sus, 2 - stanga, 3 - jos
int dl[4] = {0, -1, 0, 1};
int dc[4] = {1, 0, -1, 0};

bool este_mar = false;
led_matrice mar;
led_matrice mutare;

bool game_over = true;
int score = 0;

int joc = 1;
bool joc_ales = false;
bool repetare = false;
int cnt_miscari = 0;
bool initializare_joc = false;

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
      repetare = true;
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

void setup() 
{
  randomSeed(analogRead(A5)); 

  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, INPUT_PULLUP);
  pinMode(sunet, OUTPUT);

  lc.shutdown(0, false);      // Activează matricea
  lc.setIntensity(0, 8);      // Luminozitate (0-15)
  lc.clearDisplay(0);

  analogWrite(6, contrast);
  lcd.begin(16, 2);
}


void loop() {

  if(!joc_ales)
  {
    alege_joc();
  }

  if(joc_ales && joc == 1)
  {
    EEPROM.get(0, highScore);
    SNAKE();
  }

  if(joc_ales && joc == 2)
  {
    EEPROM.get(2, highScore);
    MEMORY();
  }

}
