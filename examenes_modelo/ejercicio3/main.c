#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <emmintrin.h>
#include <pthread.h>

#define NTHREADS 2

IplImage *Img1;

void oscurecerImagen(IplImage *img, int filaA, int colA, unsigned char cantidad){
    int fila, columna;
    __m128i cantResta=_mm_set1_epi8(cantidad);
    for(fila=0; fila<16; fila++){
        __m128i *pImg=(__m128i *)(img->imageData+(fila+filaA)*img->widthStep+(colA*3));
        for(columna=0; columna<16*3; columna+=16){
            *pImg=_mm_subs_epu8(*pImg,cantResta);
            pImg++;
        }
    }
}

void thread(void *ptr){
    int *id=(int *)ptr;
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    int medio=(Img1->width)/2;
    int colInicio, colFinal;
    if(*id==0){
        colInicio=0;
        colFinal=medio;
    }else{
        colInicio=medio;
        colFinal=Img1->width;
    }
    for(fila=0; fila<Img1->height; fila+=altoBloque){
        for(columna=colInicio; columna<colFinal; columna+=anchoBloque){
            oscurecerImagen(Img1, fila, columna, 50);
            cvShowImage("imagen threads", Img1);
            cvWaitKey(1);
        }
    }
}

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    Img1=cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvWaitKey(0);
    
    //EJERCICIO SSE
    int fila, columna;
    int altoBloque=16;
    int anchoBloque=16;
    for(fila=0; fila<Img1->height; fila+=altoBloque){
        for(columna=0; columna<Img1->width; columna+=anchoBloque){
            oscurecerImagen(Img1, fila, columna, 50);
            cvShowImage(argv[1], Img1);
            cvWaitKey(1);
        }
    }
    
    //EJERCICIO THREADS
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
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("imagen threads");
    return EXIT_SUCCESS;
}