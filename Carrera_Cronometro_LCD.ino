
// ============================================================
//   SISTEMA DE CRONOMETRAJE PARA ATLETISMO - Arduino UNO
//   v2.0 - Con LCD 16x2, Bocina Activa y Cronómetro en vivo
// ============================================================

#include <LiquidCrystal.h>

// --- PINES LCD (RS, EN, D4, D5, D6, D7) ---
const int PIN_RS = 8;
const int PIN_EN = 9;
const int PIN_D4 = 10;
const int PIN_D5 = 11;
const int PIN_D6 = 12;
const int PIN_D7 = 13;

LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

// --- PINES SISTEMA ---
const int PIN_BOTON          = 7;
const int PIN_LED_ROJO       = 3;
const int PIN_LED_VERDE      = 4;
const int PIN_TRIG           = 5;
const int PIN_ECHO           = 6;
const int PIN_LASER          = 2;
const int PIN_BOCINA         = A1;  // Bocina activa en pin analógico usado como digital
const int PIN_FOTORESISTENCIA = A0;

// --- CONFIGURACIÓN ---
const float DISTANCIA_PIE_CM  = 5.0;
const int   UMBRAL_META       = 300;
const int   NUM_PARPADEOS     = 5;
const int   INTERVALO_PARPADEO = 1000;

// --- VARIABLES ---
enum Estado {
  ESPERA_BOTON,
  VERIFICANDO_POSICION,
  SECUENCIA_LUCES,
  EN_CARRERA,
  CARRERA_TERMINADA
};

Estado estadoActual = ESPERA_BOTON;
unsigned long tiempoInicio   = 0;
unsigned long tiempoCarrera  = 0;
unsigned long ultimoLCD      = 0;

// ============================================================
void setup() {
  Serial.begin(9600);

  pinMode(PIN_BOTON,           INPUT_PULLUP);
  pinMode(PIN_LED_ROJO,        OUTPUT);
  pinMode(PIN_LED_VERDE,       OUTPUT);
  pinMode(PIN_TRIG,            OUTPUT);
  pinMode(PIN_ECHO,            INPUT);
  pinMode(PIN_LASER,           OUTPUT);
  pinMode(PIN_BOCINA,          OUTPUT);
  pinMode(PIN_FOTORESISTENCIA, INPUT);

  digitalWrite(PIN_LASER,      HIGH);
  digitalWrite(PIN_LED_ROJO,   LOW);
  digitalWrite(PIN_LED_VERDE,  LOW);
  digitalWrite(PIN_BOCINA,     LOW);

  randomSeed(analogRead(A2));

  // Iniciar LCD
  lcd.begin(16, 2);
  mostrarPantallaBienvenida();

  Serial.println("=================================");
  Serial.println("  SISTEMA DE CRONOMETRAJE LISTO  ");
  Serial.println("  Presiona el botón para iniciar ");
  Serial.println("=================================");
}

// ============================================================
void loop() {
  switch (estadoActual) {

    // --------------------------------------------------
    case ESPERA_BOTON:
      if (digitalRead(PIN_BOTON) == LOW) {
        delay(50);
        if (digitalRead(PIN_BOTON) == LOW) {
          Serial.println("\n>> Botón presionado. Verifica posición...");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Coloquese en la");
          lcd.setCursor(4, 1);
          lcd.print("Salida...");
          estadoActual = VERIFICANDO_POSICION;
        }
      }
      break;

    // --------------------------------------------------
    case VERIFICANDO_POSICION:
      if (obtenerDistancia() <= DISTANCIA_PIE_CM) {
        Serial.println(">> Pie detectado. Iniciando secuencia...");
        estadoActual = SECUENCIA_LUCES;
        ejecutarSecuenciaLuces();
      } else {
        delay(300);
      }
      break;

    // --------------------------------------------------
    case EN_CARRERA:
    {
      // Actualizar cronómetro en LCD cada 50ms
      if (millis() - ultimoLCD >= 50) {
        ultimoLCD = millis();
        unsigned long transcurrido = millis() - tiempoInicio;
        mostrarTiempoLCD(transcurrido, false);
      }

      // Verificar cruce de meta
      int valorFoto = analogRead(PIN_FOTORESISTENCIA);
      if (valorFoto > UMBRAL_META) {
        tiempoCarrera = millis() - tiempoInicio;
        estadoActual = CARRERA_TERMINADA;

        digitalWrite(PIN_LED_VERDE, LOW);

        // Bocina celebración: 3 pitidos cortos
        for (int i = 0; i < 3; i++) {
          digitalWrite(PIN_BOCINA, HIGH);
          delay(150);
          digitalWrite(PIN_BOCINA, LOW);
          delay(100);
        }


mostrarTiempoLCD(tiempoCarrera, true);

        Serial.println("\n>> ¡META CRUZADA!");
        Serial.print("   Tiempo: ");
        Serial.print(tiempoCarrera / 1000.0, 2);
        Serial.println(" segundos");
        Serial.println("=================================");
        Serial.println("  Presiona botón para reiniciar  ");
        Serial.println("=================================");
      }
    }
    break;

    // --------------------------------------------------
    case CARRERA_TERMINADA:
      digitalWrite(PIN_LED_ROJO,  LOW);
      digitalWrite(PIN_LED_VERDE, LOW);
      if (digitalRead(PIN_BOTON) == LOW) {
        delay(50);
        if (digitalRead(PIN_BOTON) == LOW) {
          Serial.println("\n>> Reiniciando sistema...");
          mostrarPantallaBienvenida();
          estadoActual = ESPERA_BOTON;
        }
      }
      break;
  }
}

