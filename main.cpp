#include "op_simd.h"
#include "escala_gris_openmp.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <SFML/Graphics.hpp>
#include <fmt/core.h>
#include <string>
#include <vector>
#include <cstring>




// Enum para saber qué estado mostrar en el HUD
enum class Mode { ORIGINAL, SIMD, OPENMP };

int main() {
    // 1. CARGAR LA IMAGEN A LA RAM (Se hace solo 1 vez)
    int ancho, alto, canales;
    std::string ruta_entrada = "../../imagenes/paisaje.png"; // Asegúrate de tener una imagen aquí
    uint8_t* img_original = stbi_load(ruta_entrada.c_str(), &ancho, &alto, &canales, STBI_rgb_alpha);
    
    if (!img_original) {
        fmt::println("Error: No se pudo cargar la imagen. Verifica la ruta.");
        return -1;
    }
    int numPixeles = ancho * alto;

    // 2. CREAR BUFFER DE VISUALIZACIÓN
    // Este buffer será modificado por nuestros algoritmos y dibujado por SFML
    std::vector<uint8_t> img_display(numPixeles * 4); 
    // Por defecto, copiamos la original al buffer de visualización
    std::memcpy(img_display.data(), img_original, numPixeles * 4);

    // 3. CONFIGURAR SFML
    sf::RenderWindow window(sf::VideoMode({(unsigned)ancho, (unsigned)alto}), "Filtro Interactivo SIMD/OpenMP");
    sf::Texture texture({(unsigned)ancho, (unsigned)alto});
    sf::Sprite sprite(texture);

    // Configurar el texto de la interfaz (HUD)
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        fmt::println("Aviso: No se encontró arial.ttf. El texto no se renderizará.");
    }
    sf::Text hudText(font, "", 20);
    hudText.setFillColor(sf::Color::Red); // Rojo para que resalte sobre imágenes oscuras o claras
    hudText.setPosition({10, 10});
    hudText.setStyle(sf::Text::Bold);

    Mode current_mode = Mode::ORIGINAL;
    std::string hud_msg = "Modo: Original\nTeclas: [1] Original  [2] SIMD  [3] OpenMP  [S] Guardar";

    // 4. GAME LOOP DE SFML
    while (window.isOpen()) {
        // Procesar eventos de teclado
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (event->is<sf::Event::KeyReleased>()) {
                auto evt = event->getIf<sf::Event::KeyReleased>();
                
                switch (evt->scancode) {
                    case sf::Keyboard::Scan::Num1:
                        current_mode = Mode::ORIGINAL;
                        std::memcpy(img_display.data(), img_original, numPixeles * 4);
                        hud_msg = "Modo: Original";
                        break;
                    
                    case sf::Keyboard::Scan::Num2:
                        current_mode = Mode::SIMD;
                        filtroImagenSIMD(img_original, img_display.data(), numPixeles);
                        hud_msg = "Modo: Escala de Grises (SIMD AVX2)";
                        break;
                    
                    case sf::Keyboard::Scan::Num3:
                        current_mode = Mode::OPENMP;
                        filtroImagenOpenMP(img_original, img_display.data(), numPixeles);
                        hud_msg = "Modo: Escala de Grises (OpenMP)";
                        break;

                    case sf::Keyboard::Scan::S:
                        if (current_mode == Mode::ORIGINAL) {
                            fmt::println("Seleccione primero un filtro (2 o 3) antes de guardar.");
                        } else {
                            // Requerimiento del examen: Guardar imagen en escala de grises real (1 canal)
                            std::vector<uint8_t> gray_1ch(numPixeles);
                            for (int i = 0; i < numPixeles; ++i) {
                                // Extraemos un solo canal, porque R=G=B en nuestro buffer procesado
                                gray_1ch[i] = img_display[i * 4]; 
                            }

                            std::string filename = (current_mode == Mode::SIMD) ? "../../imagenes/salida_simd.png" : "../../imagenes/salida_openmp.png";
                            stbi_write_png(filename.c_str(), ancho, alto, STBI_grey, gray_1ch.data(), ancho);
                            fmt::println("¡Imagen guardada exitosamente como '{}' con formato STBI_grey (1 canal)!", filename);
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        // 5. ACTUALIZAR RENDERIZADO
        // Enviamos el buffer procesado (RAM) a la textura de SFML (VRAM)
        texture.update(img_display.data());
        
        hudText.setString(hud_msg + "\nTeclas: [1] Original  [2] SIMD  [3] OpenMP  [S] Guardar");

        window.clear();
        window.draw(sprite);
        if(font.hasGlyph('A')) window.draw(hudText); // Dibujar HUD solo si la fuente cargó
        window.display();
    }

    // Liberar memoria final
    stbi_image_free(img_original);
    return 0;
}