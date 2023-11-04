<?php
require_once 'functions.php';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (isset($_POST['image'])) {
        $base64Image = $_POST['image'];
        
        // Obtener el número de cámara desde el POST si está presente
        //$numeroDeCamara = isset($_POST['camara']);

        // Eliminar espacios en blanco y retornos de carro
        $base64ImageAlt = str_replace(' ', '+', $base64Image);
        
        // Suponiendo que tu función actualizarImagenBase64 ahora acepta un segundo parámetro para el número de cámara
        if (actualizarImagenBase64(1, $base64ImageAlt)) {
            echo "Recibido correctamente desde la camara: 1";
        } else {
            echo "Error al guardar la imagen en la base de datos.";
        }

    } else {
        echo "No se ha recibido ninguna imagen.";
    }
} else {
    echo "Solicitud no válida.";
}
?>
