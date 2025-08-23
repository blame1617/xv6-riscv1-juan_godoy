Informe para Tarea 1 de Juan de Dios Godoy (Sistemas Operativos)

1- Pasos seguidos para instalar XV6:

En primer lugar cree el fork a partir del link que se entregó en clases, poniendo mi nombre para poder distinguirlo
mejor. Luego, actualizé las credenciales para git en mi wsl con Ubuntu, para asi poder clonar el repositorio que 
había forkeado. Una vez clonado el repositorio, utilicé cd para acceder al directorio de xv6, instalando make, qemu y
las otras dependencias necesarias para compilar el sistema operativo. Una vez instaladas las dependencias, ejecuté el
comando "make qemu" y así se armó el sistema de xv6, probándolo con comandos como ls y echo.

2- Problemas y soluciones:

Me encontré con dos problemas a la hora de realizar esta tarea. En primer lugar, al intentar correr qemu me percaté 
de que no se podía hacer en mi wsl, y al investigar aprendí que era porque mi versión de Ubuntu 22.04 venía con una 
versión muy antigua que no era compatible con xv6, por lo cual para solucionarlo tuve que actualizar a Ubuntu 24 y 
así se solucionó. El segundo problema que tuve fue que confundí la instrucción para generar el INFORME.md de la tarea
, utilizando mkdir para crearlo como directorio dentro de xv6 mientras se ejecutaba. Cuando traté de borrar el 
directorio con rm no me funcionaba, y averiguando en internet ví que al no tener rmdir mi única opción era utilizar 
"make clean" para reiniciar la compilación de xv6 desde 0. Una vez pude eliminar el INFORME.md lo cree de nuevo
mediante el comando touch dentro de la raíz de mi fork correctamente.

3- Confirmación de XV6:

Una vez solucionados los problemas mencionados anteriormente pude correr XV6 sin problemas, y en la entrega de WebC 
se pueden ver las capturas de pantalla con los comandos de prueba que se solicitan en esta tarea.
