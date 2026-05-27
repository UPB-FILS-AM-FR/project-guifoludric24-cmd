

#include <LiquidCrystal.h>


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int PIN_DO = 2;
const int PIN_AO = A0;  

const int PIN_BUZZER = 10;
const int PIN_LED    = 11;


const int SEUIL_ANALOGIQUE = 50;  
const int NB_CALIBRATION   = 20;

int  valeurBase    = 0;    
int  valeurCourante = 0;   
bool metalDetecte  = false;

int calibrer() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration...");
  lcd.setCursor(0, 1);
  lcd.print("Pas de metal SVP");

  long somme = 0;
  for (int i = 0; i < NB_CALIBRATION; i++) {
    somme += analogRead(PIN_AO);
    delay(50);
  }
  int moyenne = (int)(somme / NB_CALIBRATION);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Base:");
  lcd.print(moyenne);
  delay(1500);
  lcd.clear();

  return moyenne;
}

void declencherAlarme() {
  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
}


void eteindrAlarme() {
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_BUZZER, LOW);
}


void afficherLCD(int valBase, int valCourante, bool detection) {
  int delta = abs(valCourante - valBase);

  lcd.setCursor(0, 0);
  lcd.print("AO:");
  lcd.print(valCourante);
  lcd.print(" D:");
  lcd.print(delta);
  lcd.print("   ");   

  lcd.setCursor(0, 1);
  if (detection) {
    lcd.print("*** METAL ***   ");
  } else {
    lcd.print("Aucun metal     ");
  }
}

void setup() {
  
  pinMode(PIN_DO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED, LOW);

 
  lcd.begin(16, 2);
  lcd.print("Detecteur Hall");
  lcd.setCursor(0, 1);
  lcd.print("KY-024 / Arduino");
  delay(1500);

  
  valeurBase = calibrer();

  lcd.print("Pret !");
  delay(800);
  lcd.clear();
}


void loop() {
  
  valeurCourante = analogRead(PIN_AO);

 
  bool doDetecte = (digitalRead(PIN_DO) == LOW);  

  
  int delta = abs(valeurCourante - valeurBase);
  metalDetecte = doDetecte || (delta >= SEUIL_ANALOGIQUE);

 
  if (metalDetecte) {
    declencherAlarme();
  } else {
    eteindrAlarme();
  }

  
  afficherLCD(valeurBase, valeurCourante, metalDetecte);

  delay(100);
}
