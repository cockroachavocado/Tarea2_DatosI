// =======================================================
// SISTEMA DE RIEGO AUTOMÁTICO + CLIMA (versión final)
// Sin repeticiones de PEDIR_CLIMA, con hora correcta y
// actualización continua de estados de humedad.
// =======================================================


// --------------------- CLASE SENSOR ---------------------
class SensorHumedad {
  int pin;
  int humedadMin;
  int humedadMax;
  int valorActual;

  bool apagado;
  int cadaCuantosDias;
  int horaInicio;
  int minutosInicio;
  int horaFinal;
  int minutosFinal;

  unsigned long ultimoRiego;

public:
  // ----- MÉTODOS GETTER -----
  int getDias() { return cadaCuantosDias; }
  int getHoraInicio() { return horaInicio; }
  int getHoraFinal() { return horaFinal; }
  int getMinInicio() { return minutosInicio; }
  int getMinFinal() { return minutosFinal; }
  int getMinHumedad() { return humedadMin; }
  int getMaxHumedad() { return humedadMax; }
  bool getApagado() { return apagado; }

  SensorHumedad(int pinSensor = A0) {
    pin = pinSensor;
    humedadMin = 0;
    humedadMax = 100;
    valorActual = 0;
    apagado = false;
    cadaCuantosDias = 1;
    horaInicio = 0;
    minutosInicio = 0;
    horaFinal = 0;
    minutosFinal = 0;
    ultimoRiego = 0;
  }

  void configurarRangos(int minH, int maxH) {
    humedadMin = minH;
    humedadMax = maxH;
  }

  void configurarRiego(bool a, int d, int h1, int m1, int h2, int m2) {
    apagado = a;
    cadaCuantosDias = d;
    horaInicio = h1;
    minutosInicio = m1;
    horaFinal = h2;
    minutosFinal = m2;
  }

  void leer() {
    valorActual = analogRead(pin);
    valorActual = map(valorActual, 0, 1023, 100, 0); // convierte a porcentaje
  }

  int leerHumedad() { return valorActual; }
  int getMin() { return humedadMin; }
  int getMax() { return humedadMax; }

  bool necesitaRiego() {
    return valorActual < humedadMin;
  }

  bool excesoHumedad() {
    return valorActual >= humedadMax;
  }

  bool tocaRegar(int hora, int minutos, int dia) {
    if (apagado) {
      return false;
    }
    if ((dia - ultimoRiego) % cadaCuantosDias != 0) {
      return false;
    }
    int tiempoActual = hora * 60 + minutos;
    int tiempoInicio = horaInicio * 60 + minutosInicio;
    int tiempoFin = horaFinal * 60 + minutosFinal;

    // Si el rango NO cruza de día (ej: 8:00 a 18:00)
    if (tiempoInicio <= tiempoFin) {
      if (tiempoActual >= tiempoInicio && tiempoActual <= tiempoFin) {
        return true;
      }
    }
    // Si el rango SÍ cruza de día (ej: 22:00 a 2:00)
    else {
      if (tiempoActual >= tiempoInicio || tiempoActual <= tiempoFin) {
        return true;
      }
    }

    return false;
  }

  void registrarRiego(int dia) {
    ultimoRiego = dia;
  }
};


// --------------------- CLASE SISTEMA ---------------------
class Sistema {
  SensorHumedad zonas[3];
  int pinBomba;
  int hora;
  int minuto;
  int dias;
  unsigned long tiempoAnterior;

public:
  Sistema(int p, int a, int b, int c)
    : pinBomba(p),
      zonas{SensorHumedad(a), SensorHumedad(b), SensorHumedad(c)} {
    hora = 0;
    minuto = 0;
    dias = 0;
    tiempoAnterior = millis();
  }

  void preparar() {
    Serial.println("=== CONFIGURACIÓN DEL SISTEMA ===");
  }

  void configurarHora(int h, int m) {
    hora = h;
    minuto = m;
  }

  void actualizarHora() {
    unsigned long ahora = millis();

    if (ahora - tiempoAnterior >= 60000UL) {  // 1 minuto real
      tiempoAnterior = ahora;
      minuto++;
      if (minuto >= 60) {
        minuto = 0;
        hora++;
        if (hora >= 24) {
          hora = 0;
          dias++;
        }
      }
    }
  }

