#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>

// ------------------ FPS Counter Struct ------------------
typedef struct {
    Uint64 last_counter;
    double delta_time;
    double fps;
    double smooth_fps;
} FPSCounter;

void fps_init(FPSCounter* f) {
    f->last_counter = SDL_GetPerformanceCounter();
    f->delta_time = 0.0;
    f->fps = 0.0;
    f->smooth_fps = 0.0;
}

void fps_update(FPSCounter* f) {
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 freq = SDL_GetPerformanceFrequency();
    f->delta_time = (double)(now - f->last_counter) / (double)freq;
    f->fps = (f->delta_time > 0) ? (1.0 / f->delta_time) : 0.0;
    if (f->smooth_fps == 0.0) {
        f->smooth_fps = f->fps; // seed
    } else {
        double alpha = 0.1; // smoothing factor (0..1)
        f->smooth_fps = alpha * f->fps + (1.0 - alpha) * f->smooth_fps;
    }
    f->last_counter = now;
}
// ---------------------------------------------------------

SDL_Texture* render_text(SDL_Renderer* r, TTF_Font* font, const char* text, SDL_Color color, int* w, int* h) {
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text, color);
    if (!surf) return NULL;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    if (w) *w = surf->w;
    if (h) *h = surf->h;
    SDL_FreeSurface(surf);
    return tex;
}

int main(int argc, char** argv) {
    const char* font_path = NULL;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--font") && i + 1 < argc) font_path = argv[++i];
    }
    if (!font_path) {
        fprintf(stderr, "Usage: %s --font <path-to-ttf>\n", argv[0]);
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* win = SDL_CreateWindow("FPS Counter",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 100,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    TTF_Font* font = TTF_OpenFont(font_path, 28);
    if (!font) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        return 2;
    }

    FPSCounter fps;
    fps_init(&fps);

    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = 0;
        }

        // Update FPS
        fps_update(&fps);

        // Prepare text
        char buf[64];
        snprintf(buf, sizeof(buf), "FPS: %.1f", fps.smooth_fps);

        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

        SDL_Color fg = {230, 230, 235, 255};
        int tw, th;
        SDL_Texture* tex = render_text(ren, font, buf, fg, &tw, &th);
        SDL_Rect dst = { 10, 10, tw, th };
        SDL_RenderCopy(ren, tex, NULL, &dst);
        SDL_DestroyTexture(tex);

        SDL_RenderPresent(ren);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
