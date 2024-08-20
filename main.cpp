//main.cpp
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <WiFi.h>
#include <UbidotsEsp32Mqtt.h>
#include "joystick.h"
#include "menu.h"
#include "tetris.h"
#include "space_invaders.h"

#define PIN_CS 5 //Pin CS para comunicación SPI
#define H_DISPLAYS 4 //displays horizontales
#define V_DISPLAYS 4 //displays verticales

Max72xxPanel matrix = Max72xxPanel(PIN_CS, H_DISPLAYS, V_DISPLAYS); //Inicializar matriz
Joystick joystick; //Inicializar Joystick
GameSelection currentGame = NONE;

const char *UBIDOTS_TOKEN = "BBUS-1fJ8PGNk9KXkdxcl0pSOhzBKfN3lDq";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "AndroidAP869D";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "eenv7549";      // Put here your Wi-Fi password
const char *PUBLISH_DEVICE_LABEL = "esp32-arcade";     // Put here your Device label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL = "tempScore";   // Put here your Variable label to which data  will be published
const char *SUBSCRIBE_DEVICE_LABEL = "esp32-arcade";   // Replace with the device label to subscribe to
const char *SUBSCRIBE_VARIABLE_LABEL = "tempScore"; // Replace with the variable label to subscribe to
const char *PUBLISH_SCORES[] = {"Score1", "Score2", "Score3", "Score4", "Score5"};
const char *PUBLISH_ALIAS[] = {"Alias1", "Alias2", "Alias3", "Alias4", "Alias5"};


Ubidots ubidots(UBIDOTS_TOKEN);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
}


void setup() {
    Serial.begin(115200); //Inicializar comunicación serial

    // Configuarar posiciones de los displays
    for (int i = 0; i < V_DISPLAYS; i++) {
        for (int j = 0; j < H_DISPLAYS; j++) {
            matrix.setPosition(i + j * H_DISPLAYS, j, i);
        }
    }
    matrix.setRotation(0); // Asegura que la rotación sea la correcta
    matrix.setIntensity(1); //Ajustar brillo de la matriz
    matrix.fillScreen(LOW); // Limpiar la pantalla inicialmente
    matrix.write(); // Actualizar la pantalla

    Serial.println("Matriz LED inicializada");

    randomSeed(analogRead(14)); // Inicializar generador de números aleatorios
    joystick.begin(); // Inicializar joystick
    Serial.println("Joystick inicializado");

    showMenu(); // Mostrar menú inicial
    Serial.println("Menú inicial mostrado");

    ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
    ubidots.setCallback(callback);
    ubidots.setup();
    ubidots.reconnect();
    ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively


}

void loop() {
    if (currentGame == NONE) {
        currentGame = handleMenuInput();
        if (currentGame == TETRIS) {
            setupTetris();
        } else if (currentGame == INVADERS) {
            setupSpaceInvaders();
        }
    } else if (currentGame == TETRIS) {
        loopTetris();
    } else if (currentGame == INVADERS) {
        loopSpaceInvaders();
    }

    ubidots.loop();
    // Serial.print("X: ");
    // Serial.println(joystick.readX());
    // Serial.print("Y: ");
    // Serial.println(joystick.readY());
    // Serial.println(millis());
}

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   Serial.print("MOSI: ");
//   Serial.println(MOSI);
//   Serial.print("MISO: ");
//   Serial.println(MISO);
//   Serial.print("SCK: ");
//   Serial.println(SCK);
//   Serial.print("SS: ");
//   Serial.println(SS);  
// }

// void loop() {
//   // put your main code here, to run repeatedly:
// }