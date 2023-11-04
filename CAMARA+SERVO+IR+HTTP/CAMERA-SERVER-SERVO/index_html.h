const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <body>
    <h1>Feed de la Camara</h1>
    <img id="capturedImage" alt="Captured Image">
    <script>
      function refreshImage() {
        // Make a GET request to your ESP32 server to fetch the base64-encoded image
        fetch("/capture")
          .then(response => response.text())
          .then(data => {
            // Set the source of the image element
            document.getElementById("capturedImage").src = data;
          })
          .catch(error => console.error("Error fetching image: " + error));
      }

      // Refresh the image at the specified interval
      setInterval(refreshImage, 50); // Adjust the interval as needed
    </script>
  </body>
</html>
)rawliteral";
