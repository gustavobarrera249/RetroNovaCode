#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <UbidotsEsp32Mqtt.h>
#include "space_invaders.h"
#include "joystick.h"
#include "pauseMenu.h"
#include "config.h"
#include "charact.h"
#include "utils.h"
#include "menu.h"
#include "naves.h"

;extern Max72xxPanel matrix;
extern Joystick joystick;

const int bordeIzquierdo = 0;
const int bordeDerecho = 0;

const int playerStartX = 16;
const int playerY = 28;
int playerX = playerStartX;
int scoreInv = 0;

const int numRows = 4; // Número de filas de naves
const int numCols = 4; // Número de naves por fila
int invaderX[numRows][numCols];
int invaderY[numRows][numCols];
bool invaderDirection = true; // true = derecha, false = izquierda

void setupSpaceInvaders();
void handleInputInv();
void updateInvaders();
void drawGameInv();
void drawScoreInv();
void spawnInvaders();
bool checkCollisionInv(int x, int y);
void drawNave(int tipo, int x, int y);
void drawEscudo(int x, int y);
void drawPlayer(int x, int y);

void setupSpaceInvaders() {     
    randomSeed(analogRead(0));
    playerX = playerStartX;
    spawnInvaders();
    initPauseMenu(); // Inicializa el menú de pausa
}

void loopSpaceInvaders() {
    if (digitalRead(PAUSE_BUTTON_PIN) == LOW) {
        handlePauseMenu(); // Maneja el menú de pausa
    } else {
        handleInputInv();
        updateInvaders();
        drawGameInv();
        delay(200);
    }
}

void handleInputInv() {
    int xValue = joystick.readX();

    if (xValue < 1000) {
        playerX--;
        if (playerX < bordeIzquierdo + 1) playerX = bordeIzquierdo + 1; // Limita el movimiento a la izquierda dentro del área de juego
    } else if (xValue > 3000) {
        playerX++;
        if (playerX > 31-bordeDerecho) playerX = 31-bordeDerecho; // Limita el movimiento a la derecha dentro del área de juego
    }
}

void updateInvaders() {
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (invaderDirection) {
                invaderX[i][j]++;
                if (invaderX[i][j] > 31-bordeDerecho) {
                    invaderDirection = false;
                    for (int k = 0; k < numRows; k++) {
                        for (int l = 0; l < numCols; l++) {
                            invaderY[k][l]++;
                        }
                    }
                }
            } else {
                invaderX[i][j]--;
                if (invaderX[i][j] < bordeIzquierdo) {
                    invaderDirection = true;
                    for (int k = 0; k < numRows; k++) {
                        for (int l = 0; l < numCols; l++) {
                            invaderY[k][l]++;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (checkCollisionInv(invaderX[i][j], invaderY[i][j])) {
                scoreInv += 100; // Aumenta el puntaje cuando un invasor es destruido
                invaderX[i][j] = random(bordeIzquierdo, 31-bordeDerecho);
                invaderY[i][j] = random(0, 5);
            }
        }
    }
}

void drawGameInv() {
    matrix.fillScreen(LOW);

    // drawBorders(); // Dibuja los bordes

    drawPlayer(playerX, playerY); // Dibuja el jugador

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            drawNave((i < 2) ? 0 : 1, invaderX[i][j], invaderY[i][j]); // Tipo de nave según la fila
        }
    }

    // Dibuja los escudos
    drawEscudo(5, 22);
    drawEscudo(11, 22);
    drawEscudo(17, 22);
    drawEscudo(23, 22);

    // drawScoreInv(); // Dibuja el puntaje en el área correspondiente
    matrix.write();
}

void drawScoreInv() {
    char scoreStr[10];
    sprintf(scoreStr, "%d", scoreInv); // Convierte el puntaje a cadena de texto

    drawTextOnMatrix(scoreStr, 25, 2); // Dibuja el puntaje en el área correspondiente (por ejemplo, en el borde derecho)
}

void spawnInvaders() {
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            invaderX[i][j] = 8 + j * 5; // Posiciones iniciales ajustadas
            invaderY[i][j] = i * 4;     // Posiciones iniciales ajustadas
        }
    }
}

bool checkCollisionInv(int x, int y) {
    if (abs(x - playerX) <= 1 && abs(y - playerY) <= 1) {
        return true;
    }
    return false;
}

void drawNave(int tipo, int x, int y) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            if (NAVES[tipo][i] & (1 << j)) {
                matrix.drawPixel(x + j, y + i, HIGH);
            }
        }
    }
}

