/* nuklear - 1.32.0 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include <SDL.h>
#include <SDL_mouse.h>
#include <SDL_keyboard.h>


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_SOFTWARE_FONT
#include "nuklear.h"
#define NK_SDLSURFACE_IMPLEMENTATION
#include "sdl2surface_rawfb.h"

// Pulled from another example header
NK_API int nk_sdl_handle_event(struct nk_context* ctx, SDL_Event* evt)
{
    if (evt->type == SDL_QUIT) {
        exit(0);
    }
    else if (evt->type == SDL_KEYUP || evt->type == SDL_KEYDOWN) {
        /* key events */
        int down = evt->type == SDL_KEYDOWN;
        const Uint8* state = SDL_GetKeyboardState(0);
        SDL_Keycode sym = evt->key.keysym.sym;
        if (sym == SDLK_RSHIFT || sym == SDLK_LSHIFT)
            nk_input_key(ctx, NK_KEY_SHIFT, down);
        else if (sym == SDLK_DELETE)
            nk_input_key(ctx, NK_KEY_DEL, down);
        else if (sym == SDLK_RETURN)
            nk_input_key(ctx, NK_KEY_ENTER, down);
        else if (sym == SDLK_TAB)
            nk_input_key(ctx, NK_KEY_TAB, down);
        else if (sym == SDLK_BACKSPACE)
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
        else if (sym == SDLK_HOME) {
            nk_input_key(ctx, NK_KEY_TEXT_START, down);
            nk_input_key(ctx, NK_KEY_SCROLL_START, down);
        }
        else if (sym == SDLK_END) {
            nk_input_key(ctx, NK_KEY_TEXT_END, down);
            nk_input_key(ctx, NK_KEY_SCROLL_END, down);
        }
        else if (sym == SDLK_PAGEDOWN) {
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
        }
        else if (sym == SDLK_PAGEUP) {
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
        }
        else if (sym == SDLK_z)
            nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_r)
            nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_c)
            nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_v)
            nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_x)
            nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_b)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_e)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_UP)
            nk_input_key(ctx, NK_KEY_UP, down);
        else if (sym == SDLK_DOWN)
            nk_input_key(ctx, NK_KEY_DOWN, down);
        else if (sym == SDLK_LEFT) {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
            else nk_input_key(ctx, NK_KEY_LEFT, down);
        }
        else if (sym == SDLK_RIGHT) {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
            else nk_input_key(ctx, NK_KEY_RIGHT, down);
        }
        else return 0;
        return 1;
    }
    else if (evt->type == SDL_MOUSEBUTTONDOWN || evt->type == SDL_MOUSEBUTTONUP) {
        /* mouse button */
        int down = evt->type == SDL_MOUSEBUTTONDOWN;
        const int x = evt->button.x, y = evt->button.y;
        if (evt->button.button == SDL_BUTTON_LEFT) {
            if (evt->button.clicks > 1)
                nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
        }
        else if (evt->button.button == SDL_BUTTON_MIDDLE)
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
        else if (evt->button.button == SDL_BUTTON_RIGHT)
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
        return 1;
    }
    else if (evt->type == SDL_MOUSEMOTION) {
        /* mouse motion */
        if (ctx->input.mouse.grabbed) {
            int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
            nk_input_motion(ctx, x + evt->motion.xrel, y + evt->motion.yrel);
        }
        else nk_input_motion(ctx, evt->motion.x, evt->motion.y);
        return 1;
    }
    else if (evt->type == SDL_TEXTINPUT) {
        /* text input */
        nk_glyph glyph;
        memcpy(glyph, evt->text.text, NK_UTF_SIZE);
        nk_input_glyph(ctx, glyph);
        return 1;
    }
    else if (evt->type == SDL_MOUSEWHEEL) {
        /* mouse wheel */
        nk_input_scroll(ctx, nk_vec2((float)evt->wheel.x, (float)evt->wheel.y));
        return 1;
    }
    return 0;
}


