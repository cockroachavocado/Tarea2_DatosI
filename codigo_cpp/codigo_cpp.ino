
// Clase para los sensores
class SensorHumedad {
  int pin;
  int humedadMin;
  int humedadMax;
  int valorActual;

  // para la configuración de riego
  bool apagado;
  int cadaCuantosDias;
  int horaInicio;
  int minutosInicio;
  int horaFinal;
  int minutosFinal;
  
  // no sé qué significa esto xd, parece ser una referencia de último riego
  unsigned long ultimoRiego;

public:
  SensorHumedad(int pinSensor = A0) {
    pin = pinSensor;
    humedadMin = 0;
    humedadMax = 100;
    valorActual = 0;
  }

  void configurarRangos(int minH, int maxH) {
    humedadMin = minH;
    humedadMax = maxH;
  }

  void configurarRiego(bool a, int d, int h1, int h2, int m1, int m2) {
    apagado = a;
    cadaCuantosDias = d;
    horaInicio = h1;
    horaFinal = h2;
    minutosInicio = m1;
    minutosFinal = m2;
  }

  void leer() {
    valorActual = analogRead(pin);
    // Se covnierte la lectura (0-1023) a porcentaje (0-100)
    valorActual = map(valorActual, 0, 1023, 100, 0);
  }

  int leerHumedad() { return valorActual; }

  bool necesitaRiego() {
    return valorActual < humedadMin;
  }

  bool excesoHumedad() {
    return valorActual >= humedadMax;
  }

  bool tocaRegar(int hora, int minutos, int dia) {
    if (apagado) return false;
    if ((dia - ultimoRiego) % cadaCuantosDias != 0) return false;

    //compara la hora actual con un rango programado
    if (hora > horaInicio || (hora == horaInicio && minutos >= minutosInicio)) {
      if (hora < horaFinal || (hora == horaFinal && minutos <= minutosFinal)) {
        if (necesitaRiego()) {
          return true;
        }
      }
    }
    return false;
  }

  void registrarRiego(int dia) {
    ultimoRiego = dia;
  }

  void imprimirInfo(int id) {
    Serial.print("Zona ");
    Serial.print(id);
    Serial.print(": ");
    Serial.print(valorActual);
    Serial.println("% | Rango: ");
    Serial.print(humedadMin);
    Serial.print("-");
    Serial.print(humedadMax);
    Serial.println("%");
  }
};

