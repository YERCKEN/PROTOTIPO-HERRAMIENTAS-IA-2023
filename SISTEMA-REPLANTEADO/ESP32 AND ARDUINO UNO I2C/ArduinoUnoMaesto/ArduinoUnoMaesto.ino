//SERVO --------------------
#include <IRremote.h>
#include <Servo.h>

//RELOJ
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

unsigned long previousMillis = 0; 
const unsigned long interval = 250;

//PIN DEL SENSOR IR
const byte PIN_RECEPTOR_INFRARROJO = 3;

//==================================================   SERVOS   =================================================================

Servo servoHorizontal;
Servo servoVertical;

// ANGULOS INICIALES
int anguloHorizontalInicial = 90; // Ángulo horizontal inicial
int anguloVerticalInicial = 90;   // Ángulo vertical inicial
// INCREMENTO
int incrementoAngulo = 5;   

//==================================================   SETUP   =================================================================

int datoRecibido = 0;

//=============================================   LEDS INDICADORAS  =================================================================

//LED INDICADORA DE WIFI 
int pinLedWifi = 5; //LED VERDE 

//LED INDICADORA DE ERROR POST SERVO
int pinLedServo = 7; //LED AZUL

//LED INDICADORA DE ERROR POST CAM
int pinLedCamPost = 6; //LED ROJO

//LED INDICADORA  CAPTURA CAM
int pinLedCamCap = 8; //LED blanco
unsigned long inicioTiempo = 0;
unsigned long duracionEncendido = 100; // 0.5 segundos en milisegundos
bool ledEncendido = false;


void setup() {
  Serial.begin(115200);

  //HABILITACIÓN RECEPTOR Y LED DEL MÓDULOO
  IrReceiver.begin(PIN_RECEPTOR_INFRARROJO, ENABLE_LED_FEEDBACK);

  //LEDS
  pinMode(pinLedWifi, OUTPUT);
  
  pinMode(pinLedServo, OUTPUT);

  pinMode(pinLedCamPost, OUTPUT);
  pinMode(pinLedCamCap, OUTPUT);

   //PIN servo horizontal
   servoHorizontal.attach(2);  
   //PIN servo vertical
   servoVertical.attach(4);   
  
   //ANGULOS INICIALES
   servoHorizontal.write(anguloHorizontalInicial);
   servoVertical.write(anguloVerticalInicial); 
}

//==================================================   LOOP   =================================================================

