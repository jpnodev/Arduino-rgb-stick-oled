#include <SPI.h>
#include <Wire.h>
// #include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display
#define OLED_MOSI   11
#define OLED_CLK   13
#define OLED_DC    10
#define OLED_CS    9
#define OLED_RST 8

// Create the OLED display
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

//Joystick
#define VRx A0
#define VRy A1

//RGB Led
#define R A3
#define G A4
#define B A5


//Variables
int sliderSelected = 1;
int sliders[3] = {0, 0, 0};

uint8_t redValue = 0;
uint8_t greenValue = 0;
uint8_t blueValue = 0;

void setup() {
  //Starting serial log
  Serial.begin(9600);
  while (!Serial); // Wait for serial connection

  // Start OLED
  display.begin(0, true); // we dont use the i2c address but we will reset!
  // Clear the buffer
  display.clearDisplay();
  display.display();
  //Draw a rect
  display.drawRect(0, 0, display.width(), display.height()/2, 1);
  display.display();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.cp437(true);

  // Initialisation des sliders
  for (int i = 0; i < 3; i++) {
    sliders[i] = 1;
  }


  //Init des pins
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  
}

// Fonction de mappage
float mapJoystick(float value) {
  return (value - 512) / 512;
}

bool joyYPressed = false;

void loop() {
  // Lecture du joystick
  float joyX = mapJoystick(analogRead(VRx));
  float joyY = mapJoystick(analogRead(VRy));

  // Détection du mouvement haut/bas
  if (joyYPressed) {
    if (abs(joyY) < 0.05) {
      joyYPressed = false;
    }
  } else if (joyY < -0.75) {
    // Déplacement vers le haut
    sliderSelected--;
    joyYPressed = true;
  } else if (joyY > 0.75) {
    // Déplacement vers le bas
    sliderSelected++;
    joyYPressed = true;
  }
  sliderSelected = (sliderSelected + 3) % 3;

  // Modification de la valeur du slider sélectionné
  if (abs(joyX) > 0.03) {
    sliders[sliderSelected] += int(joyX * 2);
  }

  // Contraintes sur les valeurs des sliders
  for (int i = 0; i < 3; i++) {
    if (sliders[i] < 0) {
      sliders[i] = 0;
    } else if (sliders[i] > 63) {
      sliders[i] = 63;
    }
  }

  // Conversion des valeurs des sliders en valeurs RGB
  redValue = map(sliders[0], 0, 63, 0, 255);
  greenValue = map(sliders[1], 0, 63, 0, 255);
  blueValue = map(sliders[2], 0, 63, 0, 255);

  analogWrite(R, redValue);
  analogWrite(G, greenValue);
  analogWrite(B, blueValue);
  

  // Affichage des sliders sur l'écran
  display.clearDisplay();
  const char labels[] = "RGB";
  for (int i = 0; i < 3; i++) {
    display.drawFastHLine(0, 7 + i * 19, sliders[i], 1);
    display.drawFastVLine(sliders[i], 6 + i * 19, 3, 1);
    display.setCursor(70, 4 + i * 19);
    display.write(labels[i]);
    // char *labels = "RGB";
    // int16_t x = 70;
    // int16_t y = 6 + i * 19;
    // unsigned char c = labels[i];
    // uint16_t WHITE = 0xFFFF;
    // uint16_t BLACK = 0x0000;
    // uint8_t size = 1;

    // display.drawChar(x, y, c, WHITE);
  }

  display.drawLine(66, 7 + sliderSelected * 19, 68, 7 + sliderSelected * 19 - 2, 1);
  display.drawLine(78, 7 + sliderSelected * 19, 76, 7 + sliderSelected * 19 - 2, 1);

  display.drawLine(66, 7 + sliderSelected * 19, 68, 7 + sliderSelected * 19 + 2, 1);
  display.drawLine(76, 7 + sliderSelected * 19, 76, 7 + sliderSelected * 19 + 2, 1);
  
  display.display();
}
