#include "esp_camera.h"
#include <WiFi.h>

//SERVO Y IR RECIBE
#include <ESP32Servo.h>
#include <IRremote.h>

//SERVOS FICTICIOS
#define DUMMY_SERVO1_PIN 12     //We need to create 2 dummy servos.
#define DUMMY_SERVO2_PIN 13     //So that ESP32Servo library does not interfere with pwm channel and timer used by esp32 camera.

Servo dummyServo1;
Servo dummyServo2;

//SERVO REAL PARA GIRO HORIZONTAL
Servo servoHorizontal; 

//VARIABLES PARA EL CONTROL DEL SERVO
int anguloHorizontalInicial = 90;
int incrementoAngulo = 5;
int ultimaTeclaPresionada = 0;

//MODELO DE LA CÁMARA_---------------------------------
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

// ===========================
// CONEXIÓN
// ===========================
//const char* ssid = "Yercken";
//const char* password = "123456789";

const char* ssid = "CWC-7986324-2.4";
const char* password = "snns7pdTxnkw";



void startCameraServer();
void setupLedFlash(int pin);


  
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  //IR RECIBE EN EL PIN 15-----
  IrReceiver.begin(15, ENABLE_LED_FEEDBACK);  // Inicia el receptor IR en el pin 15

   // Configuración del servo
  servoHorizontal.attach(14); // Asigna el servo al pin 14 (ajusta el pin según tu conexión)
  servoHorizontal.write(anguloHorizontalInicial);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;

  //TAMAÑO DE IMAGEN----------------------------------------------------------

  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 2;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_UXGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);

  
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    // No cambies la resolución a QVGA
    // s->set_framesize(s, FRAMESIZE_QVGA);
}

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {

  // Control del servoHorizontal con el receptor IR
  if (IrReceiver.decode()) {
    int valor = IrReceiver.decodedIRData.command;

    if (valor == 67) {
      // Derecha
      anguloHorizontalInicial += incrementoAngulo;
      if (anguloHorizontalInicial > 180) {
        anguloHorizontalInicial = 180;
      }
      ultimaTeclaPresionada = 67; // Actualiza la última tecla
    } else if (valor == 68) {
      // Izquierda
      anguloHorizontalInicial -= incrementoAngulo;
      if (anguloHorizontalInicial < 0) {
        anguloHorizontalInicial = 0;
      }
      ultimaTeclaPresionada = 68; // Actualiza la última tecla
    } else if (valor == 0) {
      // Si el valor es 0, mueve en la dirección de la última tecla presionada
      if (ultimaTeclaPresionada == 67) {
        anguloHorizontalInicial += incrementoAngulo;
        if (anguloHorizontalInicial > 180) {
          anguloHorizontalInicial = 180;
        }
      } else if (ultimaTeclaPresionada == 68) {
        anguloHorizontalInicial -= incrementoAngulo;
        if (anguloHorizontalInicial < 0) {
          anguloHorizontalInicial = 0;
        }
      }
    }

    servoHorizontal.write(anguloHorizontalInicial);

    Serial.print("Dirección: ");
    Serial.print((ultimaTeclaPresionada == 67) ? "Derecha" : "Izquierda");
    Serial.print(", Ángulo: ");
    Serial.println(anguloHorizontalInicial);

    Serial.print("Valor IR recibido: ");
    Serial.println(valor);

    IrReceiver.resume(); // Continuar escuchando
  }
   // Show IP address every 5 seconds
  //delay(5000);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
}
