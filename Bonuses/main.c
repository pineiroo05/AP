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

    IplImage* Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED); //Imagen de 3 canales
    IplImage* Img2 = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED); //Imagen de 4 canales
    
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (!Img2) {
        printf("Error: fichero %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_AUTOSIZE);
    cvShowImage(argv[1], Img1);
    cvNamedWindow(argv[2], CV_WINDOW_AUTOSIZE);
    cvShowImage(argv[2], Img2);
    cvWaitKey(0);
    
    IplImage *resultado1=cvCloneImage(Img1);
    IplImage *resultado2=cvCloneImage(Img1);
    
    IplImage *resultado3=cvCloneImage(Img2);
    IplImage *resultado4=cvCloneImage(Img2);
    
    IplImage *resultado5=cvCreateImage(cvSize(Img1->width,Img1->height),IPL_DEPTH_8U, Img1->nChannels);
    IplImage *resultado6=cvCreateImage(cvSize(Img1->width,Img1->height),IPL_DEPTH_8U, Img1->nChannels);
    
    IplImage *resultado7=cvCreateImage(cvSize(Img2->width,Img2->height),IPL_DEPTH_8U, Img2->nChannels);
    IplImage *resultado8=cvCreateImage(cvSize(Img2->width,Img2->height),IPL_DEPTH_8U, Img2->nChannels);

    int x, fila, columna;
    
    //FUNDIDO A AMARILLO (vale con cualquier color)
    //Sin sse y 3 canales
    for(x=0; x<256; x++){
        for(fila=0; fila<resultado1->height; fila++){
            unsigned char *pResultado1=(unsigned char *)(resultado1->imageData+fila*resultado1->widthStep);
            for(columna=0; columna<resultado1->width; columna++){
                //componente azul
                if((*pResultado1)==0){
                    *pResultado1++;
                }else{
                    (*pResultado1)--;
                    *pResultado1++;
                }
                //componente verde
                if((*pResultado1)==255){
                    *pResultado1++;
                }else{
                    (*pResultado1)++;
                    *pResultado1++;
                }
                //componente roja
                if((*pResultado1)==255){
                    *pResultado1++;
                }else{
                    (*pResultado1)++;
                    *pResultado1++;
                }
            }
        }
        cvNamedWindow("fundido a amarillo, 3 canales sin sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido a amarillo, 3 canales sin sse", resultado1);
        cvWaitKey(1);
    }
    
    //Con sse y 3 canales
    __m128i mascara1=_mm_set_epi8(1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1);
    __m128i mascara2=_mm_set_epi8(0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0);
    __m128i mascara3=_mm_set_epi8(0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0);
    for(x=0; x<256; x++){
        for(fila=0; fila<resultado1->height; fila++){
            __m128i *pResultado2=(__m128i *)(resultado2->imageData+fila*resultado2->widthStep);
            int a=0;
            for(columna=0; columna<resultado2->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(pResultado2);
                if(a==0){
                    img=_mm_subs_epu8(img,mascara1);
                    img=_mm_adds_epu8(img,mascara2);
                    img=_mm_adds_epu8(img,mascara3);
                }
                if(a==1){
                    img=_mm_subs_epu8(img,mascara2);
                    img=_mm_adds_epu8(img,mascara3);
                    img=_mm_adds_epu8(img,mascara1);
                }
                if(a==2){
                    img=_mm_subs_epu8(img,mascara3);
                    img=_mm_adds_epu8(img,mascara1);
                    img=_mm_adds_epu8(img,mascara2);
                }
                _mm_storeu_si128(pResultado2,img);
                a=(a+1)%3;
                pResultado2++;
            }
        }
        cvNamedWindow("fundido a amarillo, 3 canales con sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido a amarillo, 3 canales con sse", resultado2);
        cvWaitKey(1);
    }
    
    //Sin sse y 4 canales
    for(x=0; x<256; x++){
        for(fila=0; fila<resultado3->height; fila++){
            unsigned char *pResultado3=(unsigned char *)(resultado3->imageData+fila*resultado3->widthStep);
            for(columna=0; columna<resultado3->width; columna++){
                //componente azul
                if((*pResultado3)>0){
                    (*pResultado3)--;
                }
                pResultado3++;
                //componente verde
                if((*pResultado3)<255){
                    (*pResultado3)++;
                }
                pResultado3++;
                //componente roja
                if((*pResultado3)<255){
                    (*pResultado3)++;
                }
                pResultado3++;
                //alfa
                pResultado3++;
            }
        }
        cvNamedWindow("fundido a amarillo, 4 canales sin sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido a amarillo, 4 canales sin sse", resultado3);
        cvWaitKey(1);
    }
    
    //Con sse y 4 canales
    __m128i mascaraR=_mm_set_epi8(0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0);
    __m128i mascaraG=_mm_set_epi8(0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0);
    __m128i mascaraB=_mm_set_epi8(0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,0,1);
    for(x=0; x<256; x++){
        for(fila=0; fila<resultado4->height; fila++){
            __m128i *pResultado4=(__m128i *)(resultado4->imageData+fila*resultado4->widthStep);
            for(columna=0; columna<resultado4->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(pResultado4);
                //componente azul
                img=_mm_subs_epu8(img,mascaraB);
                //componente verde
                img=_mm_adds_epu8(img,mascaraG);
                //componente roja
                img=_mm_adds_epu8(img,mascaraR);
                _mm_storeu_si128(pResultado4,img);
                pResultado4++;                
            }
        }
        cvNamedWindow("fundido a amarillo, 4 canales con sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido a amarillo, 4 canales con sse", resultado4);
        cvWaitKey(1);
    }
    //FUNDIDO DESDE AMARILLO
    //Sin sse y 3 canales
    for(fila=0; fila<resultado5->height; fila++){
        unsigned char *pResultado5=(unsigned char *)(resultado5->imageData+fila*resultado5->widthStep);
        for(columna=0; columna<resultado5->width; columna++){
            *pResultado5++=0;
            *pResultado5++=255;
            *pResultado5++=255;
        }
    }
    for(x=0; x<256; x++){
        for(fila=0; fila<Img1->height; fila++){
            unsigned char *pResultado5=(unsigned char *)(resultado5->imageData+fila*resultado5->widthStep);
            unsigned char *pImg1=(unsigned char *)(Img1->imageData+fila*Img1->widthStep);
            for(columna=0; columna<Img1->widthStep; columna++){
                if((*pResultado5)<(*pImg1)){
                    (*pResultado5)++;
                }else if((*pResultado5)>(*pImg1)){
                    (*pResultado5)--;
                }
                pResultado5++;
                pImg1++;
            }
        }
        cvNamedWindow("fundido desde amarillo, 3 canales sin sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido desde amarillo, 3 canales sin sse", resultado5);
        cvWaitKey(1);
    }
    
    //Con sse y 3 canales
    for(fila=0; fila<resultado6->height; fila++){
        unsigned char *pResultado6=(unsigned char *)(resultado6->imageData+fila*resultado6->widthStep);
        for(columna=0; columna<resultado6->width; columna++){
            *pResultado6++=0;
            *pResultado6++=255;
            *pResultado6++=255;
        }
    }
    for(x=0; x<256; x++){
        for(fila=0; fila<Img1->height; fila++){
            __m128i *pResultado6=(__m128i *)(resultado6->imageData+fila*resultado6->widthStep);
            __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
            int a=0;
            for(columna=0; columna<Img1->widthStep; columna+=16){
                __m128i imgOrigen=_mm_loadu_si128(pImg1);
                __m128i imgResultado=_mm_loadu_si128(pResultado6);
                __m128i a=_mm_set1_epi8(1);
                
                imgResultado=_mm_adds_epu8(imgResultado,a);
                imgResultado=_mm_min_epu8(imgResultado,imgOrigen);
                
                _mm_storeu_si128(pResultado6,imgResultado);
                pResultado6++;
                pImg1++;
            }
        }
        cvNamedWindow("fundido desde amarillo, 3 canales con sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido desde amarillo, 3 canales con sse", resultado6);
        cvWaitKey(1);
    }
    
    //Sin sse y 4 canales
    for(fila=0; fila<resultado7->height; fila++){
        unsigned char *pResultado7=(unsigned char *)(resultado7->imageData+fila*resultado7->widthStep);
        for(columna=0; columna<resultado7->width; columna++){
            *pResultado7++=0;
            *pResultado7++=255;
            *pResultado7++=255;
            *pResultado7++=255;
        }
    }
    for(x=0; x<256; x++){
        for(fila=0; fila<Img2->height; fila++){
            unsigned char *pResultado7=(unsigned char *)(resultado7->imageData+fila*resultado7->widthStep);
            unsigned char *pImg2=(unsigned char *)(Img2->imageData+fila*Img2->widthStep);
            for(columna=0; columna<Img2->width; columna++){
                //componente azul
                if((*pResultado7)<(*pImg2)){
                    (*pResultado7)++;
                }else if((*pResultado7)>(*pImg2)){
                    (*pResultado7)--;
                }
                pResultado7++;
                pImg2++;
                //componente verde
                if((*pResultado7)<(*pImg2)){
                    (*pResultado7)++;
                }else if((*pResultado7)>(*pImg2)){
                    (*pResultado7)--;
                }
                pResultado7++;
                pImg2++;
                //componente roja
                if((*pResultado7)<(*pImg2)){
                    (*pResultado7)++;
                }else if((*pResultado7)>(*pImg2)){
                    (*pResultado7)--;
                }
                pResultado7++;
                pImg2++;
                
                pResultado7++;
                pImg2++;
            }
        }
        cvNamedWindow("fundido desde amarillo, 4 canales sin sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido desde amarillo, 4 canales sin sse", resultado7);
        cvWaitKey(1);
    }
    /*Con sse y 4 canales
    __m128i amarillo=_mm_set1_epi32(0xFFFFFF00);
    for(fila=0; fila<resultado8->height; fila++){
        __m128i *pResultado8=(__m128i *)(resultado8->imageData+fila*resultado8->widthStep);
        for(columna=0; columna<resultado8->widthStep; columna+=16){
            _mm_storeu_si128(pResultado8,amarillo);
            pResultado8++;
        }
    }
    
    __m128i uno=_mm_set1_epi8(1);
    for(x=0; x<256; x++){
        for(fila=0; fila<Img2->height; fila++){
            __m128i *pResultado8=(__m128i *)(resultado8->imageData+fila*resultado8->widthStep);
            __m128i *pImg2=(__m128i *)(Img2->imageData+fila*Img2->widthStep);
            for(columna=0; columna<Img2->widthStep; columna+=16){
                __m128i original=_mm_loadu_si128(pImg2);
                __m128i salida=_mm_loadu_si128(pResultado8);
                //componente azul -> esta a cero se sube
                salida=_mm_min_epu8(_mm_adds_epu8(salida,uno), original);
                //componentes verde y roja -> estan al maximo se bajan
                salida=_mm_max_epu8(_mm_subs_epu8(salida,uno), original);
                
                _mm_storeu_si128(pResultado8,salida);
                pResultado8++;
                pImg2++;
            }
        }
        cvNamedWindow("fundido desde amarillo, 4 canales con sse", CV_WINDOW_AUTOSIZE);
        cvShowImage("fundido desde amarillo, 4 canales con sse", resultado8);
        cvWaitKey(1);
    }*/
    
    cvReleaseImage(&Img1);
    cvReleaseImage(&resultado1);
    cvReleaseImage(&resultado2);
    cvReleaseImage(&resultado3);
    cvReleaseImage(&resultado4);   
    cvReleaseImage(&resultado5);
    cvReleaseImage(&resultado6);
    cvReleaseImage(&resultado7);
    cvReleaseImage(&resultado8);
    
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("fundido a amarillo, 3 canales sin sse");  
    cvDestroyWindow("fundido a amarillo, 3 canales con sse"); 
    cvDestroyWindow("fundido a amarillo, 4 canales sin sse"); 
    cvDestroyWindow("fundido a amarillo, 4 canales con sse"); 
    cvDestroyWindow("fundido desde amarillo, 3 canales sin sse");  
    cvDestroyWindow("fundido desde amarillo, 3 canales con sse");  
    cvDestroyWindow("fundido desde amarillo, 4 canales sin sse");  
    cvDestroyWindow("fundido desde amarillo, 4 canales con sse");  
    
    return EXIT_SUCCESS;
}