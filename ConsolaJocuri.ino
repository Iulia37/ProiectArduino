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
