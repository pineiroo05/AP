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

void borrarCanalBloque(IplImage *img, int filaA, int colA){
    //BRG
    __m128i mascara1=_mm_set_epi8(0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF);
    __m128i mascara2=_mm_set_epi8(0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF);
    __m128i mascara3=_mm_set_epi8(0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0,0xFF,0xFF,0);

    int fila, columna;
    for(fila=0; fila<16; fila++){
        __m128i *pImg=(__m128i *)(img->imageData+(fila+filaA)*img->widthStep+(colA*3));
        int a=0;
        for(columna=0; columna<16*3; columna+=16){
            *pImg=_mm_and_si128(*pImg, mascara1);
            pImg++;
            *pImg=_mm_and_si128(*pImg, mascara2);
            pImg++;
            *pImg=_mm_and_si128(*pImg, mascara3);
            pImg++;
        }
    }
} 

void thread(void *ptr){
    int *id=(int *)ptr;
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    int colInicio, colFinal;
    int medio=(Img1->width)/2;
    if(*id==0){
        colInicio=0;
        colFinal=medio;
    }else{
        colInicio=medio;
        colFinal=Img1->width;
    }
    for(fila=0; fila<Img1->height; fila+=altoBloque){
        for(columna=colInicio; columna<colFinal; columna+=anchoBloque){
            borrarCanalBloque(ImgThreads, fila, columna);
            cvNamedWindow("con threads", CV_WINDOW_NORMAL);
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
    for(fila=0; fila<Img1->height; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            borrarCanalBloque(ImgSSE, fila, columna);
            cvNamedWindow("con sse", CV_WINDOW_NORMAL);
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