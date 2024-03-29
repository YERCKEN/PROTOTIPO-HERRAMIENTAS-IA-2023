<?php
require_once 'config.php';

function guardarImagenBase64($imagen, $horario) {
    global $conn;

    $stmt = $conn->prepare("INSERT INTO mi_tabla (data_base64, horario) VALUES (?, ?)");
    $stmt->bind_param("ss", $imagen, $horario);
    $resultado = $stmt->execute();

    $stmt->close();

    return $resultado;
}
function actualizarImagenBase64($id, $imagen, $horario) {
    global $conn;

    $stmt = $conn->prepare("UPDATE mi_tabla SET data_base64 = ?, horario = ? WHERE id = ?");
    $stmt->bind_param("sss", $imagen, $horario, $id);

    $resultado = $stmt->execute();

    $stmt->close();

    $conn->close();

    return $resultado;
}

function obtenerImagenBase64PorID($id) {
    global $conn;

    $stmt = $conn->prepare("SELECT data_base64 FROM mi_tabla WHERE id = ?");
    $stmt->bind_param("i", $id);

    $stmt->execute();
    $resultado = $stmt->get_result();

    if ($resultado && $resultado->num_rows > 0) {
        $fila = $resultado->fetch_assoc();
        $stmt->close();
        return $fila['data_base64'];
    } else {
        $stmt->close();
        return false;
    }
    $conn->close();
}
function obtenerUltimaImagenBase64() {
    global $conn;

    // Cambia la consulta para obtener la última imagen ordenada por ID descendente
    $stmt = $conn->prepare("SELECT data_base64 FROM mi_tabla ORDER BY id DESC LIMIT 1");

    $stmt->execute();
    $resultado = $stmt->get_result();

    if ($resultado && $resultado->num_rows > 0) {
        $fila = $resultado->fetch_assoc();
        $stmt->close();
        return $fila['data_base64'];
    } else {
        $stmt->close();
        return false;
    }
    $conn->close();
}
function actualizarMovimiento($movimiento) {
    global $conn;

    $stmt = $conn->prepare("UPDATE movimientoServo SET movimiento = ? WHERE id = 1");
    $stmt->bind_param("s", $movimiento);

    $resultado = $stmt->execute();

    $stmt->close();

    return $resultado;
}
function obtenerMovimiento() {
    global $conn;

    $sql = "SELECT movimiento FROM movimientoServo WHERE id = 1";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        return $row['movimiento'];
    } else {
        return null;
    }
}

?>