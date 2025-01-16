#include <Arduino_MKRGPS.h>
#include <MKRNB.h>  // MKR NB-IoT bibliotheek

// Timer-variabelen
unsigned long lastUpdateTime = 0;             // Tijd van de laatste update
const unsigned long updateInterval = 30000;   // 30 seconden interval
unsigned long lastConnectTime = 0;            // Tijd van laatste poging tot verbinden
const unsigned long connectInterval = 10000;  // 10 seconden interval voor proberen verbinding te maken
int updateCount = 0;                          // Teller voor aantal updates

bool useSerial = true;
bool useLed = true;
bool useNBDebug = true;

// GPS en NB-IoT variabelen
const char apn[] = "iot.1nce.net";  // Vervang door de APN van je provider
const char user[] = "";             // Laat leeg als er geen gebruikersnaam is
const char pass[] = "";             // Laat leeg als er geen wachtwoord is
const char pin[] = "";

NB nbAccess(useNBDebug);
NBClient nbClient(useNBDebug);

// LED pinnen
const int internetLedPin = 6;  // Rode LED voor internet
const int gpsLedPin = 7;       // Blauwe LED voor GPS fix
const int errorLedPin = 1;     // Extra LED voor fout bij verbinden

bool connectedToInternet = false;
bool gpsFix = false;

unsigned long lastGpsBlinkTime = 0;          // Tijd van de laatste knipper voor de GPS LED
const unsigned long gpsBlinkInterval = 500;  // Interval voor knipperen van de GPS LED (500ms)

void setup() {
  if (useSerial) {
    Serial.begin(9600);
    while (!Serial)
      ;  // Wacht tot de seriële verbinding beschikbaar is
    Serial.println("Starten van GPS en NB-IoT Test...");
  }

  // Zet de LED's uit bij de start]
  if (useLed) {
    pinMode(internetLedPin, OUTPUT);
    pinMode(gpsLedPin, OUTPUT);
    pinMode(errorLedPin, OUTPUT);
    digitalWrite(internetLedPin, LOW);
    digitalWrite(gpsLedPin, LOW);
    digitalWrite(errorLedPin, LOW);
  }

  // Controleer of de GPS-module correct is aangesloten
  if (useSerial) {
    Serial.println("Initialiseren van GPS-module...");
  }
  if (!GPS.begin()) {
    if (useSerial) {
      Serial.println("Fout: GPS-module kon niet worden geïnitialiseerd. Controleer de verbindingen!");
    }
    digitalWrite(errorLedPin, HIGH);
    while (1)
      ;  // Stop het programma als de module niet werkt
  } else {
    if (useSerial) {
      Serial.println("GPS-module succesvol geactiveerd.");
    }
  }
}

void loop() {
  // Probeer verbinding te maken met het NB-IoT netwerk
  if (!connectedToInternet && millis() - lastConnectTime >= connectInterval) {
    lastConnectTime = millis();
    attemptNBConnection();
  }

  // Controleer of het tijd is voor een update van GPS
  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();
    updateCount++;
    if (useSerial) {
      Serial.print("Update #");
      Serial.println(updateCount);

      // Print de verstreken tijd
      Serial.print("Verstreken tijd: ");
      printElapsedTime();
      Serial.println();
    }

    // Update GPS-data
    updateGPSData();
  }

  // Blijf knipperen met de GPS LED totdat er een fix is
  if (useLed) {
    if (!gpsFix && millis() - lastGpsBlinkTime >= gpsBlinkInterval) {
      lastGpsBlinkTime = millis();
      digitalWrite(gpsLedPin, !digitalRead(gpsLedPin));  // Wissel de staat van de LED (aan/uit)
    }
  }
}

// Functie om verbinding te maken met NB-IoT zonder blokkeren
void attemptNBConnection() {
  if (useSerial) {
    Serial.println("Probeer verbinding te maken met het NB-IoT netwerk...");
  }
  if (useLed) {
    digitalWrite(internetLedPin, LOW);  // Zet de LED uit tijdens de verbinding
    digitalWrite(errorLedPin, LOW);     // Zet fout-LED uit
  }

  if (nbAccess.begin(pin, apn, user, pass) == NB_READY) {
    connectedToInternet = true;
    if (useSerial) {
      Serial.println("Succesvol verbonden met NB-IoT netwerk!");
    }
    if (useLed) {
      digitalWrite(internetLedPin, HIGH);  // Zet de internet-LED aan
    }
  } else {
    connectedToInternet = false;
    if (useSerial) {
      Serial.print("Verbinding mislukt. Statuscode: ");
    }
    if (useLed) {
      digitalWrite(errorLedPin, HIGH);  // Zet fout-LED aan
    }
  }
}

// Functie om GPS-data bij te werken
void updateGPSData() {
  if (GPS.available()) {
    float latitude = GPS.latitude();
    float longitude = GPS.longitude();

    if (!isnan(latitude) && !isnan(longitude)) {
      gpsFix = true;
      if (useSerial) {
        Serial.print("GPS-fix gevonden: Latitude = ");
        Serial.print(latitude, 6);
        Serial.print(", Longitude = ");
        Serial.println(longitude, 6);
      }
      if (useLed) {
        digitalWrite(gpsLedPin, HIGH);  // GPS LED blijft aan bij fix
      }
    } else {
      gpsFix = false;
      if (useSerial) {
        Serial.println("Nog geen GPS-fix gevonden.");
      }
    }
  } else {
    gpsFix = false;
    if (useSerial) {
      Serial.println("Geen GPS-data beschikbaar.");
    }
  }
}

// Functie om de verstreken tijd in minuten en seconden te printen
void printElapsedTime() {
  unsigned long elapsedSeconds = millis() / 1000;  // milliseconden naar seconden
  int minutes = elapsedSeconds / 60;               // Aantal minuten
  int seconds = elapsedSeconds % 60;               // Overgebleven seconden

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
}
