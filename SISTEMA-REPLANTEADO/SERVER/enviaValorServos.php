<?php
require_once 'functions.php';
if (isset($_POST['movimiento'])) {
    $movimiento = $_POST['movimiento'];

    // Actualizar el valor en la base de datos
    $resultado = actualizarMovimiento($movimiento);

    if($resultado) {
        echo "Valor recibido y actualizado: " . $movimiento;
    } else {
        echo "Error al actualizar el valor en la base de datos.";
    }
} else {
    echo "No se recibió ningún valor.";
}
?>


