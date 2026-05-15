#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "mydll.h"

IplImage *createImagen(int ancho, int alto, struct RGB color){
    IplImage *imagen;
    imagen=(IplImage *)malloc(sizeof(IplImage));
    
    imagen->nSize=sizeof(IplImage);
    imagen->nChannels=3;
    imagen->depth=IPL_DEPTH_8U;
    imagen->height=alto;
    imagen->width=ancho;
    imagen->widthStep=((ancho*imagen->nChannels+3)/4)*4;
    imagen->imageSize=imagen->widthStep*imagen->height;
    imagen->origin=0;
    imagen->dataOrder=0;
    //asignacion dinamica de memoria
    imagen->imageData=(char *)malloc(imagen->imageSize);
    imagen->imageDataOrigin=imagen->imageData;
    
    imagen->roi=NULL;
    imagen->maskROI=NULL;
    imagen->imageId=NULL;
    imagen->tileInfo=NULL;
    
    int fila, columna;
    for(fila=0; fila<alto; fila++){
        unsigned char *pImagen=(unsigned char *)(imagen->imageData+fila*imagen->widthStep);
        for(columna=0; columna<ancho; columna++){
            *pImagen++=color.azul;
            *pImagen++=color.verde;
            *pImagen++=color.rojo;
        }
    }
    return imagen;
}
