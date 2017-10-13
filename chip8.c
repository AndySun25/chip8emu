#include <stdlib.h>
#include "chip8.h"


unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0x30, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
};


void resetKeys(void)
{
    for (int j=0; j<16; ++j)
    {
        key[j] = 0;
    }
}


void drawSprite(unsigned short x_pos, unsigned short y_pos, unsigned short h)
{
    unsigned short gfxi;

    V[0xF] = 0;
    for (int y=0; y<h; ++y)
    {
        gfxi = memory[I + y];
        for(int x=0; x<8; ++x)
        {
            if((gfxi & (0x80 >> x)) != 0)
            {
                if(gfx[(x_pos + x + ((y_pos + y) * 64))] == 1)
                {
                    V[0xF] = 1;
                }
                gfx[x_pos + x + ((y_pos + y) * 64)] ^= 1;
            }
        }
    }
}


void clearScreen(void)
{
    for (int j=0; j<GFX_W * GFX_H; ++j)
        gfx[j] = 0;

    draw_flag = true;
}


void cycle(void)
{
    opcode = memory[pc] << 8 | memory[pc + 1];

    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (opcode & 0x000F)
            {
                case 0x0000:
                    clearScreen();
                    pc += 2;
                break;
                case 0x000E:
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                break;
                default:
                    printf("Invalid opcode %x detected, exiting.", opcode);
                    exit(1);

            }
        break;
        case 0x1000:
            pc = opcode & (unsigned short) 0x0FFF;
        break;
        case 0x2000:
            stack[sp] = pc;
            ++sp;
            pc = opcode & (unsigned short) 0x0FFF;
        break;
        case 0x3000:
            if (V[getNibble(opcode, 2)] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
        break;
        case 0x4000:
            if (V[getNibble(opcode, 2)] != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
        break;
        case 0x5000:
            if (V[getNibble(opcode, 2)] == V[getNibble(opcode, 1)])
                pc += 4;
            else
                pc += 2;
        break;
        case 0x6000:
            V[getNibble(opcode, 2)] = (unsigned char) (opcode & 0xFF);
            pc += 2;
        break;
        case 0x7000:
            V[getNibble(opcode, 2)] += opcode & 0xFF;
            pc += 2;
        break;
        case 0x8000:
            switch (opcode & 0x000F)
            {
                case 0x0:
                    V[getNibble(opcode, 2)] = V[getNibble(opcode, 1)];
                    pc += 2;
                break;
                case 0x1:
                    V[getNibble(opcode, 2)] |= V[getNibble(opcode, 1)];
                    pc += 2;
                break;
                case 0x2:
                    V[getNibble(opcode, 2)] &= V[getNibble(opcode, 1)];
                    pc += 2;
                break;
                case 0x3:
                    V[getNibble(opcode, 2)] ^= V[getNibble(opcode, 1)];
                    pc += 2;
                break;
                case 0x4:
                {
                    unsigned short res = V[getNibble(opcode, 2)] + V[getNibble(opcode, 1)];
                    V[getNibble(opcode, 2)] = (unsigned char) res;
                    if (res > 0xFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    pc += 2;
                }
                break;
                case 0x5:
                {
                    if (V[getNibble(opcode, 1)] > V[getNibble(opcode, 2)])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[getNibble(opcode, 2)] -= V[getNibble(opcode, 1)];
                    pc += 2;
                }
                break;
                case 0x6:
                    V[0xF] = V[getNibble(opcode, 2)] & (unsigned char) 0x1;
                    V[getNibble(opcode, 2)] >>= 1;
                    pc += 2;
                break;
                case 0x7:
                {
                    if (V[getNibble(opcode, 1)] > V[getNibble(opcode, 2)])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[getNibble(opcode, 2)] = V[getNibble(opcode, 1)] - V[getNibble(opcode, 2)];
                    pc += 2;
                }
                break;
                case 0xE:
                    V[0xF] = V[getNibble(opcode, 1)] >> 7;
                    V[getNibble(opcode, 2)] <<= 1;
                    pc += 2;
                break;
                default:
                    printf("Invalid opcode %x detected, exiting.", opcode);
                    exit(1);
            }
        break;
        case 0x9000:
            if (V[getNibble(opcode, 2)] != V[getNibble(opcode, 1)])
                pc += 4;
            else
                pc += 2;
        break;
        case 0xA000:
            I = opcode & (unsigned short) 0x0FFF;
            pc += 2;
        break;
        case 0xB000:
            pc = V[0] + (opcode & (unsigned short) 0x0FFF);
        break;
        case 0xC000:
            V[getNibble(opcode, 2)] = (unsigned char) ((opcode & 0x00FF) & (rand() % 0xFF));
            pc += 2;
        break;
        case 0xD000:
        {
            unsigned char x_pos = V[getNibble(opcode, 2)];
            unsigned char y_pos = V[getNibble(opcode, 1)];
            unsigned char h = (unsigned char) getNibble(opcode, 0);
            drawSprite(x_pos, y_pos, h);
            draw_flag = true;
            pc += 2;
        }
        break;
        case 0xE000:
            switch (opcode & 0x00FF)
            {
                case 0x009E:
                    if (key[V[getNibble(opcode, 2)]] != 0)
                        pc += 4;
                    else
                        pc += 2;
                break;
                case 0x00A1:
                    if (key[V[getNibble(opcode, 2)]] == 0)
                        pc += 4;
                    else
                        pc += 2;
                break;
                default:
                    printf("Invalid opcode %x detected, exiting.", opcode);
                    exit(1);

            }
        break;
        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x0007:
                    V[getNibble(opcode, 2)] = delay_timer;
                    pc += 2;
                break;
                case 0x000A:
                {
                    bool pressed = false;
                    for (unsigned char i=0; i<16; ++i)
                    {
                        if (key[i] != 0)
                        {
                            V[getNibble(opcode, 2)] = i;
                            pressed = true;
                        }
                    }
                    if (!pressed)
                        return;

                    pc += 2;
                }
                break;
                case 0x0015:
                    delay_timer = V[(unsigned char) getNibble(opcode, 2)];
                    pc += 2;
                break;
                case 0x0018:
                    sound_timer = V[(unsigned char) getNibble(opcode, 2)];
                    pc += 2;
                break;
                case 0x001E:
                    if (I + V[getNibble(opcode, 2)] > 0x0FFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[getNibble(opcode, 2)];
                    pc += 2;
                break;
                case 0x0029:
                    I = (unsigned short) (V[getNibble(opcode, 2)] * 5);
                    pc += 2;
                break;
                case 0x0033:
                {
                    unsigned char val = (unsigned char) getNibble(opcode, 2);
                    memory[I] = (unsigned char) (val / 100);
                    memory[I + 1] = (unsigned char) ((val / 10) % 10);
                    memory[I + 2] = (unsigned char) ((val % 100) % 10);
                    pc += 2;
                }
                break;
                case 0x0055:
                    for (int i=0; i<=getNibble(opcode, 2); ++i)
                        memory[i + I] = V[i];

                    I += getNibble(opcode, 2) + 1;
                    pc += 2;
                break;
                case 0x0065:
                    for (int i=0; i<=getNibble(opcode, 2); ++i)
                        V[i] = memory[i + I];

                    I += getNibble(opcode, 2) + 1;
                    pc += 2;
                break;
                default:
                    printf("Invalid opcode %x detected, exiting.", opcode);
                    exit(1);
            }
        break;
        default:
            printf("Invalid opcode %x detected, exiting.", opcode);
            exit(1);
    }
}


void reset(void)
{
    for (int j=0; j<16; ++j)
    {
        V[j] = 0;
        stack[j] = 0;
        key[j] = 0;
    }

    for (int j=0; j<4096; ++j)
        if (j < 80)
            memory[j] = fontset[j];
        else
            memory[j] = 0;

    sp = 0;
    pc = 0x200;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;

    clearScreen();
}


bool load(const char * filename)
{
    reset();

    // Open file
    FILE * file = fopen(filename, "rb");
    if (file == NULL)
    {
        return false;
    }

    fseek(file , 0 , SEEK_END);
    long l_size = ftell(file);
    rewind(file);

    // Allocate memory to contain the whole file
    char * buffer = (char*)malloc(sizeof(char) * l_size);
    if (buffer == NULL)
    {
        return false;
    }

    // Copy the file into the buffer
    size_t result = fread(buffer, 1, l_size, file);
    if (result != l_size)
    {
        return false;
    }

    // Copy buffer to Chip8 memory
    if((4096-512) > l_size)
    {
        for(int j = 0; j < l_size; ++j)
            memory[j + 512] = (unsigned char) buffer[j];
    }
    else
    {
        return false;
        // printf("Error: ROM too big for memory");
    }

    // Close file, free buffer
    fclose(file);
    free(buffer);

    return true;
}
