<?php
$host = "localhost";
$usuario = "id21461313_admin";
$password = "Esp32cam#";
$base_de_datos = "id21461313_postesp32cambd";

// Crear conexión
$conn = new mysqli($host, $usuario, $password, $base_de_datos);

// Comprobar conexión
if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}

?>