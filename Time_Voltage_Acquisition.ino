// --- Code d'Acquisition Pro : Déclencheur Automatique (Trigger) ---

const int pinTension = 34;

// Variables pour notre chronomètre intelligent
bool testDemarre = false;       // Un "drapeau" pour savoir si on a commencé
unsigned long tempsDepart = 0;  // Pour mémoriser l'heure exacte du clic

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("En attente de l'allumage du moteur...");
  Serial.println("Temps(ms),Tension(V)"); // L'en-tête pour Excel
}

void loop() {
  // 1. Lire et calculer la tension
  int valeurBrute = analogRead(pinTension);
  float tensionLueParESP = (valeurBrute / 4095.0) * 3.3;
  float tensionMoteur = tensionLueParESP * 5.0;

  // 2. LE DÉCLENCHEUR (TRIGGER)
  // Si le test n'a pas encore démarré ET que la tension monte d'un coup...
  if (testDemarre == false && tensionMoteur > 2.0) {
    testDemarre = true;         // On lève le drapeau : le test commence !
    tempsDepart = millis();     // On enregistre ce moment précis comme étant le "Zéro"
  }

  // 3. ENREGISTREMENT DES DONNÉES
  // Si le drapeau est levé, on affiche les données en continu
  if (testDemarre == true) {
    // On calcule le vrai temps écoulé depuis le clic
    unsigned long tempsEcoule = millis() - tempsDepart;
    
    // Affichage au format CSV : Temps,Tension
    Serial.print(tempsEcoule);
    Serial.print(",");
    Serial.println(tensionMoteur);
  }

  // 4. On prend 10 mesures par seconde
  delay(100); 
}