void drawEscudo(int x, int y) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            if (shield[0][i] & (1 << j)) {
                matrix.drawPixel(x + j, y + i, HIGH);
            }
        }
    }
}

void drawPlayer(int x, int y) {
    matrix.drawPixel(x, y, HIGH);
    matrix.drawPixel(x - 1, y + 1, HIGH);
    matrix.drawPixel(x + 1, y + 1, HIGH);
    matrix.drawPixel(x, y + 1, HIGH);
}



// #include <Arduino.h>
// #include <SPI.h>
// #include <Adafruit_GFX.h>
// #include <Max72xxPanel.h>
// #include "space_invaders.h"
// #include "joystick.h"
// #include "pauseMenu.h"
// #include "config.h"
// #include "charact.h"
// #include "utils.h"
// #include "menu.h"

// extern Max72xxPanel matrix;
// extern Joystick joystick;

// const int playerStartX = 16;
// const int playerY = 28;
// int playerX = playerStartX;
// int scoreInv = 0;

// const int invaderRows = 3;  // Número de filas de invasores
// const int invaderCols = 5;  // Número de columnas de invasores
// int invaderX[invaderRows][invaderCols];
// int invaderY[invaderRows];
// bool invaderDirection = true; // true = derecha, false = izquierda

// int bulletX = -1;
// int bulletY = -1;
// bool bulletActive = false;
// bool gameOver = false;

// void setupSpaceInvaders();
// void handleInputInv();
// void updateInvaders();
// void updateBullet();
// void handlePauseButton();
// void drawGameInv();
// void drawScoreInv();
// void spawnInvaders();
// bool checkCollisionInv(int x, int y);
// void resetInvaders();
// void endGame();

// void setupSpaceInvaders() {     
//     randomSeed(analogRead(14));
//     playerX = playerStartX;
//     spawnInvaders();
//     initPauseMenu(); // Inicializa el menú de pausa
// }

// void loopSpaceInvaders() {
//     if (digitalRead(PAUSE_BUTTON_PIN) == LOW) {
//         handlePauseButton();
//         handlePauseMenu(); // Maneja el menú de pausa
//     } else {
//         if (!gameOver) {
//             handleInputInv();
//             updateInvaders();
//             updateBullet();
//             drawGameInv();
//             delay(200);
//         } else {
//             endGame();
//         }
//     }
// }

// void handleInputInv() {
//     int xValue = joystick.readX();

//     if (xValue < 1) {
//         playerX--;
//         if (playerX < 8) playerX = 8; // Limita el movimiento a la izquierda dentro del área de juego
//     } else if (xValue > 4094) {
//         playerX++;
//         if (playerX > 23) playerX = 23; // Limita el movimiento a la derecha dentro del área de juego
//     }

//     if (joystick.isPressed() && !bulletActive) {
//         bulletX = playerX;
//         bulletY = playerY - 1;
//         bulletActive = true;
//     }
// }

// void updateInvaders() {
//     bool invadersReachedPlayer = false;

//     for (int row = 0; row < invaderRows; row++) {
//         if (invaderY[row] >= playerY - 2) { // Si algún invasor alcanza la fila del jugador
//             invadersReachedPlayer = true;
//             break;
//         }
//     }

//     if (!invadersReachedPlayer) {
//         for (int row = 0; row < invaderRows; row++) {
//             for (int col = 0; col < invaderCols; col++) {
//                 if (invaderDirection) {
//                     invaderX[row][col]++;
//                     if (invaderX[row][col] > 23) {
//                         invaderDirection = false;
//                         for (int r = 0; r < invaderRows; r++) {
//                             invaderY[r]++;
//                         }
//                     }
//                 } else {
//                     invaderX[row][col]--;
//                     if (invaderX[row][col] < 8) {
//                         invaderDirection = true;
//                         for (int r = 0; r < invaderRows; r++) {
//                             invaderY[r]++;
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     // Verifica colisiones entre el jugador y los invasores
//     for (int row = 0; row < invaderRows; row++) {
//         for (int col = 0; col < invaderCols; col++) {
//             if (checkCollisionInv(playerX, playerY)) {
//                 gameOver = true;
//                 return;
//             }
//         }
//     }
// }

// void updateBullet() {
//     if (bulletActive) {
//         bulletY--;
//         if (bulletY < 0) {
//             bulletActive = false;
//         } else {
//             for (int row = 0; row < invaderRows; row++) {
//                 for (int col = 0; col < invaderCols; col++) {
//                     if (invaderX[row][col] != -1 && checkCollisionInv(bulletX, bulletY)) {
//                         scoreInv += 100;
//                         invaderX[row][col] = -1; // Elimina el invasor
//                         bulletActive = false;
//                         break;
//                     }
//                 }
//             }
//         }
//     }
// }

