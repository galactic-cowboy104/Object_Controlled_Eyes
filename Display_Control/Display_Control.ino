#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Configuración de los pines de la pantalla (ESP32)
#define TFT_CS     5
#define TFT_DC     4
#define TFT_RST    2

// Inicializa el objeto de la librería de la pantalla
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Define los colores para la interfaz
#define NEGRO   ST77XX_BLACK
#define CYAN    tft.color565(0, 255, 255) // Cyan brillante

// Parámetros de dimensiones y espaciado de los ojos
int eyeWidth = 40;       
int eyeHeight = 50;      
int eyeGap = 10;         
int screenW = 160;       
int screenH = 80;        

// Variables para las posiciones base de los ojos
int leftEyeX_Base;
int rightEyeX_Base;
int eyeY_Base;

// Coordenadas de desplazamiento recibidas desde el script de Python
int moveX = 0;
int moveY = 0;

// Almacenamiento de la posición previa para limpiar el rastro del dibujo
int prevEyeOffsetX = 0;
int prevEyeOffsetY = 0;

// Variables de control para el parpadeo automático
unsigned long lastBlinkTime = 0;
long blinkInterval = 3000; 

void setup() {
  // Inicia la comunicación serie a la misma velocidad que el script de Python
  Serial.begin(115200);

  // Inicializa el driver de la pantalla con la resolución de 160x80
  tft.initR(INITR_MINI160x80); 
  tft.setRotation(3); // Establece el modo horizontal
  tft.fillScreen(NEGRO); // Asegura que la pantalla empiece en negro

  // Calcula las posiciones centrales de cada ojo en la pantalla
  leftEyeX_Base = (screenW / 2) - eyeWidth - (eyeGap / 2);
  rightEyeX_Base = (screenW / 2) + (eyeGap / 2);
  eyeY_Base = (screenH - eyeHeight) / 2;

  // Genera una semilla aleatoria para el intervalo de parpadeo
  randomSeed(analogRead(0));
  
  // Dibuja los ojos en su posición inicial
  drawEyes();
}

void loop() {
  // Lógica para activar el parpadeo en tiempos aleatorios
  unsigned long currentMillis = millis();
  if (currentMillis - lastBlinkTime > blinkInterval) {
    blink();
    lastBlinkTime = currentMillis;
    blinkInterval = random(2000, 5000); 
  }

  // Comprueba si hay datos disponibles en el puerto serie
  if (Serial.available() > 0) {
    // Lee la cadena de texto hasta encontrar un salto de línea (\n)
    String data = Serial.readStringUntil('\n');
    // Busca la posición de la coma para separar las coordenadas
    int commaIndex = data.indexOf(',');
    
    if (commaIndex > 0) {
      // Extrae y convierte las coordenadas a números enteros
      int newX = data.substring(0, commaIndex).toInt();
      int newY = data.substring(commaIndex + 1).toInt();
      
      // Actualiza la animación solo si hay un cambio en el movimiento
      if (newX != moveX || newY != moveY) {
        moveX = newX;
        moveY = newY;
        drawEyes();
      }
    }
  }
}

void drawEyes() {
  // Borra el rastro de la posición anterior pintando rectángulos negros
  if (moveX != prevEyeOffsetX || moveY != prevEyeOffsetY) {
    tft.fillRect(leftEyeX_Base + prevEyeOffsetX, eyeY_Base + prevEyeOffsetY, eyeWidth, eyeHeight, NEGRO);
    tft.fillRect(rightEyeX_Base + prevEyeOffsetX, eyeY_Base + prevEyeOffsetY, eyeWidth, eyeHeight, NEGRO);
  }

  // Dibuja los nuevos ojos usando rectángulos redondeados en color cyan
  tft.fillRoundRect(leftEyeX_Base + moveX, eyeY_Base + moveY, eyeWidth, eyeHeight, 15, CYAN);
  tft.fillRoundRect(rightEyeX_Base + moveX, eyeY_Base + moveY, eyeWidth, eyeHeight, 15, CYAN);

  // Guarda la posición actual para el próximo ciclo de borrado
  prevEyeOffsetX = moveX;
  prevEyeOffsetY = moveY;
}

void blink() {
  // Animación de parpadeo bajando un bloque negro gradualmente
  for (int h = 0; h <= eyeHeight; h += 10) {
    tft.fillRect(leftEyeX_Base + prevEyeOffsetX, eyeY_Base + prevEyeOffsetY, eyeWidth, h, NEGRO);
    tft.fillRect(rightEyeX_Base + prevEyeOffsetX, eyeY_Base + prevEyeOffsetY, eyeWidth, h, NEGRO);
    delay(10); 
  }
  delay(100); 
  
  // Borra cualquier residuo y vuelve a mostrar los ojos abiertos
  tft.fillRect(leftEyeX_Base + prevEyeOffsetX, eyeY_Base + prevEyeOffsetY, eyeWidth, eyeHeight, NEGRO);
  tft.fillRect(rightEyeX_Base + prevEyeOffsetX, eyeY_Base + prevEyeOffsetY, eyeWidth, eyeHeight, NEGRO);
  drawEyes();
}