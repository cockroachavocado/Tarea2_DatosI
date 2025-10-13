import requests
import serial
import time

#-------> Clase para obtener datos del clima 
class climaAPI: 
    def __init__(self, apiKey):
        self.apiKey = apiKey
        #la latitud y longitud son los datos de Cartago -> se solicita la probabilidad de lluvia de ahí 
        self.latitud = 9.8644 
        self.longitud = -83.9194
        self.url = f"http://api.openweathermap.org/data/2.5/weather?lat={self.latitud}&lon={self.longitud}&appid={self.apiKey}&units=metric"


    
    def obtenerClima(self):
        respuesta = requests.get(self.url)
        if respuesta.status_code == 200:
            datos = respuesta.json()
           #Revisar si existe lluvia
            lluvia = datos.get("rain", {})
            #la lluvia en la última hora si existe, si no, 0
            mm_lluvia = lluvia.get("1h", 0)
            # Estimar probabilidad: 0 mm -> 0%, >0 mm -> 80-100%
            if mm_lluvia == 0:
                probabilidad = 0
            elif mm_lluvia < 5:
                probabilidad = 80
            else:
                probabilidad = 100
            return probabilidad
        else:
            print("Error al obtener datos:", respuesta.status_code)
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
    CLAVEAPI = "fe04d12cf37b177359ff855076fda4e4" #la key para solicitar los datos, no tocar
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
    


        