#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <complex>

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

std::complex<double> lerp_complex(std::complex<double> a, std::complex<double> b, const double t) {
    return a * (1.0 - t) + b * t;
}

void juliaSet(SDL_Renderer* renderer, double& t, double& t_step) {
    const std::complex<double> JULIA_C(-0.8, 0.156);
    // Viewport params
    const double x_min = -2.0;
    const double x_max = 2.0;
    const double y_min = -2.0;
    const double y_max = 2.0;

    for(int y = 0; y < MATRIX_SIZE; y++) {
        for(int x = 0; x < MATRIX_SIZE; x++) {
            std::complex<double> z_pixel(
                x_min + (double)x / MATRIX_SIZE * (x_max - x_min),
                y_min + (double)y / MATRIX_SIZE * (y_max - y_min)
            );

            // Morphing logic
            // When t = 0, we have  the Mandelbrot set (z0=0, c=z_pixel).
            // When t = 1, we have the Julia set (z0=z_pixel, c=JULIA_C).
            std::complex<double> c = lerp_complex(z_pixel, JULIA_C, t);
            std::complex<double> z = lerp_complex({0.0, 0.0}, z_pixel, t);

            int i = 0;
            for(i = 0; i < MAX_ITERATIONS && std::norm(z) < 4; i++) {
                z = z * z + c; 
            }

            SDL_Color color;
            if (i == MAX_ITERATIONS) {
                color = {0, 0, 0, 255};

                //drawMatrixPixel(renderer, x, y, PIXEL_SIZE, color);
            } else {
                //SDL_Color color = {255, 255, 255, 255}; //{(uint8_t)(i), (uint8_t)(i), (uint8_t)(i), 255};
                double smooth_i = i + 1 - log(log(std::abs(z))) / log(2);
                SDL_Color color1 = {0, 0, 0, 255};
                SDL_Color color2 = {255, 255, 255, 255};

                double fraction = smooth_i - i;
                uint8_t r = (uint8_t)(color1.r * (1.0 - fraction) + color2.r * fraction);
                uint8_t g = (uint8_t)(color1.g * (1.0 - fraction) + color2.g * fraction);
                uint8_t b = (uint8_t)(color1.b * (1.0 - fraction) + color2.b * fraction);
                color = {r, g, b, 255};
                //drawMatrixPixel(renderer, x, y, PIXEL_SIZE, final_color);
            }
            drawMatrixPixel(renderer, x, y, PIXEL_SIZE, color);
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

    // SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    // SDL_RenderClear(renderer);

    double t = 0.0;
    double t_step = 0.0005;

    //mandelbrotSetEscapeAlgorithm(renderer);

    //SDL_RenderPresent(renderer);

    bool quit = false;
    SDL_Event e;
    while(!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        juliaSet(renderer, t, t_step);

        SDL_RenderPresent(renderer);

        t += t_step;
        if (t > 1.0) {
            t = 1.0;
            t_step *= -1.0;
        } else if (t < 0.0) {
            t = 0.0;
            t_step *= -1.0;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
