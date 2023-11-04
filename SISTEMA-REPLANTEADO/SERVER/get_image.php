<?php
require_once 'functions.php';

$image_base64_1 = obtenerImagenBase64PorID(1);
$image_base64_2 = obtenerImagenBase64PorID(2);

$response = [
    'image1' => $image_base64_1,
    'image2' => $image_base64_2
];

header('Content-Type: application/json');
echo json_encode($response);
?>
