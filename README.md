# PROTOTIPO-HERRAMIENTAS-IA-2023

# DIAGRAMA DE CONEXIONES TTL Convertidor Serial Adaptador & El ESP32-CAM
1. FT232RL FTDI Mini USB a TTL Convertidor Serial Adaptador
2. ESP-32S

![diagramaESP32_CAM_TTL_CONVERTIDOR](https://github.com/YERCKEN/ESP-32S-CAMERA-WEB-SERVER-UXGA_1600x1200/assets/42678417/a2dd1b55-92d4-4cb9-ae4a-77eb37563150)

# CONGIGURACIÓN DE LA BOARD 

![MicrosoftTeams-image (2)](https://github.com/YERCKEN/ESP-32S-CAMERA-WEB-SERVER-UXGA_1600x1200/assets/42678417/d0f53881-8174-4aad-905d-a59a70e005fd)

# PINES DEL ESP32-CAM
![ESP-32S PINES](https://github.com/YERCKEN/ESP-32S-CAMERA-WEB-SERVER-UXGA_1600x1200/assets/42678417/a14ad18c-8bfb-4116-8069-5d71629ba7aa)

# ACTUALIZACIÓN 3/11/2023 

Se estuvo trabajando en el replanteamiento del proyecto, ya que se consideró utilizar ARDUINO UNO R3 para que se encague del manejo de los servos, reloj, IR RECIBE.
**Lo que se logró es lo siquiente:**

**- COMUNICACIÓN:** 

La comunación entre el ESP32-CAM y el ARDUINO es através de UART, primeramente se planetó usar I2C pero hubo problemas y se optó por esta opción.
-  PINES PARA LA COMUNICACIÓN: (se puede observar en el diagrama más abajo)
    - esp32 (TX) <--> arduino (RX)
    -  esp32 (RX) <--> arduino (TX)
      
**- NUEVOS DISPOSITVOS:** 

Arduino UNO, y Reloj DS1307 para pruebas iniciales ya que no se cuenta con el que se utilizará finalmente, sin embargo el fucionamiento debe ser igual.

**- FUNCIONALIDADES:** 

El neuvo plantemiento del sistema se basa en la utilización de métodos POST para enviar y recibir información del servidor, por parte de lo que es captura y manera de transformar la imagen (BASE64) sigue igual lo que ha cambiado es la manera en la que se intercambia la información.


Actualmente no se puede monitorear los valores que está recibiendo el ESP32-CAM através de la consola ya que ella está conectada al ARDUINO UNO, por lo cual se establecieron números tipo (INT) que actuan como códigos de control, los valores son los siguieentes.

-  **CONTROL DE SERVOS**
   -  (69) = Indica que el POST al servidor ha terminado correctamente
   - (-69) = Indica que el POST al servidor ha terminado incorrectamente
   -   (1) = Indica que el servo debe moverse a la derecha    (este valor es obtenido através del POST)
   -  (-1) = Indica que el servo debe moverse a la  Izquierda (este valor es obtenido através del POST)
   -   (0) = Indica que el servo no debe moverse               (este valor es obtenido através del POST)


    - Luego que los valores que indican la dirección en la que se deben mover los servos llegan al ESP32-CAM luego del POST son enviados al ARDUINO UNO y el se encarga de moverlos
    - En el caso que a el ARDUINO UNO RECIBA UN VALOR (-69) enciende el LED AZUL indicando error de POST (en el diagrama se puede observar el LED)
      
-  **CONTROL DE WIFI**
  -  (7) = Indica que el módulo está conectado a un WIFI
  - (-7) = Indica que el módulo no está conectado a un WIFI
  - Si al arduino UNO le llega un valor (7) enceiende la LED verde indicando que el módulo está correctamente conectado
  - Si al arduino UNO le llega un valor (-7) el LED verde no enciende hasta que salga un código positivo

-  **CONTROL DEL ESTADO DEL POST PARA ENVIO DE IMAGEN AL SERVIDOR**
  -  (40) = Indica que el POST ha terminado correctamente
  - (-40) = Indica error de POST
  - cuando el arduino Uno recibe un codigo 40 enciende el LED BLANCO por 100ms esto indica el POST terminó correctamente y el servidor obtuvo la imagen.

-  **CONTROL DE LAS CAPTURAS CON EL RELOJ**
   - (7306) = Indica que el ESP32-CAM debe tomar capturas y mandarlas al servidor, básicamente gracias al reloj se pueden establecer periodos de tiempo en los que el ESP32-CAM debe enviar
  capturas al servidor, esto está siendo implementado para tener una simulación de lo que sería un horario.


# DIAGRAMA CONEXIÓN: ARDUINO UNO R3, ESP-32S, SERVO MOTOR, IR RECIBE, TTLtoUSB, Reloj DS1307
![Diagrama SensorIR-ServoMotor-ArduinoUno-Esp32Cam_bb](https://github.com/YERCKEN/PROTOTIPO-HERRAMIENTAS-IA-2023/assets/42678417/e6c95fe2-eb71-47ee-bcfd-74c789abcd8d)
