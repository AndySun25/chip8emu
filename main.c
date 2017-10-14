#include <curses.h>
#include <pthread.h>
#include <stdlib.h>
#include "chip8.h"


#define PERIOD_60   (1.0 / 60)
#define PERIOD_500  (1.0 / 500)


void setUp(void)
{
    initscr();
    refresh();
    curs_set(0);
    noecho();
    halfdelay(1);

    // Draw frame
    for (int y=0; y<GFX_H + 2; ++y)
    {
        for (int x=0; x<GFX_W + 2; ++x)
        {
            move(y, x);
            if (y == 0 && x == 0)
                insch(ACS_ULCORNER);
            else if (y == GFX_H + 1 && x == 0)
                insch(ACS_LLCORNER);
            else if (y == 0 && x == GFX_W + 1)
                insch(ACS_URCORNER);
            else if (y == GFX_H + 1 && x == GFX_W + 1)
                insch(ACS_LRCORNER);
            else if (y == 0 || y == GFX_H + 1)
                insch(ACS_HLINE);
            else if (x == 0 || x == GFX_W + 1)
                insch(ACS_VLINE);
        }
    }
}


void tearDown(void)
{
    nocbreak();
    echo();
    endwin();
}


void render(void)
{
    for (int y=0; y<GFX_H; ++y)
    {
        for (int x=0; x<GFX_W; ++x)
        {
            move(y + 1, x + 1);
            delch();
            if (gfx[(y * GFX_W) + x] == 0)
                insch(' ');
            else
                insch(ACS_CKBOARD);
        }
    }
    refresh();
}


int input;
bool quit = false;


void * checkInput(void)
{
    while (!quit)
    {
        if ((input = getch()) != ERR)
        {
            if (input == 27)
                quit = true;
            else if (input == '0')
                key[0x0] = 1;
            else if (input == '1')
                key[0x1] = 1;
            else if (input == '2')
                key[0x2] = 1;
            else if (input == '3')
                key[0x3] = 1;
            else if (input == '4')
                key[0x4] = 1;
            else if (input == '5')
                key[0x5] = 1;
            else if (input == '6')
                key[0x6] = 1;
            else if (input == '7')
                key[0x7] = 1;
            else if (input == '8')
                key[0x8] = 1;
            else if (input == '9')
                key[0x9] = 1;
            else if (input == 'a')
                key[0xA] = 1;
            else if (input == 'b')
                key[0xB] = 1;
            else if (input == 'c')
                key[0xC] = 1;
            else if (input == 'd')
                key[0xD] = 1;
            else if (input == 'e')
                key[0xE] = 1;
            else if (input == 'f')
                key[0xF] = 1;
            //printf("%i", input);
            //fflush(stdout);
        }
        else
            resetKeys();
    }
}


void * updateTimers(void)
{
    struct timespec tspec, _;
    clock_t loop_start;
    double duration;

    while (!quit)
    {
        loop_start = clock();

        if(delay_timer > 0)
            --delay_timer;
        if(sound_timer > 0)
            --sound_timer;

        duration = (double) (clock() - loop_start) / CLOCKS_PER_SEC;

        if (duration < PERIOD_60)
        {
            tspec.tv_nsec = (long) ((PERIOD_60 - duration) * 1000000000);
            nanosleep(&tspec, &_);
        }
    }
}


int main(int argc, char **argv)
{
    struct timespec tspec, _;

    if(argc < 2)
    {
        printf("Usage: chip8emu chip8application\n\n");
        return 1;
    }

    if (!load(argv[1]))
    {
        printf("Failed to load ROM!");
        return 1;
    }

    setUp();
    clock_t loop_start;
    double duration;

    pthread_t t_input, t_timers;
    pthread_create(&t_input, NULL, checkInput, NULL);
    pthread_create(&t_timers, NULL, updateTimers, NULL);

    while(!quit)
    {
        loop_start = clock();
        cycle();
        if (draw_flag)
        {
            render();
            draw_flag = false;
        }
        duration = (double) (clock() - loop_start) / CLOCKS_PER_SEC;

        if (duration < PERIOD_500)
        {
            tspec.tv_nsec = (long) ((PERIOD_500 - duration) * 1000000000);
            nanosleep(&tspec, &_);
        }
    }

    tearDown();
    return 0;
}