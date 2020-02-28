#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#define DEBUG       0
#define DELAY       1
#define MEMORY_SIZE 16

///////////////////////////////////////////////////////////
// VIRTUAL COMPUTER CODE //////////////////////////////////
///////////////////////////////////////////////////////////

// "Physical" parts
char MEM[MEMORY_SIZE];
char ACC; // use low 4 bits only
char PC; // should only go to f in hex
char IR; // high 4 bits is instruction, low 4 bits is value

// Logical parts
int EXIT = 0;

void init_computer()
{
    if (DEBUG) printf("Loading memory.\n");
    // MEMORY
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        // MEM[i] = 0x0;
        if (DEBUG) printf("\t%p\n", &MEM[i]);
    }
    // REGISTERS
    ACC = 0x0; 
    if (DEBUG) printf("\t%p\n", &ACC);
    // PROGRAM COUNTER
    PC = 0x0;
    if (DEBUG) printf("\t%p\n", &PC);
    // INSTRUCTION REGISTER
    IR = 0x0;
    if (DEBUG) printf("\t%p\n", &IR);

    // All done !
    if (DEBUG) printf("Memory initialized.\n");
}

void load_program_to_memory()
{
    // TODO: load memory from file.
    MEM[0]  = 0b00001111; // NOP 15
    MEM[1]  = 0b00000001; // NOP 1
    MEM[2]  = 0b00000000; // NOP 0
    MEM[3]  = 0b01010000; // OUT
    MEM[4]  = 0b01000010; // STO 2
    MEM[5]  = 0b00100001; // ADD 1
    MEM[6]  = 0b10000000; // CMP 0
    MEM[7]  = 0b01100011; // JMP 3
    MEM[8]  = 0b01000010; // STO 2
    MEM[9]  = 0b01110000; // HLT
    MEM[10] = 0b00000000;
    MEM[11] = 0b11111111; // -
    MEM[12] = 0b00000000; //
    MEM[13] = 0b10011010; // L
    MEM[14] = 0b10011010; // O
    MEM[15] = 0b11011011; // L
}

void fetch_decode_execute()
{
    if (DEBUG) printf("Fetch at %X\n", PC);
    IR = MEM[PC];

    if (DEBUG) printf("Decode at %X\n", PC);
    char op_code = (0b11110000 & IR) >> 4;
    char mod = 0b00001111 & IR;

    if (DEBUG) printf("\top_code : 0x%X\n\tmodifier : 0x%X\n", op_code, mod);

    if (DEBUG) printf("Execute at %X\n", PC);
    switch (op_code)
    {
        case 0: // 0000 NOP
            PC++;
            break;
        case 1: // 0001 LDA
            ACC = MEM[mod];
            PC++;
            break;
        case 2: // 0010 ADD
            ACC = 0b1111 & (ACC + MEM[mod]);
            PC++;
            break;
        case 3: // 0011 SUB
            ACC = 0b1111 & (ACC - MEM[mod]);
            PC++;
            break;
        case 4: // 0100 STO
            MEM[mod] = ACC;
            PC++;
            break;
        case 5: // 0101 OUT
            printf("[OUT] 0x%X\n", ACC);
            PC++;
            break;
        case 6: // 0110 JMP
            PC = mod;
            break;
        case 7: // 0111 HLT
            EXIT = 1;
            break;
        case 8: // 1000 CMP
            PC += (ACC == MEM[mod]);
            PC++;
            break;
        default: // UNKNOWN OP, please advance program counter.
            PC++;
            break;
    }
    if (DEBUG) printf("\n");
}

void boot_computer()
{
    init_computer();
    load_program_to_memory();
}

///////////////////////////////////////////////////////////
// UTILS //////////////////////////////////////////////////
///////////////////////////////////////////////////////////

int get_bit_at(int number, int index)
{
    return (number & (1 << index) ) >> index;
}

///////////////////////////////////////////////////////////
// DISPLAY STUFF //////////////////////////////////////////
///////////////////////////////////////////////////////////

// Window Stuff
const char* WINDOW_TITLE = "vmachine";
const int WINDOW_WIDTH = 608;
const int WINDOW_HEIGHT = 512;
const Uint32 WINDOW_FLAGS = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

typedef struct {
    int r;
    int g;
    int b;
    int a;
} Color;

