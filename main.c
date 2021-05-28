#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "render.h"

///////////////////////////////////////////////////////////
// SIMUALATION PROPERTIES /////////////////////////////////
///////////////////////////////////////////////////////////

#define DEBUG       0
// Delay between each clock cycle in ms.
// A complete clock cycle has 3 steps.
// A delay of 100ms means the clock operate at 10hz
#define DELAY       0
// Clearly show the computer is working with 4 bits
#define MEMORY_SIZE 0b10000

///////////////////////////////////////////////////////////
// VIRTUAL COMPUTER CODE //////////////////////////////////
///////////////////////////////////////////////////////////

// "Physical" parts
static char unsigned MEM[MEMORY_SIZE];
static char unsigned ACC; // use low 4 bits only
static char unsigned PC; // should only go to f in hex
static char unsigned IR; // high 4 bits is instruction, low 4 bits is value

// Logical parts
static int EXIT = 0;

void init_computer()
{
#if DEBUG
    printf("Initializing memory and registers.\n");
#endif
    // MEMORY
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        MEM[i] = 0x0;
#if DEBUG
		printf("\t@%02d -> %02x\n", i, MEM[i]);
#endif
	}
    // REGISTER
    ACC = 0x0; 
    // PROGRAM COUNTER
    PC = 0x0;
    // INSTRUCTION REGISTER
    IR = 0x0;
#if DEBUG
    printf("\t%p\n", &ACC);
    printf("\t%p\n", &IR);
    printf("\t%p\n", &PC);
    printf("Memory initialized.\n");
#endif
}

void load_program_to_memory(const char* program_filepath)
{
	FILE* file = fopen(program_filepath, "r");

	if (!file)
	{
		printf("Could not read file: %s\n", program_filepath);
		printf("Loading default test program...\n");

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
	else
	{
		char c;
		int byte = 0;
		int bit = 0;
#if DEBUG		
		printf("Loading memory from file...\n");
#endif
		while (fscanf(file, "%c", &c) == 1)
		{
			if (c != '0' && c != '1')
				continue;

			// reading bits in file from left to right
			int bitmask = 0b10000000 >> bit;
			if (c == '1')
			{
				MEM[byte] = MEM[byte] | bitmask;
				bit++;
			}
			else if (c == '0')
			{
				bit++;
			}

			if (bit == 8)
			{
				++byte;
				bit = 0;
			}

			if (byte == MEMORY_SIZE && bit > 0)
			{
				printf("Memory overflow.\n");
				exit(EXIT_FAILURE);
			}
		}	
	}
#if DEBUG	
	printf("Memory content:\n");
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		printf("\t@%02d -> %02x\n", i, MEM[i]);
	}
#endif
}

void fetch_decode_execute()
{
#if DEBUG
    printf("Fetch at %X\n", PC);
#endif
	IR = MEM[PC];

#if DEBUG
    if (DEBUG) printf("Decode at %X\n", PC);
#endif
	char op_code = (0b11110000 & IR) >> 4;
    char mod = 0b00001111 & IR;

#if DEBUG
    printf("\top_code : 0x%X\n\tmodifier : 0x%X\n", op_code, mod);
	printf("Execute at %X\n", PC);
#endif
	switch (op_code)
    {
        case 0: // 0000 NOP
            ++PC;
            break;
        case 1: // 0001 LDA
            ACC = MEM[mod];
            ++PC;
            break;
        case 2: // 0010 ADD
            ACC = 0b1111 & (ACC + MEM[mod]);
            ++PC;
            break;
        case 3: // 0011 SUB
            ACC = 0b1111 & (ACC - MEM[mod]);
            ++PC;
            break;
        case 4: // 0100 STO
            MEM[mod] = ACC;
            ++PC;
            break;
        case 5: // 0101 OUT
            printf("[OUT] 0x%X\n", ACC);
            ++PC;
            break;
        case 6: // 0110 JMP
            PC = mod;
            break;
        case 7: // 0111 HLT
            EXIT = 1;
            break;
        case 8: // 1000 CMP
            PC += (ACC == MEM[mod]);
            ++PC;
            break;
        default: // UNKNOWN OP, please advance program counter.
            ++PC;
            break;
    }
#if DEBUG
	printf("\n");
#endif
}

void boot_computer(const char* program_filepath)
{
    init_computer();
    load_program_to_memory(program_filepath);
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

	unsigned int newTime;
	unsigned int oldTime;
	unsigned int frameTime;
	unsigned int tickTime = 0;
	bool tick = false;

    init_sdl_window_renderer(&window, &renderer);

    boot_computer(argv[1]); 

	newTime = SDL_GetTicks();
    while (!EXIT)
    {
		oldTime = newTime;
		newTime = SDL_GetTicks();
		frameTime = newTime - oldTime;
		
		tickTime += frameTime;
		if (tickTime > DELAY) {
			tickTime = 0;
			tick = true;
		}

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                EXIT = true;
            }
        }

		if (tick)
		{
			tick = false;
			fetch_decode_execute();
		}

        draw_everything(renderer);
    }

    wait_for_input_loop(); 

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
