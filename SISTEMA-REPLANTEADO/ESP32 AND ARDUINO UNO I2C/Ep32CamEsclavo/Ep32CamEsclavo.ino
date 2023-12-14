//======================================================================================
// ESP32 CAM 
//======================================================================================

#include "esp_camera.h" //FUNCIONA EL /LEFT Y /RIGHT, LA IMAGEN/VIDEO NO CARGA
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h> // Agrega esta línea para utilizar la librería HTTPClient

// MODELO DE LA CÁMARA
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

WebServer server(80);


//======================================================================================
// CONEXIÓN WIFI
//======================================================================================

//const char* ssid = "COWIFI209056991/0"; // Cambia a tu SSID de Wi-Fi
//const char* password = "WiFi-83498547"; // Cambia a la contraseña de tu Wi-Fi

//const char* ssid = "Infoplaza 464 UTP"; // Cambia a tu SSID de Wi-Fi
//const char* password = "1nf0Pla464"; // Cambia a la contraseña de tu Wi-Fi

//const char* ssid = "Yercken";
//const char* password = "123456789";

const char* ssid = "CWC-7986324";
const char* password = "snns7pdTxnkw";

//======================================================================================
// CONEXIÓN SERVIDOR
//======================================================================================
//JEISSON
const char* urlServidor = "http://esp32server2.000webhostapp.com/index.php";
const char* urlServidorDirection = "http://esp32server2.000webhostapp.com/direction_endpoint.php";
const char* urlServidorDirectionY = "http://esp32server2.000webhostapp.com/direction_endpoint2.php";

//ORLANDO
//const char* urlServidor = "http://190.32.101.144/esp32/index.php";
//const char* urlServidorDirection = "http://190.32.101.144/esp32/direction_endpoint.php";

//EDWAR
//const char* urlServidor = "http://esp32camimgb64recibe.000webhostapp.com/index.php";


//======================================================================================
// TIEMPO
//======================================================================================

unsigned long previousMillis = 0;  // Variable para almacenar el tiempo del último llamado
const long interval = 500;         // Intervalo deseado en milisegundos (0.5 segundos)

unsigned long previousSendTime = 0;  // Variable para almacenar el tiempo del último llamado
const long sendInterval = 500;         // Intervalo deseado en milisegundos (0.5 segundos)

unsigned long lastWifiCheckTime = 0;
const unsigned long wifiCheckInterval = 5000; 
//--------------------------------------------------------------------------------------

void startCameraServer();
//void captureImage();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
  // CAMBIO DE RESOLUCIÓN
  config.frame_size = FRAMESIZE_SVGA; 
  config.jpeg_quality = 10;
  config.fb_count = 2;
  
    if (psramFound()) {

     // CAMBIO DE RESOLUCIÓN
    config.frame_size = FRAMESIZE_SVGA;
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




//======================================================================================
// FUNCIÓN PARA LA CODIFICACIÓN A BASE 64
//======================================================================================

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


//======================================================================================
// FUNCIÓN CAPTURA DE IMG
//======================================================================================

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

    //server.send(200, "text/html", base64Image);
  } else {
    server.send(500, "text/plain", "Camera capture failed");
  }
}


//======================================================================================
// ENVIAR POST
//======================================================================================

void enviarImagenPorPOST(String base64Image) {
  HTTPClient http;

  // Establece la URL del servidor
  http.begin(urlServidor);

  // Configura la solicitud POST con la imagen codificada en base64
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String postPayload = "image=" + base64Image;

  // Realiza la solicitud POST
  int httpCode = http.POST(postPayload);

  if (httpCode > 0) {
    //Serial.printf("[HTTP] POST éxito, código: %d\n", httpCode);
    // Imprime la respuesta del servidor
    //String response = http.getString();
    //Serial.println(response);
     Serial.println(40);
  } else {
    //Serial.printf("[HTTP] POST fallido, código: %d\n", httpCode);
    Serial.println(-40);
  }

  // Libera los recursos de HTTPClient
  http.end();
}

//======================================================================================
// CAPTURAR LA IMG MÁS ACTUALIZADA
//======================================================================================

void capturaOnly() {
  if (lastCapturedImage) {
    String base64Image = "data:image/jpeg;base64,";
    
    // Encode the image data to base64
    base64Image += base64_encode(lastCapturedImage->buf, lastCapturedImage->len);
    server.sendHeader("Access-Control-Allow-Origin", "*");

    // Llama a la función para enviar la imagen por POST
    enviarImagenPorPOST(base64Image);
    //server.send(200, "text/html", "ENVIADO");
  } else {
    server.send(500, "text/plain", "No image captured yet.");
  }
}



