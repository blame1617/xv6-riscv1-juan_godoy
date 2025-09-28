Informe para Tarea 1 de Manuel Vergara (Sistemas Operativos)

1- Funcionamiento de las llamadas al sistema:

Básicamente la llamada principal que es al sistema es la de getAncestor, la cual permite objener los ancestros de un proceso. El sistema busca hacia arriba en el árbol de procesos hasta encontrar el correspondiente.

2- Explicación de las modificaciones realizadas:

Se creó el archivo yosoytupadre.c, en el cual llama a la syscall getancestor(int n) para obtener los primeros ancestros del proceso actual, luegop imprime sus PIDs en la consola. Para que compilara se hicieron varias modificaciones , entre ellas definir un número nuevo de syscall, declarar la función, asociar el syscall en el array de syscalls, agregar la entrada y finalmente definir la función sys que recibe el argumento n del usuario.

3- Dificultades encontradas y cómo se resolvieron.:

Tuve problema con la definición de las variables, ya que al principio se me dificultó entender la organización de los archivos y las modificaciones necesarias para hacer la compilación. Sobretodo la parte de la asignación del número de llamada, ya que sin querer ocupé números que ya estaban en uso. Además surgieron los típicos problemas de compilación en los que muchas veces se solucionaban colocando make clean. 
