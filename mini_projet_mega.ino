#include <Keypad.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h>  // Inclut la bibliothèque Servo

Servo monServo;  // Crée un objet Servo pour contrôler le servomoteur

int pinServo = 10;  // Broche de contrôle du servomoteur (peut être 9, 10, 11, etc.)

// Crée un objet lcd pour contrôler l'écran
rgb_lcd lcd;

// Définition des broches pour le clavier matriciel
#define L1 9
#define L2 8
#define L3 7
#define L4 6
#define C1 5
#define C2 4
#define C3 3
#define C4 2

// Définition du clavier
const byte lignes = 4;       // 4 lignes
const byte colonnes = 4;     // 4 colonnes

char code[lignes][colonnes] = {   // code des touches
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte broches_lignes[lignes] = {L1, L2, L3, L4};     // Broches des lignes
byte broches_colonnes[colonnes] = {C1, C2, C3, C4}; // Broches des colonnes

Keypad clavier = Keypad(makeKeymap(code), broches_lignes, broches_colonnes, lignes, colonnes);

// Variables pour la gestion du code
String codeSaisi = "";          // Code en cours de saisie
String codeCorrect = "1234";    // Code correct (à adapter si besoin)
int tentatives = 0;             // Compteur des tentatives
bool codeValide = false;        // Variable pour savoir si le code est valide

void setup() 
{
  Serial.begin(9600);           // Initialisation du moniteur série
  lcd.begin(16, 2);             // Initialisation de l'écran 16x2
  lcd.setRGB(0, 213, 255);      // Couleur de fond bleu clair
  lcd.print("Saisir code :");   // Affiche le texte initial
  monServo.attach(pinServo);  // Attache le servomoteur à la broche 9 (ou autre)
}

void loop() {
  if (tentatives >= 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Acces bloque !");  // Message de blocage
    lcd.setCursor(0, 1);
    lcd.print("Error-Code-1034");  // Message de blocage
    lcd.setRGB(255, 0, 0);      // Fond rouge pour signaler l'erreur
    while (1);                  // Boucle infinie pour bloquer le programme
  }

  char touche = clavier.getKey();  // Vérifie si une touche a été pressée

  if (touche != NO_KEY) {          // Si une touche est appuyée
    Serial.println(touche);        // Affiche la touche dans le moniteur série

    if (touche == '*') {
      // Supprime le dernier caractère si on appuie sur '*'
      if (codeSaisi.length() > 0) {
        codeSaisi.remove(codeSaisi.length() - 1); // Supprime le dernier caractère
        lcd.setCursor(0, 1);
        lcd.print("                ");           // Efface la ligne
        lcd.setCursor(0, 1);
        lcd.print(codeSaisi);                   // Réaffiche le code partiel
      }
    } else if (touche == '#') {
      // Valide le code si on appuie sur '#'
      if (codeSaisi.length() == 4) {            // Vérifie que le code est de 4 chiffres
        if (codeSaisi == codeCorrect) {
          codeValide = true;                   // Le code est valide
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Bienvenue !");             // - - - - - ICI CODE BON :: ACCES AUTORISE :: - - - - -
          lcd.setRGB(0, 255, 0);                // Fond vert pour signaler le succès

          // Tourner le servomoteur seulement si le code est valide
          monServo.write(0);  // Position de départ à 0° (cela peut être différent selon le modèle)
          delay(1000);  // Attends 1 seconde
          
          monServo.write(90); // Position à 90°
          delay(1000);  // Attends 1 seconde
          
          monServo.write(180); // Position à 180°
          delay(1000);  // Attends 1 seconde

          monServo.write(180); // Continue à tourner dans un sens (par exemple, antihoraire)
          delay(5000);  // Tourner pendant 5 secondes
          
          monServo.write(0);  // Arrêter le servomoteur
          delay(5000);  // Arrêt pendant 5 secondes

        } else {
          tentatives++;                         // Incrémente les tentatives
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Code incorrect");
          lcd.setRGB(255, 0, 0);                // Fond rouge pour signaler l'erreur
          delay(2000);                          // Pause pour afficher l'erreur
          lcd.setRGB(0, 213, 255);              // Retour à la couleur initiale
          lcd.clear();
          lcd.print("Saisir code :");
          codeSaisi = "";                       // Réinitialise la saisie
        }
      } else {
        lcd.setCursor(0, 1);
        lcd.print("Code incomplet");
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("                ");          // Efface le message d'erreur
        lcd.setCursor(0, 1);
        lcd.print(codeSaisi);                   // Réaffiche le code saisi
      }
    } else {
      // Ajoute une touche au code si ce n'est pas '*' ou '#'
      if (codeSaisi.length() < 4) {             // Empêche de saisir plus de 4 caractères
        codeSaisi += touche;
        lcd.setCursor(0, 1);
        lcd.print(codeSaisi);                   // Affiche le code saisi
      }
    }
  }

  // Si le code est valide et que le servomoteur n'a pas encore tourné, on le fait tourner
  if (codeValide) {
    // Le servomoteur peut tourner ici en fonction de ta logique.
    // L'exécution du servomoteur se fait seulement lorsque codeValide devient vrai.
  }
}
