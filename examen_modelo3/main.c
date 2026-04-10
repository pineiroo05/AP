#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <emmintrin.h>

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    IplImage* Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvWaitKey(0);
    
    IplImage *ejercicio1=cvCloneImage(Img1);
    IplImage *ejercicio2=cvCloneImage(Img1);
    
    int fila, columna, x;
    
    //SIN SSE
    //componente roja
    for(x=0; x<256; x++){
        for(fila=0; fila<ejercicio1->height; fila++){
            unsigned char *pEjercicio1=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep)+2;
            for(columna=0; columna<ejercicio1->width; columna++){
                if((*pEjercicio1)>0){
                    (*pEjercicio1)--;
                }
                pEjercicio1+=4;
            }
        }
        cvNamedWindow("ejercicio1", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio1", ejercicio1);
        cvWaitKey(1);
    }
    //componente verde
    for(x=0; x<256; x++){
        for(fila=0; fila<ejercicio1->height; fila++){
            unsigned char *pEjercicio1=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep)+1;
            for(columna=0; columna<ejercicio1->width; columna++){
                if((*pEjercicio1)>0){
                    (*pEjercicio1)--;
                }
                pEjercicio1+=4;
            }
        }
        cvNamedWindow("ejercicio1", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio1", ejercicio1);
        cvWaitKey(1);
    }
    //componente azul
    for(x=0; x<256; x++){
        for(fila=0; fila<ejercicio1->height; fila++){
            unsigned char *pEjercicio1=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep);
            for(columna=0; columna<ejercicio1->width; columna++){
                if((*pEjercicio1)>0){
                    (*pEjercicio1)--;
                }
                pEjercicio1+=4;
            }
        }
        cvNamedWindow("ejercicio1", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio1", ejercicio1);
        cvWaitKey(1);
    }
    
    //CON SSE
    __m128i mascaraR=_mm_set_epi8(0,1,0,0, 0,1,0,0, 0,1,0,0 ,0,1,0,0);
    __m128i mascaraG=_mm_set_epi8(0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0);
    __m128i mascaraB=_mm_set_epi8(0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,0,1);
    //componente roja
    for(x=0; x<256; x++){
        for(fila=0; fila<ejercicio2->height; fila++){
            __m128i *pEjercicio2=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep);
            for(columna=0; columna<ejercicio2->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(pEjercicio2);
                img=_mm_subs_epu8(img,mascaraR);
                _mm_storeu_si128(pEjercicio2,img);
                pEjercicio2++;
            }
        }
        cvNamedWindow("ejercicio2", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio2", ejercicio2);
        cvWaitKey(1);
    }
    //componente verde
    for(x=0; x<256; x++){
        for(fila=0; fila<ejercicio2->height; fila++){
            __m128i *pEjercicio2=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep);
            for(columna=0; columna<ejercicio2->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(pEjercicio2);
                img=_mm_subs_epu8(img,mascaraG);
                _mm_storeu_si128(pEjercicio2,img);
                pEjercicio2++;
            }
        }
        cvNamedWindow("ejercicio2", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio2", ejercicio2);
        cvWaitKey(1);
    }
    //componente azul
    for(x=0; x<256; x++){
        for(fila=0; fila<ejercicio2->height; fila++){
            __m128i *pEjercicio2=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep);
            for(columna=0; columna<ejercicio2->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(pEjercicio2);
                img=_mm_subs_epu8(img, mascaraB);
                _mm_storeu_si128(pEjercicio2,img);
                pEjercicio2++;
            }
        }
        cvNamedWindow("ejercicio2", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio2", ejercicio2);
        cvWaitKey(1);
    }

    cvReleaseImage(&Img1);
    cvReleaseImage(&ejercicio1);
    cvReleaseImage(&ejercicio2);
    
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("ejercicio1");
    cvDestroyWindow("ejercicio2");
    return EXIT_SUCCESS;
}