/**
 * fifteen.c
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board

int load[DIM_MAX * DIM_MAX];
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;


// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE *file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();
    
    
    


    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(100000);
        }

        // sleep thread for animation's sake
        usleep(100000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(1000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
    //set the size of the board
    int d2 = d * d;
    
    
    //check is d is odd
    if (d % 2 == 1)
    {
        int j = d2 - 1;
        
        //create a descending array starting with (d*d)-1 down to 1
        for (int i = 0; i < d2; i++)
        {
            load[i] = j;
            j--;
        }
    }
    
    else
    {
        int j = d2 - 1;
        
        //create the array with elemtns in descending order
        for (int i = 0; i < d2; i++)
        {
            load[i] = j;
            j--;
        }

        //swap last 2 placess in array
        int swap1 = load[d2-3];
        int swap2 = load[d2 -2];
        load[d2-3] = swap2;
        load[d2-2] = swap1;
    }
    
    //using a counter, iterate thourgh i and j of the 2 dimnesonal array board[][] and set each cell the the value of the load[] array
    int count = 0;
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            board[i][j] = load[count];
            count++;
        }
    }
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
   //interate thourgh i and j
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            //if cell equals zero, this will be our blank cell.
            if (board[i][j] == 0)
            {
                printf("   ");
            }
            
            //print the value of each cell in the 2D array board [][] using special %2i for double spaces
            else
            {
                printf("%2i ", board[i][j]);
            }
        }
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{

    //interate thorugh i and j loops
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            //check to see if the user input number is on the board, if it is thewn continue to check for blank tile "0"
            if (board[i][j] == tile)
            {
                //check if blank tile is to the right
                if (board[i][j+1] == 0 && (j + 1) <= (d - 1))
                {
                    //log the location of the blank tile
                    int blanki = i;
                    int blankj = j+1;
                    
                    //set the number slected to the blank tile sace
                    board[i][j] = 0;
                    board[blanki][blankj] = tile;
                    
                    return true;
                }
                
                //check if blank tile is to the left
                else if (board[i][j-1] == 0 && (j - 1) >= 0)
                {
                    //log the location of the blank tile
                    int blanki = i;
                    int blankj = j-1;
                    
                    //set the number slected to the blank tile sace
                    board[i][j] = 0;
                    board[blanki][blankj] = tile;
                    
                    return true;
                }
                
                //check if blank tile is to the bottom
                else if (board[i+1][j] == 0 && (i + 1) <= (d - 1))
                {
                    //log the location of the blank tile
                    int blanki = i+1;
                    int blankj = j;
                    
                    //set the number slected to the blank tile sace
                    board[i][j] = 0;
                    board[blanki][blankj] = tile;
                    
                    return true;
                }
                
                //check if blank tile is to the top
                else if (board[i-1][j] == 0 && (i - 1) >= 0)
                {
                    
                    //log the location of the blank tile
                    int blanki = i-1;
                    int blankj = j;
                    
                    //set the number slected to the blank tile sace
                    board[i][j] = 0;
                    board[blanki][blankj] = tile;
                    
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    // create a counter and correcttile variables
    int cnt = 1;
    int correct_tiles = 0;
    
    //loop thorugh i and j on board checking if the value of tile is correct ie [0][0] = 1, [0][1] = 2, [0][2] = 3....
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            //if tile value matches correct number then add +1 to the correct_tiles counter
            if (board[i][j] == cnt)
            {
                correct_tiles++;
            }
            
            //keep adding to the cnt counter, even if tiles are incorrect.
            cnt++;
            
            //finally, when the number of correct tiles counted reaches (d^2)-1 then return true and stop the while loop.
            if (correct_tiles == (d * d) - 1)
            {
                return true;
            }
        }
    }
    return false;
}