// ============================================================
//   SECUENCIA DE LUCES CON BOCINA
// ============================================================
void ejecutarSecuenciaLuces() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  PREPARADOS...");

  for (int i = 0; i < NUM_PARPADEOS; i++) {

    // Verificar salida en falso
    if (obtenerDistancia() > DISTANCIA_PIE_CM) {
      Serial.println("!! SALIDA EN FALSO - Reiniciando");
      digitalWrite(PIN_LED_ROJO,  LOW);
      digitalWrite(PIN_LED_VERDE, LOW);
      digitalWrite(PIN_BOCINA,    LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" SALIDA EN FALSO");
      lcd.setCursor(0, 1);
      lcd.print("  Reiniciando...");
      delay(2000);
      estadoActual = VERIFICANDO_POSICION;
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Vuelva a la");
      lcd.setCursor(4, 1);
      lcd.print("Salida...");
      return;
    }

    // LED + Bocina encendidos
    digitalWrite(PIN_LED_ROJO, HIGH);
    digitalWrite(PIN_BOCINA,   HIGH);
    lcd.setCursor(0, 1);
    lcd.print("  Luz ");
    lcd.print(i + 1);
    lcd.print(" de ");
    lcd.print(NUM_PARPADEOS);
    lcd.print("     ");
    Serial.print("   Luz roja ");
    Serial.print(i + 1);
    Serial.println(" encendida");
    delay(500);

    // LED + Bocina apagados
    digitalWrite(PIN_LED_ROJO, LOW);
    digitalWrite(PIN_BOCINA,   LOW);
    delay(500);
  }

  // Espera aleatoria monitoreando el pie
  long tiempoAleatorio = random(0, 5001);
  Serial.print(">> Espera aleatoria: ");
  Serial.print(tiempoAleatorio / 1000.0, 2);
  Serial.println(" seg...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  En posicion...");

  unsigned long inicioEspera = millis();
  while (millis() - inicioEspera < tiempoAleatorio) {
    if (obtenerDistancia() > DISTANCIA_PIE_CM) {
      Serial.println("!! SALIDA EN FALSO durante espera - Reiniciando");
      digitalWrite(PIN_BOCINA, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" SALIDA EN FALSO");
      lcd.setCursor(0, 1);
      lcd.print("  Reiniciando...");
      delay(2000);
      estadoActual = VERIFICANDO_POSICION;
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Vuelva a la");
      lcd.setCursor(4, 1);
      lcd.print("Salida...");
      return;
    }
    delay(50);
  }

  // ¡LUZ VERDE + BOCINA!
  digitalWrite(PIN_LED_VERDE, HIGH);
  digitalWrite(PIN_BOCINA,    HIGH);
  delay(300);                         // Pitido de salida más largo
  digitalWrite(PIN_BOCINA,    LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  EN CARRERA!   ");
  lcd.setCursor(0, 1);
  lcd.print("Tiempo: 0.00s   ");

  tiempoInicio = millis();
  Serial.println("\n>> ¡LUZ VERDE! ¡SALIDA!");
  estadoActual = EN_CARRERA;
}

// ============================================================
//   MOSTRAR TIEMPO EN LCD
// ============================================================
void mostrarTiempoLCD(unsigned long ms, bool esFinal) {
  unsigned long segundos   = ms / 1000;
  unsigned long centesimas = (ms % 1000) / 10;

  lcd.setCursor(0, 0);
  if (esFinal) {
    lcd.print("  TIEMPO FINAL  ");
  } else {
    lcd.print("  EN CARRERA!   ");
  }


lcd.setCursor(0, 1);
  lcd.print("Tiempo: ");
  if (segundos < 10) lcd.print("0");
  lcd.print(segundos);
  lcd.print(".");
  if (centesimas < 10) lcd.print("0");
  lcd.print(centesimas);
  lcd.print("s      ");
}

// ============================================================
//   PANTALLA DE BIENVENIDA
// ============================================================
void mostrarPantallaBienvenida() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" CRONOMETRO ");
  lcd.setCursor(0, 1);
  lcd.print(" Pulsa el boton ");
}

// ============================================================
//   DISTANCIA ULTRASÓNICO
// ============================================================
float obtenerDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duracion = pulseIn(PIN_ECHO, HIGH, 30000);
  return duracion * 0.0343 / 2.0;
}

