// Объявляем, как библиотеку, чтобы иметь доступ
// к разделяемым ресурсам, например состоянию игры
//.pragma library

var gameState       // Локальное объявление состояния игры
                    // в нашем случае это будет игровая область gameArea
function getGameState() { return gameState; }

var gameField;      // Игровое поле, игровая сетка
// Создаём заготовку для вируса
var virusComponent = Qt.createComponent("Virus.qml");

// Создаём заготовку для частицы
var particleComponent = Qt.createComponent("Particle.qml");

var virus;
var particle;
var sprite;
var sprite2;
var count = 0;

function createVirusObjects() {
    virus = Qt.createComponent("Virus.qml");

    var x =getRandomRound(1, 500);
    var y = getRandomRound(1, 1000);
    count++;
    sprite = virus.createObject(grid, {"uri": count, "x": x, "y": y});

    if (sprite == null) {
        // Error Handling
        console.log("Error creating object");
    }

}

function createParticleObjects(startX, startY, endX, endY) {
    particle = Qt.createComponent("Particle.qml");

    count++;
    sprite2 = particle.createObject(grid, {"uri": count, "startX": startX, "startY": startY, "endX":endX, "endY":endY});

    if (sprite2 == null) {
        // Error Handling
        console.log("Error creating object");
    }

}


// Функция получения случайного целого числа
// в диапазоне чисел включительно
function getRandomRound(min, max)
{
    return Math.round(Math.random() * (max - min) + min);
}


function viewObject(obj){
    obj.forEach(function(item, i, obj){
        console.log(i + ' ' + item);
    });
}
