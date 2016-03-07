#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>

#include <wiringPi.h>
#include <lcd.h>

#ifndef TRUE
#  define   TRUE    (1==1)
#  define   FALSE   (1==2)
#endif

// global variables
#define THEKEY -42
int SIZE = 0;
int COLUMNS = 0;
int ROWS = 0;
static int lcdHandle;
static unsigned char uparrow[8] =
{
    0b00000,
    0b00000,
    0b00000,
    0b00100,
    0b01110,
    0b11111,
    0b00000,
    0b00000
};
static unsigned char downarrow[8] =
{
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
};
static unsigned char rightarrow[8] =
{
    0b00000,
    0b00000,
    0b01000,
    0b01100,
    0b01110,
    0b01100,
    0b01000,
    0b00000
};
static unsigned char leftarrow[8] =
{
    0b00000,
    0b00000,
    0b00010,
    0b00110,
    0b01110,
    0b00110,
    0b00010,
    0b00000
};
    
// prototypes
int initialization(int* symbols);
void printgame(const int* symbols);
void slide(const char direction, int* thekeypos, int* symbols);
void swap(int* a, int* b);
bool check(const int* const symbols);
void shuffle(int* array, size_t n);
bool solvable(const int* const symbols, const int thekeypos);
void handler(int signal);
void quit(void);

int main(void)
{
    srand48(time(NULL));
    signal(SIGINT, handler);
    wiringPiSetup();
    lcdHandle = lcdInit(4,20, 4, 11,10, 0,1,2,3,0,0,0,0);
    lcdCharDef(lcdHandle, 0, uparrow);
    lcdCharDef(lcdHandle, 1, downarrow);
    lcdCharDef(lcdHandle, 2, rightarrow);
    lcdCharDef(lcdHandle, 3, leftarrow);
playagain: ;
    lcdDisplay(lcdHandle, TRUE);
    lcdClear(lcdHandle);
    lcdHome(lcdHandle);
    delay(3000);
    lcdPuts(lcdHandle, "*****Game Mode*****");
    lcdPosition(lcdHandle, 0, 1);
    lcdPuts(lcdHandle, "       ");
    lcdPutchar(lcdHandle, 0);
    lcdPuts(lcdHandle, " HARD       ");
    lcdPosition(lcdHandle, 0, 2);
    lcdPuts(lcdHandle, "      ");
    lcdPutchar(lcdHandle, 1);
    lcdPuts(lcdHandle, " NORMAL      ");
    lcdPosition(lcdHandle, 0, 3);
    lcdPuts(lcdHandle, "  ");
    lcdPutchar(lcdHandle, 3);
    lcdPuts(lcdHandle, " EASY    ");
    lcdPutchar(lcdHandle, 2);
    lcdPuts(lcdHandle, " QUIT  ");
    while (1)
    {
        if (digitalRead(4) == HIGH)
        {
            SIZE = 16;
            COLUMNS = 4;
            ROWS = 4;
            break;
        }
        if (digitalRead(5) == HIGH)
        {
            SIZE = 9;
            COLUMNS = 3;
            ROWS = 3;
            break;
        }
        if (digitalRead(6) == HIGH)
        {
            SIZE = 4;
            COLUMNS = 2;
            ROWS = 2;
            break;
        }
        if (digitalRead(7) == HIGH)
        {
            quit();
        }
    }
playagain2: ;
    int symbols[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        symbols[i] = i + 1;
    }
    int thekeypos = initialization(symbols);

    // check if the puzzle is solvable and solved to begin with
    // if it is not solvable or it is solved go to the beginning
    if (!solvable(symbols, thekeypos) || check(symbols) == 1)
    {
        goto playagain2;
    }

    while(1)
    {
        // print the game
        printgame(symbols);

        // delay for button press
        delay(500);

        // control the direction input
        while (1)
        {
            if (digitalRead(4) == HIGH && digitalRead(5) == HIGH && digitalRead(6) == HIGH && digitalRead(7) == HIGH)
            {
                goto playagain;
            }
            if (digitalRead(4) == HIGH)
            {
                slide('w', &thekeypos, symbols);
                break;
            }
            if (digitalRead(5) == HIGH)
            {
                slide('s', &thekeypos, symbols);
                break;
            }
            if (digitalRead(6) == HIGH)
            {
                slide('a', &thekeypos, symbols);
                break;
            }
            if (digitalRead(7) == HIGH)
            {
                slide('d', &thekeypos, symbols);
                break;
            }
        }

        // check if the game is ended
        if (check(symbols) == 1)
        {
            lcdClear(lcdHandle);
            lcdHome(lcdHandle);
            delay(2500);
            lcdPuts(lcdHandle, "   Congratulations  ");
            lcdPosition(lcdHandle, 0, 1);
            lcdPuts(lcdHandle, " You have completed ");
            lcdPosition(lcdHandle, 0, 2);
            lcdPuts(lcdHandle, "      the game      ");
            lcdPosition(lcdHandle, 0, 3);
            lcdPuts(lcdHandle, "Press to continue");
            while (1)
            {
                if (digitalRead(4) == HIGH || digitalRead(5) == HIGH || digitalRead(6) == HIGH || digitalRead(7) == HIGH)
                {
                    break;
                }
            }
            lcdClear(lcdHandle);
            lcdHome(lcdHandle);
            delay(2500);
            lcdPuts(lcdHandle, "  Would you like to ");
            lcdPosition(lcdHandle, 0, 1);
            lcdPuts(lcdHandle, "     play again?    ");
            lcdPosition(lcdHandle, 0, 3);
            lcdPuts(lcdHandle, " (n) ");
            lcdPutchar(lcdHandle, 1);
            lcdPuts(lcdHandle, "         (y) ");
            lcdPutchar(lcdHandle, 0);
            while (1)
            {
                if (digitalRead(4) == HIGH)
                {
                    goto playagain;
                }
                if (digitalRead(5) == HIGH)
                {
                    quit();
                }
            }
        }
    }
    return 0;
}

