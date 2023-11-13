document.addEventListener("DOMContentLoaded", function() {
    
    function getImageFromPHP() {
        return fetch('get_image.php')
            .then(response => {
                if (!response.ok) {
                    throw new Error('Error de red');
                }
                return response.json();  // Cambiar a .json()
            });
    }

    function insertarImagen(images) {
        var imageContainer1 = document.getElementById("imageContainer1");
        var imageContainer2 = document.getElementById("imageContainer2");

        if (imageContainer1) {
            imageContainer1.innerHTML = '';
            let img1 = document.createElement("img");
            img1.src = images.image1; 
            imageContainer1.appendChild(img1);
        }

        if (imageContainer2) {
            imageContainer2.innerHTML = '';
            let img2 = document.createElement("img");
            img2.src = images.image2; 
            imageContainer2.appendChild(img2);
        }
    }

    function fetchAndInsertImage() {
        getImageFromPHP()
            .then(data => {
                insertarImagen(data);
            })
            .catch(error => {
                console.error('Error al obtener la imagen:', error);
            });
    }

    setInterval(fetchAndInsertImage, 1000);  // Cambiado de 500 a 5000 para actualizar cada 5 segundos

    fetchAndInsertImage();
});
document.addEventListener('DOMContentLoaded', function() {
    // Referencia a los botones
    const leftButton = document.getElementById('leftButton');
    const rightButton = document.getElementById('rightButton');

    sendValueToServer('0');

    leftButton.addEventListener('mousedown', function() {
        sendValueToServer('-1');
    });

    leftButton.addEventListener('mouseup', function() {
        sendValueToServer('0');
    });

    rightButton.addEventListener('mousedown', function() {
        sendValueToServer('1');
    });

    rightButton.addEventListener('mouseup', function() {
        sendValueToServer('0');
    });
});
document.addEventListener('DOMContentLoaded', function() {
    // Referencia a los botones
    const upButton = document.getElementById('upButton');
    const downButton = document.getElementById('downButton');

    sendValueToServer('0');

    // Cambiar los valores de los botones upButton y downButton
    downButton.addEventListener('mousedown', function() {
        sendValueToServer('2');  // Cambiar de '-2' a '2'
    });

    downButton.addEventListener('mouseup', function() {
        sendValueToServer('0');
    });

    upButton.addEventListener('mousedown', function() {
        sendValueToServer('-2');  // Cambiar de '2' a '-2'
    });

    upButton.addEventListener('mouseup', function() {
        sendValueToServer('0');
    });
});

function sendValueToServer(movimiento) {
    // Usamos Fetch para enviar la petición AJAX
    fetch('enviaValorServos.php', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: 'movimiento=' + movimiento
    })
    .then(response => response.text())  // Convirtiendo la respuesta a texto
    .then(data => {
        //alert(data);  // Mostramos la respuesta del servidor (mensaje de confirmación)
        console.log(data)
    })
    .catch(error => {
        console.error('Error:', error);
        console.log('Error al enviar el valor.');
    });
}