// void drawGameInv() {
//     matrix.fillScreen(LOW);

//     drawBorders(); // Dibuja los bordes

//     // Dibuja al jugador como una "T"
//     matrix.drawPixel(playerX - 1, playerY, HIGH);
//     matrix.drawPixel(playerX, playerY, HIGH);
//     matrix.drawPixel(playerX + 1, playerY, HIGH);
//     matrix.drawPixel(playerX, playerY - 1, HIGH);

//     // Dibuja el disparo si está activo
//     if (bulletActive) {
//         matrix.drawPixel(bulletX, bulletY, HIGH);
//     }

//     // Dibuja a los invasores
//     for (int row = 0; row < invaderRows; row++) {
//         for (int col = 0; col < invaderCols; col++) {
//             if (invaderX[row][col] != -1) {
//                 matrix.drawPixel(invaderX[row][col] - 1, invaderY[row], HIGH);
//                 matrix.drawPixel(invaderX[row][col], invaderY[row], HIGH);
//                 matrix.drawPixel(invaderX[row][col] + 1, invaderY[row], HIGH);
//                 matrix.drawPixel(invaderX[row][col] - 1, invaderY[row] + 1, HIGH);
//                 matrix.drawPixel(invaderX[row][col], invaderY[row] + 1, HIGH);
//                 matrix.drawPixel(invaderX[row][col] + 1, invaderY[row] + 1, HIGH);
//             }
//         }
//     }

//     drawScoreInv(); // Dibuja el puntaje en el área correspondiente
//     matrix.write();
// }

// void drawScoreInv() {
//     char scoreStr[10];
//     sprintf(scoreStr, "%d", scoreInv); // Convierte el puntaje a cadena de texto

//     drawVertTextOnMatrix(scoreStr, 25, 2); // Dibuja el puntaje en el área correspondiente (por ejemplo, en el borde derecho)
// }

// void spawnInvaders() {
//     for (int row = 0; row < invaderRows; row++) {
//         for (int col = 0; col < invaderCols; col++) {
//             invaderX[row][col] = 10 + (col * 4); // Coloca los invasores en columnas con espacio adecuado
//         }
//         invaderY[row] = row * 3; // Ajusta la separación entre filas
//     }
// }

// bool checkCollisionInv(int x, int y) {
//     // Revisa la colisión con los bordes del invasor 3x2
//     for (int row = 0; row < invaderRows; row++) {
//         for (int col = 0; col < invaderCols; col++) {
//             if (invaderX[row][col] != -1 && abs(x - invaderX[row][col]) <= 1 && abs(y - invaderY[row]) <= 1) {
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// void endGame() {
//     matrix.fillScreen(LOW);
//     drawTextOnMatrix("GAME", 2, 10); 
//     drawTextOnMatrix("OVER", 2, 18); 
//     matrix.write();
//     while (true) {
//         // El juego se detiene aquí, esperando un reinicio manual
//     }
// }




// #include <Arduino.h>
// #include <SPI.h>
// #include <Adafruit_GFX.h>
// #include <Max72xxPanel.h>
// #include "space_invaders.h"
// #include "joystick.h"
// #include "pauseMenu.h"
// #include "config.h"
// #include "charact.h"
// #include "utils.h"
// #include "menu.h"

// extern Max72xxPanel matrix;
// extern Joystick joystick;

// const int playerStartX = 16;
// const int playerY = 28;
// int playerX = playerStartX;
// int scoreInv = 0;

// const int numInvaders = 5;
// int waitTime = 1;
// int waiting = 0;
// int invaderX[numInvaders];
// int invaderY[numInvaders];
// bool invaderDirection = true; // true = derecha, false = izquierda

// int bulletX = -1;
// int bulletY = -1;
// bool bulletActive = false;

// void setupSpaceInvaders();
// void handleInputInv();
// void updateInvaders();
// void updateBullet();
// void handlePauseButton();
// void drawGameInv();
// void drawScoreInv();
// void spawnInvaders();
// bool checkCollisionInv(int x, int y);
// void resetInvaders();

// void setupSpaceInvaders() {     
//     randomSeed(analogRead(14));
//     playerX = playerStartX;
//     spawnInvaders();
//     initPauseMenu(); // Inicializa el menú de pausa
// }

// void loopSpaceInvaders() {
//     if (digitalRead(PAUSE_BUTTON_PIN) == LOW) {
//         handlePauseButton();
//         handlePauseMenu(); // Maneja el menú de pausa
//     } else {
//         handleInputInv();
//         updateInvaders();
//         updateBullet();
//         drawGameInv();
//         delay(200);
//     }
// }

