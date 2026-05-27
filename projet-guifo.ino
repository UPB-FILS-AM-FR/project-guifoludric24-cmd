

#include <LiquidCrystal.h>


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


const int PIN_OSCILLATEUR = 2;   
const int PIN_LED         = 11;  


const unsigned long DUREE_MESURE_MS = 100;   
const int           NB_CALIBRATION  = 20;


const float SEUIL_VARIATION_PCT = 0.5;  

volatile unsigned long compteurImpulsions = 0;  
unsigned long frequenceBase   = 0;              
unsigned long frequenceCourante = 0;           
bool          metalDetecte    = false;

void ISR_compteur() {
  compteurImpulsions++;
}


unsigned long mesurerFrequence() {
  compteurImpulsions = 0;
  unsigned long debut = millis();

  
  attachInterrupt(digitalPinToInterrupt(PIN_OSCILLATEUR), ISR_compteur, RISING);
  while (millis() - debut < DUREE_MESURE_MS);
  detachInterrupt(digitalPinToInterrupt(PIN_OSCILLATEUR));

 
  unsigned long freq = (compteurImpulsions * 1000UL) / DUREE_MESURE_MS;
  return freq;
}


unsigned long calibrer() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration...");
  lcd.setCursor(0, 1);
  lcd.print("Ne pas approcher");

  unsigned long somme = 0;
  for (int i = 0; i < NB_CALIBRATION; i++) {
    somme += mesurerFrequence();
    delay(50);
  }
  unsigned long moyenne = somme / NB_CALIBRATION;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Base:");
  lcd.print(moyenne);
  lcd.print("Hz");
  delay(1500);

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


void afficherLCD(unsigned long freqBase, unsigned long freqCourante, float variation, bool detection) {
  lcd.setCursor(0, 0);
  lcd.print("F:");
  lcd.print(freqCourante);
  lcd.print("Hz  ");  

  lcd.setCursor(0, 1);
  if (detection) {
    lcd.print("*** METAL ***   ");
  } else {
    lcd.print("Var:");

    int varEntier  = (int)variation;
    int varDecimal = (int)((variation - varEntier) * 100);
    lcd.print(varEntier);
    lcd.print(".");
    if (varDecimal < 10) lcd.print("0");
    lcd.print(varDecimal);
    lcd.print("%  ");
  }
}
===
void setup() {
  
  pinMode(PIN_OSCILLATEUR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED, LOW);

  lcd.begin(16, 2);
  lcd.print("Detecteur LC");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation..");
  delay(1500);

 
  frequenceBase = calibrer();

  lcd.clear();
  lcd.print("Pret !");
  delay(800);
  lcd.clear();
}


void loop() {
  
  frequenceCourante = mesurerFrequence();

  float variation = 0.0;
  if (frequenceBase > 0) {
    long delta = (long)frequenceCourante - (long)frequenceBase;
    variation = (float)abs(delta) / (float)frequenceBase * 100.0;
  }

 
  metalDetecte = (variation >= SEUIL_VARIATION_PCT);

 
  if (metalDetecte) {
    declencherAlarme();
  } else {
    eteindrAlarme();
  }

  afficherLCD(frequenceBase, frequenceCourante, variation, metalDetecte);


  delay(50);
}
