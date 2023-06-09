/*
   MIT License

   Copyright (c) 2023 Fethbita

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// global variables
#define THEKEY -42
int SIZE = 0;
int COLUMNS = 0;
int ROWS = 0;
char STRINGBUFFER[6];

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

// prototypes
void setup();
void loop();
void restart();
void congratulations();
int initialization(int* symbols);
void printgame(const int* symbols);
void slide(const char direction, int* thekeypos, int* symbols);
void swap(int* a, int* b);
bool check(const int* const symbols);
void shuffle(int* array, size_t n);
bool solvable(const int* const symbols, const int thekeypos);
void quit(void);

void setup() {
	// initialize LCD and set up the number of columns and rows
	lcd.begin(20, 4);

	// create characters
	lcd.createChar(0, uparrow);
	lcd.createChar(1, downarrow);
	lcd.createChar(2, leftarrow);
	lcd.createChar(3, rightarrow);

	restart();

	Serial.begin(9600);
	pinMode(6, INPUT); // Button up
	pinMode(5, INPUT); // Button down
	pinMode(4, INPUT); // Button left
	pinMode(3, INPUT); // Button right

	randomSeed(analogRead(0));
}

void loop()
{
	if (SIZE != 0) {
		restart();
		delay(500);
	}
	while (1)
	{
		if (digitalRead(6) == HIGH)
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
		if (digitalRead(4) == HIGH)
		{
			SIZE = 4;
			COLUMNS = 2;
			ROWS = 2;
			break;
		}
		if (digitalRead(3) == HIGH)
		{
			quit();
		}
	}

	int symbols[SIZE];
	for (int i = 0; i < SIZE; i++)
	{
		symbols[i] = i + 1;
	}
	int thekeypos = initialization(symbols);

	// check if the puzzle is solvable and solved to begin with
	// if it is not solvable or it is solved start over
	if (!solvable(symbols, thekeypos) || check(symbols) == 1)
	{
		return;
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
			if (digitalRead(6) == HIGH && digitalRead(5) == HIGH && digitalRead(4) == HIGH && digitalRead(3) == HIGH)
			{
				return;
			}
			if (digitalRead(6) == HIGH)
			{
				slide('w', &thekeypos, symbols);
				break;
			}
			if (digitalRead(5) == HIGH)
			{
				slide('s', &thekeypos, symbols);
				break;
			}
			if (digitalRead(4) == HIGH)
			{
				slide('a', &thekeypos, symbols);
				break;
			}
			if (digitalRead(3) == HIGH)
			{
				slide('d', &thekeypos, symbols);
				break;
			}
		}

		// check if the game is ended
		if (check(symbols) == 1)
		{
			printgame(symbols);
			lcd.home();
			lcd.print("*");
			lcd.setCursor(19, 0);
			lcd.print("*");
			lcd.setCursor(0, 3);
			lcd.print("*");
			lcd.setCursor(19, 3);
			lcd.print("*");
			delay(1500);
			congratulations();
			return;
		}
	}
}

void restart() {
	lcd.clear();
	SIZE = 0; COLUMNS = 0; ROWS = 0;

	// setup screen
	lcd.home();
	lcd.print("*****Game Mode*****");
	lcd.setCursor(7, 1);
	lcd.write(byte(0));
	lcd.setCursor(9, 1);
	lcd.print("HARD");
	lcd.setCursor(6, 2);
	lcd.write(byte(1));
	lcd.setCursor(8, 2);
	lcd.print("NORMAL");
	lcd.setCursor(2, 3);
	lcd.write(byte(2));
	lcd.setCursor(4, 3);
	lcd.print("EASY");
	lcd.setCursor(12, 3);
	lcd.write(byte(3));
	lcd.setCursor(14, 3);
	lcd.print("QUIT");
}

void congratulations()
{
	lcd.clear();
	delay(500);

	lcd.setCursor(3, 0);
	lcd.print("Congratulations");
	lcd.setCursor(1, 1);
	lcd.print("You have completed");
	lcd.setCursor(6, 2);
	lcd.print("the game");
	lcd.setCursor(0, 3);
	lcd.print("Press to continue");
	while (1)
	{
		if (digitalRead(6) == HIGH || digitalRead(5) == HIGH || digitalRead(4) == HIGH || digitalRead(3) == HIGH)
		{
			break;
		}
	}
	lcd.clear();
	delay(1500);
	lcd.setCursor(2, 0);
	lcd.print("Would you like to");
	lcd.setCursor(5, 1);
	lcd.print("play again?");
	lcd.setCursor(1, 3);
	lcd.print("(n)");
	lcd.setCursor(5, 3);
	lcd.write(byte(1));
	lcd.setCursor(15, 3);
	lcd.print("(y)");
	lcd.setCursor(19, 3);
	lcd.write(byte(0));
	while (1)
	{
		if (digitalRead(6) == HIGH)
		{
			return;
		}
		if (digitalRead(5) == HIGH)
		{
			quit();
		}
	}
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
	lcd.clear();
	if (COLUMNS < 4)
	{
		lcd.setCursor(2, 3);
		lcd.write(byte(0));
		lcd.setCursor(7, 3);
		lcd.write(byte(1));
		lcd.setCursor(12, 3);
		lcd.write(byte(2));
		lcd.setCursor(17, 3);
		lcd.write(byte(3));
	}
	lcd.home();
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
			lcd.setCursor(pos, counter);
		}
		if (symbols[i] != THEKEY)
		{
			lcd.setCursor(pos, counter);
			itoa(symbols[i], STRINGBUFFER, 10); 
			lcd.print(STRINGBUFFER);
			pos += increment;
		}
		else
		{
			lcd.setCursor(pos, counter);
			lcd.print(" ");
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
			return false;
		}
	}
	return true;
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
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
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
		return true;
	}
	// If the width is even, then every solvable state has an even number of inversions if the blank is on an odd numbered row counting from the bottom
	else if (!(COLUMNS % 2) && ((((thekeypos / COLUMNS) % 2) + ROWS) % 2) && !(counter % 2))
	{
		return true;
	}
	// If the width is even, then every solvable state has an odd number of inversions if the blank is on an even numbered row counting from the bottom
	else if (!(COLUMNS % 2) && !((((thekeypos / COLUMNS) % 2) + ROWS) % 2) && (counter % 2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Clears the screen and quits the program
void quit()
{
	lcd.clear();
	lcd.setCursor(5, 1);
	lcd.print("BYE BYE...");
	lcd.setCursor(9, 2);
	lcd.print("5");
	delay(1000);
	lcd.setCursor(9, 2);
	lcd.print("4");
	delay(1000);
	lcd.setCursor(9, 2);
	lcd.print("3");
	delay(1000);
	lcd.setCursor(9, 2);
	lcd.print("2");
	delay(1000);
	lcd.setCursor(9, 2);
	lcd.print("1");
	delay(1000);
	lcd.setCursor(9, 2);
	lcd.print("0");
	delay(100);
	lcd.clear();
	lcd.noDisplay();
	exit(0);
}