  void verificarRiego() {
    bool activarBomba = false;

    Serial.print("📨 Hora actual: ");
    if (hora < 10) Serial.print("0");
    Serial.print(hora);
    Serial.print(":");
    if (minuto < 10) Serial.print("0");
    Serial.println(minuto);
    Serial.println("------------------------------------");

    for (int i = 0; i < 3; i++) {
      zonas[i].leer();
      int humedad = zonas[i].leerHumedad();

      Serial.print("📨 Zona ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(humedad);
      Serial.print("% | Rango: ");
      Serial.print(zonas[i].getMin());
      Serial.print("-");
      Serial.print(zonas[i].getMax());
      Serial.print("%  ");

      // --- Evaluar condiciones de riego ---
      if (zonas[i].necesitaRiego()) {
        Serial.println("(💧 Necesita riego)");
        activarBomba = true;
      }
      else if (zonas[i].tocaRegar(hora, minuto, dias)) {
        Serial.println("(🕒 Toca riego)");
        activarBomba = true;
      }
      else if (zonas[i].excesoHumedad()) {
        Serial.println("(💦 Exceso de humedad)");
      }
      else {
        Serial.println("(OK)");
      }
    }

    // --- Encender o apagar bomba al final ---
    if (activarBomba) {
      Serial.println("⚙️ Activando bomba...");
      digitalWrite(pinBomba, HIGH);
    } else {
      Serial.println("💧 Bomba apagada.");
      digitalWrite(pinBomba, LOW);
    }

    Serial.println("------------------------------------");
  }


  SensorHumedad& getZona(int i) {
    return zonas[i];
  }
};


// --------------------- VARIABLES GLOBALES ---------------------
Sistema sistema(8, A0, A1, A2);
int estado = 0;
int data;
int probLluvia = -1;
bool climaRecibido = false;
bool configurado = false;
bool climaSolicitado = false;

bool horaSolicitada = false;
bool horaConfigurada = false;
bool minutosSolicitados = false;
bool s1c = false;
bool s2c = false;
bool s3c = false;

bool s1activo = false;
bool s1d = false;
bool sensor1min = false;
bool sensor1max = false;
bool horaS1A = false;
bool minS1A= false;
bool horaS1B = false;
bool minS1B = false;

bool s2activo = false;
bool s2d = false;
bool sensor2min = false;
bool sensor2max = false;
bool horaS2A = false;
bool minS2A= false;
bool horaS2B = false;
bool minS2B = false;

bool s3activo = false;
bool s3d = false;
bool sensor3min = false;
bool sensor3max = false;
bool horaS3A = false;
bool minS3A= false;
bool horaS3B = false;
bool minS3B = false;

int hora, mins, min, max;

unsigned long ultimaVerificacion = 0;
unsigned long ultimaPeticionClima = 0;



// --------------------- SETUP ---------------------
void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  Serial.println("Arduino iniciado. Esperando configuración...");
}

// --------------------- LOOP PRINCIPAL ---------------------
void loop() {
  unsigned long ahora = millis();

  // --- Leer datos desde Python ---
  if (Serial.available() > 0) {
    int dato = Serial.parseInt();

    if (estado == 0) { 
      // ====== 1️⃣ CLIMA ======
      if (dato >= 0 && dato <= 100) {
        probLluvia = dato;
        climaRecibido = true;
        climaSolicitado = false;
        Serial.print("🌦️ Probabilidad de lluvia recibida: ");
        Serial.print(probLluvia);
        Serial.println("%");
        Serial.println("------------------------------------");
        estado = 1;
      }
    }

    else if (estado == 1) { 
      // ====== 2️⃣ HORA ======
      hora = dato;
      while (!Serial.available()) ;  // esperar minutos
      mins = Serial.parseInt();
      sistema.configurarHora(hora, mins);
      horaConfigurada = true;
      Serial.print("🕒 Hora inicial: ");
      Serial.print(hora);
      Serial.print(":");
      Serial.println(mins);
      estado = 2;
    }

    else if (estado == 2) {
      // ====== 3️⃣ ZONA 1 ======
      static int datosZona[8];
      static int index = 0;
      datosZona[index++] = dato;

      if (index == 8) {
        sistema.getZona(0).configurarRiego(
          datosZona[0], datosZona[1], datosZona[2], datosZona[3],
          datosZona[4], datosZona[5]);
        sistema.getZona(0).configurarRangos(datosZona[6], datosZona[7]);
        Serial.println("✅ Zona 1 configurada correctamente.");
        index = 0;
        estado = 3;
      }
      Serial.print("Zona configurada: apagado=");
      Serial.print(datosZona[0]);
      Serial.print(", dias=");
      Serial.print(datosZona[1]);
      Serial.print(", horaInicio=");
      Serial.print(datosZona[2]);
      Serial.print(", horaFin=");
      Serial.print(datosZona[3]);
      Serial.print(", minInicio=");
      Serial.print(datosZona[4]);
      Serial.print(", minFin=");
      Serial.println(datosZona[5]);
    }

    else if (estado == 3) {
      // ====== 4️⃣ ZONA 2 ======
      static int datosZona[8];
      static int index = 0;
      datosZona[index++] = dato;

      if (index == 8) {
        sistema.getZona(1).configurarRiego(
          datosZona[0], datosZona[1], datosZona[2], datosZona[3],
          datosZona[4], datosZona[5]);
        sistema.getZona(1).configurarRangos(datosZona[6], datosZona[7]);
        Serial.println("✅ Zona 2 configurada correctamente.");
        index = 0;
        estado = 4;
      }
    }

    else if (estado == 4) {
      // ====== 5️⃣ ZONA 3 ======
      static int datosZona[8];
      static int index = 0;
      datosZona[index++] = dato;

      if (index == 8) {
        sistema.getZona(2).configurarRiego(
          datosZona[0], datosZona[1], datosZona[2], datosZona[3],
          datosZona[4], datosZona[5]);
        sistema.getZona(2).configurarRangos(datosZona[6], datosZona[7]);
        Serial.println("✅ Zona 3 configurada correctamente.");
        index = 0;
        estado = 5;
        Serial.println("✅ Configuración completa.");
      }
    }
  }

  // --- Pedir clima una vez por hora ---
  if (estado == 0 && !climaSolicitado && 
      (ultimaPeticionClima == 0 || (millis() - ultimaPeticionClima >= 3600000UL))) {
    Serial.println("PEDIR_CLIMA");
    ultimaPeticionClima = millis();
    climaSolicitado = true;
  }

  // --- Verificar riego cada 10 s ---
  if (estado == 5 && climaRecibido && (ahora - ultimaVerificacion >= 10000UL)) {
    ultimaVerificacion = ahora;

    if (probLluvia > 70) {
      Serial.println("🌧️ Alta probabilidad de lluvia, no se activará el riego.");
      digitalWrite(8, LOW);
    } else {
      sistema.actualizarHora();
      sistema.verificarRiego();
    }
  }

  delay(200);
}
