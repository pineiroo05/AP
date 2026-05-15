#ifndef MYDLL_H
#define MYDLL_H
#include <opencv/cv.h>
#include <opencv/highgui.h>

struct RGB{
    unsigned int rojo;
    unsigned int verde;
    unsigned int azul;
};

IplImage *createImagen(int ancho, int alto, struct RGB color);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* MYDLL_H */

