unsigned long startMillis;
int hora, minuto;
int dia, mes, max;
bool horaConfigurada = false;
bool dummy_mes = true;
bool dummy_dia = true;
bool dummy_hora = true;
bool dummy_minuto = true;

void setup() {
  Serial.begin(9600);
  Serial.println("Ingrese la fecha y hora. Primero el mes, luego el día, luego la hora (formato militar), luego los minutos");
}

void loop() {
  // Si llega algo por el monitor serial
  if (Serial.available()) {
    String entrada = Serial.readStringUntil('\n');
    entrada.trim();
    while (dummy_mes) {
      
      int mes_prueba;
      if ((sscanf(entrada.c_str(), "%d", &mes_prueba) == 1) && (0 < mes_prueba && mes_prueba < 13)) {
        mes = mes_prueba;
        dummy_mes = false;
        Serial.println("Mes guardado correctamente");
        switch (mes) {
          case 1: case 3: case 5: case 7: case 8: case 10: case 12: max = 32; break;
          case 2: max = 29; break;
          case 4: case 6: case 9: case 11: max = 31; break;
        }
        Serial.println("Ingrese el día");
        String entrada = Serial.readStringUntil('\n');
        return;
      } else {
        Serial.println("Por favor, ingrese un mes correcto (1 - 12)");
        String entrada = Serial.readStringUntil('\n');
        return;
      }

    }
    while (dummy_dia) {
      
      int dia_prueba;
      if ((sscanf(entrada.c_str(), "%d", &dia_prueba) == 1) && (0 < dia_prueba && dia_prueba < max)) {
        dia = dia_prueba;
        dummy_dia = false;
        Serial.println("Día guardado correctamente");
        Serial.println("Ingrese la hora");
        String entrada = Serial.readStringUntil('\n');
        return;

      } else {
        Serial.println("Por favor, ingrese un día correcto (1 -");
        Serial.println(max);
        Serial.println(")");
        String entrada = Serial.readStringUntil('\n');
        return; 
      }

    }
    while (dummy_hora) {
      
      int hora_prueba;
      if ((sscanf(entrada.c_str(), "%d", &hora_prueba) == 1) && (-1 < hora_prueba && hora_prueba < 25)) {
        hora = hora_prueba;
        dummy_hora = false;
        Serial.println("Hora guardada correctamente");
        Serial.println("Ingrese los minutos");
        String entrada = Serial.readStringUntil('\n');
        return;
      } else {
        Serial.println("Por favor, ingrese una hora correcta (1 - 24)");
        String entrada = Serial.readStringUntil('\n');
        return;
      }

    }
    while (dummy_minuto) {
      
      int minuto_prueba;
      if ((sscanf(entrada.c_str(), "%d", &minuto_prueba) == 1) && (-1 < minuto_prueba && minuto_prueba < 60)) {
        minuto = minuto_prueba;
        dummy_minuto = false;
        Serial.println("Minutos guardados correctamente");
        horaConfigurada = true;
        startMillis = millis();
        String entrada = Serial.readStringUntil('\n');
        return;

      } else {
        Serial.println("Por favor, ingrese minutos correctos");
        String entrada = Serial.readStringUntil('\n');
        return;
      }
    }
  }
}
