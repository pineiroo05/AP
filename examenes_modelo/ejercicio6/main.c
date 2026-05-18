#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <pthread.h>

#define NTHREADS 2

IplImage *Img1;
IplImage *ImgSSE;
IplImage *ImgThreads;

void aclararImagen(IplImage *img, int filaA, int colA, unsigned char cantidad){
    __m128i cantidadSumar=_mm_set1_epi8(cantidad);
    int fila, columna;
    for(fila=0; fila<16; fila++){
        __m128i *pImg=(__m128i *)(img->imageData+(fila+filaA)*img->widthStep+(colA*3));
        for(columna=0; columna<16*3; columna+=16){
            *pImg=_mm_adds_epu8(*pImg, cantidadSumar);
            pImg++;
        }
    }
}

void thread(void *ptr){
    int *id=(int *)ptr;
    int fila, columna, filaInicio, filaFinal;
    int medio=(Img1->height)/2;
    int altoBloque=16;
    int anchoBloque=16;
    if(*id==0){
        filaInicio=0;
        filaFinal=medio;
    }else{
        filaInicio=medio;
        filaFinal=Img1->height;
    }
    cvNamedWindow("con threads", CV_WINDOW_NORMAL);
    int cantidad=33;
    for(fila=filaInicio; fila<filaFinal; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            aclararImagen(ImgThreads, fila, columna, cantidad);
            cvShowImage("con threads", ImgThreads);
            cvWaitKey(1);
        }
    }
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
    
    ImgSSE=cvCloneImage(Img1);
    ImgThreads=cvCloneImage(Img1);

    //CON SSE
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    int cantidad=33;
    cvNamedWindow("con sse", CV_WINDOW_NORMAL);
    for(fila=0; fila<Img1->height; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            aclararImagen(ImgSSE, fila, columna, cantidad);
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
    cvReleaseImage(&ImgSSE);
    cvReleaseImage(&ImgThreads);
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("con sse");
    cvDestroyWindow("con threads");
    return EXIT_SUCCESS;
}