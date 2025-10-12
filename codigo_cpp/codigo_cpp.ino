unsigned long startMillis;
int hora, minuto;
int dia, mes, max;
int humedad1min, humedad1max, humedad2min, humedad2max, humedad3min, humedad3max;
bool horaConfigurada = false;
bool dummy_mes, dummy_dia, dummy_hora, dummy_minuto = true;
bool dummy_dia1a, dummy_hora1a, dummy_minuto1a, dummy_dia1b, dummy_hora1b, dummy_minuto1b = true;
bool dummy_dia2a, dummy_hora2a, dummy_minuto2a, dummy_dia2b, dummy_hora2b, dummy_minuto2b = true;
bool dummy_dia3a, dummy_hora3a, dummy_minuto3a, dummy_dia3b, dummy_hora3b, dummy_minuto3b = true;
bool dummy_humedad_min1, dummy_humedad_max1 = true;
bool dummy_humedad_min2, dummy_humedad_max2 = true;
bool dummy_humedad_min3, dummy_humedad_max3 = true;
bool dummy_lluvia = true;

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
        Serial.println("Ingrese la humedad mínima del sensor 1");
        String entrada = Serial.readStringUntil('\n');
        return;
      } else {
        Serial.println("Por favor, ingrese minutos correctos");
        String entrada = Serial.readStringUntil('\n');
        return;
      }
    }










    while (dummy_humedad_min1) {
      int humedad1A;
      if ((sscanf(entrada.c_str(), "%d", &humedad1A) == 1) && humedad1A >= 0) {
        humedad1min = humedad1A;
        dummy_humedad_min1 = false;
        Serial.println("Humedad mínima del sensor 1 guardada correctamente");
        Serial.println("Ingrese la humedad máxima del sensor 1");
        String entrada = Serial.readStringUntil('\n');
        return;

      } else {
        Serial.println("Por favor, ingrese una humedad mínima válida");
        String entrada = Serial.readStringUntil('\n');
        return;
      }
    }
    while (dummy_humedad_max1) {
      int humedad1B;
      if ((sscanf(entrada.c_str(), "%d", &humedad1B) == 1) && humedad1B > humedad1min) {
        humedad1max = humedad1B;
        dummy_humedad_max1 = false;
        Serial.println("Humedad máxima del sensor 1 guardada correctamente");
        Serial.println("Ingrese la humedad mínima del sensor 2");
        String entrada = Serial.readStringUntil('\n');
        return;

      } else {
        Serial.println("Por favor, ingrese una humedad máxima válida");
        String entrada = Serial.readStringUntil('\n');
        return;
      }
    }



    while (dummy_humedad_min2) {
      int humedad2A;
      if ((sscanf(entrada.c_str(), "%d", &humedad2A) == 1) && humedad2A >= 0) {
        humedad2min = humedad2A;
        dummy_humedad_min2 = false;
        Serial.println("Humedad mínima del sensor 2 guardada correctamente");
        Serial.println("Ingrese la humedad máxima del sensor 2");
        String entrada = Serial.readStringUntil('\n');
        return;

      } else {
        Serial.println("Por favor, ingrese una humedad mínima válida");
        String entrada = Serial.readStringUntil('\n');
        return;
      }
    }
    while (dummy_humedad_max1) {
      int humedad2B;
      if ((sscanf(entrada.c_str(), "%d", &humedad2B) == 1) && humedad2B > humedad2min) {
        humedad2max = humedad2B;
        dummy_humedad_max2 = false;
        Serial.println("Humedad máxima del sensor 2 guardada correctamente");
        Serial.println("Ingrese la humedad mínima del sensor 3");
        String entrada = Serial.readStringUntil('\n');
        return;

      } else {
        Serial.println("Por favor, ingrese una humedad máxima válida");
        String entrada = Serial.readStringUntil('\n');
        return;
      }
    }



    while (dummy_humedad_min3) {
      int humedad3A;
      if ((sscanf(entrada.c_str(), "%d", &humedad3A) == 1) && humedad3A >= 0) {
        humedad3min = humedad3A;
        dummy_humedad_min3 = false;
        Serial.println("Humedad mínima del sensor 3 guardada correctamente");
        Serial.println("Ingrese la humedad máxima del sensor 3");
        String entrada = Serial.readStringUntil('\n');
        return;

      } else {
        Serial.println("Por favor, ingrese una humedad mínima válida");
        String entrada = Serial.readStringUntil('\n');
        return;
      }
    }
    while (dummy_humedad_max3) {
      int humedad3B;
      if ((sscanf(entrada.c_str(), "%d", &humedad3B) == 1) && humedad3B > humedad3min) {
        humedad3max = humedad3B;
        dummy_humedad_max3 = false;
        Serial.println("Humedad máxima del sensor 3 guardada correctamente");
        Serial.println("Gracias por ingresar todos los datos necesarios :)");
        String entrada = Serial.readStringUntil('\n');
        return;

      } else {
        Serial.println("Por favor, ingrese una humedad máxima válida");
        String entrada = Serial.readStringUntil('\n');
        return;
      }
    }
  }
}
