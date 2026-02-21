import cv2
import numpy as np
import serial
import time

# --- CONFIGURACIÓN ---
# Cambia '/dev/ttyUSB0' por el puerto donde esté conectado tu ESP32
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200
WINDOW_NAME = "Tracking y Serial"

def main():
    # Valores calibrados para la detección del color naranja
    lower_orange = np.array([0, 144, 77])
    upper_orange = np.array([119, 255, 255])
    kernel = np.ones((11, 11), np.uint8)

    # Intenta establecer la conexión serie con el ESP32
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.05)
        time.sleep(2) # Da tiempo al ESP32 para que se reinicie y establezca la conexión
        print(f"Conectado al ESP32 en el puerto {SERIAL_PORT}")
    except Exception as e:
        print(f"Error: No se pudo conectar al ESP32 en {SERIAL_PORT}. {e}")
        return

    # Inicia la captura de video desde la cámara web (índice 0 con backend V4L2)
    cap = cv2.VideoCapture(0, cv2.CAP_V4L2)
    if not cap.isOpened():
        print("Error: No se pudo abrir la cámara web.")
        return

    # Configuración de resolución y tasa de refresco de la cámara
    cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*'MJPG'))
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
    cap.set(cv2.CAP_PROP_FPS, 30)

    print("Iniciando tracking y envío de datos... Presiona 'q' para salir.")

    while True:
        success, frame = cap.read()
        if not success:
            break

        # Procesamiento de imagen para segmentar el color y reducir ruido
        blurred = cv2.GaussianBlur(frame, (11, 11), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(hsv, lower_orange, upper_orange)
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
        mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)

        # Encuentra los contornos en la máscara generada
        contours, _ = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        if len(contours) > 0:
            # Selecciona el contorno de mayor área
            c = max(contours, key=cv2.contourArea)
            if cv2.contourArea(c) > 4000:
                x, y, w, h = cv2.boundingRect(c)
                centro_x = int(x + w / 2)
                centro_y = int(y + h / 2)

                # --- MAPEO DE COORDENADAS ---
                # Eje X: Mapeo del rango de la cámara al rango de movimiento horizontal
                pupilX = int(np.interp(centro_x, [0, 1280], [24, -24]))
                
                # Eje Y: Mapeo del rango de la cámara al rango de movimiento vertical
                pupilY = int(np.interp(centro_y, [0, 720], [-34, 34]))

                # --- ENVÍO SERIAL ---
                try:
                    # Se envían las coordenadas mapeadas seguidas de un salto de línea
                    mensaje = f"{pupilX},{pupilY}\n"
                    ser.write(mensaje.encode('utf-8'))

                    # Dibujo de la retroalimentación visual en el frame
                    cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                    cv2.circle(frame, (centro_x, centro_y), 5, (0, 0, 255), -1)
                    texto = f"Enviando ESP32: X:{pupilX} Y:{pupilY}"
                    cv2.putText(frame, texto, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
                except Exception as e:
                    print(f"Error al enviar datos al ESP32: {e}")

        # Muestra la ventana con el resultado del tracking
        cv2.imshow(WINDOW_NAME, frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Libera los recursos al finalizar la ejecución
    cap.release()
    ser.close()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()