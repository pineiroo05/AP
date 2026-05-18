Los ejercicios de esta carpeta corresponden a enunciados de ejercicios pedidos a una ia:



**Ejercicio 1 — Invertir imagen (efecto negativo)**

Haz la transición bloque a bloque mostrando el resultado. Pausa de 100ms.

* SSE2: Escribe invertirBloque que invierta los colores de un bloque 16x16 de Img1 (255 - pixel) usando SSE2.

void invertirBloque(IplImage \*img, int filaA, int colA);

* Threads: Modifica el ejercicio para que el thread 0 haga la mitad superior y el thread 1 la mitad inferior.



**Ejercicio 2 — Intercambiar bloques entre dos imágenes**

* SSE2: Escribe intercambiarBloque que intercambie un bloque 16x16 entre Img1 e Img2 (swap, no copia).

Muestra la transición bloque a bloque con pausa de 100ms.

cvoid intercambiarBloque(IplImage \*imgA, int filaA, int colA, IplImage \*imgB, int filaB, int colB);

* Threads: Thread 0 hace las filas pares, thread 1 las filas impares.



**Ejercicio 3 — Oscurecer imagen progresivamente**

* SSE2: Escribe oscurecerBloque que reste un valor fijo (ej: 50) a cada canal de cada píxel de un bloque 16x16 usando \_mm\_subs\_epu8 (resta sin desbordamiento).

Aplícalo bloque a bloque con visualización. Pausa 100ms.

cvoid oscurecerBloque(IplImage \*img, int filaA, int colA, unsigned char cantidad);

* Threads: Thread 0 mitad izquierda, thread 1 mitad derecha.



**Ejercicio 4 — Mezclar dos imágenes (blend)**

* SSE2: Escribe mezclarBloque que haga la media de cada pixel entre Img1 e Img2 y lo guarde en Img2, usando SSE2 (\_mm\_avg\_epu8).

Muestra la transición bloque a bloque con pausa de 100ms.

cvoid mezclarBloque(IplImage \*imgA, int filaA, int colA, IplImage \*imgB, int filaB, int colB);

* Threads: Thread 0 cuadrante superior izquierdo, thread 1 cuadrante superior derecho, etc. (si defines NTHREADS 4).



**Ejercicio 5 — Poner en gris un canal**

* SSE2: Escribe borrarCanalBloque que ponga a 0 un canal específico (R, G o B) de un bloque 16x16 usando SSE2 y una máscara (\_mm\_and\_si128).

Aplícalo bloque a bloque con visualización. Pausa 100ms.

cvoid borrarCanalBloque(IplImage \*img, int filaA, int colA, int canal);

* Threads: Thread 0 mitad izquierda, thread 1 mitad derecha
