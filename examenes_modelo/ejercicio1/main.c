#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <emmintrin.h>
#include <pthread.h>

#define NTHREADS 2

IplImage *Img1;

void invertirBloque(IplImage *img, int filaA, int columnaA){
    __m128i mascara=_mm_set1_epi8(0xFF);
    int fila, columna;
    for(fila=0; fila<16; fila++){
        __m128i *pImg=(__m128i *)(img->imageData+(fila+filaA)*img->widthStep+(columnaA*3));
        for(columna=0; columna<16*3; columna+=16){
            *pImg=_mm_xor_si128(*pImg, mascara);
            pImg++;
        }
    }
}

void thread(void *ptr){
    int *id=(int *)ptr;
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    int mitadImagen=(Img1->height)/2;
    int filaInicio, filaFinal;
    
    if(*id==0){
        filaInicio=0;
        filaFinal=mitadImagen;
    }else{
        filaInicio=mitadImagen;
        filaFinal=Img1->height;
    }
    for(fila=filaInicio; fila<filaFinal; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            invertirBloque(Img1, fila, columna);
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
    
    /*APARTADO SOLO CON SSE
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    for(fila=0; fila<Img1->height; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            invertirBloque(Img1, fila, columna);
        }
    }
    cvShowImage(argv[1], Img1);
    cvWaitKey(0);*/
    
    //APARTADO THREADS
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
    cvShowImage(argv[1], Img1);
    cvWaitKey(0);

    cvReleaseImage(&Img1);
    cvDestroyWindow(argv[1]);
    return EXIT_SUCCESS;
}