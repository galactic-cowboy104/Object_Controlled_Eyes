# Object_Controlled_Eyes

Este proyecto implementa un sistema de seguimiento de objetos en tiempo real utilizando visión artificial para controlar una animación de ojos en una pantalla TFT. El sistema se basa en una arquitectura distribuida: un script de **Python** se encarga del procesamiento de imagen pesado y un **ESP32** gestiona la interfaz gráfica y las animaciones orgánicas.

## Materiales Necesarios

- **ESP32**: Microcontrolador principal encargado de la recepción de datos y control de la pantalla.
- **Pantalla TFT ST7735 (160x80)**: Display donde se muestra la animación de los ojos.
- **Webcam Externa**: Dispositivo de captura de video para el seguimiento del objeto.
- **Laptop**: Actúa como servidor de procesamiento para ejecutar el algoritmo de visión artificial en Python.
- **Cable USB**: Para la comunicación serial y alimentación del ESP32.
- **Cables de Conexión**: Para realizar la interfaz SPI entre la pantalla y el microcontrolador.

## Contenido del Repositorio

1. **`Vision_Controlled_Object_Tracking.py`**: Script de Python que utiliza OpenCV para detectar un objeto por color (HSV), calcula su centro de masa y envía las coordenadas mapeadas vía serial.
2. **`Vision_Controlled_Eye_Animation.ino`**: Código para el ESP32 que recibe las coordenadas, gestiona el borrado selectivo de pixeles para evitar parpadeos y ejecuta una animación de parpadeo aleatorio.
3. **Diagrama de Conexiones**: Detalle de los pines SPI utilizados.

## Instrucciones de Uso

### Configuración del ESP32 (Firmware)
1. Abre el archivo `.ino` en el IDE de Arduino.
2. Instala las librerías necesarias desde el gestor de bibliotecas:
   - **Adafruit GFX Library**
   - **Adafruit ST7735 and ST7789 Library**
3. Verifica que los pines definidos coincidan con tu cableado (CS: 5, DC: 4, RST: 2).
4. Sube el código a tu placa ESP32.

### Configuración de Python (Visión Artificial)
1. Asegúrate de tener Python 3.x instalado.
2. Instala las dependencias necesarias:
   ```bash
   pip install opencv-python numpy pyserial
   ```
3. En el script `Vision_Controlled_Object_Tracking.py`, asegúrate de que la variable `SERIAL_PORT` apunte al puerto correcto (ej. `/dev/ttyUSB0` en Linux o `COMX` en Windows).

### Conexiones Físicas
Realiza las siguientes conexiones entre el ESP32 y la pantalla ST7735:

| Pantalla TFT | ESP32 (GPIO) | Función |
| :--- | :--- | :--- |
| **VCC** | 3.3V | Alimentación |
| **GND** | GND | Tierra |
| **SCK** | GPIO 18 | Reloj SPI |
| **SDA** | GPIO 23 | Datos SPI |
| **CS** | GPIO 5 | Chip Select |
| **RESET** | GPIO 4 | Reset |
| **DC** | GPIO 2 | Data/Command |



### Uso del Sistema
1. Conecta el ESP32 a la computadora.
2. Coloca la webcam apuntando al área de trabajo.
3. Ejecuta el script de Python:
   ```bash
   python Vision_Controlled_Object_Tracking.py
   ```
4. Mueve un objeto de color naranja (o el color que hayas calibrado) frente a la cámara. Los ojos en la pantalla TFT seguirán el movimiento del objeto de forma fluida.

## Detalles Técnicos

- **Mapeo de Coordenadas**: Se utiliza la función $np.interp()$ para traducir la resolución de la cámara (1280x720) al rango de movimiento permitido para las pupilas en la pantalla.
- **Borrado Selectivo**: Para optimizar el rendimiento y evitar el *flicker*, el ESP32 solo borra la posición anterior de los ojos antes de dibujar la nueva, en lugar de refrescar toda la pantalla.
- **Filtro HSV**: El seguimiento se basa en el espacio de color HSV para mayor robustez ante cambios de iluminación ambiental.



## Créditos

Este proyecto fue desarrollado por **Zoé Trejo**, Research Intern en el Laboratorio de Instrumentación Espacial (LINX) - UNAM, como parte de experimentos de control y visión artificial aplicados a micro-robótica.

---
¡Disfruta construyendo este sistema de visión artificial! 🚀🤖