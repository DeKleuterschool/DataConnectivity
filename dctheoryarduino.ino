#include <Arduino_MKRGPS.h>
#include <MKRNB.h> // MKR NB-IoT bibliotheek

// Timer-variabelen
unsigned long lastUpdateTime = 0;  // Tijd van de laatste update
const unsigned long updateInterval = 30000;  // 30 seconden interval
unsigned long lastConnectTime = 0;  // Tijd van laatste poging tot verbinden
const unsigned long connectInterval = 10000;  // 10 seconden interval voor proberen verbinding te maken
int updateCount = 0;  // Teller voor aantal updates

// GPS en NB-IoT variabelen
const char apn[] = "iot.1nce.net";  // Vervang door de APN van je provider
const char user[] = "";  // Laat leeg als er geen gebruikersnaam is
const char pass[] = "";  // Laat leeg als er geen wachtwoord is

NB nbAccess;
NBClient nbClient;

// LED pinnen
const int internetLedPin = 6;  // Rode LED voor internet
const int gpsLedPin = 7;       // Blauwe LED voor GPS fix
const int errorLedPin = 1;     // Extra LED voor fout bij verbinden

bool connectedToInternet = false;
bool gpsFix = false;

unsigned long lastGpsBlinkTime = 0; // Tijd van de laatste knipper voor de GPS LED
const unsigned long gpsBlinkInterval = 500; // Interval voor knipperen van de GPS LED (500ms)

void setup() {
  // Zet de LED's uit bij de start
  pinMode(internetLedPin, OUTPUT);
  pinMode(gpsLedPin, OUTPUT);
  pinMode(errorLedPin, OUTPUT);  // Zet pin 1 als output voor de extra LED
  digitalWrite(internetLedPin, LOW);  // Zet de internet LED uit
  digitalWrite(gpsLedPin, LOW);       // Zet de GPS LED uit
  digitalWrite(errorLedPin, LOW);     // Zet de error LED uit

  // Controleer of de GPS-module correct is aangesloten
  if (!GPS.begin()) {
    while (1); // Stop het programma als de module niet werkt
  } else {
  }
}

void loop() {
  // Probeer verbinding te maken met het NB-IoT netwerk
  if (millis() - lastConnectTime >= connectInterval && !connectedToInternet) {
    lastConnectTime = millis();
    
    // Zet de LED's om te knipperen tijdens poging tot verbinding
    digitalWrite(internetLedPin, HIGH); // LED aan om knipperen te starten
    delay(500);  // Wacht een korte tijd
    digitalWrite(internetLedPin, LOW);  // LED uit
    delay(500);  // Wacht een korte tijd

    /*int status = nbAccess.begin(apn, user, pass);
    if (status == NB_READY) {
      connectedToInternet = true;
      Serial.println("Succesvol verbonden met het internet via NB-IoT.");
      digitalWrite(internetLedPin, HIGH); // Zet de LED continu aan als verbonden
      digitalWrite(errorLedPin, LOW);     // Zet de error LED uit bij succes
    } else {
      connectedToInternet = false;
      Serial.println("Fout: Geen verbinding met NB-IoT netwerk.");
      digitalWrite(internetLedPin, LOW);  // Zet de LED uit als de verbinding mislukt
      digitalWrite(errorLedPin, HIGH);    // Zet de error LED aan als het niet lukt
    }*/
  }

  // Controleer of het tijd is voor een update van GPS
  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();
    updateCount++;  // Verhoog de update-teller

    // Update GPS-data
    if (GPS.available()) {
      float latitude = GPS.latitude();
      float longitude = GPS.longitude();

      if (!isnan(latitude) && !isnan(longitude)) {
        gpsFix = true;  // GPS-fix gevonden
        // Zet de GPS LED continu aan bij GPS fix
        digitalWrite(gpsLedPin, HIGH);  // GPS LED blijft aan bij fix
      } else {
        gpsFix = false;  // GPS-fix nog niet gevonden
      }
    } else {
      gpsFix = false;  // Geen GPS data beschikbaar
    }
  }

  // Blijf knipperen met de GPS LED totdat er een fix is
  if (!gpsFix && millis() - lastGpsBlinkTime >= gpsBlinkInterval) {
    lastGpsBlinkTime = millis();
    digitalWrite(gpsLedPin, !digitalRead(gpsLedPin)); // Wissel de staat van de LED (aan/uit)
  }
}

/* Functie om de verstreken tijd in minuten en seconden te printen
void printElapsedTime() {
  unsigned long elapsedSeconds = millis() / 1000;  // milliseconden naar seconden
  int minutes = elapsedSeconds / 60;                 // Aantal minuten
  int seconds = elapsedSeconds % 60;                 // Overgebleven seconden

  if (minutes > 0) {
    Serial.print(minutes);
    if (minutes == 1) {
      Serial.print(" minuut ");
    } else {
      Serial.print(" minuten ");
    }
  }
  Serial.print(seconds);
  if (seconds == 1) {
    Serial.print(" seconde");
  } else {
    Serial.print(" seconden");
  }
}*/
