/*
Ejercicio 1: Comparar todos los bloques de una imagen. Los dos mas parecidos ponerlos en blanco
Ejercicio 2: Igual pero con sse
Ejercicio 3: Igual pero con hilos 
*/

#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <pthread.h>

#define NTHREADS 4

IplImage *Img1;
IplImage *ImgEj1;
IplImage *ImgEj2;
IplImage *ImgEj3;

// SIN SSE
int compararBloque(IplImage *img, int filaA, int colA, int filaB, int colB){
    int fila, columna;
    int diferencia=0;
    for(fila=0; fila<32; fila++){
        unsigned char *pBloq1=(unsigned char *)(img->imageData+(fila+filaA)*img->widthStep+(colA*3));
        unsigned char *pBloq2=(unsigned char *)(img->imageData+(fila+filaB)*img->widthStep+(colB*3));
        for(columna=0; columna<32*3; columna++){
            diferencia+=abs((*pBloq1)-(*pBloq2));
            pBloq1++;
            pBloq2++;
        }
    }
    return diferencia;
}

void blanquear(IplImage *img, int filaA, int colA){
    int fila, columna;
    for(fila=0; fila<32; fila++){
        unsigned char *pImg=(unsigned char *)(img->imageData+(fila+filaA)*img->widthStep+(colA*3));
        for(columna=0; columna<32*3; columna++){
            *pImg=255;
            pImg++;
        }
    }
}

//CON SSE
int compararBloqueSSE(IplImage *img, int filaA, int colA, int filaB, int colB){
    int fila, columna;
    int diferencia=0;
    for(fila=0; fila<32; fila++){
        __m128i *pBloq1=(__m128i *)(img->imageData+(fila+filaA)*img->widthStep+(colA*3));
        __m128i *pBloq2=(__m128i *)(img->imageData+(fila+filaB)*img->widthStep+(colB*3));
        for(columna=0; columna<32*3; columna+=16){
            __m128i A=_mm_loadu_si128(pBloq1);
            __m128i B=_mm_loadu_si128(pBloq2);
            __m128i C=_mm_sad_epu8(A,B);
            __m128i D=_mm_srli_si128(C,8);
            __m128i E=_mm_add_epi32(C,D);
            pBloq1++;
            pBloq2++;
            diferencia+=_mm_cvtsi128_si32(E);
        }
    }
    return diferencia;
}

void blanquearSSE(IplImage *img, int filaA, int colA){
    int fila, columna;
    for(fila=0; fila<32; fila++){
        __m128i *pImg=(__m128i *)(img->imageData+(fila+filaA)*img->widthStep+(colA*3));
        for(columna=0; columna<32*3; columna+=16){
            *pImg=_mm_set1_epi8(255);
            pImg++;
        } 
    }
}