void sendDirection(const char* direction) {
  
  HTTPClient http;
  // Establece la URL del servidor
  http.begin(urlServidorDirection);

  // Configura la solicitud POST
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String postPayload = "direction=" + String(direction);  // Envía la dirección como un parámetro

  // Realiza la solicitud POST
  int httpCode = http.POST(postPayload);

  if (httpCode > 0) {

    Serial.println(69);
    //Serial.printf("[HTTP] POST SERVO CON éxito, código: %d\n", httpCode);
    // Imprime la respuesta del servidor
    String response = http.getString();

    //Serial.println(response);

    //Serial.println("\n");

    //MOVIMIENTO DEL SERVO SEGÚN LA RESPUESTA
    if (response.equals("1")){ 
         Serial.println(1);
      }
    else if(response.equals("2")){
        Serial.println(2);
      }
    else if(response.equals("-2")){
        Serial.println(-2);
      }
    else if(response.equals("-1")){
        Serial.println(-1);
      }
    else{
        //Serial.println("SIN MOVIMEINTO");
    }

  } else {
    //Serial.printf("[HTTP] POST SERVO fallido, código: %d\n", httpCode);
    Serial.println(-69);
  }

  // Libera los recursos de HTTPClient
  http.end();
}





//======================================================================================
// POWER SERVER
//======================================================================================


void startCameraServer() {

   //CAPTURA INSTA DESDE CAUQLUIER IP ===========================================================================================
  /*
  server.on("/capturaInsta", HTTP_GET, []()
    {
        // Habilita el encabezado 'Access-Control-Allow-Origin' para permitir solicitudes desde cualquier origen
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Max-Age", "10000");
        server.sendHeader("Access-Control-Allow-Methods", "GET");
                
        // Captura la imagen actual
        captureImage();
        capturaOnly();
  });
  */
  // ----------------------------------------------------------------------------------------------------------------------------

  //CONTROL DE SERVOS DESDE CUALQUIER IP =================================================================
  
  server.on("/L", HTTP_GET, [](){
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Access-Control-Max-Age", "10000");
      server.sendHeader("Access-Control-Allow-Methods", "GET");

      Serial.println(-1);
    });
    
  server.on("/R", HTTP_GET, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Access-Control-Max-Age", "10000");
      server.sendHeader("Access-Control-Allow-Methods", "GET");
      
      Serial.println(1);
    });

  server.on("/U", HTTP_GET, [](){
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Access-Control-Max-Age", "10000");
      server.sendHeader("Access-Control-Allow-Methods", "GET");

      Serial.println(2);
    });
    
  server.on("/D", HTTP_GET, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Access-Control-Max-Age", "10000");
      server.sendHeader("Access-Control-Allow-Methods", "GET");
      
      Serial.println(-2);
    });

  // -----------------------------------------------------------------------------------------------
  server.begin();
}

//======================================================================================
// LOOP
//======================================================================================

void loop() {

  //sendDirection("holaMandameLadireccion");
  //delay(250);
  
  // Obtiene el tiempo actual
  unsigned long currentMillis = millis(); 

  
  // Enviar la dirección cada 250 ms
  if (currentMillis - previousSendTime >= sendInterval) {
    sendDirection("holaMandameLadireccion");
    //sendDirectionY("holaMandameLadireccion");
    previousSendTime = currentMillis; // Actualizar el tiempo del último envío
  }
  
  

  if (Serial.available() > 0) {
    // Si hay datos disponibles en el puerto serie
    int serialData = Serial.parseInt(); // Lee el número enviado por Arduino

    if (serialData == 7306) {
     // Verifica si ha pasado el tiempo del intervalo
      if (currentMillis - previousMillis >= interval) 
        {
          previousMillis = currentMillis;  // Actualiza el tiempo del último llamado

          // Llama a la función capturaOnly cada medio segundo
          captureImage();
          capturaOnly();
        }
    }
  }
  


  // Verifica si ha pasado el tiempo del intervalo para el chequeo de WiFi
  if (currentMillis - lastWifiCheckTime >= wifiCheckInterval) {
    lastWifiCheckTime = currentMillis;  // Actualiza el tiempo del último chequeo

    // Verifica el estado de la conexión WiFi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println(-7);
      WiFi.reconnect();  // Intenta reconectar
    } else {
      Serial.println(7);
    }
  }

  // Handle the client requests (HTTP endpoints)
  server.handleClient();

}



