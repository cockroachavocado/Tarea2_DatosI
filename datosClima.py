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

import serial
import time

# -------------------- CONFIGURACIÓN --------------------
puerto = "COM3"  # cámbialo si tu Arduino usa otro
baud = 9600
arduino = serial.Serial(puerto, baud, timeout=1)
time.sleep(2)

print("🌿 MONITOR DEL SISTEMA DE RIEGO - Python <-> Arduino 🌿")
print("--------------------------------------------------------")

# Datos fijos (puedes pedirlos con input() si prefieres)
hora = int(input("Hora actual (0-23): "))
mins = int(input("Minutos actuales (0-59): "))

# Datos para tres zonas
zonas = []
for i in range(3):
    print(f"\n=== Zona {i+1} ===")
    apagado = int(input("¿Zona apagada? (1 sí / 0 no): "))
    if apagado == 0:
        dias = int(input("¿Cada cuántos días se riega?: "))
        hIni = int(input("Hora de inicio (0-23): "))
        mIni = int(input("Minuto de inicio (0-59): "))
        hFin = int(input("Hora de fin (0-23): "))
        mFin = int(input("Minuto de fin (0-59): "))
    else:
        dias = hIni = mIni = hFin = mFin = 0
    hMin = int(input("Humedad mínima (0-100): "))
    hMax = int(input("Humedad máxima (0-100, >min): "))

    zonas.append([apagado, dias, hIni, mIni, hFin, mFin, hMin, hMax])

# --------------------------------------------------------

def enviar(valor):
    arduino.write(f"{valor}\n".encode())
    print(f"➡️ Enviado: {valor}")
    time.sleep(0.3)

# --------------------------------------------------------


CLAVE_API = "a97765407e894bc1ae4d2d7dee85fdd7"
PUERTO = "COM3"
clima = ClimaAPI(CLAVE_API)
prob = clima.obtener_probabilidad()


try:
    while True:
        if arduino.in_waiting > 0:
            msg = arduino.readline().decode(errors='ignore').strip()
            if msg:
                print("📨", msg)

            # 1️⃣ Cuando Arduino pide el clima
            if "PEDIR_CLIMA" in msg:
                enviar(prob)

            # 2️⃣ Cuando confirma haber recibido el clima
            elif "Probabilidad de lluvia recibida" in msg:
                enviar(hora)
                enviar(mins)
                print("🕒 Hora enviada.")
                for i, z in enumerate(zonas):
                    print(f"🌱 Enviando zona {i+1}...")
                    for valor in z:
                        enviar(valor)
                print("✅ Configuración enviada al Arduino correctamente.")
                print("--------------------------------------------------------")

        time.sleep(0.1)

except KeyboardInterrupt:
    print("\n🛑 Programa detenido manualmente.")
    arduino.close()