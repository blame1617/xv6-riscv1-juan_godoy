Informe para Tarea 1 de Manuel Vergara y Juan de Dios Godoy (Sistemas Operativos)

1- Funcionamiento de las llamadas al sistema:

Básicamente tenemos 2 llamadas principales que es al sistema es la de getAncestor y la de getppid, la primera nos permite obtener el ancestro n-esimos de un proceso, mientras que el getppid devuelve el PID del padre directo del proceso actual. El sistema busca hacia arriba en el árbol de procesos hasta encontrar los correspondientes.

2- Explicación de las modificaciones realizadas:

Se creó el archivo yosoytupadre.c, en el cual llama a la syscall getancestor(int n) para obtener los primeros ancestros del proceso actual, luego imprime sus PIDs en la consola. Para que compilara se hicieron varias modificaciones , entre ellas definir un número nuevo de syscall tanto para getAncestor como para getppid, declarar la función, asociar el syscall en el array de syscalls, agregar la entrada y finalmente definir la función sys que recibe el argumento n del usuario. Se ajustó el Makefile para que incluyera el archivo de prueba y el programa yosoytupadre llama a ambas funciones para testear las syscalls.

3- Dificultades encontradas y cómo se resolvieron:

Tuve problema con la definición de las variables, ya que al principio se me dificultó entender la organización de los archivos y las modificaciones necesarias para hacer la compilación. Sobretodo la parte de la asignación del número de llamada, ya que sin querer ocupé números que ya estaban en uso. Además surgieron los típicos problemas de compilación en los que muchas veces se solucionaban colocando make clean. 
