#include <SPI.h>
#include <SD.h>

#define SD_CS 10 // Chip Select pour votre DFR0229
#define FILE_NAME "/T_Multiplication2.csv"   // Nom du fichier

File myFile;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation de la carte SD...");

  // Initialisation SD
  if (!SD.begin(SD_CS)) {
    Serial.println("Échec de l'initialisation de la carte SD !");
    return;
  }
  Serial.println("Carte SD initialisée avec succès.");

  // *** Fonctions de base ***
  //Ecrire_CarteSD(FILE_NAME,"TEST CREE");
  //Ajoute_CarteSD(FILE_NAME,"TEST AJOUT");
  //Lire_CarteSD_AllLine(FILE_NAME);

  // Génération du tableau de données
  Serial.println(""); 
  Serial.print("Ecriture des tables de multiplication");
  myFile = SD.open(FILE_NAME, FILE_WRITE);   //Normalement append mais le détecte pas
  if (myFile) {
    for(int i = 1; i <=10; i++){
      for(int j = 1; j <=10; j++){
        myFile.print(String(i));
        myFile.print("x");
        myFile.print(String(j));
        myFile.print("=");
        myFile.print(String(i*j));
        myFile.print("\t");
      }
      myFile.println("");
    }
    myFile.println("");  
    myFile.close();

  } else {
    Serial.println("Impossible d'ouvrir test.txt en écriture.");
  }
  Serial.println(": Terminé");
  Serial.println("");
  
  // Lecture et afichage des donnnées enregistrées
  Serial.println("Lecture des données présentes dans la carte SD");
  Lire_CarteSD_AllLine(FILE_NAME);
}

void loop() {
}

// ***************   CREE ET ECRIT DU TEXTE DANS UN FICHIER   ****************
void Ecrire_CarteSD(String file, String message)
{
  myFile = SD.open(file, FILE_WRITE);
  if (myFile) {
    myFile.println(message);
    myFile.close();
  } else {
    Serial.println("Impossible d'ouvrir en écriture.");
  }
}

// ******************   AJOUTE DU TEXTE  DANS UN FICHIER   *******************
void Ajoute_CarteSD(String file, String message)
{
  myFile = SD.open(file, FILE_WRITE);   //Normalement append mais le détecte pas
  if (myFile) {
    myFile.println(message);
    myFile.close();
  } else {
    Serial.println("Impossible d'ouvrir en ajout.");
  }
}

// ****************   LIT TOUTES LES LIGNES D'UN FICHIER   ******************
void Lire_CarteSD_AllLine(String file)
{
  myFile = SD.open(file, FILE_READ);
  if (myFile) {
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("Impossible d'ouvrir le fichier en lecture.");
  }
}