// void handleInputInv() {
//     int xValue = joystick.readX();

//     if (xValue < 1) {
//         playerX--;
//         if (playerX < 8) playerX = 8; // Limita el movimiento a la izquierda dentro del área de juego
//     } else if (xValue > 4094) {
//         playerX++;
//         if (playerX > 23) playerX = 23; // Limita el movimiento a la derecha dentro del área de juego
//     }

//     if (joystick.isPressed() && !bulletActive) {
//         bulletX = playerX;
//         bulletY = playerY - 1;
//         bulletActive = true;
//     }
// }

// void updateInvaders() {
//     waiting++;
//     if (waiting >= waitTime){
//         waiting = 0;
//         for (int i = 0; i < numInvaders; i++) {
//             if (invaderDirection) {
//                 if (invaderX[i] >= 23) {
//                     invaderDirection = false;
//                     if (invaderY[i] != playerY){
//                         for (int j = 0; j < numInvaders; j++) {
//                             invaderY[j]++;
//                         }
//                     }
//                 }
//                 else {
//                     for (int j = 0; j < numInvaders; j++) {
//                         invaderX[i]++;
//                     }
//                 }
//             } 
//             else {
//                 if (invaderX[i] <= 8) {
//                     invaderDirection = true;
//                     if (invaderY[i] != playerY){
//                         for (int j = 0; j < numInvaders; j++) {
//                             invaderY[j]++;
//                         }
//                     }
//                 }
//                 else {
//                     for (int j = 0; j < numInvaders; j++) {
//                         invaderX[i]--;
//                     }
//                 }
//             }
//         }

//         for (int i = 0; i < numInvaders; i++) {
//         if (invaderX[i] != -1 && checkCollisionInv(invaderX[i], invaderY[i])) {
//             scoreInv += 100; // Aumenta el puntaje cuando un invasor es destruido
//             invaderX[i] = -1; // Marca el invasor como eliminado
//         }
//     }

//         bool allDestroyed = true;
//         for (int i = 0; i < numInvaders; i++) {
//             if (invaderX[i] != -1) {
//                 allDestroyed = false;
//                 break;
//             }
//         }

//         if (allDestroyed) {
//             spawnInvaders();
//         }
//     }
// }

// void drawGameInv() {
//     matrix.fillScreen(LOW);

//     drawBorders(); // Dibuja los bordes

//     // Dibuja al jugador como una "T"
//     matrix.drawPixel(playerX - 1, playerY, HIGH);
//     matrix.drawPixel(playerX, playerY, HIGH);
//     matrix.drawPixel(playerX + 1, playerY, HIGH);
//     matrix.drawPixel(playerX, playerY - 1, HIGH);

//     // Dibuja el disparo si está activo
//     if (bulletActive) {
//         matrix.drawPixel(bulletX, bulletY + 1, HIGH);
//         matrix.drawPixel(bulletX, bulletY, HIGH);
//     }

//     // Dibuja a los invasores
//     for (int i = 0; i < numInvaders; i++) {
//         if (invaderX[i] != -1) {
//             matrix.drawPixel(invaderX[i], invaderY[i], HIGH);
//         }
//     }

//     drawScoreInv(); // Dibuja el puntaje en el área correspondiente
//     matrix.write();
// }


// void drawScoreInv() {
//     char scoreStr[10];
//     sprintf(scoreStr, "%d", scoreInv); // Convierte el puntaje a cadena de texto

//     drawTextOnMatrix(scoreStr, 25, 2); // Dibuja el puntaje en el área correspondiente (por ejemplo, en el borde derecho)
// }

// void updateBullet() {
//     if (bulletActive) {
//         bulletY++;
//         if (bulletY > 31) {
//             bulletActive = false;
//         } else {
//             for (int i = 0; i < numInvaders; i++) {
//                 if (invaderX[i] != -1 && checkCollisionInv(bulletX, bulletY)) {
//                     scoreInv += 100;
//                     invaderX[i] = -1;
//                     bulletActive = false;
//                     break;
//                 }
//             }
//         }
//     }
// }


// void spawnInvaders() {
//     for (int i = 0; i < numInvaders; i++) {
//         invaderX[i] = 8 + (i * 2); // Distribuye los invasores en posiciones fijas
//         invaderY[i] = 0;
//     }
// }

// bool checkCollisionInv(int x, int y) {
//     if (abs(x - playerX) <= 1 && abs(y - playerY) <= 1) {
//         return true;
//     }
//     return false;
// }
