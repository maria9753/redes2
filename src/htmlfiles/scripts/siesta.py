import time
import sys

print("Content-Type: text/html\r\n\r\n") # Cabecera mínima
print("<h1>Inicio de la siesta de 10 segundos...</h1>")
sys.stdout.flush() # Forzamos a que el texto salga ya hacia el servidor

time.sleep(10) # El script se queda aquí parado 10 segundos

print("<h1>¡Me he despertado!</h1>")