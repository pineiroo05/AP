#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "mydll.h"

int main(int argc, char** argv) {
    struct RGB color;
    color.rojo=255;
    color.verde=0;
    color.azul=0;
    
    int ancho=20;
    int alto=20;
    IplImage *imagen=createImagen(ancho, alto, color);
    
    cvNamedWindow("Imagen", CV_WINDOW_NORMAL);
    cvShowImage("Imagen", imagen);
    cvWaitKey(0);

    cvReleaseImage(&imagen);
    cvDestroyWindow("Imagen");

    return EXIT_SUCCESS;
}