// Clase del sistema como tal
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

  // Configuración de la hora
  void configurarHora(int h, int m) {
    hora = h;
    minuto = m;
  }

  void actualizarHora() {
    unsigned long ahora = millis();

    // Cada 60 mil ms es un minuto
    if (ahora - tiempoAnterior >= 60000UL) {
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

      // Imprimir la hora actual
      Serial.print("Tiempo actual:");
      if (hora < 10) Serial.print("0");
      Serial.print(hora);
      Serial.print(":");
      if (minuto < 10) Serial.print("0");
      Serial.print(minuto);
      Serial.print(" | Día");
      Serial.println(dias);
    }
  }


  // Lógica principal
  void verificiarRiego() {
    bool activarBomba = false;
    for (int i = 0; i < 3; i++) {
      zonas[i].leer();
      zonas[i].imprimirInfo(i + 1);

      if (zonas[i].tocaRegar(hora, minuto, dias)) {
        activarBomba = true;
        zonas[i].registrarRiego(dias);
      }
    }
    if (activarBomba) {
      Serial.println("Activando bomba...");
      digitalWrite(pinBomba, HIGH);
    } else {
      digitalWrite(pinBomba, LOW);
      Serial.println("No hace falta regar...");
    }
    Serial.println("");
  }

  SensorHumedad& getZona(int i) {
    return zonas[i];
  } 


  void iniciar() {
    pinMode(pinBomba, OUTPUT);
    digitalWrite(pinBomba, LOW);

    Serial.println("=== CONFIGURACIÓN DEL SISTEMA ===");

    // --- Lectura y validación de la hora ---
    int horaIngresada = -1;
    int minutoIngresado = -1;

    do {
      Serial.print("Ingrese la hora inicial (0-23): ");
      while (Serial.available() == 0); // espera entrada
      horaIngresada = Serial.parseInt();
      while (Serial.available() > 0) Serial.read(); // limpia el buffer

      if (horaIngresada < 0 || horaIngresada > 23) {
        Serial.println("Hora inválida. Debe ser un número entre 0 y 23.\n");
      }
    } while (horaIngresada < 0 || horaIngresada > 23);

    do {
      Serial.print("Ingres los minutos actuales (0 - 59): ");
      while (Serial.available() == 0);
      minutoIngresado = Serial.parseInt();
      while (Serial.available() > 0) Serial.read();
      if (minutoIngresado < 0 || minutoIngresado > 59)
        Serial.println("Minuto inválido. Debe ser un número entre 0 y 59. \n");
    } while (minutoIngresado < 0 || minutoIngresado > 59);

    configurarHora(horaIngresada, minutoIngresado);
    Serial.print("Hora inicial guardada: ");
    Serial.println(hora);
    Serial.print(":");
    Serial.println(minuto);
    Serial.println();


    // Configuración por zona
    for (int i = 0; i < 3; i++) {
      Serial.print("Configuración de zona");
      Serial.print(i + 1);
      Serial.println("");

      bool apagado = false;
      int opcion = 0;
      do {
        Serial.println("¿La zona estará siempre apagada? (1: sí, 0: no): ");
        while (Serial.available() == 0);
        opcion = Serial.parseInt();
        while (Serial.available() > 0) Serial.read();
        if (opcion != 0 && opcion != 1)
          Serial.println("Opción inválida, ingrese 1 o 0. \n");
      } while (opcion != 0 && opcion != 1);
      apagado = (opcion == 1);

      // Cada cuánto se riega
      int f = 1;
      if (!apagado) {
        do {
          Serial.print("¿Cada cuántos días se regará? (valor mayor o igual a 1): ");
          while (Serial.available() == 0);
          f = Serial.parseInt();
          while (Serial.available() > 0) Serial.read();
          if (f < 1)
            Serial.println("Valor inválido, debe ser mayor o igual a 1");
        } while (f < 1);
      }

      //Horario de riego
      int hI = 0, mI = 0, hF = 0, mF = 0;
      if (!apagado) {
        do {
          Serial.print("Ingrese hora de inicio (0 - 23): ");
          while (Serial.available() == 0);
          hI = Serial.parseInt();
          while (Serial.available() > 0) Serial.read();
        } while (hI < 0 || hI > 23);
        Serial.println("");

        do {
          Serial.print("Ingrese minuto de inicio (0-59): ");
          while (Serial.available() == 0);
          mI = Serial.parseInt();
          while (Serial.available() > 0) Serial.read();
        } while (mI < 0 || mI > 59);
        Serial.println("");

        do {
          Serial.print("Ingrese hora de fin (0-23): ");
          while (Serial.available() == 0);
          hF = Serial.parseInt();
          while (Serial.available() > 0) Serial.read();
        } while (hF < 0 || hF > 23);
        Serial.println("");

        do {
          Serial.print("Ingrese minuto de fin (0-59): ");
          while (Serial.available() == 0);
          mF = Serial.parseInt();
          while (Serial.available() > 0) Serial.read();
        } while (mF < 0 || mF > 59);
        Serial.println("");
        Serial.println("");
      }

      // Rangos de humedad
      int minH = 0, maxH = 0;
      do {
        Serial.print("Ingrese humedad mínima (0-100): ");
        while (Serial.available() == 0);
        minH = Serial.parseInt();
        while (Serial.available() > 0) Serial.read();
      } while (minH < 0 || minH > 100);
      Serial.print(minH);
      Serial.println("");

      do {
        Serial.print("Ingrese humedad máxima (0-100): ");
        while (Serial.available() == 0);
        maxH = Serial.parseInt();
        while (Serial.available() > 0) Serial.read();
      } while (maxH < 0 || maxH > 100 || maxH <= minH);
      Serial.println(maxH);

      zonas[i].configurarRangos(minH, maxH);
      zonas[i].configurarRiego(apagado, f, hI, mI, hF, mF);

      Serial.println("✅ Zona configurada correctamente.\n");
    }

    Serial.println("Configuración completada correctamente.");
    Serial.println("------------------------------------");

  }

  void actualizarTiempo() {
    unsigned long tiempoActual = millis();

    // 1 hora = 3600000 ms (tiempo real)
    if (tiempoActual - tiempoAnterior >= 3600000UL) {
      tiempoAnterior = tiempoActual;
      hora++;
      if (hora % 24 == 0) {
        dias++;
      }
      Serial.print("Hora actual: ");
      Serial.print(hora % 24);
      Serial.print(":00  |  Día: ");
      Serial.println(dias);
    }
  }

  void verificarRiego() {
    bool activarBomba = false;

    for (int i = 0; i < 3; i++) {
      zonas[i].leer();
      zonas[i].imprimirInfo(i + 1);

      if (zonas[i].necesitaRiego()) {
        activarBomba = true;
      }
    }

    if (activarBomba) {
      Serial.println("⚙️ Activando bomba...");
      digitalWrite(pinBomba, HIGH);
    } else {
      digitalWrite(pinBomba, LOW);
      Serial.println("💧 Humedad suficiente, bomba apagada.");
    }

    Serial.println("------------------------------------");
  }
};

// =========================
// CONFIGURACIÓN PRINCIPAL
// =========================

Sistema sistema(8, A0, A1, A2);  // Bomba en pin 8, sensores en A0-A2

void setup() {
  Serial.begin(9600);
  delay(1000);
  sistema.iniciar();
}

void loop() {
  sistema.actualizarTiempo();
  sistema.verificarRiego();
  delay(2000); // leer cada 2 segundos
}