//THREADS+SSE
void thread(void *ptr){
    int *id=(int *)ptr;
    int filaA, columnaA, filaB, columnaB;
    int filaBlanquearA, columnaBlanquearA, filaBlanquearB, columnaBlanquearB;
    int altoBloque=32;
    int anchoBloque=32;
    int diferencia=(32*32*255*3)+1;
    //Hilos intercalados
    for(filaA=(*id)*altoBloque; filaA<Img1->height; filaA+=altoBloque*NTHREADS){
        for(columnaA=0; columnaA<Img1->width; columnaA+=anchoBloque){
            for(filaB=0; filaB<Img1->height; filaB+=altoBloque){
                for(columnaB=0; columnaB<Img1->width; columnaB+=anchoBloque){
                    if(filaA!=filaB || columnaA!=columnaB){
                        int nuevaDiferencia=compararBloqueSSE(ImgEj3, filaA, columnaA, filaB, columnaB);
                        if(nuevaDiferencia<diferencia){
                            diferencia=nuevaDiferencia;
                            filaBlanquearA=filaA;
                            filaBlanquearB=filaB;
                            columnaBlanquearA=columnaA;
                            columnaBlanquearB=columnaB;
                        }
                    }
                }
            }
        }
    }
    blanquearSSE(ImgEj3, filaBlanquearA, columnaBlanquearA);
    blanquearSSE(ImgEj3, filaBlanquearB, columnaBlanquearB);
}

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvWaitKey(0);
    
    ImgEj1=cvCloneImage(Img1);
    ImgEj2=cvCloneImage(Img1);
    ImgEj3=cvCloneImage(Img1);
    
    //SIN SSE
    int filaA, columnaA, filaB, columnaB;
    int filaBlanquearA, columnaBlanquearA, filaBlanquearB, columnaBlanquearB;
    int altoBloque=32;
    int anchoBloque=32;
    int diferencia=(32*32*255*3)+1;
    for(filaA=0; filaA<Img1->height; filaA+=altoBloque){
        for(columnaA=0; columnaA<Img1->width; columnaA+=anchoBloque){
            for(filaB=0; filaB<Img1->height; filaB+=altoBloque){
                for(columnaB=0; columnaB<Img1->width; columnaB+=anchoBloque){
                    if(filaA!=filaB || columnaA!=columnaB){
                        int nuevaDiferencia=compararBloque(ImgEj1, filaA, columnaA, filaB, columnaB);
                        if(nuevaDiferencia<diferencia){
                            diferencia=nuevaDiferencia;
                            filaBlanquearA=filaA;
                            columnaBlanquearA=columnaA;
                            filaBlanquearB=filaB;
                            columnaBlanquearB=columnaB;
                        }
                    }
                }
            }
        }
    }
    blanquear(ImgEj1, filaBlanquearA, columnaBlanquearA);
    blanquear(ImgEj1, filaBlanquearB, columnaBlanquearB);
    cvNamedWindow("ejercicio 1", CV_WINDOW_NORMAL);
    cvShowImage("ejercicio 1", ImgEj1);
    cvWaitKey(0);
    
    //CON SSE
    diferencia=(32*32*255*3)+1;
    filaBlanquearA=0;
    columnaBlanquearA=0;
    filaBlanquearB=0;
    columnaBlanquearB=0;
    for(filaA=0; filaA<Img1->height; filaA+=altoBloque){
        for(columnaA=0; columnaA<Img1->width; columnaA+=anchoBloque){
            for(filaB=0; filaB<Img1->height; filaB+=altoBloque){
                for(columnaB=0; columnaB<Img1->width; columnaB+=anchoBloque){
                    if(filaA!=filaB || columnaA!=columnaB){
                        int nuevaDiferencia=compararBloqueSSE(ImgEj2, filaA, columnaA, filaB, columnaB);
                        if(nuevaDiferencia<diferencia){
                            diferencia=nuevaDiferencia;
                            filaBlanquearA=filaA;
                            filaBlanquearB=filaB;
                            columnaBlanquearA=columnaA;
                            columnaBlanquearB=columnaB;
                        }
                    }
                }
            }
        }
    }
    blanquearSSE(ImgEj2, filaBlanquearA, columnaBlanquearA);
    blanquearSSE(ImgEj2, filaBlanquearB, columnaBlanquearB);
    cvNamedWindow("ejercicio 2", CV_WINDOW_NORMAL);
    cvShowImage("ejercicio 2", ImgEj2);
    cvWaitKey(0);
    
    //CON THREADS
    pthread_t threads[NTHREADS];
    int id[NTHREADS];
    int i;
    for(i=0; i<NTHREADS; i++){
        id[i]=i;
        pthread_create(&threads[i], NULL, (void *)&thread, (void *)&id[i]);
    }
    for(i=0; i<NTHREADS; i++){
        pthread_join(threads[i], NULL);
    }
    cvNamedWindow("ejercicio 3", CV_WINDOW_NORMAL);
    cvShowImage("ejercicio 3", ImgEj3);
    cvWaitKey(0);

    cvReleaseImage(&Img1);
    cvReleaseImage(&ImgEj1);
    cvReleaseImage(&ImgEj2);
    cvReleaseImage(&ImgEj3);
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("ejercicio 1");
    cvDestroyWindow("ejercicio 2");
    cvDestroyWindow("ejercicio 3");
    return EXIT_SUCCESS;
}