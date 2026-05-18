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

void sumarBloque(IplImage *imgA, int filaA, int colA, IplImage *imgB, int filaB, int colB){
    int fila, columna;
    for(fila=0; fila<16; fila++){
        __m128i *pImgA=(__m128i *)(imgA->imageData+(fila+filaA)*imgA->widthStep+(colA*3));
        __m128i *pImgB=(__m128i *)(imgB->imageData+(fila+filaB)*imgB->widthStep+(colB*3));
        for(columna=0; columna<16*3; columna+=16){
            *pImgB=_mm_adds_epu8(*pImgA, *pImgB);
            pImgA++;
            pImgB++;
        }
    }
}

void thread(void *ptr){
    int *id=(int *)ptr;
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    cvNamedWindow("con threads", CV_WINDOW_NORMAL);
    for(fila=(*id)*altoBloque; fila<Img1->height; fila+=altoBloque*NTHREADS){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            sumarBloque(Img1, fila, columna, ImgThreads, fila, columna);
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
    
    ImgSSE=cvCloneImage(Img2);
    ImgThreads=cvCloneImage(Img2);

    //CON SSE
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    cvNamedWindow("con sse", CV_WINDOW_NORMAL);
    for(fila=0; fila<Img1->height; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            sumarBloque(Img1, fila, columna, ImgSSE, fila, columna);
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