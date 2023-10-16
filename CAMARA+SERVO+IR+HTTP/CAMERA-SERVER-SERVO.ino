#include "esp_camera.h" //FUNCIONA EL /LEFT Y /RIGHT, LA IMAGEN/VIDEO NO CARGA
#include <WiFi.h>
#include <WebServer.h>
#include <IRremote.h>
#include <ESP32Servo.h>
#include "index_html.h"  // Include the HTML file as a C string
#include "capture_html.h"  // Include the HTML file as a C string

//SERVOS FICTICIOS
#define DUMMY_SERVO1_PIN 12     //We need to create 2 dummy servos.
#define DUMMY_SERVO2_PIN 13     //So that ESP32Servo library does not interfere with pwm channel and timer used by esp32 camera.

Servo dummyServo1;
Servo dummyServo2;

// SERVO REAL PARA GIRO HORIZONTAL
Servo servoHorizontal;

// VARIABLES PARA EL CONTROL DEL SERVO
int anguloHorizontalInicial = 90;
int incrementoAngulo = 5;
int ultimaTeclaPresionada = 0;

// MODELO DE LA CÁMARA
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

WebServer server(80);

const char* ssid = "COWIFI209056991/0"; // Cambia a tu SSID de Wi-Fi
const char* password = "WiFi-83498547"; // Cambia a la contraseña de tu Wi-Fi

void startCameraServer();
void moveServoLeft();
void moveServoRight();
void captureImage();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  //IR RECIBE EN EL PIN 15-----
  IrReceiver.begin(15, ENABLE_LED_FEEDBACK);  // Inicia el receptor IR en el pin 15

  // Configuración del servo
  servoHorizontal.attach(14); // Asigna el servo al pin 14 (ajusta el pin según tu conexión)
  servoHorizontal.write(anguloHorizontalInicial);


  // Configuración de la cámara
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA; // Resolución 1600x1200
  config.jpeg_quality = 10;
  config.fb_count = 2;
  
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA; // Establece tu resolución deseada
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.pixel_format = PIXFORMAT_JPEG;
  }

  // Inicializa la cámara
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Error en la inicialización de la cámara: 0x%x", err);
    return;
  }

  // Configuración de tu Wi-Fi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi conectado");

  // Configura el servidor de la cámara
  startCameraServer();

  Serial.print("¡Cámara lista! Usa 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' para conectarte");
}


void moveServoLeft() {
  anguloHorizontalInicial -= incrementoAngulo;
  if (anguloHorizontalInicial < 0) {
    anguloHorizontalInicial = 0;
  }

  // Update the servo position
  servoHorizontal.write(anguloHorizontalInicial);

  server.send(200, "text/plain", "Movido a la izquierda");
}

void moveServoRight() {
  anguloHorizontalInicial += incrementoAngulo;
  if (anguloHorizontalInicial > 180) {
    anguloHorizontalInicial = 180;
  }

  // Update the servo position
  servoHorizontal.write(anguloHorizontalInicial);

  server.send(200, "text/plain", "Movido a la derecha");
}

String base64_encode(const uint8_t* data, size_t length) {
  static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  String encoded;
  encoded.reserve(((length + 2) / 3) * 4);

  for (size_t i = 0; i < length; i += 3) {
    uint32_t octet_a = i < length ? data[i] : 0;
    uint32_t octet_b = i + 1 < length ? data[i + 1] : 0;
    uint32_t octet_c = i + 2 < length ? data[i + 2] : 0;

    encoded += base64_chars[(octet_a >> 2) & 0x3F];
    encoded += base64_chars[((octet_a << 4 | octet_b >> 4) & 0x3F)];
    encoded += base64_chars[((octet_b << 2 | octet_c >> 6) & 0x3F)];
    encoded += base64_chars[octet_c & 0x3F];
  }

  return encoded;
}


camera_fb_t* lastCapturedImage = nullptr;

void captureImage() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (fb) {
    if (lastCapturedImage) {
      esp_camera_fb_return(lastCapturedImage);
    }
    lastCapturedImage = fb;

    String base64Image = "data:image/jpeg;base64,";
    base64Image += base64_encode(fb->buf, fb->len);

    server.send(200, "text/html", base64Image);
  } else {
    server.send(500, "text/plain", "Camera capture failed");
  }
}

void capturaOnly() {
  if (lastCapturedImage) {
    String base64Image = "data:image/jpeg;base64,";

    // Encode the image data to base64
    base64Image += base64_encode(lastCapturedImage->buf, lastCapturedImage->len);

    // Send the base64-encoded image in the response
    server.send(200, "text/html", base64Image);
  } else {
    server.send(500, "text/plain", "No image captured yet.");
  }
}


void startCameraServer() {
  server.on("/", HTTP_GET, [](){
    server.send(200, "text/html", index_html); // Serve the HTML file
  });

  server.on("/capture", HTTP_GET, captureImage);
  server.on("/aux", HTTP_GET, capturaOnly);

  server.on("/capture", HTTP_OPTIONS, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Max-Age", "10000");
    server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    server.send(204);
});
  server.on("/captura", HTTP_GET, [](){
      server.send(200, "text/html", capture_html); // Serve the HTML file
    });

  server.on("/left", HTTP_GET, moveServoLeft);
  server.on("/right", HTTP_GET, moveServoRight);
  server.begin();
}

void loop() {
  // Handle the client requests (HTTP endpoints)
  server.handleClient();

  // Control the servoHorizontal with the IR remote
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
    }

    // Update the servo position
    servoHorizontal.write(anguloHorizontalInicial);

    Serial.print("Dirección: ");
    Serial.print((valor == 67) ? "Derecha" : "Izquierda");
    Serial.print(", Ángulo: ");
    Serial.println(anguloHorizontalInicial);

    Serial.print("Valor IR recibido: ");
    Serial.println(valor);

    IrReceiver.resume(); // Continue listening for IR remote commands
  }
}



