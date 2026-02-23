#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define ANADIDO 20

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Error: Usage %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    //CV_LOAD_IMAGE_COLOR = 1 forces the resultant IplImage to be colour.
    //CV_LOAD_IMAGE_GRAYSCALE = 0 forces a greyscale IplImage.
    //CV_LOAD_IMAGE_UNCHANGED = -1
    IplImage* Img = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);

    // Always check if the program can find the image file
    if (!Img) {
        printf("Error: file %s not found\n", argv[1]);
        return EXIT_FAILURE;
    }

    // a visualization window is created with title: image file name
    cvNamedWindow(argv[1],  CV_WINDOW_AUTOSIZE);
    cvShowImage(argv[1], Img);
    cvWaitKey(0);

    IplImage* ImgBLUE = cvCreateImage(cvSize(Img->width, Img->height), IPL_DEPTH_8U, 3);
    IplImage* ImgGREEN = cvCreateImage(cvSize(Img->width, Img->height), IPL_DEPTH_8U, 3);
    IplImage* ImgRED = cvCreateImage(cvSize(Img->width, Img->height), IPL_DEPTH_8U, 3);
    IplImage* ImgMitad = cvCreateImage(cvSize(Img->width, Img->height), IPL_DEPTH_8U, 3);
    IplImage* ImgMitad2 = cvCreateImage(cvSize(Img->width, Img->height), IPL_DEPTH_8U, 3);
    IplImage* ImgGris = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 1);
    IplImage* ImgApilada = cvCreateImage(cvSize(Img->width, (Img->height)*4), IPL_DEPTH_8U, 3);
    IplImage* ImgMarco = cvCreateImage(cvSize((Img->width)+ANADIDO,(Img->height)+ANADIDO), IPL_DEPTH_8U, 3);
    IplImage* Tablero1 = cvCreateImage(cvSize(512, 512), IPL_DEPTH_8U, 1);
    IplImage* Tablero3 = cvCreateImage(cvSize(512, 512), IPL_DEPTH_8U, 3);
    IplImage* Tablero4 = cvCreateImage(cvSize(512, 512), IPL_DEPTH_8U, 4);
    
    int fila, columna;

    //EJERCICIO MOSTRAR LOS 3 CANALES
    for (fila=0; fila<Img->height; fila++) {
        unsigned char *pImg = (unsigned char *)(Img->imageData + fila * Img->widthStep);
        unsigned char *pImgBLUE = (unsigned char *)(ImgBLUE->imageData + fila * ImgBLUE->widthStep);
        unsigned char *pImgGREEN = (unsigned char *)(ImgGREEN->imageData+fila*ImgGREEN->widthStep);
        unsigned char *pImgRED = (unsigned char *)(ImgRED->imageData+fila*ImgRED->widthStep);
        for (columna = 0; columna < Img->width; columna++) {
            //componente azul
            *pImgBLUE++ = *pImg++;
            *pImgBLUE++ = 0;
            *pImgBLUE++ = 0;
            //componente verde
            *pImgGREEN++ = 0;
            *pImgGREEN++ = *pImg++;
            *pImgGREEN++ = 0;
            //componente roja
            *pImgRED++ = 0;
            *pImgRED++ = 0;
            *pImgRED++ = *pImg++;
        }
    }
    
    //EJERCICIO MITADES 1
    for(fila=0; fila<(Img->height)/2; fila++){
        unsigned char *pImg=(unsigned char *)(Img->imageData+(fila+(Img->height/2))*Img->widthStep);
        unsigned char *pImgMitad=(unsigned char *)(ImgMitad->imageData+fila*ImgMitad->widthStep);
        for(columna=0; columna<Img->width; columna++){
            *pImgMitad++=*pImg++;
            *pImgMitad++=*pImg++;
            *pImgMitad++=*pImg++;
        }
    }
    
    for(fila=0; fila<(Img->height)/2; fila++){
        unsigned char *pImg=(unsigned char *)(Img->imageData+fila*Img->widthStep);
        unsigned char *pImgMitad=(unsigned char *)(ImgMitad->imageData+(fila+(Img->height/2))*ImgMitad->widthStep);
        for(columna=0; columna<Img->width; columna++){
            *pImgMitad++=*pImg++;
            *pImgMitad++=*pImg++;
            *pImgMitad++=*pImg++;
        }
    }
    
    //EJERCICIO MITADES 2
    for(fila=0; fila<(Img->height); fila++){
        unsigned char *pImg=(unsigned char *)(Img->imageData+(fila)*Img->widthStep+(Img->width*Img->nChannels)/2);
        unsigned char *pImgMitad2=(unsigned char *)(ImgMitad2->imageData+(fila)*ImgMitad2->widthStep);
        for(columna=0; columna<(Img->width)/2; columna++){
            *pImgMitad2++=*pImg++;
            *pImgMitad2++=*pImg++;
            *pImgMitad2++=*pImg++;            
        }
    }
    
    for(fila=0; fila<(Img->height); fila++){
        unsigned char *pImg=(unsigned char *)(Img->imageData+fila*Img->widthStep);
        unsigned char *pImgMitad2=(unsigned char *)(ImgMitad2->imageData+fila*ImgMitad2->widthStep+(Img->width*Img->nChannels)/2);
        for(columna=0; columna<(Img->width)/2; columna++){
            *pImgMitad2++=*pImg++;
            *pImgMitad2++=*pImg++;
            *pImgMitad2++=*pImg++;
        }
    }
    
    //EJERCICIO GRIS EN FUNCION DE LA FILA
    for(fila=0; fila<256; fila++){
        unsigned char *pImgGris=(unsigned char *)(ImgGris->imageData+fila*ImgGris->widthStep);
        for(columna=0; columna<256; columna++){
            *pImgGris=fila;
            pImgGris++;
        }
    }
    
    //EJERCICIO COMPONENTES APILADAS
    for(fila=0; fila<Img->height; fila++){
        unsigned char *pImg=(unsigned char *)(Img->imageData+fila*Img->widthStep);
        unsigned char *pOriginal=(unsigned char *)(ImgApilada->imageData+fila*ImgApilada->widthStep);
        unsigned char *pBlue=(unsigned char *)(ImgApilada->imageData+(fila+(Img->height))*ImgApilada->widthStep);
        unsigned char *pGreen=(unsigned char *)(ImgApilada->imageData+(fila+2*(Img->height))*ImgApilada->widthStep);
        unsigned char *pRed=(unsigned char *)(ImgApilada->imageData+(fila+3*(Img->height))*ImgApilada->widthStep);
        for(columna=0; columna<Img->width; columna++){
            unsigned char B=*pImg++;
            unsigned char G=*pImg++;
            unsigned char R=*pImg++;
            //Parte original
            *pOriginal++=B;
            *pOriginal++=G;
            *pOriginal++=R;
            //Parte roja
            *pRed++=0;
            *pRed++=0;
            *pRed++=R;
            //Parte verde
            *pGreen++=0;
            *pGreen++=G;
            *pGreen++=0;
            //Parte azul
            *pBlue++=B;
            *pBlue++=0;
            *pBlue++=0;
        }
    }
    
    //EJERCICIO MARCO
    for(fila=0; fila<(Img->height); fila++){
        unsigned char *pImg=(unsigned char *)(Img->imageData+fila*Img->widthStep);
        unsigned char *pImgMarco=(unsigned char *)(ImgMarco->imageData+(fila+(ANADIDO/2))*ImgMarco->widthStep+(ANADIDO/2)*Img->nChannels);
        for(columna=0; columna<(Img->width); columna++){
            *pImgMarco++=*pImg++;
            *pImgMarco++=*pImg++;
            *pImgMarco++=*pImg++;
        }
    }
    
    //TABLEROS
    int tamCuadro = 64; //Viene de 512/8
    unsigned char color;
    
    for(fila=0; fila<(Tablero1->height); fila++){
        unsigned char *pTablero1=(unsigned char *)(Tablero1->imageData+fila*Tablero1->widthStep);
        unsigned char *pTablero3=(unsigned char *)(Tablero3->imageData+fila*Tablero3->widthStep);
        unsigned char *pTablero4=(unsigned char *)(Tablero4->imageData+fila*Tablero4->widthStep);
        for(columna=0; columna<(Tablero1->width); columna++){
            if(((fila/tamCuadro)+(columna/tamCuadro))%2 == 0){
                color = 255;
            }else{
                color = 0;
            }
            //Tablero 1 canal
            *pTablero1++=color;
            //Tablero 3 canales
            *pTablero3++=color;
            *pTablero3++=color;
            *pTablero3++=color;
            //Tablero 4 canales
            *pTablero4++=color;
            *pTablero4++=color;
            *pTablero4++=color;
            *pTablero4++=color;
        }
    }

    // crea y muestras las ventanas con las im genes
    cvNamedWindow("Componente BLUE", CV_WINDOW_AUTOSIZE);
    cvShowImage("Componente BLUE", ImgBLUE);
    cvNamedWindow("Componente GREEN", CV_WINDOW_AUTOSIZE);
    cvShowImage("Componente GREEN", ImgGREEN);
    cvNamedWindow("Componente RED", CV_WINDOW_AUTOSIZE);
    cvShowImage("Componente RED", ImgRED);
    cvWaitKey(0);
    cvNamedWindow("Ejercicio mitad 1", CV_WINDOW_AUTOSIZE);
    cvShowImage("Ejercicio mitad 1", ImgMitad);
    cvNamedWindow("Ejercicio mitad 2", CV_WINDOW_AUTOSIZE);
    cvShowImage("Ejercicio mitad 2", ImgMitad2);
    cvWaitKey(0);
    cvNamedWindow("Imagen gris", CV_WINDOW_AUTOSIZE);
    cvShowImage("Imagen gris", ImgGris);
    cvWaitKey(0);
    cvNamedWindow("Componentes apiladas", CV_WINDOW_NORMAL);
    cvShowImage("Componentes apiladas", ImgApilada);
    cvWaitKey(0);
    cvNamedWindow("Imagen marco", CV_WINDOW_AUTOSIZE);
    cvShowImage("Imagen marco", ImgMarco);
    cvWaitKey(0);
    cvNamedWindow("Talero 1 canal", CV_WINDOW_AUTOSIZE);
    cvShowImage("Tablero 1 canal", Tablero1);
    cvNamedWindow("Tablero 3 canales", CV_WINDOW_AUTOSIZE);
    cvShowImage("Tablero 3 canales", Tablero3);
    cvNamedWindow("Tablero 4 canales", CV_WINDOW_AUTOSIZE);
    cvShowImage("Tablero 4 canales", Tablero4);
    cvWaitKey(0); 

    cvReleaseImage(&Img);
    cvReleaseImage(&ImgBLUE);
    cvReleaseImage(&ImgGREEN);
    cvReleaseImage(&ImgRED);
    cvReleaseImage(&ImgMitad);
    cvReleaseImage(&ImgMitad2);
    cvReleaseImage(&ImgGris);
    cvReleaseImage(&ImgApilada);
    cvReleaseImage(&ImgMarco);
    cvReleaseImage(&Tablero1);
    cvReleaseImage(&Tablero3);
    cvReleaseImage(&Tablero4);
    
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("Componente BLUE");
    cvDestroyWindow("Componente GREEN");
    cvDestroyWindow("Componente RED");
    cvDestroyWindow("Ejercicio mitad");
    cvDestroyWindow("Ejercicio mitad 2");
    cvDestroyWindow("Imagen gris");
    cvDestroyWindow("Componentes apiladas");
    cvDestroyWindow("Imagen marco");
    cvDestroyWindow("Tablero 1 canal");
    cvDestroyWindow("Tablero 3 canales");
    cvDestroyWindow("Tablero 4 canales");
    
    return EXIT_SUCCESS;
}
