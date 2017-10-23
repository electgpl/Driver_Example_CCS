//////////////////////////////////////////////////////////////////////////
////                          EX_MALLOC.C                             ////
////                                                                  ////
////  This program demonstrates the use of malloc() to allocate a     ////
////  block of memory. The user can add or delete words from a        ////
////  list, or show the lists contents. When the user instructs       ////
////  the program to add a new word, malloc() is used to allocate     ////
////  memory for a new cell. free() is used to deallocate the         ////
////  memory when the user instructs to delete a cell. When the user  ////
////  wants to know the contents of the list, the contents of each    ////
////  cell is displayed. A pointer to the next cell is also stored    ////
////  in a cell. This is used to cycle through the list.              ////
////                                                                  ////
////  Jumpers:                                                        ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX            ////
////                                                                  ////
////  This example will work with the PCM and PCH compilers.  The     ////
////  following conditional compilation lines are used to include a   ////
////  valid device for each compiler.  Change the device, clock and   ////
////  RS232 pins for your hardware if needed.                         ////
//////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS   ////
//// C compiler.  This source code may only be distributed to other   ////
//// licensed users of the CCS C compiler.  No other use,             ////
//// reproduction or distribution is permitted without written        ////
//// permission.  Derivative programs created using this software     ////
//// in object code form are not restricted in any way.               ////
//////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)
#include <16F877.h>
#device *=16
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

// Includes the functionality for malloc()
#include <STDLIBM.H>

// Included for get_int()
#include <STDLIB.H>

// Included for get_string()
#include <input.c>

struct Cell
{
	char text[10];							// Stores text up to 10 characters
	struct Cell* nextCell;				// A pointer to the next cell
};


void main()
{
	unsigned int num;						// Used to get a number for deleting a cell
	char key;								// Stores the key pressed for user input
	unsigned int count;					// Create a counter for the for loop
	struct Cell* beginCell;				// Create a pointer to the first cell
	struct Cell* endCell;				// Create a pointer for the last cell
	struct Cell* pCell;					// Create a Cell pointer
	struct Cell* pCell2;					// Create another Cell pointer

	beginCell = malloc(sizeof(Cell));// Allocates memory using malloc()
	endCell = beginCell;					// Set the end equal to the beginning
	beginCell->nextCell = NULL;		// Initialize the pointers
	pCell = NULL;
	pCell2 = NULL;

	while(TRUE){							// Loop forever
		do{									// Loops until a, d, or s is pressed.
			printf("\nPress 'a' to add a word, 'd' to delete a word,");
			printf(" or 's' to show the entered words.");
			key = getch();			// Gets a key
		}while(key != 'a' && key != 'd' && key != 's');

		if(key == 'a')	// If a was pressed, then add a cell
		{
         endCell->nextCell = malloc(sizeof(Cell)); // Allocate memory for a new cell
			if(endCell->nextCell != NULL)			// Check if memory was allocated
			{
				printf("\nEnter a word: ");		// Prompt the user for a word
				get_string(endCell->text, 10);	// Get the word and store it in the new cell
				endCell = endCell->nextCell;  	// Set the end cell to the new cell
				endCell->nextCell = NULL;			// Set the new Cell's next pointer to NULL
			}
		}
		else if(key == 's')	// If s was pressed, then show the text in every cell
		{
			pCell = beginCell;						// Set a pointer to the first cell
			count = 1;									// Set the word counter at 1
			while(pCell != endCell){				// Loops until the pointer is NULL
				printf("\nWord %u: %s", count, pCell);// Output the text stored in the cell
				pCell = pCell->nextCell;			// Set the cell pointer to the next cell
				++count;
			}
		}
		else if(key == 'd')	// If d was pressed, then delete a cell
		{
			printf("\nWhich word number would you like to delete?\n ");
			num = get_long();								// Get the word number to delete
			pCell2 = pCell = beginCell;				// Set a pointer to the first cell

			for(count=1; count<num && pCell != endCell; ++count){
				pCell2 = pCell;							// Save the pointer
				pCell = pCell->nextCell;				// Set the cell pointer to the next cell
			}

			if(pCell != endCell && num > 0){			// Check for valid pointer
				pCell2->nextCell = pCell->nextCell;	// Relink cells
				if(pCell == beginCell)					// Check if the first cell is deleted
					beginCell = beginCell->nextCell;	// First cell pointer points at next cell
				free(pCell);								// Free the deleted cell's memory
			}
		}
	}			// End loop forever
}				// End main
