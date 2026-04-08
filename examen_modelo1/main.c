#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    IplImage* Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED); //TIENE QUE SER UNA IMAGEN DE 4 CANALES!!!!!
    
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvWaitKey(0);

    IplImage *ejercicio1=cvCreateImage(cvSize((Img1->width)*2, (Img1->height)*3), IPL_DEPTH_8U, Img1->nChannels);
    IplImage *ejercicio2=cvCreateImage(cvSize((Img1->width)*2, (Img1->height)*3), IPL_DEPTH_8U, Img1->nChannels);
    
    int fila, columna;
    
    //Sin sse
    for(fila=0; fila<Img1->height; fila++){
        unsigned char *pImg1=(unsigned char *)(Img1->imageData+fila*Img1->widthStep);
        unsigned char *pOriginal=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep);
        unsigned char *pRoja=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep+(Img1->width*Img1->nChannels));
        unsigned char *pVerde=(unsigned char *)(ejercicio1->imageData+(fila+Img1->height)*ejercicio1->widthStep+(Img1->width*Img1->nChannels));
        unsigned char *pAzul=(unsigned char *)(ejercicio1->imageData+(fila+2*(Img1->height))*ejercicio1->widthStep+(Img1->width*Img1->nChannels));
        for(columna=0; columna<Img1->width; columna++){
            unsigned char B=*pImg1++;
            unsigned char G=*pImg1++;
            unsigned char R=*pImg1++;
            unsigned char A=*pImg1++;
            //original
            *pOriginal++=B;
            *pOriginal++=G;
            *pOriginal++=R;
            *pOriginal++=A;
            //rojo
            *pRoja++=0;
            *pRoja++=0;
            *pRoja++=R;
            *pRoja++=A;
            //verde
            *pVerde++=0;
            *pVerde++=G;
            *pVerde++=0;
            *pVerde++=A;
            //azul
            *pAzul++=B;
            *pAzul++=0;
            *pAzul++=0;
            *pAzul++=A;
        }
    }
    cvNamedWindow("ejercicio 1", CV_WINDOW_NORMAL);
    cvShowImage("ejercicio 1", ejercicio1);
    cvWaitKey(0);
    
    //Con sse
    __m128i mascaraRojo=_mm_set1_epi32(0xFF0000);
    __m128i mascaraVerde=_mm_set1_epi32(0x00FF00);
    __m128i mascaraAzul=_mm_set1_epi32(0x0000FF);
    for(fila=0; fila<Img1->height; fila++){
        __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
        __m128i *parteOriginal=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep);
        __m128i *parteRoja=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep+(Img1->width*Img1->nChannels));
        __m128i *parteVerde=(__m128i *)(ejercicio2->imageData+(fila+Img1->height)*ejercicio2->widthStep+(Img1->width*Img1->nChannels));
        __m128i *parteAzul=(__m128i *)(ejercicio2->imageData+(fila+2*(Img1->height))*ejercicio2->widthStep+(Img1->width*Img1->nChannels));
        for(columna=0; columna<Img1->widthStep; columna+=16){
            //original
            *parteOriginal++=_mm_and_si128(_mm_set1_epi32(0xFFFFFF), *pImg1);
            //rojo
            *parteRoja++=_mm_and_si128(mascaraRojo, *pImg1);
            //verde 
            *parteVerde++=_mm_and_si128(mascaraVerde, *pImg1);
            //azul
            *parteAzul++=_mm_and_si128(mascaraAzul, *pImg1);
            
            *pImg1++;
        }
    }
    cvNamedWindow("ejercicio 2", CV_WINDOW_NORMAL);
    cvShowImage("ejercicio 2", ejercicio2);
    cvWaitKey(0);

    cvReleaseImage(&Img1);
    cvReleaseImage(&ejercicio1);
    cvReleaseImage(&ejercicio2);
    
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("ejercicio 1");
    cvDestroyWindow("ejercicio 2");
    return EXIT_SUCCESS;
}