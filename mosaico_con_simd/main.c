#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

void copiarBloque(int filaO, int columnaO, IplImage *imgO, int filaD, int columnaD, IplImage *imgD, int alto, int ancho){
    int fila, columna;
    for(fila=0; fila<alto; fila++){
        __m128i *pImgO=(__m128i *)(imgO->imageData+(filaO+fila)*imgO->widthStep+(columnaO*3));
        __m128i *pImgD=(__m128i *)(imgD->imageData+(filaD+fila)*imgD->widthStep+(columnaD*3));
        for(columna=0; columna<ancho*3; columna+=16){
            *pImgD++=*pImgO++;
        }
    }
}

int compararBloque(int fila1, int columna1, IplImage *img1, int fila2, int columna2, IplImage *img2, int alto, int ancho){
    int fila,columna;
    int sad=0;
    for(fila=0; fila<alto; fila++){
        __m128i *pImg1=(__m128i *)(img1->imageData+(fila+fila1)*img1->widthStep+(columna1*3));
        __m128i *pImg2=(__m128i *)(img2->imageData+(fila+fila2)*img2->widthStep+(columna2*3));
        for(columna=0; columna<ancho*3; columna+=16){
            __m128i A=_mm_loadu_si128(pImg1);
            __m128i B=_mm_loadu_si128(pImg2);
            __m128i C=_mm_sad_epu8(*pImg1, *pImg2);
            __m128i D=_mm_srli_si128(C, 8);
            __m128i E=_mm_add_epi32(C, D);
            sad+=_mm_cvtsi128_si32(E);
            pImg1++;
            pImg2++;
        }
    }
    return sad;
}

int compararBloque2(int fila1, int col1, IplImage* img1, int fila2, int col2, IplImage* img2, int alto, int ancho){
    unsigned int diferenciaPixel=0;
    int fila,columna;
    for(fila=0; fila<alto; fila++){
        unsigned char *pImg1=(unsigned char *)(img1->imageData+(fila+fila1)*img1->widthStep+(col1*3));
        unsigned char *pImg2=(unsigned char *)(img2->imageData+(fila+fila2)*img2->widthStep+(col2*3));
        for(columna=0; columna<ancho*3; columna++){
            diferenciaPixel+=abs(((*pImg1)-(*pImg2)));
            pImg1++;
            pImg2++;
        }
    }
    return diferenciaPixel;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    IplImage* Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    IplImage* Img2 = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);
    
    // Always check if the program can find a file
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }
    if(!Img2){
        printf("Error: fichero %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }

    // a visualization window is created with title 'image'
    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvNamedWindow(argv[2], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvShowImage(argv[2], Img2);
    cvWaitKey(0);
    
    IplImage* imgMosaico=cvCloneImage(Img2);
    int filaO, filaD, columnaO, columnaD;

/*
    int fila, columna;
    int altoFila=32;
    int anchoColumna=32;
    for(fila=0; fila<1; fila+=altoFila){
        for(columna=0; columna<1; columna+=anchoColumna){
            copiarBloque(fila, columna, Img1, fila, columna, Img2, altoFila, anchoColumna);
            
            cvNamedWindow(argv[2], CV_WINDOW_NORMAL);
            cvShowImage(argv[2], Img2);
            cvWaitKey(1);
        }
    }
*/
    printf("Diferencia pixels: %d", compararBloque(0, 0, Img1, 144, 976, Img2, 16, 16));
    printf("\nDiferencia pixels: %d", compararBloque(0, 16, Img1, 144, 976, Img2, 16, 16));

    int filaA=0;
    int columnaA=0;
    for(filaO=0; filaO<Img1->height; filaO+=16){
        for(columnaO=0; columnaO<Img1->width; columnaO+=16){
            int diferencia=(16*16*255*3)+1;
            for(filaD=0; filaD<Img2->height; filaD+=16){
                for(columnaD=0; columnaD<Img2->width; columnaD+=16){
                    int nuevaDiferencia=compararBloque(filaO, columnaO, Img1, filaD, columnaD, Img2, 16, 16);
                    if(nuevaDiferencia<diferencia){
                        diferencia=nuevaDiferencia;
                        filaA=filaD;
                        columnaA=columnaD;
                    }
                }
            }
            copiarBloque(filaA, columnaA, Img2, filaO, columnaO, imgMosaico, 16, 16);
        }
    }
    cvNamedWindow("Mosaico sin simd", CV_WINDOW_NORMAL);
    cvShowImage("Mosaico sin simd", imgMosaico);
    cvWaitKey(0);

    
    cvReleaseImage(&Img1);
    cvReleaseImage(&Img2);
    cvReleaseImage(&imgMosaico);
    
    cvDestroyWindow(argv[1]);
    cvDestroyWindow(argv[2]);
    cvDestroyWindow("Mosaico sin simd");
    return (EXIT_SUCCESS);
}

