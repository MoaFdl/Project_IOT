// --- DATI BASE ---
#define UNITA_MISURA "segnale analogico"
#define TIPO_SENSORE "battito APDS-9008"
#define DESCRIZIONE_ARD "arduino uno r3 collegato a bottone e sensore APDS-9008"

// --- CONFIGURAZIONE PIN ---
#define PULSE_SENSOR_PIN A0 // Pin analogico del sensore APDS-9008
#define  BUTTON_PIN 8       // Pin digitale del pulsante
#define  LED_PIN LED_BUILTIN // LED integrato (solitamente pin 13)


// --- VARIABILI GLOBALI ---
int Signal;                      // Valore letto dal sensore
bool isMeasuring = false;        // Stato del sistema (se sta misurando o no)
unsigned long startTime = 0;     // Variabile per memorizzare l'inizio del timer


const unsigned long TOTAL_TIME = 10000;    // Durata totale misurazione
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  
  Serial.begin(115200);
  Serial.println(UNITA_MISURA);
  Serial.println(TIPO_SENSORE);
  Serial.println(DESCRIZIONE_ARD);
}

void loop() {
  // 1. CONTROLLO DEL BOTTONE
  // Se non stiamo misurando e il bottone viene premuto (LOW perché va a GND)
  if (!isMeasuring && digitalRead(BUTTON_PIN) == LOW) {
    isMeasuring = true;
    delay(500); // Piccolo ritardo per evitare rimbalzi del tasto (debounce)
    startTime = millis(); // Salva il tempo attuale
    
  }
  if (isMeasuring) {
    unsigned long tempoPassato = millis() - startTime; // Tempo trascorso dall'avvio
  if (tempoPassato < TOTAL_TIME) {
      Signal = analogRead(PULSE_SENSOR_PIN);
      Serial.println(Signal); 
      digitalWrite(LED_PIN, HIGH); // Accendiamo il LED per indicare che stiamo registrando
    }
    else {
      isMeasuring = false;
      digitalWrite(LED_PIN, LOW);
      Serial.println("Ciclo terminato. Premi il bottone per riavviare.");
      Serial.println(UNITA_MISURA);
      Serial.println(TIPO_SENSORE);
      Serial.println(DESCRIZIONE_ARD);
    }
    
    delay(50); // Mantiene la frequenza di campionamento gestibile
  }
}