#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>
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

    // a visualization window is created with title 'image'
    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], Img1);
    cvWaitKey(0);
    
    IplImage *ejercicio1=cvCreateImage(cvSize(Img1->width, Img1->height),IPL_DEPTH_8U, Img1->nChannels);
    IplImage *ejercicio2=cvCreateImage(cvSize(Img1->width, Img1->height),IPL_DEPTH_8U, Img1->nChannels);
    IplImage *ejercicio3=cvCloneImage(Img1);
    IplImage *ejercicio4=cvCloneImage(Img1);
    IplImage *ejercicio5=cvCloneImage(Img1);
    IplImage *ejercicio6=cvCloneImage(Img1);

    int fila, columna, x;
    int mitad=(Img1->height)/2;

    //SIN SSE
    for(fila=0; fila<mitad; fila++){
        unsigned char *pImg1=(unsigned char *)(Img1->imageData+fila*Img1->widthStep);
        unsigned char *parteAlfa=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep);
        for(columna=0; columna<Img1->width; columna++){
            unsigned int alfa=*(pImg1+3);
            *parteAlfa++=alfa;
            *parteAlfa++=alfa;
            *parteAlfa++=alfa;
            *parteAlfa++=255;
            pImg1+=4;
        }
    }
    for(fila=mitad; fila<Img1->height;fila++){
        unsigned char *parteRosa=(unsigned char *)(ejercicio1->imageData+fila*ejercicio1->widthStep);
        for(columna=0; columna<Img1->width; columna++){
            *parteRosa++=0xAF;
            *parteRosa++=0x86;
            *parteRosa++=0xD7;
            *parteRosa++=255;
        }
    }
    cvNamedWindow("ejercicio1", CV_WINDOW_AUTOSIZE);
    cvShowImage("ejercicio1", ejercicio1);
    cvWaitKey(0);
    
    //CON SSE
    __m128i mascaraAlfa=_mm_set1_epi32(0xFF000000);
    for(fila=0; fila<mitad; fila++){
        __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
        __m128i *pAlfa=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep);
        for(columna=0; columna<Img1->widthStep; columna+=16){
            __m128i img=_mm_loadu_si128(pImg1);
            __m128i alfa=_mm_and_si128(img,mascaraAlfa);
            
            __m128i rojo=_mm_srli_epi32(alfa, 24);
            __m128i verde=_mm_srli_epi32(alfa,16);
            __m128i azul=_mm_srli_epi32(alfa,8);
            
            __m128i gris=_mm_or_si128(alfa,rojo);
            gris=_mm_or_si128(gris,verde);
            gris=_mm_or_si128(gris,azul);
            
            _mm_storeu_si128(pAlfa,gris);
            pImg1++;
            pAlfa++;
        }
    }
    for(fila=mitad; fila<Img1->height; fila++){
        __m128i *pRosa=(__m128i *)(ejercicio2->imageData+fila*ejercicio2->widthStep);
        for(columna=0; columna<Img1->widthStep; columna+=16){
            //*pRosa++=colorRosa;
            __m128i img=_mm_loadu_si128(pRosa);
            img=_mm_set1_epi32(0xD786AF);
            _mm_storeu_si128(pRosa,img);
            pRosa++;
        }
    }
    cvNamedWindow("ejercicio2", CV_WINDOW_AUTOSIZE);
    cvShowImage("ejercicio2", ejercicio2);
    cvWaitKey(0);
    
    //LA MITAD SUPERIOR TRANSICIONA A ROSA
    //Sin sse
    for (x = 0; x < 256; x++) {
        for (fila = 0; fila < mitad; fila++) {
            unsigned char *parteFundido = (unsigned char *) (ejercicio3->imageData + fila * ejercicio3->widthStep);
            for (columna = 0; columna < Img1->width; columna++) {
                //componente azul
                if((*parteFundido)<0xAF){
                    (*parteFundido)++;
                }else if((*parteFundido)>0xAF){
                    (*parteFundido)--;
                }
                parteFundido++;
                //componente verde
                if((*parteFundido)<0x86){
                    (*parteFundido)++;
                }else if((*parteFundido)>0x86){
                    (*parteFundido)--;
                }
                parteFundido++;
                //compnente roja
                if((*parteFundido)<0xD7){
                    (*parteFundido)++;
                }else if((*parteFundido)>0xD7){
                    (*parteFundido)--;
                }
                parteFundido++;
                //alfa
                parteFundido++;
            }
        }
        for (fila = mitad; fila < Img1->height; fila++) {
            unsigned char *parteRosa = (unsigned char *) (ejercicio3->imageData + fila * ejercicio3->widthStep);
            for (columna = 0; columna < Img1->width; columna++) {
                *parteRosa++ = 0xAF;
                *parteRosa++ = 0x86;
                *parteRosa++ = 0xD7;
                *parteRosa++ = 255;
            }
        }
        cvNamedWindow("ejercicio3", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio3", ejercicio3);
        cvWaitKey(1);
    }
    //Con sse
    __m128i mascaraR=_mm_set_epi8(0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0);
    __m128i mascaraG=_mm_set_epi8(0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0);
    __m128i mascaraB=_mm_set_epi8(0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,0,1);
    for(x=0; x<256; x++){
        for(fila=0;fila<mitad; fila++){
            __m128i *pFundido=(__m128i *)(ejercicio4->imageData+fila*ejercicio4->widthStep);
            for(columna=0; columna<Img1->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(pFundido);
                //componente azul
                img=_mm_adds_epu8(img, mascaraB);
                //componente verde
                img=_mm_adds_epu8(img, mascaraG);
                //componente roja
                img=_mm_adds_epu8(img, mascaraR);
                
                img=_mm_min_epu8(img, _mm_set1_epi32(0x00D786AF));
                _mm_storeu_si128(pFundido, img);
                pFundido++;
            }
        }
        for(fila=mitad; fila<Img1->height; fila++){
            __m128i *pRosa=(__m128i *)(ejercicio4->imageData+fila*ejercicio4->widthStep);
            for(columna=0; columna<Img1->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(pRosa);
                img=_mm_set1_epi32(0xD786AF);
                _mm_storeu_si128(pRosa,img);
                pRosa++;
            }
        }
        cvNamedWindow("ejercicio4", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio4", ejercicio4);
        cvWaitKey(1);
    }
    
    //DOBLE FUNDIDO
    //Sin sse
    for(fila=mitad; fila<Img1->height; fila++){
        unsigned char *inferior=(unsigned char *)(ejercicio5->imageData+fila*ejercicio5->widthStep);
        for(columna=0; columna<Img1->width; columna++){
            *inferior++=0xAF;
            *inferior++=0x86;
            *inferior++=0xD7;
            *inferior++=255;
        }
    }
    
    for(x=0; x<256; x++){
        for(fila=0; fila<mitad; fila++){
            unsigned char *fundidoSuperior=(unsigned char *)(ejercicio5->imageData+fila*ejercicio5->widthStep);
            for(columna=0; columna<Img1->width; columna++){
                //componente azul
                if((*fundidoSuperior)<0xAF){
                    (*fundidoSuperior)++;
                }else if((*fundidoSuperior)>0xAF){
                    (*fundidoSuperior)--;
                }
                fundidoSuperior++;
                //componente verde
                if((*fundidoSuperior)<0x86){
                    (*fundidoSuperior)++;
                }else if((*fundidoSuperior)>0x86){
                    (*fundidoSuperior)--;
                }
                fundidoSuperior++;
                //componente roja
                if((*fundidoSuperior)<0xD7){
                    (*fundidoSuperior)++;
                }else if((*fundidoSuperior)>0xD7){
                    (*fundidoSuperior)--;
                }
                fundidoSuperior++;                
                //alfa
                fundidoSuperior++;                
            }
        }
        for(fila=mitad; fila<Img1->height; fila++){
            unsigned char *pImg1=(unsigned char *)(Img1->imageData+fila*Img1->widthStep);
            unsigned char *fundidoInferior=(unsigned char *)(ejercicio5->imageData+fila*ejercicio5->widthStep);
            for(columna=0; columna<Img1->width; columna++){
                //componente azul
                if((*fundidoInferior)<(*pImg1)){
                    (*fundidoInferior)++;
                }else if((*fundidoInferior)>(*pImg1)){
                    (*fundidoInferior)--;
                }
                fundidoInferior++;
                pImg1++;
                //componente verde
                if((*fundidoInferior)<(*pImg1)){
                    (*fundidoInferior)++;
                }else if((*fundidoInferior)>(*pImg1)){
                    (*fundidoInferior)--;
                }
                fundidoInferior++;
                pImg1++;
                //componente roja
                if((*fundidoInferior)<(*pImg1)){
                    (*fundidoInferior)++;
                }else if((*fundidoInferior)>(*pImg1)){
                    (*fundidoInferior)--;
                }
                fundidoInferior++;
                pImg1++;
                //alfa
                fundidoInferior++;
                pImg1++;
            }
        }
        cvNamedWindow("ejercicio5", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio5", ejercicio5);
        cvWaitKey(1);
    }
    
    //Con sse
    for(fila=mitad; fila<Img1->height; fila++){
        __m128i *rosa=(__m128i *)(ejercicio6->imageData+fila*ejercicio6->widthStep);
        for(columna=0; columna<Img1->widthStep; columna+=16){
            __m128i img=_mm_loadu_si128(rosa);
            img=_mm_set1_epi32(0xFFD786AF);
            _mm_storeu_si128(rosa,img);
            rosa++;
        }
    }
    //__m128i mascaraR=_mm_set_epi8(0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0);
    //__m128i mascaraG=_mm_set_epi8(0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0);
    //__m128i mascaraB=_mm_set_epi8(0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,0,1);
    for(x=0; x<256; x++){
        for(fila=0; fila<mitad; fila++){
            __m128i *superior=(__m128i *)(ejercicio6->imageData+fila*ejercicio6->widthStep);
            for(columna=0; columna<Img1->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(superior);
                //componente azul
                img=_mm_adds_epu8(img,mascaraB);
                //componente verde
                img=_mm_adds_epu8(img,mascaraG);
                //componente roja
                img=_mm_adds_epu8(img,mascaraR);
                
                img=_mm_min_epu8(img, _mm_set1_epi32(0xD786AF));
                _mm_storeu_si128(superior,img);
                superior++;
            }
        }
        for(fila=mitad; fila<Img1->height; fila++){
            __m128i *pImg1=(__m128i *)(Img1->imageData+fila*Img1->widthStep);
            __m128i *inferior=(__m128i *)(ejercicio6->imageData+fila*ejercicio6->widthStep);
            for(columna=0; columna<Img1->widthStep; columna+=16){
                __m128i img=_mm_loadu_si128(inferior);
                __m128i original=_mm_loadu_si128(pImg1);
                //componente azul
                img=_mm_subs_epu8(img, mascaraB);
                //componente verde
                img=_mm_subs_epu8(img, mascaraG);
                //componente roja
                img=_mm_subs_epu8(img, mascaraR);
                
                img=_mm_min_epu8(img,original);
                _mm_storeu_si128(inferior, img);
                inferior++;
                pImg1++;
            }
        }
        cvNamedWindow("ejercicio6", CV_WINDOW_AUTOSIZE);
        cvShowImage("ejercicio6", ejercicio6);
        cvWaitKey(1);
    }
    
    // memory release for img before exiting the application
    cvReleaseImage(&Img1);
    cvReleaseImage(&ejercicio1);
    cvReleaseImage(&ejercicio2);
    cvReleaseImage(&ejercicio3);
    cvReleaseImage(&ejercicio4);
    cvReleaseImage(&ejercicio5);
    cvReleaseImage(&ejercicio6);
    // Self-explanatory
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("ejercicio1");
    cvDestroyWindow("ejercicio2");
    cvDestroyWindow("ejercicio3");
    cvDestroyWindow("ejercicio4");
    cvDestroyWindow("ejercicio5");
    cvDestroyWindow("ejercicio6");
    return EXIT_SUCCESS;
}