static int sdl_button_to_nk(int button)
{
    switch (button)
    {
    default:
        //ft
    case SDL_BUTTON_LEFT:
        return NK_BUTTON_LEFT;
        break;
    case SDL_BUTTON_MIDDLE:
        return NK_BUTTON_MIDDLE;
        break;
    case SDL_BUTTON_RIGHT:
        return NK_BUTTON_RIGHT;
        break;

    }
}


static void grid_demo(struct nk_context* ctx)
{
    static char text[3][64];
    static int text_len[3];
    static const char* items[] = { "Item 0","item 1","item 2" };
    static int selected_item = 0;
    static int check = 1;

    int i;
    if (nk_begin(ctx, "Grid Demo", nk_rect(600, 350, 275, 250),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE |
        NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_dynamic(ctx, 30, 2);
        nk_label(ctx, "Floating point:", NK_TEXT_RIGHT);
        nk_edit_string(ctx, NK_EDIT_FIELD, text[0], &text_len[0], 64, nk_filter_float);
        nk_label(ctx, "Hexadecimal:", NK_TEXT_RIGHT);
        nk_edit_string(ctx, NK_EDIT_FIELD, text[1], &text_len[1], 64, nk_filter_hex);
        nk_label(ctx, "Binary:", NK_TEXT_RIGHT);
        nk_edit_string(ctx, NK_EDIT_FIELD, text[2], &text_len[2], 64, nk_filter_binary);
        nk_label(ctx, "Checkbox:", NK_TEXT_RIGHT);
        nk_checkbox_label(ctx, "Check me", &check);
        nk_label(ctx, "Combobox:", NK_TEXT_RIGHT);
        if (nk_combo_begin_label(ctx, items[selected_item], nk_vec2(nk_widget_width(ctx), 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            for (i = 0; i < 3; ++i)
                if (nk_combo_item_label(ctx, items[i], NK_TEXT_LEFT))
                    selected_item = i;
            nk_combo_end(ctx);
        }
    }
    nk_end(ctx);
}



int main(int argc, char** argv)
{
    struct nk_color clear = { 0,100,0,255 };
    struct nk_vec2 vec;
    struct nk_rect bounds = { 40,40,0,0 };

    SDL_Init(SDL_INIT_VIDEO);
    printf("sdl init called...\n");

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_DisplayMode dm;

    SDL_GetDesktopDisplayMode(0, &dm);

    printf("desktop display mode %d %d\n", dm.w, dm.h);


    SDL_Window* window = SDL_CreateWindow("Puzzle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dm.w - 200, dm.h - 200, SDL_WINDOW_OPENGL);
    if (!window)
    {
        printf("can't open window!\n");
        exit(1);
    }


    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, dm.w - 200, dm.h - 200, 32, SDL_PIXELFORMAT_ARGB8888);


    struct sdlsurface_context* context = nk_sdlsurface_init(surface, 13.0f);
    struct nk_context* ctx = &context->ctx;

    while (1)
    {
        nk_input_begin(ctx);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            nk_sdl_handle_event(&(context->ctx), &event);
        }
        nk_input_end(&(context->ctx));

        bounds.w = 400;
        bounds.h = 400;
        if (nk_begin(&(context->ctx), "Test", bounds, NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE))
        {
            enum { EASY, HARD };
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button")) {
                printf("button pressed\n");
            }
            nk_layout_row_dynamic(ctx, 40, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 45, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            // My test wigets
            nk_layout_row_dynamic(&(context->ctx), 20, 2);
            static char label[] = "Words";
            nk_text_wrap(ctx, label, strlen(label));
            nk_text_wrap(ctx, label, strlen(label));

            static char input[20];
            static int input_len;
            nk_label(ctx, "Title:", NK_TEXT_RIGHT);
            nk_edit_string(ctx, NK_EDIT_FIELD, &input, &input_len, 20, NULL);
        }
        nk_end(&(context->ctx));

        grid_demo(&(context->ctx));

        nk_sdlsurface_render(context, clear, 1);




        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, tex, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(tex);

    }

    nk_sdlsurface_shutdown(context);

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
