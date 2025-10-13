import requests
import serial
import time
from datetime import datetime, timezone, timedelta

#-------> Clase para obtener datos del clima 
class climaAPI: 
    def __init__(self, apiKey):
        self.apiKey = apiKey
        #la latitud y longitud son los datos de Cartago -> se solicita la probabilidad de lluvia de ahí 
        self.latitud = 9.8667 
        self.longitud = -83.9167
        self.url = f'https://api.weatherbit.io/v2.0/forecast/daily?lat={self.latitud}&lon={self.longitud}&key={self.apiKey}&days=1'


    
    def obtenerClima(self):
        try:
            respuesta = requests.get(self.url)
            if respuesta.status_code != 200:
                print("Error al obtener datos:", respuesta.status_code)
                return None

            datos = respuesta.json()
            if 'data' not in datos or len(datos['data']) == 0:
                print("No hay datos disponibles.")
                return None

            # Extraer probabilidad de lluvia (pop) del primer día
            probabilidad = datos['data'][0].get('pop', 0)
            # lluvia total esperada en mm de lluvia
            mm_total = datos['data'][0].get('precip', 0)

            print(f"Probabilidad de lluvia hoy: {probabilidad}%, lluvia total estimada: {mm_total} mm")
            return probabilidad

        except Exception as e:
            print("Error al consultar Weatherbit:", e)
            return None
            
        
#-------> Clase para manejar conexión con el Arduino
class conexionArduino: 
    def __init__(self, puerto, baudios=9600):
        self.puerto = puerto
        self.baudios = baudios
        try:
            self.arduino = serial.Serial(puerto, baudios, timeout=2)
            time.sleep(2)  # Esperar a que Arduino se inicie
            print("Conectado al Arduino en", puerto)
        except Exception as e:
            print("Error al conectar con Arduino:", e)
            self.arduino = None
            
    def enviarDatos(self, mensaje):
        if self.arduino:
            self.arduino.write((str(mensaje) + "\n").encode('utf-8'))
            print("Enviado al Arduino:", mensaje)
            

#-------> Clase que coordina todo 
class probLluviaParaArduino:
    def __init__(self, claveAPI, puertoArduino):
        self.claveAPI = climaAPI(claveAPI)
        self.arduino = conexionArduino(puertoArduino)
        
    def ejecutar(self):
        prob_lluvia = self.claveAPI.obtenerClima()
        if prob_lluvia is not None:
            self.arduino.enviarDatos(prob_lluvia)
            
#-------> Ejecutar todo 

if __name__ == "__main__":
    
    #DESCOMENTAR LO DE PUERTOARDUINO Y SISTEMA PARA EJECUTAR LA TRANSMISIÓN DE DATOS
    CLAVEAPI = "0816e9b97867497691f44dea8f7263a6" #la key para solicitar los datos, no tocar
    #PUERTOARDUINO = poner el puerto del arduino aquí y descomentar 
    
    #sistema = probLluviaParaArduino(CLAVEAPI, PUERTOARDUINO)
    #sistema.ejecutar()
    
    #PROBAR EN CONSOLA 
    api = climaAPI(CLAVEAPI)
    probabilidad = api.obtenerClima()
    if probabilidad is not None:
        print(f"La probabilidad de lluvia hoy en Cartago es: {probabilidad}%")
    else:
        print("No se pudo obtener la probabilidad de lluvia")
    


        