// initialize the game for the first time usage and return thekey position
int initialization(int* symbols)
{
    shuffle(symbols, SIZE);
    for (int i = 0; i < SIZE; i++)
    {
        if (symbols[i] == SIZE) 
        {
            symbols[i] = THEKEY;
            return i;
        }
    }
    return -1;
}

// prints the game state
void printgame(const int* symbols)
{
    lcdClear(lcdHandle);
    if (COLUMNS < 4)
    {
        lcdPosition(lcdHandle, 2, 3);
        lcdPutchar(lcdHandle, 0);
        lcdPosition(lcdHandle, 7, 3);
        lcdPutchar(lcdHandle, 1);
        lcdPosition(lcdHandle, 12, 3);
        lcdPutchar(lcdHandle, 2);
        lcdPosition(lcdHandle, 17, 3);
        lcdPutchar(lcdHandle, 3);
    }
    lcdHome(lcdHandle);
    int counter = 0, pos, increment;
    if (COLUMNS == 2) {pos = 4; increment = 11;}
    if (COLUMNS == 3) {pos = 3; increment = 6;}
    if (COLUMNS == 4) {pos = 2; increment = 5;}
    for (int i = 0; i < SIZE; i++)
    {
        if (i != 0 && i % COLUMNS == 0)
        {
            counter++;
            if (COLUMNS == 2) {pos = 4;}
            if (COLUMNS == 3) {pos = 3;}
            if (COLUMNS == 4) {pos = 2;}
            lcdPosition(lcdHandle, pos, counter);
        }
        if (symbols[i] != THEKEY)
        {
            lcdPosition(lcdHandle, pos, counter);
            lcdPrintf(lcdHandle, "%d", symbols[i]);
            pos += increment;
        }
        else
        {
            lcdPosition(lcdHandle, pos, counter);
            lcdPuts(lcdHandle, " ");
            pos += increment;
        }
    }
}

