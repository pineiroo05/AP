#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <emmintrin.h>

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    //CV_LOAD_IMAGE_COLOR = 1 forces the resultant IplImage to be colour.
    //CV_LOAD_IMAGE_GRAYSCALE = 0 forces a greyscale IplImage.
    //CV_LOAD_IMAGE_UNCHANGED = -1
    IplImage* Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    IplImage* Img2 = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);
    
    // Always check if the program can find a file
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }else if (!Img2) {
        printf("Error: fichero %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }

    // a visualization window is created with title 'image'
    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvNamedWindow(argv[2], CV_WINDOW_NORMAL);
    cvShowImage(argv[2], Img2);
    cvWaitKey(0);
    
    IplImage* fundido1=cvCloneImage(Img1);
    IplImage* fundido2=cvCloneImage(Img1);
    IplImage* fundido3=cvCreateImage(cvSize(Img1->width, Img1->height), IPL_DEPTH_8U, Img1->nChannels);
    IplImage* fundido4=cvCreateImage(cvSize(Img1->width, Img1->height), IPL_DEPTH_8U, Img1->nChannels);
    IplImage* fundido5=cvCloneImage(Img2);
    IplImage* fundido6=cvCloneImage(Img2);
    IplImage* copia=cvCreateImage(cvSize(Img1->width, Img1->height), IPL_DEPTH_8U, Img1->nChannels);
    int fila, columna, x;
    
    //FUNDIDO A NEGRO SIN SSE
    for(x=0; x<256; x++){
        for(fila=0; fila<fundido1->height; fila++){
            unsigned char *pFundido1=(unsigned char *)(fundido1->imageData+fila*fundido1->widthStep);
            for(columna=0; columna<fundido1->widthStep; columna++){
                if((*pFundido1)==0){
                    *(pFundido1)=0;
                    *pFundido1++;
                }else{
                    (*pFundido1)--;
                    *pFundido1++;
                }
            }
        }
        cvNamedWindow("Fundido a negro sin sse", CV_WINDOW_NORMAL);
        cvShowImage("Fundido a negro sin sse", fundido1);
        cvWaitKey(1);
    }
    
    //FUNDIDO A NEGRO CON SSE
    //16 bytes a la vez
    for(x=0; x<256; x++){
        for(fila=0; fila<fundido2->height; fila++){
            __m128i *pFundido2=(__m128i *)(fundido2->imageData+fila*fundido2->widthStep);
            for(columna=0; columna<fundido2->widthStep; columna+=16){
                //El if-else de antes no hace falta pq trabajo con saturacion
                __m128i B=_mm_set1_epi8(1);
                *pFundido2=_mm_subs_epu8(*pFundido2,B);
                *pFundido2++;
            }
        }
        cvNamedWindow("Fundido a negro con sse", CV_WINDOW_NORMAL);
        cvShowImage("Fundido a negro con sse", fundido2);
        cvWaitKey(1);
    }
    
    //FUNDIDO DESDE NEGRO SIN SSE
    for(x=0; x<256; x++){
        for(fila=0; fila<fundido3->height; fila++){
            unsigned char *pImg2=(unsigned char *)(Img2->imageData+fila*Img2->widthStep);
            unsigned char *pFundido3=(unsigned char *)(fundido3->imageData+fila*fundido3->widthStep);
            for(columna=0; columna<fundido3->widthStep; columna++){
                if(*pFundido3>*pImg2){
                    *pFundido3=*pImg2;
                }else{
                    (*pFundido3)++;
                }    
                *pFundido3++;
                *pImg2++;
            }
        }
        cvNamedWindow("Fundido desde negro sin sse", CV_WINDOW_NORMAL);
        cvShowImage("Fundido desde negro sin sse", fundido3);
        cvWaitKey(1);
    }
    
    //FUNDIDO DESDE NEGRO CON SSE
    for(x=0; x<256; x++){
        for(fila=0; fila<fundido4->height; fila++){
            __m128i *pImg2=(__m128i *)(Img2->imageData+fila*Img2->widthStep);
            __m128i *pFundido4=(__m128i *)(fundido4->imageData+fila*fundido4->widthStep);
            for(columna=0; columna<fundido4->widthStep; columna+=16){
                __m128i B=_mm_set1_epi8(1);
                *pFundido4=_mm_adds_epu8(*pFundido4, B);
                *pFundido4=_mm_min_epu8(*pImg2, *pFundido4);
                *pFundido4++;
                *pImg2++;
            }
        }
        cvNamedWindow("Fundido desde negro con sse", CV_WINDOW_NORMAL);
        cvShowImage("Fundido desde negro con sse", fundido4);
        cvWaitKey(1);
    }
    
    //FUNDIDO ENCADENADO SIN SSE
    for(x=0; x<256; x++){
        for(fila=0; fila<Img2->height; fila++){
            unsigned char *pImg1=(unsigned char *)(Img1->imageData+fila*Img1->widthStep);
            unsigned char *pFundido5=(unsigned char *)(fundido5->imageData+fila*fundido5->widthStep);
            for(columna=0; columna<Img2->widthStep; columna++){
                if((*pFundido5)<(*pImg1)){
                    (*pFundido5)++;
                }else if((*pFundido5)>(*pImg1)){
                    (*pFundido5)--;
                }else{
                    (*pFundido5)=(*pImg1);
                }
                *pImg1++;
                *pFundido5++;
            }
        }   
        cvNamedWindow("fundido encadenado sin sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido encadenado sin sse", fundido5);
        cvWaitKey(1);
    }
    
    //FUNDIDO ENCADENADO CON SSE
    for(x=0; x<256; x++){
        for(fila=0; fila<Img2->height; fila++){
            __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
            __m128i *pFundido6=(__m128i *)(fundido6->imageData+fila*fundido6->widthStep);
            for(columna=0; columna<Img2->widthStep; columna+=16){
                __m128i objetivo=_mm_loadu_si128(pImg1);
                __m128i actual=_mm_loadu_si128(pFundido6);               
                __m128i B=_mm_set1_epi8(1);
                //Si (*pFundido6)<(*pImg1)
                __m128i diferencia1=_mm_subs_epu8(objetivo,actual);
                __m128i anadido=_mm_min_epu8(diferencia1,B);
                actual=_mm_adds_epu8(actual,anadido);
                //Si (*pFundido6)>(*pImg1)
                __m128i diferencia2=_mm_subs_epu8(actual,objetivo);
                __m128i sustraccion=_mm_min_epu8(diferencia2,B);
                actual=_mm_subs_epu8(actual,sustraccion);
                
                _mm_storeu_si128(pFundido6,actual);
                *pImg1++;
                *pFundido6++;
            }
        }
        cvNamedWindow("fundido encadenado con sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido encadenado con sse", fundido6);
        cvWaitKey(1);
    }
    
    //COPIA CON SSE
    //Al hacer puntero++ ahora es +16
    for(fila=0; fila<Img1->height; fila++){
        __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
        __m128i *pCopia=(__m128i *)(copia->imageData+fila*copia->widthStep);
        for(columna=0; columna<Img1->widthStep; columna+=16){
            *pCopia++=*pImg1++;
        }
    }
    cvNamedWindow("Copia con sse", CV_WINDOW_NORMAL);
    cvShowImage("Copia con sse", copia);
    cvWaitKey(0);
    
    
    // memory release for img before exiting the application
    cvReleaseImage(&Img1);
    cvReleaseImage(&fundido1);
    cvReleaseImage(&fundido2);
    cvReleaseImage(&fundido3);
    cvReleaseImage(&fundido4);
    cvReleaseImage(&fundido5);
    cvReleaseImage(&fundido6);
    cvReleaseImage(&copia);
    
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("Fundido a negro sin sse");
    cvDestroyWindow("Fundido a negro con sse");
    cvDestroyWindow("Fundido desde negro sin sse");
    cvDestroyWindow("Fundido desde negro con sse");
    cvDestroyWindow("Fundido encadenado sin sse");
    cvDestroyWindow("Fundido encadenado con sse");
    cvDestroyWindow("Copia con sse");
    return EXIT_SUCCESS;
}