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
    }else if(!Img2){
        printf("Error: fichero %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }

    // a visualization window is created with title 'image'
    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvNamedWindow(argv[2], CV_WINDOW_NORMAL);
    // img is shown in 'image' window
    cvShowImage(argv[1], Img1);
    cvShowImage(argv[2], Img2);
    cvWaitKey(0);

    IplImage* verde=cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 4);
    IplImage* amarillo=cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 4);
    
    IplImage* R1=cvCreateImage(cvSize(Img1->width, Img1->height), IPL_DEPTH_8U, Img1->nChannels);
    IplImage* G1=cvCreateImage(cvSize(Img1->width, Img1->height), IPL_DEPTH_8U, Img1->nChannels);
    IplImage* B1=cvCreateImage(cvSize(Img1->width, Img1->height), IPL_DEPTH_8U, Img1->nChannels);
    IplImage* A=cvCreateImage(cvSize(Img1->width, Img1->height), IPL_DEPTH_8U, Img1->nChannels);
    IplImage* PR=cvCreateImage(cvSize(Img1->width, Img1->height), IPL_DEPTH_8U, Img1->nChannels);
    
    IplImage* R2=cvCreateImage(cvSize(Img2->width, Img2->height), IPL_DEPTH_8U, Img2->nChannels);
    IplImage* G2=cvCreateImage(cvSize(Img2->width, Img2->height), IPL_DEPTH_8U, Img2->nChannels);
    IplImage* B2=cvCreateImage(cvSize(Img2->width, Img2->height), IPL_DEPTH_8U, Img2->nChannels);
    
    int fila, columna;
    
    //IMAGENES VERDE Y AMARILLO
    int colorVerde=0xA4C639;
    int colorAmarillo=0xF5E050;
    for(fila=0; fila<verde->height; fila++){
        __m128i *pVerde=(__m128i *)(verde->imageData+fila*verde->widthStep);
        for(columna=0; columna<verde->widthStep; columna+=16){
            *pVerde=_mm_set1_epi32(colorVerde);
            *pVerde++;
        }
    }
    
    for(fila=0; fila<amarillo->height; fila++){
        __m128i *pAmarillo=(__m128i *)(amarillo->imageData+fila*amarillo->widthStep);
        for(columna=0; columna<amarillo->widthStep; columna+=16){
            *pAmarillo=_mm_set1_epi32(colorAmarillo);
            *pAmarillo++;
        }
    }
    
    //COMPONENTES
    int mascaraR=0xFF0000;
    int mascaraG=0x00FF00;
    int mascaraB=0x0000FF;
    for(fila=0; fila<Img1->height; fila++){
        __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
        __m128i *pR=(__m128i *)(R1->imageData+fila*R1->widthStep);
        __m128i *pG=(__m128i *)(G1->imageData+fila*G1->widthStep);
        __m128i *pB=(__m128i *)(B1->imageData+fila*B1->widthStep);
        for(columna=0; columna<Img1->widthStep; columna+=16){
            __m128i A=_mm_set1_epi32(mascaraB);
            *pB++=_mm_and_si128(A, *pImg1);
            __m128i B=_mm_set1_epi32(mascaraG);
            *pG++=_mm_and_si128(B, *pImg1);
            __m128i C=_mm_set1_epi32(mascaraR);
            *pR++=_mm_and_si128(C, *pImg1);
            *pImg1++;
        }
    }
    
    //ALFA
    __m128i mascaraA=_mm_set1_epi32(0xFF000000);
    for(fila=0; fila<Img1->height; fila++){
        __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
        __m128i *pA=(__m128i *)(A->imageData+fila*A->widthStep);
        for(columna=0; columna<Img1->widthStep; columna+=16){
            __m128i original=_mm_loadu_si128(pImg1);
            __m128i alfa=_mm_and_si128(original, mascaraA);
            
            __m128i rojo=_mm_srli_epi32(alfa, 8);
            __m128i azul=_mm_srli_epi32(alfa, 16);
            __m128i verde=_mm_srli_epi32(alfa, 24);
            
            __m128i gris=_mm_or_si128(alfa, rojo);
            gris=_mm_or_si128(gris, azul);
            gris=_mm_or_si128(gris, verde);
            
            _mm_storeu_si128(pA, gris);
            
            pImg1++;
            pA++;
        }
    }
    
    //PIXELS ROJO
    __m128i mascaraPR=_mm_set1_epi32(0x00800000);
    __m128i mascara=_mm_set1_epi32(0xFF000000);
    for(fila=0; fila<Img1->height; fila++){
        __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
        __m128i *pPR=(__m128i *)(PR->imageData+fila*PR->widthStep);
        for(columna=0; columna<Img1->widthStep; columna+=16){
            __m128i original=_mm_loadu_si128(pImg1);
            __m128i pr=_mm_and_si128(original, mascara);
            
            __m128i azul=_mm_srli_epi32(pr, 16);
            __m128i verde=_mm_srli_epi32(pr, 24);
            
            __m128i resultado=_mm_or_si128(pr, mascaraPR);
            resultado=_mm_or_si128(resultado, azul);
            resultado=_mm_or_si128(resultado, verde);
            
            _mm_storeu_si128(pPR, resultado);
            
            pImg1++;
            pPR++;
        }
    }
    
    //COMPONENTES (IMG 3 CANALES) no caben pixels enteros en el vector de 16
    __m128i mascara1=_mm_set_epi8(0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF);
    __m128i mascara2=_mm_set_epi8(0, 0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0);
    __m128i mascara3=_mm_set_epi8(0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0, 0, 0xFF, 0, 0);
    for(fila=0; fila<Img2->height; fila++){
        __m128i *pImg2=(__m128i *)(Img2->imageData+fila*Img2->widthStep);
        __m128i *pB=(__m128i *)(B2->imageData+fila*B2->widthStep);
        __m128i *pG=(__m128i *)(G2->imageData+fila*G2->widthStep);
        __m128i *pR=(__m128i *)(R2->imageData+fila*R2->widthStep);
        int a=0;
        for(columna=0; columna<Img2->widthStep; columna+=16){
            if(a==0){
                *pB=_mm_and_si128(*pImg2, mascara1);
                *pG=_mm_and_si128(*pImg2, mascara2);
                *pR=_mm_and_si128(*pImg2, mascara3);
                a++;
            }else if(a==1){
                *pB=_mm_and_si128(*pImg2, mascara2);
                *pG=_mm_and_si128(*pImg2, mascara3);
                *pR=_mm_and_si128(*pImg2, mascara1);
                a++;
            }else if(a==2){
                *pB=_mm_and_si128(*pImg2, mascara3);
                *pG=_mm_and_si128(*pImg2, mascara1);
                *pR=_mm_and_si128(*pImg2, mascara2);
                a++;
            }
            *pB++;
            *pG++;
            *pR++;
            *pImg2++;
            a=(a+1)%3;
        }
    }
    
    
    cvNamedWindow("Verde", CV_WINDOW_NORMAL);
    cvNamedWindow("Amarillo", CV_WINDOW_NORMAL);
    cvShowImage("Verde", verde);
    cvShowImage("Amarillo", amarillo);
    cvWaitKey(0);
    
    cvNamedWindow("Rojo 4 canales", CV_WINDOW_NORMAL);
    cvShowImage("Rojo 4 canales", R1);
    cvNamedWindow("Verde 4 canales", CV_WINDOW_NORMAL);
    cvShowImage("Verde 4 canales", G1);
    cvNamedWindow("Azul 4 canales", CV_WINDOW_NORMAL);
    cvShowImage("Azul 4 canales", B1);
    cvNamedWindow("Alfa", CV_WINDOW_NORMAL);
    cvShowImage("Alfa", A);
    cvNamedWindow("Pixels rojo", CV_WINDOW_NORMAL);
    cvShowImage("Pixels rojo", PR);
    cvWaitKey(0);
    
    cvNamedWindow("Rojo 3 canales", CV_WINDOW_NORMAL);
    cvShowImage("Rojo 3 canales", R2);
    cvNamedWindow("Verde 3 canales", CV_WINDOW_NORMAL);
    cvShowImage("Verde 3 canales", G2);
    cvNamedWindow("Azul 3 canales", CV_WINDOW_NORMAL);
    cvShowImage("Azul 3 canales", B2);
    cvNamedWindow("Azul 3 canales", CV_WINDOW_NORMAL);
    cvWaitKey(0);
    
    // memory release for img before exiting the application
    cvReleaseImage(&Img1);
    cvReleaseImage(&verde);
    cvReleaseImage(&amarillo);
    cvReleaseImage(&R1);
    cvReleaseImage(&G1);
    cvReleaseImage(&B1);
    cvReleaseImage(&A);
    cvReleaseImage(&PR);
    cvReleaseImage(&R2);
    cvReleaseImage(&G2);
    cvReleaseImage(&B2);
    
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("Verde");
    cvDestroyWindow("Amarillo");
    cvDestroyWindow("Rojo 4 canales");
    cvDestroyWindow("Verde 4 canales");
    cvDestroyWindow("Azul 4 canales");
    cvDestroyWindow("Alfa");
    cvDestroyWindow("Pixels rojo");
    cvDestroyWindow("Rojo 3 canales");
    cvDestroyWindow("Verde 3 canales");
    cvDestroyWindow("Azul 3 canales");
    return EXIT_SUCCESS;
}