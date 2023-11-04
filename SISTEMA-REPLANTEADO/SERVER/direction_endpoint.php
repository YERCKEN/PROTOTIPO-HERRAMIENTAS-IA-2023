<?php
require_once 'functions.php';
session_start();

// Si se envía un valor de dirección desde la página web
if(isset($_POST['direction'])) {
    $_SESSION['direction'] = $_POST['direction'];

    // Obtener el valor de movimiento directamente de la base de datos
    $movimiento = obtenerMovimiento();
    
    if($movimiento !== null) {
        echo $movimiento;
    }


    exit;

}
?>


