// --- BROCHES (Pins) ---
const int pinTension = 34; // Capteur de tension (Analogique)
const int pinVitesse = 14; // Capteur de vitesse (Numérique DO)

// --- VARIABLES POUR LA VITESSE ET LE FILTRE ---
volatile unsigned long compteurImpulsions = 0; 
volatile unsigned long dernierTempsImpulsion = 0; // NOUVEAU : Mémoire pour le filtre anti-bruit
unsigned long tempsPrecedent = 0;
const int intervalleAffichage = 1000; // Prendre une mesure toutes les 100 ms
const float trousParTour = 20.0; // Disque standard de 20 trous

// --- VARIABLES POUR LE DÉCLENCHEUR (L'INTERRUPTEUR) ---
bool experienceDemarree = false; 
unsigned long tempsZero = 0;     

// --- FONCTION D'INTERRUPTION (AVEC LE FILTRE) ---
void IRAM_ATTR compterTrous() {
  unsigned long tempsActuelInterruption = micros(); // On regarde l'heure en microsecondes
  
  // LE FILTRE : Si plus de 2000 microsecondes (2 ms) se sont écoulées, c'est un VRAI trou !
  // Sinon (les parasites ultra-rapides du moteur), on les ignore complètement.
  if (tempsActuelInterruption - dernierTempsImpulsion > 2000) {
    compteurImpulsions++;
    dernierTempsImpulsion = tempsActuelInterruption;
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(pinTension, INPUT);
  pinMode(pinVitesse, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(pinVitesse), compterTrous, FALLING);

  Serial.println("Temps(ms);Tension(V);Vitesse(RPM)");
  Serial.println("En attente de la fermeture de l'interrupteur..."); 
}

void loop() {
  unsigned long tempsActuel = millis();

  if (tempsActuel - tempsPrecedent >= intervalleAffichage) {
    
    // 1. LECTURE DE LA TENSION
    int valeurAnalogique = analogRead(pinTension);
    
    // --- LA FORMULE COMPLÈTE INTÉGRÉE ---
    float tension = (valeurAnalogique / 4095.0) * 3.3 * 5.0; 

    // 2. VÉRIFICATION DE L'INTERRUPTEUR (Le Déclencheur)
    if (experienceDemarree == false) {
      if (tension > 1.0) { // Si la tension dépasse 1V, le bouton a été pressé !
        experienceDemarree = true;
        tempsZero = tempsActuel; // On bloque le chronomètre à 0
        compteurImpulsions = 0;  // On met la vitesse à 0 pour le départ
      } else {
        // Le bouton n'est pas encore pressé, on patiente
        tempsPrecedent = tempsActuel;
        return; 
      }
    }

    // 3. CALCULS PENDANT L'EXPÉRIENCE
    // Le chronomètre démarre officiellement ici
    unsigned long tempsEcoule = tempsActuel - tempsZero;

    // Récupération sécurisée des impulsions de vitesse
    noInterrupts();
    unsigned long impulsions = compteurImpulsions;
    compteurImpulsions = 0;
    interrupts();
    
    // Calcul de la vitesse en Tours Par Minute (RPM)
    float vitesseRPM = (impulsions / trousParTour) * (60000.0 / intervalleAffichage);

    // 4. AFFICHAGE POUR EXCEL (SUR LE MONITEUR SÉRIE / PUTTY)
    Serial.print(tempsEcoule); 
    Serial.print(";");
    Serial.print(tension, 2); // Affiche la tension avec 2 chiffres après la virgule
    Serial.print(";");
    Serial.println(vitesseRPM, 0); // Affiche la vitesse sans virgule (entier)

    // Mise à jour du temps pour le prochain cycle
    tempsPrecedent = tempsActuel;
  }
}