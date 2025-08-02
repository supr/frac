#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <vector>
#include <cmath>

const int WINDOW_SIZE = 800; //576;
const int MATRIX_SIZE = 64;
const int MAX_ITERATIONS = 0xff;
const int PIXEL_SIZE = WINDOW_SIZE / MATRIX_SIZE;

void drawMatrixPixel(SDL_Renderer* renderer, int matrix_x, int matrix_y, int pixel_size, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_FRect pixel_rect;
    pixel_rect.x = matrix_x * pixel_size;
    pixel_rect.y = matrix_y * pixel_size;
    pixel_rect.w = pixel_size;
    pixel_rect.h = pixel_size;

    SDL_RenderFillRect(renderer, &pixel_rect);
}

void mandelbrotSetEscapeAlgorithm(SDL_Renderer* renderer)  {
    for(int y = 0; y < MATRIX_SIZE; y++) {
        for(int x = 0; x < MATRIX_SIZE; x++) {
            /* Chess board pattern 
                SDL_Color current_color;
                if ((x+y) % 2 == 0) {
                    current_color = {200, 200, 200, 255};
                } else {
                    current_color = {100, 100, 100, 255};
                }

                drawMatrixPixel(renderer, x, y, PIXEL_SIZE, current_color);
            */

            // Map the integer (x, y) coordinates to the complex plane.
            // We map the MATRIX_SIZE pixels to the range [-2, 1] for the real part (cx)
            // and [-1.5, 1] for the imaginary part (cy)
            double cx = (x / (double)(MATRIX_SIZE - 1)) * 3.0 - 2.0;
            double cy = (y / (double)(MATRIX_SIZE - 1)) * 3.0 - 1.5;

            double zx = 0.0;
            double zy = 0.0;

            int i = 0;

            for (i = 0; i < MAX_ITERATIONS; i++) {
                // Check the escape condition magnitude of Z > 2
                // (we check the magnitude squared > 4 to avoid slow sqrt)
                if (zx * zx + zy * zy >= 4.0) {
                    break; // Escaped
                }

                double new_zx = zx * zx - zy * zy + cx;
                double new_zy = 2 * zx * zy + cy;
                zx = new_zx;
                zy = new_zy;
            }

            if (i == MAX_ITERATIONS) {
                SDL_Color color = {0, 0, 0, 255};

                drawMatrixPixel(renderer, x, y, PIXEL_SIZE, color);
            } else {
                //SDL_Color color = {255, 255, 255, 255}; //{(uint8_t)(i), (uint8_t)(i), (uint8_t)(i), 255};
                double smooth_i = i + 1 - log(log(sqrt(zx * zx + zy * zy))) / log(2);
                SDL_Color color1 = {0, 0, 0, 255};
                SDL_Color color2 = {255, 255, 255, 255};

                double fraction = smooth_i - i;
                uint8_t r = (uint8_t)(color1.r * (1.0 - fraction) + color2.r * fraction);
                uint8_t g = (uint8_t)(color1.g * (1.0 - fraction) + color2.g * fraction);
                uint8_t b = (uint8_t)(color1.b * (1.0 - fraction) + color2.b * fraction);
                SDL_Color final_color = {r, g, b, 255};
                drawMatrixPixel(renderer, x, y, PIXEL_SIZE, final_color);
            }
        }
    }
}

int main(void) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Fractal", WINDOW_SIZE, WINDOW_SIZE, 0);
    if(!window) {
        std::cerr << "SDL could not create window! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if(!renderer) {
        std::cerr << "SDL could not create renderer! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    mandelbrotSetEscapeAlgorithm(renderer);

    SDL_RenderPresent(renderer);

    bool quit = false;
    SDL_Event e;
    while(!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
