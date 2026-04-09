#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    //CV_LOAD_IMAGE_COLOR = 1 forces the resultant IplImage to be colour.
    //CV_LOAD_IMAGE_GRAYSCALE = 0 forces a greyscale IplImage.
    //CV_LOAD_IMAGE_UNCHANGED = -1
    IplImage* Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    
    // Always check if the program can find a file
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvWaitKey(0);

    printf("Numero canales: %d\n", Img1->nChannels);
    
    
    IplImage *ejercicio1=cvCreateImage(cvSize((Img1->width)*2, (Img1->height)*2), IPL_DEPTH_8U, Img1->nChannels);
    IplImage *ejercicio2=cvCreateImage(cvSize((Img1->width)*2, (Img1->height)*2), IPL_DEPTH_8U, Img1->nChannels);
    
    int fila, columna;
    
    //Sin sse
    for(fila=0; fila<Img1->height; fila++){
        unsigned char *pImg1=(unsigned char *)(Img1->imageData+fila*Img1->widthStep);
        unsigned char *parteOriginal=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep);
        unsigned char *parteAmarilla=(unsigned char *)(ejercicio1->imageData+(fila+Img1->height)*ejercicio1->widthStep);
        unsigned char *parteCian=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep+(Img1->width*Img1->nChannels));
        unsigned char *parteMagenta=(unsigned char *)(ejercicio1->imageData+(fila+Img1->height)*ejercicio1->widthStep+(Img1->width*Img1->nChannels));
        for(columna=0; columna<Img1->width; columna++){
            unsigned char R=*pImg1++;
            unsigned char G=*pImg1++;
            unsigned char B=*pImg1++;
            unsigned char A=*pImg1++;
            //original
            *parteOriginal++=R;
            *parteOriginal++=G;
            *parteOriginal++=B;
            *parteOriginal++=A;
            //amarillo
            *parteAmarilla++=R;
            *parteAmarilla++=G;
            *parteAmarilla++=0;
            *parteAmarilla++=A;
            //cian
            *parteCian++=0;
            *parteCian++=G;
            *parteCian++=B;
            *parteCian++=A;
            //magenta
            *parteMagenta++=R;
            *parteMagenta++=0;
            *parteMagenta++=B;
            *parteMagenta++=A;
        }
    }
    cvNamedWindow("ejercicio 1", CV_WINDOW_AUTOSIZE);
    cvShowImage("ejercicio 1", ejercicio1);
    cvWaitKey(0);
    
    printf("A %d\n", Img1->widthStep);
    printf("B %d\n", Img1->width*Img1->nChannels);
    
    //Con sse
    __m128i mascaraAmarilla=_mm_set1_epi32(0xFFFF00);
    __m128i mascaraCian=_mm_set1_epi32(0x00FFFF);
    __m128i mascaraMagenta=_mm_set1_epi32(0xFF00FF);
    for (fila=0; fila<Img1->height; fila++){
        __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
        __m128i *pOriginal=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep);
        __m128i *pAmarilla=(__m128i *)(ejercicio2->imageData+(fila+Img1->height)*ejercicio2->widthStep);
        __m128i *pCian=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep+(Img1->width*Img1->nChannels));
        __m128i *pMagenta=(__m128i *)(ejercicio2->imageData+(fila+Img1->height)*ejercicio2->widthStep+(Img1->width*Img1->nChannels));
        
        for(columna=0; columna<Img1->widthStep; columna+=16){
            __m128i datos=_mm_loadu_si128(pImg1);
            //original
            _mm_storeu_si128(pOriginal, _mm_and_si128(_mm_set1_epi32(0xFFFFFF), datos));
            //amarilla
            _mm_storeu_si128(pAmarilla, _mm_and_si128(mascaraAmarilla, datos));
            //cian
            _mm_storeu_si128(pCian, _mm_and_si128(mascaraCian, datos));
            //magenta
            _mm_storeu_si128(pMagenta, _mm_and_si128(mascaraMagenta, datos));
            
            *pImg1++;
            *pOriginal++;
            *pAmarilla++;
            *pCian++;
            *pMagenta++;
        }
    }
    cvNamedWindow("ejercicio 2", CV_WINDOW_AUTOSIZE);
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