// swipes the empty slot and the direction
void slide(const char direction, int* thekeypos, int* symbols)
{
    // if it is not the leftmost selected and direction right is selected
    if ((*thekeypos % COLUMNS) != 0 && direction == 'd')
    {
        swap(&symbols[*thekeypos], &symbols[*thekeypos - 1]);
        *thekeypos = *thekeypos - 1;
        return;
    }
    // if it is not the rightmost selected and direction left is selected
    if ((*thekeypos % COLUMNS) != (COLUMNS - 1) && direction == 'a')
    {
        swap(&symbols[*thekeypos], &symbols[*thekeypos + 1]);
        *thekeypos = *thekeypos + 1;
        return;
    }
    // if it is not the downmost selected and direction up is selected
    if (*thekeypos < (COLUMNS * (ROWS - 1)) && direction == 'w')
    {
        swap(&symbols[*thekeypos], &symbols[*thekeypos + COLUMNS]);
        *thekeypos = *thekeypos + COLUMNS;
        return;
    }
    // if it is not the uppermost selected and direction down is selected 
    if ((*thekeypos - COLUMNS) >= 0 && direction == 's')
    {
        swap(&symbols[*thekeypos], &symbols[*thekeypos - COLUMNS]);
        *thekeypos = *thekeypos - COLUMNS;
        return;
    }
}

// swap the two symbols
void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// if the game is completed, returns 1
bool check(const int* const symbols)
{
    for (int i = 0; i < SIZE; i++)
    {
        if (symbols[i] != i + 1 && symbols[i] != THEKEY)
        {
            return 0;
        }
    }
    return 1;
}

// http://benpfaff.org/writings/clc/shuffle.html
// http://c-faq.com/lib/randrange.html
/* Arrange the N elements of ARRAY in random order.
 * Only effective if N is much smaller than RAND_MAX;
 * if this may not be the case, use a better random
 * number generator. */
// shuffles the given array (our symbols)
void shuffle(int* array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + (int)(drand48() * (n - i));
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

// http://people.math.sfu.ca/~jtmulhol/math302/notes/9-15-puzzle
// https://www.cs.bham.ac.uk/~mdr/teaching/modules04/java2/TilesSolvability.html
// returns 1 if the puzzle is solvable
bool solvable(const int* const symbols, const int thekeypos)
{
    int counter = 0;
    for (int i = 0; i < SIZE - 1; i++)
    {
        for (int j = i + 1; j < SIZE; j++)
        {
            if (symbols[i] != THEKEY && symbols[j] != THEKEY && symbols[i] > symbols[j])
            {
                counter++;
            }
        }
    }
    // If the width is odd, then every solvable state has an even number of inversions
    if ((COLUMNS % 2) && !(counter % 2))
    {
        return 1;
    }
    // If the width is even, then every solvable state has an even number of inversions if the blank is on an odd numbered row counting from the bottom
    else if (!(COLUMNS % 2) && ((((thekeypos / COLUMNS) % 2) + ROWS) % 2) && !(counter % 2))
    {
        return 1;
    }
    // If the width is even, then every solvable state has an odd number of inversions if the blank is on an even numbered row counting from the bottom
    else if (!(COLUMNS % 2) && !((((thekeypos / COLUMNS) % 2) + ROWS) % 2) && (counter % 2))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Handler
void handler(int signal)
{
    if (signal == SIGINT)
    {
        quit();
    }
}

// Clears the screen and quits the program
void quit(void)
{
    lcdClear(lcdHandle);
    lcdPosition(lcdHandle, 0, 1);
    lcdPuts(lcdHandle, "     BYE BYE...     ");
    lcdPosition(lcdHandle, 9, 2);
    lcdPuts(lcdHandle, "5");
    delay(1000);
    lcdPosition(lcdHandle, 9, 2);
    lcdPuts(lcdHandle, "4");
    delay(1000);
    lcdPosition(lcdHandle, 9, 2);
    lcdPuts(lcdHandle, "3");
    delay(1000);
    lcdPosition(lcdHandle, 9, 2);
    lcdPuts(lcdHandle, "2");
    delay(1000);
    lcdPosition(lcdHandle, 9, 2);
    lcdPuts(lcdHandle, "1");
    delay(1000);
    lcdPosition(lcdHandle, 9, 2);
    lcdPuts(lcdHandle, "0");
    delay(100);
    lcdClear(lcdHandle);
    lcdDisplay(lcdHandle, FALSE);
    exit(0);
}
