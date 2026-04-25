#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <Windows.h>

void copiarBloque(int filaO, int columnaO, IplImage *imgO, int filaD, int columnaD, IplImage *imgD, int alto, int ancho){
    int fila, columna;
    for(fila=0; fila<alto; fila++){
        unsigned char *pImgO=(unsigned char *)(imgO->imageData+(filaO+fila)*imgO->widthStep+(columnaO*3));
        unsigned char *pImgD=(unsigned char *)(imgD->imageData+(filaD+fila)*imgD->widthStep+(columnaD*3));
        for(columna=0; columna<ancho; columna++){
            *pImgD++=*pImgO++;
            *pImgD++=*pImgO++;
            *pImgD++=*pImgO++;
        }
    }
}

/*Sumar valor abs de las diferencias
Comparar si está bien: bloq consigo mismo da cero
Comparo por componente de color. Cuidado con el signo. Trabajo e valor absoluto. Probar a hacerlo consigo misma -> deberia dar 0*/
int compararBloque(int fila1, int col1, IplImage* img1, int fila2, int col2, IplImage* img2, int alto, int ancho){
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
    //ESTO TODO ES PARA QUE SE ABRA LA VENTANITA PARA ELEGIR LA FOTO
    OPENFILENAME ofn;
    char szFile[100];
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize=sizeof(ofn);
    ofn.hwndOwner=NULL;
    ofn.lpstrFile=szFile;
    ofn.lpstrFile[0]='\0';
    ofn.nMaxFile=sizeof(szFile);
    ofn.lpstrFilter="All\0*.*\0";
    ofn.nFilterIndex=1;
    ofn.lpstrFileTitle=NULL;
    ofn.nMaxFileTitle=0;
    ofn.lpstrInitialDir=NULL;
    ofn.Flags=OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    GetOpenFileName(&ofn);

    if (argc != 3) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    IplImage* Img1 = cvLoadImage(ofn.lpstrFile, CV_LOAD_IMAGE_UNCHANGED);
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
    /*COPIAR BLOQUE DE UNA ESQUINA    
    for(fila=0; fila<256; fila++){
        unsigned char *pImg1=(unsigned char *)(Img1->imageData+fila*Img1->widthStep);
        unsigned char *pImg2=(unsigned char *)(Img2->imageData+fila*Img2->widthStep);
        for(columna=0; columna<256; columna++){
            *pImg2++=*pImg1++;
            *pImg2++=*pImg1++;
            *pImg2++=*pImg1++;
        }
    }
    printf("Diferencia pixels: %d", compararBloque(0, 0, Img1, 144, 976, Img2, 16, 16));
    //TRANSICION IMAGENES
    /*int altoFila=32;
    int anchoColumna=32;
    for(fila=0; fila<Img1->height; fila+=altoFila){
        for(columna=0; columna<Img1->width; columna+=anchoColumna){
            copiarBloque(fila, columna, Img1, fila, columna, Img2, altoFila, anchoColumna);
            
            cvNamedWindow(argv[2], CV_WINDOW_NORMAL);
            cvShowImage(argv[2], Img2);
            cvWaitKey(1);
        }
    }*/
    //MOSAICO
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
    
    // memory release for img before exiting the application
    cvReleaseImage(&Img1);
    cvReleaseImage(&Img2);
    cvReleaseImage(&imgMosaico);
    // Self-explanatory
    cvDestroyWindow(argv[1]);
    cvDestroyWindow(argv[2]);
    cvDestroyWindow("Mosaico sin simd");
    return EXIT_SUCCESS;
}