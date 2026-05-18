#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <pthread.h>

#define NTHREADS 2

IplImage *Img1;
IplImage *Img2;
IplImage *ImgSSE;
IplImage *ImgThreads;

void copiarBloque(IplImage *img, int filaA, int colA, int filaB, int colB){
    int fila, columna;
    for(fila=0; fila<16; fila++){
        __m128i *pImgOrigen=(__m128i *)(img->imageData+(fila+filaA)*img->widthStep+(colA*3));
        __m128i *pImgDest=(__m128i *)(img->imageData+(fila+filaB)*img->widthStep+(colB*3));
        for(columna=0; columna<16*3; columna+=16){
            *pImgDest=*pImgOrigen;
            pImgOrigen++;
            pImgDest++;
        }
    }   
}


void thread(void *ptr){
    int *id=(int *)ptr;
    int fila, columna, filaInicio, filaFin;
    int medioAlto=(Img1->height)/2;
    int altoBloque=16;
    int anchoBloque=16;
    if(*id==0){
        filaInicio=0;
        filaFin=medioAlto;
    }else{
        filaInicio=medioAlto;
        filaFin=Img1->height;
    }
    cvNamedWindow("con threads", CV_WINDOW_NORMAL);
    for(fila=filaInicio; fila<filaFin; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            copiarBloque(ImgThreads, fila, columna, fila, columna+((Img1->width)/2));
            cvShowImage("con threads", ImgThreads);
            cvWaitKey(1);
        }
    }
}

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    Img2 = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);
    
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }if (!Img2) {
        printf("Error: fichero %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvNamedWindow(argv[2], CV_WINDOW_NORMAL);
    cvShowImage(argv[2], Img2);
    cvWaitKey(0);
    
    ImgSSE=cvCloneImage(Img1);
    ImgThreads=cvCloneImage(Img1);

    //CON SSE
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    cvNamedWindow("con sse", CV_WINDOW_NORMAL);
    for(fila=0; fila<Img1->height; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            copiarBloque(ImgSSE, fila, columna, fila, columna+((Img1->width)/2));
            cvShowImage("con sse", ImgSSE);
            cvWaitKey(1);
        }
    }
    
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

    cvReleaseImage(&Img1);
    cvReleaseImage(&Img2);
    cvReleaseImage(&ImgSSE);
    cvReleaseImage(&ImgThreads);
    cvDestroyWindow(argv[1]);
    cvDestroyWindow(argv[2]);
    cvDestroyWindow("con sse");
    cvDestroyWindow("con threads");
    return EXIT_SUCCESS;
}