Color COLOR_BLACK           = { .r = 0,   .g = 0,   .b = 0,   .a = 255, };
Color COLOR_WHITE           = { .r = 230, .g = 230, .b = 230, .a = 255, };
Color COLOR_WHITE_HIGHLIGHT = { .r = 230, .g = 230, .b = 255, .a = 255, };    
Color COLOR_BLACK_HIGHLIGHT = { .r = 20,  .g = 20,  .b = 50,  .a = 255, };

void init_sdl_window_renderer(SDL_Window **window, SDL_Renderer **renderer)
{
    if(SDL_Init(SDL_INIT_EVERYTHING))
    {
        printf("[ERROR] Could not initialize SDL.\n");
    }

    *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        WINDOW_FLAGS
    );

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_SOFTWARE);
}

void draw_rect(SDL_Renderer* r, int x, int y, int w, int h, Color c)
{
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    for (int dy = y; dy < y + h; dy++)
    {
        for (int dx = x; dx < x + w; dx++)
        {
            SDL_RenderDrawPoint(r, dx, dy);
        }
    }
}

void draw_program_counter(SDL_Renderer* r, int x, int y, int ppb)
{
    Color color;
    bool bit;

    for (int n = 0; n < 4; n++)
    {
        bit = get_bit_at(PC, n);
        if (bit) color = COLOR_WHITE;
        else     color = COLOR_BLACK;
        draw_rect(
            r,
            x + (4 * ppb) - ppb - (n * ppb),
            y,
            ppb,
            ppb,
            color
        );
    }
}

void draw_accumulator(SDL_Renderer* r, int x, int y, int ppb)
{
    Color color;
    bool bit;

    for (int n = 0; n < 4; n++)
    {
        bit = get_bit_at(ACC, n);
        if (bit) color = COLOR_WHITE;
        else     color = COLOR_BLACK;
        draw_rect(
            r,
            x + (4 * ppb) - ppb - (n * ppb),
            y,
            ppb,
            ppb,
            color
        );
    }
}

void draw_instruction_register(SDL_Renderer* r, int x, int y, int ppb)
{
    Color color;
    bool bit;

    for (int n = 0; n < 8; n++)
    {
        bit = get_bit_at(IR, n);
        if (bit) color = COLOR_WHITE;
        else     color = COLOR_BLACK;
        draw_rect(
            r,
            x + (8 * ppb) - ppb - (n * ppb),
            y,
            ppb,
            ppb,
            color
        );
    }
}

void draw_memory(SDL_Renderer* r, int x, int y, int ppb)
{
    bool active;
    bool bit;

    Color color;

    for (int mem = 0; mem < MEMORY_SIZE; mem++)
    {
        for (int n = 0; n < 8; n++)
        {
            active = (mem == PC);
            bit = get_bit_at(MEM[mem], n);
            if      ( bit &&  active) color = COLOR_WHITE_HIGHLIGHT;
            else if ( bit && !active) color = COLOR_WHITE;
            else if (!bit &&  active) color = COLOR_BLACK_HIGHLIGHT;
            else                      color = COLOR_BLACK;
            draw_rect(
                r,                               // renderer
                x + (8 * ppb) - ppb - (n * ppb), // x
                y + mem * ppb,                   // y
                ppb,                             // width
                ppb,                             // height
                color                            // color
            );
        }
    }    
}

void draw_everything(SDL_Renderer* r)
{
    SDL_SetRenderDrawColor(r, 55, 55, 55, 255);
    SDL_RenderClear(r);

    draw_program_counter(r, 96, 32, 32);
    draw_accumulator(r, 96, 128, 32);
    draw_instruction_register(r, 32, 224, 32);
    draw_memory(r, 320, 0, 32);

    SDL_RenderPresent(r);
}

void wait_for_input_loop()
{
    SDL_Event event;

    // flush all old events
    while (SDL_PollEvent(&event)) {}

    while (true)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                return;
            }
        }
        SDL_Delay(10);
    }
}

///////////////////////////////////////////////////////////
// MAIN ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Event event;

    init_sdl_window_renderer(&window, &renderer);

    boot_computer();    

    while (!EXIT)
    {
        if (DELAY) SDL_Delay(333);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                EXIT = true;
            }
        }

        fetch_decode_execute();

        draw_everything(renderer);
    }

    wait_for_input_loop(); 

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
