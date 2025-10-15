import serial
import time
import requests
import os

# ---------------------- CLASE API DEL CLIMA ----------------------
class ClimaAPI:
    def __init__(self, api_key):
        self.api_key = api_key
        self.lat = 9.8667
        self.lon = -83.9167
        self.url = f'https://api.weatherbit.io/v2.0/forecast/daily?lat={self.lat}&lon={self.lon}&key={self.api_key}&days=1'

    def obtener_probabilidad(self):
        try:
            r = requests.get(self.url)
            if r.status_code != 200:
                print("Error al consultar API:", r.status_code)
                return None
            data = r.json()
            prob = data['data'][0].get('pop', 0)
            print(f"🌦️  Probabilidad de lluvia: {prob}%")
            return prob
        except Exception as e:
            print("Error al conectar con la API:", e)
            return None


# ---------------------- CLASE CONEXIÓN ARDUINO ----------------------
class ConexionArduino:
    def __init__(self, puerto, baudios=9600):
        try:
            self.arduino = serial.Serial(puerto, baudios, timeout=2)
            time.sleep(2)
            print(f"✅ Conectado al Arduino en {puerto}")
        except Exception as e:
            print("❌ No se pudo conectar al Arduino:", e)
            self.arduino = None

    def enviar(self, dato):
        """Envía un valor (como texto) al Arduino"""
        if self.arduino:
            self.arduino.write(f"{dato}\n".encode("utf-8"))
            print("➡️ Enviado:", dato)
            time.sleep(0.5)

    def leer_linea(self):
        """Lee una línea completa del puerto serial"""
        if self.arduino and self.arduino.in_waiting > 0:
            return self.arduino.readline().decode("utf-8", errors="ignore").strip()
        return None


# ---------------------- FUNCIÓN PARA CONFIGURAR SISTEMA ----------------------
def configurar_sistema(arduino):
    print("\n🧠 Iniciando configuración del sistema...")

    # Hora actual
    hora = int(input("Ingrese hora actual (0-23): "))
    arduino.enviar(hora)

    minuto = int(input("Ingrese minutos actuales (0-59): "))
    arduino.enviar(minuto)

    # Configuración de las 3 zonas
    for i in range(1, 4):
        print(f"\n=== Zona {i} ===")

        apagado = int(input("¿Zona apagada? (1: sí, 0: no): "))
        arduino.enviar(apagado)

        if apagado == 0:
            dias = int(input("¿Cada cuántos días se riega?: "))
            arduino.enviar(dias)

            hora_inicio = int(input("Hora de inicio (0-23): "))
            arduino.enviar(hora_inicio)

            minuto_inicio = int(input("Minuto de inicio (0-59): "))
            arduino.enviar(minuto_inicio)

            hora_fin = int(input("Hora de fin (0-23): "))
            arduino.enviar(hora_fin)

            minuto_fin = int(input("Minuto de fin (0-59): "))
            arduino.enviar(minuto_fin)


        humedad_min = int(input("Humedad mínima (0-100): "))
        arduino.enviar(humedad_min)

        humedad_max = int(input("Humedad máxima (0-100, > mínima): "))
        arduino.enviar(humedad_max)

    print("\n✅ Configuración enviada al Arduino correctamente.\n")


# ---------------------- FUNCIÓN MONITOR DE ESTADO ----------------------
def mostrar_estado(lineas):
    """Despliega en pantalla el estado actual del sistema"""
    os.system("cls" if os.name == "nt" else "clear")  # limpia pantalla
    print("🌿 MONITOR DEL SISTEMA DE RIEGO - Python <-> Arduino 🌿")
    print("--------------------------------------------------------")
    for l in lineas[-20:]:  # mostrar solo las últimas 20 líneas
        print(l)
    print("--------------------------------------------------------")
    print("Presiona Ctrl+C para detener.\n")


# ---------------------- PROGRAMA PRINCIPAL ----------------------
if __name__ == "__main__":
    CLAVE_API = "0816e9b97867497691f44dea8f7263a6"
    PUERTO = "COM3"  # Cambia según tu PC

    clima = ClimaAPI(CLAVE_API)
    arduino = ConexionArduino(PUERTO)
    historial = []  # guarda las últimas líneas del estado

    print("\n💻 Esperando mensajes del Arduino...\n")

    try:
        while True:
            linea = arduino.leer_linea()
            if linea:
                historial.append(f"📨 {linea}")

                # --- Detecta eventos ---
                if "PEDIR_CLIMA" in linea:
                    prob = clima.obtener_probabilidad()
                    if prob is None:
                        prob = -1
                    arduino.enviar(prob)
                    historial.append(f"➡️ Enviada probabilidad: {prob}%")

                elif "=== CONFIGURACIÓN DEL SISTEMA ===" in linea:
                    configurar_sistema(arduino)

                # Mostrar siempre el estado actualizado
                mostrar_estado(historial)

            time.sleep(0.1)

    except KeyboardInterrupt:
        print("\n🛑 Programa detenido manualmente.")
