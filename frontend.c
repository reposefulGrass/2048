
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "backend.h"

#define WINDOW_WIDTH    540
#define WINDOW_HEIGHT   640

SDL_Texture *
load_texture (SDL_Renderer *renderer, char *path) {
    SDL_Surface *surface = IMG_Load(path);
    if (surface == NULL) {
        fprintf(stderr, "Unable to load surface %s!\n", path);
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        fprintf(stderr, "Unable to load texture: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(surface);

    return texture;
}


SDL_Texture **
load_tiles (SDL_Renderer *renderer) {
    SDL_Texture **tiles = (SDL_Texture **) malloc(sizeof(SDL_Surface *) * 11);
    char generic_path[32];

    for (int i = 0; i < 12; i++) {
        snprintf(generic_path, 32, "./resources/tile_%d.bmp", i);
        tiles[i] = load_texture(renderer, generic_path);
    }

    return tiles;
}


void
destroy_tiles (SDL_Texture **tiles) {
    for (int i = 0; i < 12; i++) {
        SDL_DestroyTexture(tiles[i]);
    }
    free(tiles);
}


void
draw_score (
    SDL_Renderer *renderer,
    TTF_Font *font,
    SDL_Color color,
    unsigned int score
) {
    char score_str[10];
    memcpy(score_str, "", 10);
    sprintf(score_str, "%d", score);
    printf("%s\n", score_str);

    SDL_Surface *text_surface = TTF_RenderText_Solid(font, score_str, color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    int text_width = 0, text_height = 0;
    SDL_QueryTexture(text_texture, NULL, NULL, &text_width, &text_height);

    SDL_Rect dest_rect = {
        .x = 20,
        .y = 20,
        .w = text_width,
        .h = text_height
    };

    SDL_RenderCopy(renderer, text_texture, NULL, &dest_rect);     

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}


void
draw_board (
    SDL_Renderer *renderer,
    SDL_Texture **tiles, 
    CELL *board
) {
    SDL_Rect tile_background = {
        .x = 20,
        .y = 110,
        .w = 500,
        .h = 510
    };

    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
    SDL_RenderFillRect(renderer, &tile_background);

    SDL_SetRenderDrawColor(renderer, 112, 128, 144, 255);

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            int x_pos = x * 120 + 40;
            int y_pos = y * 120 + 130;

            SDL_Rect src_area;
            src_area.x = 0;
            src_area.y = 0;
            src_area.w = 100;
            src_area.h = 100;

            SDL_Rect dest_area;
            dest_area.x = x_pos;
            dest_area.y = y_pos;
            dest_area.w = 100;
            dest_area.h = 100;

            CELL cell = board[y * BOARD_SIZE + x];
            if (*cell != 0) {
                SDL_Texture *tile_selected = tiles[((int) log2(*cell)) - 1];
                SDL_RenderCopy(renderer, tile_selected, &src_area, &dest_area);
            }
            else {
                SDL_Texture *tile_selected = tiles[11];
                SDL_RenderCopy(renderer, tile_selected, &src_area, &dest_area);
            }
        }
    }
}


void
initialize_sdl (SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        printf("Unable to load ttf: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    *window = SDL_CreateWindow(
        "2048",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        0
    );
    if (window == NULL) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    *renderer = SDL_CreateRenderer(
        *window, 
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if (renderer == NULL) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Unable to initialization IMG: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
}


int 
main () {
    srand(time(NULL));

    CELL *board = create_board();
    unsigned int score = 0;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    initialize_sdl(&window, &renderer);

    TTF_Font *font = TTF_OpenFont("./resources/SansSerif.ttf", 60);
    if (font == NULL) {
        fprintf(stderr, "Unable to load font 'sansserif': %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Color font_color = {255, 255, 255};

    SDL_Texture **tiles = load_tiles(renderer);
    SDL_Event event;

    // Draw background
    SDL_SetRenderDrawColor(renderer, 112, 128, 144, 255);
    SDL_RenderClear(renderer);

    // initial setup
    insert_two_randomly(board);
    draw_board(renderer, tiles, board);
    draw_score(renderer, font, font_color, score);
    SDL_RenderPresent(renderer);
    
    bool quit = false;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                    quit = true;       
                    break;
            }

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        score += apply_gravity_to_board(board, UP);
                        insert_two_randomly(board);
                        break;

                    case SDLK_DOWN:
                        score += apply_gravity_to_board(board, DOWN);
                        insert_two_randomly(board);
                        break;
                    
                    case SDLK_LEFT:
                        score += apply_gravity_to_board(board, LEFT);
                        insert_two_randomly(board);
                        break;

                    case SDLK_RIGHT:
                        score += apply_gravity_to_board(board, RIGHT);
                        insert_two_randomly(board);
                        break;
                }

                SDL_RenderClear(renderer);
                draw_board(renderer, tiles, board);
                draw_score(renderer, font, font_color, score);
                SDL_RenderPresent(renderer);
            }
        }
        if (quit) break;

        if (no_more_moves(board)) {
            // you've lost
            break; 
        }

        if (game_won(board)) {
            // you've won
            break; 
        }

        SDL_Delay(100);
    }

    destroy_board(board);
    destroy_tiles(tiles);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(font);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}


