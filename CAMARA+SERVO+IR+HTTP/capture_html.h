const char capture_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <body>
    <h1>Imagen Capturada</h1>
    <img id="capturedImage" alt="Captura">
    <script>
      // Function to load the captured image when the page is opened
      function loadCapturedImage() {
        // Make a GET request to your ESP32 server to fetch the base64-encoded image
        fetch("/aux")
          .then(response => response.text())
          .then(data => {
            // Set the source of the image element
            document.getElementById("capturedImage").src = data;
          })
          .catch(error => console.error("Error fetching image: " + error));
      }

      // Load the captured image when the page is opened
      loadCapturedImage();
    </script>
  </body>
</html>
)rawliteral";