void loop() {

  unsigned long currentMillis = millis(); // Obtener el tiempo actual

  //OBTENIENDO DATOS DE LOS SERVOS ---------------------------------------------------------------------------------
  if (Serial.available() > 0) {
    datoRecibido = Serial.parseInt();

    // Procesa el dato recibido
    //Serial.print("Se recibió la dirección del servo = ");
    //Serial.println(datoRecibido);

    // VERIFICACIÓN DE DIRRECCIÓN
    //DERECCHA
    if (datoRecibido == 1) {
      moveServoRight();
    //IZQUIERDA
    } else if (datoRecibido == -1) {
      moveServoLeft();
    //ARRIBA
    } else if (datoRecibido == 2) {
      moveServoUp();
    //ABAJO
    } else if (datoRecibido == -2) {
      moveServoDown();

    //CONECTIVIDAD WIFI
    } else if (datoRecibido == -7) {
      digitalWrite(pinLedWifi, LOW);

    }else if (datoRecibido == 7) {
      digitalWrite(pinLedWifi, HIGH);

    //SERVO POST 
    }else if (datoRecibido == 69) {
      digitalWrite(pinLedServo, LOW);

    }else if (datoRecibido == -69) {
      digitalWrite(pinLedServo, HIGH);
      
    //CAM POST
    }else if (datoRecibido == 40) {
      digitalWrite(pinLedCamPost, LOW);
      digitalWrite(pinLedCamCap, HIGH);

      inicioTiempo = millis(); // Guarda el tiempo actual
      ledEncendido = true;

    }else if (datoRecibido== -40) {
      digitalWrite(pinLedCamPost, HIGH);
    }
    
  }

  if (ledEncendido && millis() - inicioTiempo >= duracionEncendido) {
    digitalWrite(pinLedCamCap, LOW); // Apaga el LED
    ledEncendido = false; // Restablece el estado del LED
  }

  //RELOJ ---------------------------------------------------------------------------------
  
  // Verificar si ha transcurrido un segundo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // La fecha y la hora actual obtenida desde el chip RTC.
    tmElements_t datetime;
      // Obtener la fecha y hora desde el chip RTC
    if (RTC.read(datetime)) {

      int currentHour = datetime.Hour;
      int currentMinute = datetime.Minute;
      
      //HORAS DE IMPRESION
      if (currentHour == 17 && currentMinute >= 35 && currentMinute <= 50) {
        Serial.println(7306);
      }

      
      //IMPRESION DE HORA ------------------------------------
      
      /*
      // Comenzamos a imprimir la informacion
      Serial.print(F("OK, Time = "));
      print2digits(datetime.Hour);
      Serial.write(':');
      print2digits(datetime.Minute);
      Serial.write(':');
      print2digits(datetime.Second);
      Serial.println();
      
      
      //IMPRESION DE FECHA -----------------------------------

      Serial.print(F(", Date (D/M/Y) = "));
      Serial.print(datetime.Day);
      Serial.write('/');
      Serial.print(datetime.Month);
      Serial.write('/');
      Serial.print(tmYearToCalendar(datetime.Year));
      Serial.println();
     */
    }

    /*
    else {
      if (RTC.chipPresent()) {
        // El reloj esta detenido, es necesario ponerlo a tiempo
        Serial.println(F("EL DS1307 ESTA DETENIDO"));
        Serial.println(F("CORRE EL PROGRAMA PARA PONERLO A TIEMPO"));
      }
      else {
        // No se puede comunicar con el RTC en el bus I2C, revisar las conexiones.
        Serial.println(F("NO SE DETECTO EL DS1307"));
        Serial.println(F("REVISA TUS CONEXIONES E INTENTA DE NUEVO"));
      }
      // Esperamos indefinidamente
      for (;;); 
    }*/
  }


  
  // Control del servoHorizontal con el receptor IR
  if (IrReceiver.decode()) {
    int valor = IrReceiver.decodedIRData.command;

    static unsigned long lastIRTime = 0;
    static bool lastIRWasRight = false;

    if (millis() - lastIRTime > 1) {
      // If it's been more than 1 milliseconds since the last IR command,
      // accept the new command, regardless of the previous direction.
      lastIRWasRight = false;
    }

    if (valor == 67) {
      // Derecha
      if (!lastIRWasRight) {
        anguloHorizontalInicial += incrementoAngulo;
        if (anguloHorizontalInicial > 180) {
          anguloHorizontalInicial = 180;
        }
        lastIRTime = millis();
        lastIRWasRight = true;
      }
    } else if (valor == 68) {
      // Izquierda
      anguloHorizontalInicial -= incrementoAngulo;
      if (anguloHorizontalInicial < 0) {
        anguloHorizontalInicial = 0;
      }
      lastIRTime = millis();
      lastIRWasRight = false;

    }else if (valor == 70) {
    // Arriba
    anguloVerticalInicial += incrementoAngulo;
    if (anguloVerticalInicial > 180) {
      anguloVerticalInicial = 180;
    }
  } else if (valor == 21) {
    // Abajo
    anguloVerticalInicial -= incrementoAngulo;
    if (anguloVerticalInicial < 0) {
      anguloVerticalInicial = 0;
    }
  }

    // Update the servo position
    servoHorizontal.write(anguloHorizontalInicial);
    servoVertical.write( anguloVerticalInicial);

    //Serial.print("Dirección: ");
    //Serial.print((valor == 67) ? "Derecha" : "Izquierda");
    //Serial.print(", Ángulo: ");
    //Serial.println(anguloHorizontalInicial);

    //Serial.print("Valor IR recibido: ");
    //Serial.println(valor);

    IrReceiver.resume(); // Continue listening for IR remote commands
  }
  
}

//Funcion auxiliar para imprimir siempre 2 digitos ====================================================

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}


//================================================== FUNCIONES MOVIENTO SERVO =================================================================

//IZQUIERDA ------------------------------------------------
void moveServoLeft() {
  anguloHorizontalInicial -= incrementoAngulo;
  if (anguloHorizontalInicial < 0) {
    anguloHorizontalInicial = 0;
  }

  // Actualiza la posición del servo
  servoHorizontal.write(anguloHorizontalInicial);
}

//DERECHA -------------------------------------------------
void moveServoRight() {
  anguloHorizontalInicial += incrementoAngulo;
  if (anguloHorizontalInicial > 180) {
    anguloHorizontalInicial = 180;
  }

  // Actualiza la posición del servo
  servoHorizontal.write(anguloHorizontalInicial);
}


// ARRIBA ------------------------------------------------
void moveServoUp() {
  anguloVerticalInicial += incrementoAngulo;
  if (anguloVerticalInicial > 180) {
    anguloVerticalInicial = 180;
  }

  // Actualiza la posición del servo vertical
  servoVertical.write(anguloVerticalInicial);
}

// ABAJO -------------------------------------------------
void moveServoDown() {
  anguloVerticalInicial -= incrementoAngulo;
  if (anguloVerticalInicial < 0) {
    anguloVerticalInicial = 0;
  }

  // Actualiza la posición del servo vertical
  servoVertical.write(anguloVerticalInicial);
}
