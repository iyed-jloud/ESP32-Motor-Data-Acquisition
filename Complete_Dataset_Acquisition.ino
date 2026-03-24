// ==========================================
// CODE D'ACQUISITION FINAL (4 COLONNES)
// IA DATASET : Temps / Tension / Vitesse / Courant
// ==========================================

// --- BROCHES (Pins) ---
const int pinTension = 34; // Capteur de tension
const int pinCourant = 35; // Capteur de courant ACS712 (via diviseur)
const int pinVitesse = 14; // Capteur de vitesse

// --- VARIABLES POUR LA VITESSE ET LE FILTRE ---
volatile unsigned long compteurImpulsions = 0; 
volatile unsigned long dernierTempsImpulsion = 0; 
unsigned long tempsPrecedent = 0;
const int intervalleAffichage = 500; // 100 ms = 10 lignes de données par seconde !
const float trousParTour = 20.0;     // Disque standard de 20 trous

// --- VARIABLES POUR LE DÉCLENCHEUR (TRIGGER) ---
bool experienceDemarree = false; 
unsigned long tempsZero = 0;     

// --- FONCTION D'INTERRUPTION (FILTRE ANTI-BRUIT) ---
void IRAM_ATTR compterTrous() {
  unsigned long tempsActuelInterruption = micros();
  // Filtre à 2000 microsecondes (2ms) pour ignorer les parasites
  if (tempsActuelInterruption - dernierTempsImpulsion > 2000) {
    compteurImpulsions++;
    dernierTempsImpulsion = tempsActuelInterruption;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Configuration du capteur de vitesse
  pinMode(pinVitesse, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinVitesse), compterTrous, FALLING);

  // En-têtes pour ton fichier CSV
  Serial.println("En attente de l'allumage de l'interrupteur...");
  Serial.println("Temps(ms);Tension(V);Vitesse(RPM);Courant(A)"); 
}

void loop() {
  unsigned long tempsActuel = millis();

  // On fait une mesure toutes les "intervalleAffichage" (100 ms)
  if (tempsActuel - tempsPrecedent >= intervalleAffichage) {
    
    // 1. LECTURE DE LA TENSION
    int valTension = analogRead(pinTension);
    float tension = (valTension / 4095.0) * 3.3 * 5.0; 

    // --- LE DÉCLENCHEUR ---
    // Si la tension est faible, le moteur est éteint, on attend.
    if (experienceDemarree == false) {
      if (tension > 1.0) { // L'interrupteur est sur ON !
        experienceDemarree = true;
        tempsZero = tempsActuel; // On fixe le temps zéro
        compteurImpulsions = 0;  // On remet les compteurs à plat
      } else {
        tempsPrecedent = tempsActuel;
        return; // On annule le reste et on recommence la boucle
      }
    }

    // --- À PARTIR D'ICI, LE MOTEUR TOURNE ET ON ENREGISTRE ---

    // 2. TEMPS ÉCOULÉ
    unsigned long tempsEcoule = tempsActuel - tempsZero;

    // 3. VITESSE (Calcul sécurisé)
    noInterrupts();
    unsigned long impulsions = compteurImpulsions;
    compteurImpulsions = 0; // On remet à zéro pour les 100 prochaines millisecondes
    interrupts();
    float vitesseRPM = (impulsions / trousParTour) * (60000.0 / intervalleAffichage);

    // 4. COURANT (ACS712 30A + Diviseur de tension)
    int valCourant = analogRead(pinCourant);
    
    // a) Calcul de la tension reçue par la broche D35 (max 3.3V)
    float tensionD35 = (valCourant / 4095.0) * 3.3;
    
    // b) On annule l'effet de ton diviseur de tension (1k et 2k) pour retrouver le 5V
    // Multiplicateur = (R1 + R2) / R2 = (1000 + 2000) / 2000 = 1.5
    float tensionACS = tensionD35 * 1.5; 
    
    // c) Transformation en Ampères
    // À 0 Ampère, le capteur envoie 2.5V. 
    // Pour un 30A, la sensibilité est de 66mV (0.066V) par Ampère.
    float courantA = (tensionACS - 2.5) / 0.066;

    // Petit filtre logiciel : le capteur ACS712 tremble un peu naturellement.
    // Si on est très proche de 0 (bruit de fond), on force à 0 pour avoir une belle dataset.
    if (abs(courantA) < 0.15) {
      courantA = 0.0;
    }

    // 5. AFFICHAGE POUR EXCEL (SÉPARATEUR POINT-VIRGULE)
    Serial.print(tempsEcoule);
    Serial.print(";");
    Serial.print(tension);
    Serial.print(";");
    Serial.print(vitesseRPM);
    Serial.print(";");
    Serial.println(courantA);

    // On mémorise l'heure pour la prochaine boucle
    tempsPrecedent = tempsActuel; 
  }
}