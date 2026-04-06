Sistema de cronometraje para atletismo con Arduino UNO

Proyecto educativo de cronometraje automático diseñado para medir el tiempo de un corredor de forma simple pero funcional.

¿Qué hace este sistema?

Este proyecto permite automatizar una salida y registrar el tiempo final de carrera mediante sensores conectados a Arduino.

Funcionalidades principales:
-detección de posición de salida con sensor ultrasónico
-secuencia de luces de preparación
-detección de salida falsa
-cronómetro en tiempo real en pantalla LCD
-señal acústica de salida y llegada
-detección de meta mediante barrera láser y fotoresistencia

Componentes utilizados:
-Arduino UNO
-LCD 16x2
-Sensor ultrasónico HC-SR04
-Fotoresistencia
-Láser
-LEDs rojo y verde
-Bocina activa
-Pulsador

Funcionamiento:
-Se pulsa el botón de inicio
-El corredor se posiciona en salida
-Se ejecuta secuencia de preparación
-El sistema lanza salida aleatoria
-Se inicia cronómetro
-Se detecta la llegada y se muestra tiempo final

Limitaciones actuales: Actualmente el sistema está diseñado para un único corredor, con enfoque educativo y funcional.

Próximas mejoras:
-soporte para múltiples corredores
-mejora de precisión temporal
-almacenamiento de resultados

Archivos del repositorio:
-código fuente Arduino
-imágenes del montaje
-esquema de conexiones
