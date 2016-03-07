#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// global variables
#define SIZE 9
#define COLUMNS 3
#define ROWS 3
#define THEKEY -42
int symbols[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

// prototypes
void initialization();
void swap(int* a, int* b);
bool check(void);
void shuffle(int *array, size_t n);

int main(void)
{
    initialization();
    int selection = 0;
    char direction;
    while(1)
    {
        for (int i = 0; i < SIZE; i++)
        {
            if (i != 0 && i % COLUMNS == 0)
            {
                printf("\n");
            }
            if (selection == i)
            {
                if (symbols[i] != THEKEY)
                {
                    printf("|%d|\t", symbols[i]);
                }
                else
                {
                    printf("| |\t");
                }
            }
            else
            {
                if (symbols[i] != THEKEY)
                {
                    printf(" %d\t", symbols[i]);
                }
                else
                {
                    printf(" \t");
                }
            }
        }
        if (check() == 1)
        {
            printf("\nYou have completed the game.\n");
            break;
        }
        printf("\nr for right; l for left; u for up; d for down; o to move the piece\n");
tryagain:
        scanf(" %c", &direction);
        if (direction != 'r' && direction != 'l' && direction != 'u' && direction != 'd' && direction != 'o')
        {
            printf("Try again\n");
            goto tryagain;
        }
        printf("I am a character: %c\n", direction);
        if (direction == 'r')
        {
            if ((selection % (COLUMNS)) == (COLUMNS - 1))
            {
                selection -= (COLUMNS - 1);
            }
            else
            {
                selection++;
            }
        }
        else if (direction == 'l')
        {
            if ((selection % (COLUMNS)) == 0)
            {
                selection += (COLUMNS - 1);
            }
            else
            {
                selection--;
            }
        }
        else if (direction == 'd')
        {
            if (selection >= (COLUMNS * (ROWS - 1)))
            {
                selection -= COLUMNS * (ROWS - 1);
            }
            else
            {
                selection += COLUMNS;
            }
        }
        else if (direction == 'u')
        {
            if (selection - COLUMNS < 0)
            {
                selection += (COLUMNS * (ROWS - 1));
            }
            else
            {
                selection -= COLUMNS;
            }
        }
        else if (direction == 'o')
        {
            // if it is not the leftmost selected and has thekey on its' left
            if (selection % COLUMNS != 0 && symbols[selection - 1] == THEKEY)
            {
                swap(&symbols[selection], &symbols[selection - 1]);
            }
            // if it is not the rightmost selected and has thekey on its' right 
            if ((selection % (COLUMNS)) != (COLUMNS - 1) && symbols[selection + 1] == THEKEY)
            {
                swap(&symbols[selection], &symbols[selection + 1]);
            }
            // if it is not the downmost selected and has thekey on its' down
            if (selection < (COLUMNS * (ROWS - 1)) && symbols[selection + COLUMNS] == THEKEY)
            {
                swap(&symbols[selection], &symbols[selection + COLUMNS]);
            }
            // if it is not the uppermost selected and has thekey on its' up
            if (selection - COLUMNS >= 0 && symbols[selection - COLUMNS] == THEKEY)
            {
                swap(&symbols[selection], &symbols[selection - COLUMNS]);
            }
        }
    }
}

// initialize the game for the first time usage
void initialization()
{
    srand(time(NULL));
    shuffle(symbols, SIZE);
    int todelete = rand() % 9;
    symbols[todelete] = THEKEY;
}

// swap the two symbols
void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// if the game if completed, returns 1
bool check(void)
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
// shuffles the given array (our symbols)
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}
