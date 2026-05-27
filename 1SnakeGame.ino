
const long MOVE_INTERVAL = 350;
const int SNAKE_MAX = 64;
const int MIN = 0;
const int MAX = 7;

const byte ANODE_PINS[8] = {13, 12, 11, 10, 9, 8, 7, 6};
const byte CATHODE_PINS[8] = {A3, A2, A1, A0, 5, 4, 3, 2};

static byte ledOn[8][8];

int snakeX[SNAKE_MAX];
int snakeY[SNAKE_MAX];
int snakeLength = 1;
int xDirection = 1;
int yDirection = 0;

byte foodX, foodY;
long lastMoveTime = 0;

void setup() {
    for (byte i = 0; i < 8; i++) {
        pinMode(ANODE_PINS[i], OUTPUT);
        pinMode(CATHODE_PINS[i], OUTPUT);
    }

    for (byte i = 0; i < 8; i++) {
        digitalWrite(ANODE_PINS[i], HIGH);
        digitalWrite(CATHODE_PINS[i], HIGH);
    }

    // set up joystick
    pinMode(A5, INPUT); // x
    pinMode(A4, INPUT); // y

    // set up snake location
    snakeX[0] = 4;
    snakeY[0] = 4;

    spawnFood();

    Serial.begin(115200);
    Serial.setTimeout(100);
}

void loop() {
    joyStickValues();

    if (millis() - lastMoveTime > MOVE_INTERVAL) {
        slitherSnake();
        lastMoveTime = millis();
    }

    drawGame();
}

void joyStickValues() {
    int xVal = analogRead(A5);
    int yVal = analogRead(A4);
    
    // TODO: LAB OH
    if (xVal > 550 && (xDirection != -1)) {
        xDirection = 1; // right
        yDirection = 0;
    } 
    else if (xVal < 450 && (xDirection != 1)) {
        xDirection = -1; // left
        yDirection = 0;
    }
    else if (yVal < 450 && (yDirection != -1)) {
        xDirection = 0;
        yDirection = 1; // down
    }
    else if (yVal > 650 && (yDirection != 1)) {
        xDirection = 0;
        yDirection = -1; // up
    }
}

void slitherSnake() {
    int newX = snakeX[0] + xDirection;
    int newY = snakeY[0] + yDirection;

    if (outOfBounds(newX, newY) || collided(newX, newY)) {
        gameOver();
        return;
    }

    bool ateFood = (newX == foodX && newY == foodY);
    if (ateFood && snakeLength < SNAKE_MAX) {
        snakeLength++;
        spawnFood();
    }

    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    snakeX[0] = newX;
    snakeY[0] = newY;
}

void drawGame() {
    // Reset display buffer
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            ledOn[i][j] = 0;
        }
    }

    // Map snake to display buffer
    for (int i = 0; i < snakeLength; i++) {
        int x = snakeX[i];
        int y = snakeY[i];
        ledOn[x][y] = 1;
    }

    // Map food to display buffer
    ledOn[foodX][foodY] = 1;

    display(ledOn);
}

void spawnFood() {
    foodX = random(MIN, MAX + 1);
    foodY = random(MIN, MAX + 1);

    // regenerate food if collided
    while (collided(foodX, foodY)) {
        foodX = random(MIN, MAX + 1);
        foodY = random(MIN, MAX + 1);
    }
}

bool collided(int x, int y) {
    for (int i = 0; i < snakeLength; i++) {
        if (snakeX[i] == x && snakeY[i] == y) {
            return true;
        }
    }
    return false;
}

bool outOfBounds(int x, int y) {
    return (x < 0 || x > 7 || y < 0 || y > 7);
}

void gameOver() {
    for (byte i = 0; i < 8; i++) {
        digitalWrite(ANODE_PINS[i], LOW);
        for (byte j = 0; j < 8; j++) {
            digitalWrite(CATHODE_PINS[j], LOW);
            delayMicroseconds(50);
            digitalWrite(CATHODE_PINS[j], HIGH);
        }
    }

    snakeLength = 1;
    snakeX[0] = 4;
    snakeY[0] = 4;
    xDirection = 1;
    yDirection = 0;
    spawnFood();
}

void display(byte pattern[8][8]) {
    for (byte i = 0; i < 8; i++) {
        for (byte j = 0; j < 8; j++) {
            if (pattern[i][j] == 1) {
                digitalWrite(CATHODE_PINS[j], LOW);
            } else {
                digitalWrite(CATHODE_PINS[j], HIGH);
            }
        }
        
        digitalWrite(ANODE_PINS[i], LOW);
        delayMicroseconds(10); // Adjust higher (e.g., 1000) if screen is too dim
        digitalWrite(ANODE_PINS[i], HIGH);
    }
}
