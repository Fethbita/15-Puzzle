#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// global variables
#define SIZE 9
#define COLUMNS 3
#define ROWS 3
#define THEKEY -42

// prototypes
int initialization(int* const symbols);
void printgame(const int *symbols, const int selection);
void moveSelection(int *selection, const char direction);
void slide(const int selection, int *symbols);
void swap(int *a, int *b);
bool check(int *symbols);
void shuffle(int *array, size_t n);
bool solvable(int *symbols, const int thekeypos);

int main(void)
{
    srand48(time(NULL));
playagain: ;
	int symbols[SIZE];
	for (int i = 0; i < SIZE; i++)
	{
		symbols[i] = i + 1;
	}
    int thekeypos = initialization(symbols);

    // check if the puzzle is solvable
    // if it not go to the beginning
    if (!solvable(symbols, thekeypos))
    {
        goto playagain;
    }

    int selection = 0;
    char direction = '?';

    while(1)
    {
		printgame(symbols, selection);
        if (check(symbols) == 1)
        {
            printf("\nYou have completed the game.\n");
			do {
                direction = '?';
				printf("Would you like to play again? (y/n)\n");
				scanf(" %c", &direction);
				if (direction == 'y')
				{
					goto playagain;
				}
				else if (direction == 'n')
				{
					return 0;
				}
			} while (direction != 'y' && direction != 'n');
        }

		// Print the control mechanism
        printf("\nd for right; a for left; w for up; s for down; f to move the piece\n");

		// control the direction input
        scanf(" %c", &direction);
		while (direction != 'd' && direction != 'a' && direction != 'w' && direction != 's' && direction != 'f')
        {
            printf("Wrong input\n");
            scanf(" %c", &direction);
		} 
        printf("I am a character: %c\n", direction);

        if (direction == 'd' || direction == 'a' || direction == 'w' || direction == 's')
        {
            moveSelection(&selection, direction);
        }
        else if (direction == 'f')
        {
            slide(selection, symbols);
        }
    }
	return 0;
}

// initialize the game for the first time usage and return thekey position
int initialization(int* const symbols)
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
void printgame(const int *symbols, const int selection)
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
}

// moves the selection cursor
void moveSelection(int *selection, const char direction)
{
	if (direction == 'd')
	{
		if ((*selection % (COLUMNS)) == (COLUMNS - 1))
		{
			*selection = *selection - (COLUMNS - 1);
		}
		else
		{
			*selection = *selection + 1;
		}
        return;
	}
	else if (direction == 'a')
	{
		if ((*selection % (COLUMNS)) == 0)
		{
			*selection = *selection + (COLUMNS - 1);
		}
		else
		{
			*selection = *selection - 1;
		}
        return;
	}
	else if (direction == 's')
	{
		if (*selection >= (COLUMNS * (ROWS - 1)))
		{
			*selection = *selection - (COLUMNS * (ROWS - 1));
		}
		else
		{
			*selection = *selection + COLUMNS;
		}
        return;
	}
	else if (direction == 'w')
	{
		if (*selection - COLUMNS < 0)
		{
			*selection = *selection + (COLUMNS * (ROWS - 1));
		}
		else
		{
			*selection = *selection - COLUMNS;
		}
        return;
	}
}

// swipes the selection and the empty slot
void slide(const int selection, int *symbols)
{
    // if it is not the leftmost selected and has thekey on its' left
    if (selection % COLUMNS != 0 && symbols[selection - 1] == THEKEY)
    {
        swap(&symbols[selection], &symbols[selection - 1]);
        return;
    }
    // if it is not the rightmost selected and has thekey on its' right 
    if ((selection % (COLUMNS)) != (COLUMNS - 1) && symbols[selection + 1] == THEKEY)
    {
        swap(&symbols[selection], &symbols[selection + 1]);
        return;
    }
    // if it is not the downmost selected and has thekey on its' down
    if (selection < (COLUMNS * (ROWS - 1)) && symbols[selection + COLUMNS] == THEKEY)
    {
        swap(&symbols[selection], &symbols[selection + COLUMNS]);
        return;
    }
    // if it is not the uppermost selected and has thekey on its' up
    if (selection - COLUMNS >= 0 && symbols[selection - COLUMNS] == THEKEY)
    {
        swap(&symbols[selection], &symbols[selection - COLUMNS]);
        return;
    }
}

// swap the two symbols
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// if the game is completed, returns 1
bool check(int *symbols)
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
void shuffle(int *array, size_t n)
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
bool solvable(int *symbols, const int thekeypos)
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
