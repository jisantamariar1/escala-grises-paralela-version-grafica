
#include "escala_gris_serial.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <string>

// uint8_t, uint32_t c std int
#include <cstdint>


void filtroImagenSerial(std::string rutaEntrada,
                  std::string rutaSalida)
{


    int ancho, alto, canales;



    // --------------------------------------------------------
    // Variables para almacenar los canales individuales
    // del píxel actual.
    //
    // uint8_t:
    // entero sin signo de 8 bits (1 byte).
    //
    // Cada canal de color ocupa exactamente 1 byte.
    //
    // rango:
    // 0 -> 255
    // --------------------------------------------------------
    uint8_t a, r, g, b;
    uint8_t gris;



    // --------------------------------------------------------
    // CARGAR IMAGEN
    //
    // stbi_load devuelve:
    //
    // uint8_t*
    //
    // o sea:
    //
    // "la dirección de memoria donde empieza la imagen"
    //
    // La imagen NO se guarda como matriz 2D real.
    //
    // En memoria realmente queda así:
    //
    // R G B A R G B A R G B A ...
    //
    // Todo seguido linealmente.
    //
    //
    // Parámetros:
    //
    // rutaEntrada.c_str()
    // -> convierte std::string a texto estilo C.
    //
    // &ancho
    // -> dirección donde stb guardará el ancho.
    //
    // &alto
    // -> dirección donde stb guardará el alto.
    //
    // &canales
    // -> dirección donde stb guardará los canales originales.
    //
    // STBI_rgb_alpha
    // -> obligamos a stb a devolver SIEMPRE RGBA.
    //
    // Entonces:
    // cada píxel tendrá 4 bytes.
    // --------------------------------------------------------
    uint8_t* imagen =
    stbi_load(
        rutaEntrada.c_str(),
        &ancho,
        &alto,
        &canales,
        STBI_rgb_alpha
    );



    // --------------------------------------------------------
    // Aunque la imagen original fuera RGB,
    // ahora stb ya la convirtió a RGBA.
    //
    // Entonces trabajaremos siempre con:
    //
    // R G B A
    //
    // = 4 canales
    // --------------------------------------------------------
    canales = 4;



    // --------------------------------------------------------
    // Crear buffer para la imagen de salida.
    //
    // new[] reserva memoria dinámica en RAM.
    //
    // Tamaño:
    //
    // ancho * alto * canales
    //
    // Ejemplo:
    //
    // 800 * 600 * 4
    //
    // porque:
    // - 800 columnas
    // - 600 filas
    // - 4 bytes por píxel RGBA
    //
    // IMPORTANTE:
    // luego debemos liberar esta memoria con delete[]
    // --------------------------------------------------------
    uint8_t* imagenSalida = new uint8_t[ancho * alto * canales];



    // --------------------------------------------------------
    // RECORRER IMAGEN
    //
    // y -> filas
    // x -> columnas
    // --------------------------------------------------------
    for(int y = 0; y < alto; y++)
    {

        for(int x = 0; x < ancho; x++)
        {

            // ------------------------------------------------
            // La imagen es lineal en memoria.
            //
            // Necesitamos convertir:
            //
            // (x,y)
            //
            // en:
            //
            // posición dentro del arreglo.
            //
            //
            // (y * ancho + x)
            //
            // calcula el número de píxel.
            //
            //
            // Luego:
            //
            // * canales
            //
            // porque cada píxel ocupa:
            //
            // RGBA = 4 bytes
            //
            //
            // Ejemplo:
            //
            // ancho = 800
            // canales = 4
            //
            // x=0,y=0 -> 0
            // x=1,y=0 -> 4
            // x=2,y=0 -> 8
            //
            // avanza de 4 en 4
            // porque cada píxel ocupa 4 bytes.
            // ------------------------------------------------
            int indice =
            (y * ancho + x) * canales;



            // ------------------------------------------------
            // LEER CANALES
            //
            // imagen[indice]
            //
            // significa:
            //
            // acceder al byte número "indice"
            // desde el inicio de la imagen.
            //
            //
            // El píxel actual en memoria se ve así:
            //
            // indice+0 -> R
            // indice+1 -> G
            // indice+2 -> B
            // indice+3 -> A
            //
            // ------------------------------------------------
            r = imagen[indice + 0];

            g = imagen[indice + 1];

            b = imagen[indice + 2];

            a = imagen[indice + 3];



            // ------------------------------------------------
            // AQUÍ VA EL FILTRO
            //
            // Ya tenemos acceso directo a los canales.
            //
            // En C++ con stb normalmente NO necesitamos:
            //
            // - empaquetar
            // - desempaquetar
            // - bit shifting
            //
            // porque los canales ya están separados.
            //
            // ------------------------------------------------

            //ejemplo con gray_scale = 0.21*R+0.72*G+0.07*B
            gris = 0.21*r+0.72*g+0.07*b;




            // ------------------------------------------------
            // GUARDAR NUEVO PÍXEL
            //
            // Escribimos los nuevos canales
            // en la imagen de salida.
            //
            // Nuevamente:
            //
            // +0 -> R
            // +1 -> G
            // +2 -> B
            // +3 -> A
            // ------------------------------------------------
            imagenSalida[indice + 0] = gris;

            imagenSalida[indice + 1] = gris;

            imagenSalida[indice + 2] = gris;

            imagenSalida[indice + 3] = a;

        }
    }



    // --------------------------------------------------------
    // GUARDAR PNG
    //
    // Parámetros:
    //
    // rutaSalida.c_str()
    // -> nombre/ruta archivo
    //
    // ancho
    // -> columnas
    //
    // alto
    // -> filas
    //
    // canales
    // -> RGBA = 4
    //
    // imagenSalida
    // -> dirección donde empiezan los bytes
    //
    // ancho * canales
    // -> bytes por fila (stride)
    //
    // Ejemplo:
    //
    // 800 * 4 = 3200 bytes por fila
    // --------------------------------------------------------
    stbi_write_png(
        rutaSalida.c_str(),
        ancho,
        alto,
        canales,
        imagenSalida,
        ancho * canales
    );



    // --------------------------------------------------------
    // LIBERAR MEMORIA
    //
    // stbi_load usa memoria dinámica internamente.
    //
    // Entonces debemos liberar la imagen original.
    //
    // IMPORTANTE:
    //
    // NO usamos delete[]
    //
    // porque stb internamente usa malloc().
    //Su nombre es el acrónimo de Memory Allocation (Asignación de Memoria).
    // Entonces debemos usar:
    //
    // stbi_image_free()
    // --------------------------------------------------------
    stbi_image_free(imagen);



    // --------------------------------------------------------
    // Liberar memoria de la imagen de salida.
    //
    // Aquí sí usamos delete[]
    //
    // porque nosotros reservamos memoria usando:
    //
    // new[]
    // --------------------------------------------------------
    delete[] imagenSalida;
}