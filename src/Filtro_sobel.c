#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main() {
    char nombre_archivo[100];

    printf("Ingrese el nombre de la imagen BMP (ej: imagen.bmp): ");
    fgets(nombre_archivo, sizeof(nombre_archivo), stdin);

    nombre_archivo[strcspn(nombre_archivo, "\n")] = 0;

    // ===== RUTA INPUT =====
    char ruta_input[150];
    sprintf(ruta_input, "input/%s", nombre_archivo);

    FILE *imagen_color = fopen(ruta_input, "rb");

    if (imagen_color == NULL) {
        printf("Error al abrir la imagen en carpeta input\n");
        return 1;
    }

    // ===== NOMBRE BASE =====
    char nombre_base[100];
    strcpy(nombre_base, nombre_archivo);

    char *punto = strrchr(nombre_base, '.');
    if (punto != NULL) {
        *punto = '\0';
    }

    // ===== RUTA OUTPUT =====
    char nombre_sobel[150];
    sprintf(nombre_sobel, "output/%s_sobel.bmp", nombre_base);

    FILE *imagen_sobel = fopen(nombre_sobel, "wb");

    if (!imagen_sobel) {
        printf("Error al crear archivo en carpeta output\n");
        return 1;
    }

    int umbral = 125;
    unsigned char titulo[54];

    fread(titulo, sizeof(unsigned char), 54, imagen_color);
    fwrite(titulo, sizeof(unsigned char), 54, imagen_sobel);

    int ancho = *(int*)&titulo[18];
    int alto  = *(int*)&titulo[22];

    printf("Ancho: %d, Alto: %d\n", ancho, alto);

    // Memoria dinámica
    unsigned char **m3 = malloc(alto * sizeof(unsigned char*));
    for(int i=0; i<alto; i++){
        m3[i] = malloc(ancho * sizeof(unsigned char));
    }

    // Escala de grises
    for(int i=0; i<alto; i++){
        for(int j=0; j<ancho; j++){
            unsigned char B = getc(imagen_color);
            unsigned char G = getc(imagen_color);
            unsigned char R = getc(imagen_color);

            unsigned char gris = (unsigned char)(0.3*R + 0.59*G + 0.11*B);
            m3[i][j] = gris;
        }
    }

    int **gx = malloc(alto * sizeof(int*));
    int **gy = malloc(alto * sizeof(int*));

    for(int i=0; i<alto; i++){
        gx[i] = malloc(ancho * sizeof(int));
        gy[i] = malloc(ancho * sizeof(int));
    }

    // Sobel X
    for(int i=0; i<alto; i++){
        for(int j=0; j<ancho; j++){

            if (i==0 || j==0 || i==alto-1 || j==ancho-1){
                gx[i][j] = 0;
            } else {
                gx[i][j] =
                    (m3[i-1][j+1] - m3[i-1][j-1]) +
                    2*(m3[i][j+1] - m3[i][j-1]) +
                    (m3[i+1][j+1] - m3[i+1][j-1]);
            }

            int val = abs(gx[i][j]);
            if (val > 255) val = 255;
        }
    }

    // Sobel Y
    for(int i=0; i<alto; i++){
        for(int j=0; j<ancho; j++){

            if (i==0 || j==0 || i==alto-1 || j==ancho-1){
                gy[i][j] = 0;
            } else {
                gy[i][j] =
                    (m3[i+1][j-1] - m3[i-1][j-1]) +
                    2*(m3[i+1][j] - m3[i-1][j]) +
                    (m3[i+1][j+1] - m3[i-1][j+1]);
            }

            int val = abs(gy[i][j]);
            if (val > 255) val = 255;
        }
    }

    // Sobel final
    for(int i=0; i<alto; i++){
        for(int j=0; j<ancho; j++){
            int g = (int)sqrt(gx[i][j]*gx[i][j] + gy[i][j]*gy[i][j]);

            if (g > umbral) g = 255;
            else g = 0;

            putc(g, imagen_sobel);
            putc(g, imagen_sobel);
            putc(g, imagen_sobel);
        }
    }

    fclose(imagen_color);
    fclose(imagen_sobel);

    printf("Procesamiento terminado\n");

    // Abrir resultado
    char comando[200];
    sprintf(comando, "start %s", nombre_sobel);
    system(comando);

    return 0;
}