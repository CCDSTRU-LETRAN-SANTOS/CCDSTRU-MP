/*
 * Authors           : Letran, Jessica C. (12505757)
 *                   : Santos, Michaela Lynn L. (12505765)
 * Section           : S22A
 * Last Modified     : 03/29/26
*/

/* Preprocessor Directives */
#include <stdio.h>
#include <string.h>

/* Definitions */
#define MAX_SUBS 9

/**
* Stores the coordinates of a subset
*/
typedef struct{
    int x;      // stores the row
    int y;      // stores the column
} Coordinate;

/* Function Prototypes */
void Remove(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], int go);
void Replace(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], int *go, int *found);
void Expand(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], int go, int *found);
void Update(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], int go, int *found, int *good);
void NextPlayerMove(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], 
                    int *go, int *good, int *start, int *val, int *over, int *found);
void GameOver(Coordinate R[], Coordinate B[], char result[]);

/* Helper Functions */

/**
 * This function checks if two coordinates are equal
 * @param a is the first coordinate
 * @param b is the second coordinate
 * @return 1 if the coordinates are equal, 0 if not
 */
int equal(Coordinate a, Coordinate b) 
{
    return (a.x == b.x && a.y == b.y);
}

/**
 * This function checks if the coordinate is within the board boundaries
 * @param p is the coordinate to check
 * @return 1 if it is within boundaries, 0 if not
 */
int valid(Coordinate p) 
{
    return (p.x >= 1 && p.x <= 3 && p.y >= 1 && p.y <= 3);
}

/**
 * This function counts the number of non-zero (not null) coordinates in a set
 * @param p is an array of coordinates
 * @return the number of set of coordinates in the array
 */
int cardinality(Coordinate p[MAX_SUBS])
{
    int size = 0;
    for(int i=0; i<MAX_SUBS; i++) {
        if(p[i].x != 0 && p[i].y != 0)
            size++;
    }
    return size;
}

/**
 * This function centers the text to be printed
 * @param text is a pointer to a string
 */
void printCentered(char* text){
	int len = strlen(text);
	int padding = (47 - len) / 2;
	if (padding < 0) // avoid negative, ie. %-10s
		padding = 0;
	printf("\n%*s%s\n", padding, "", text);
}

/* Function Implementations */

/**
 * This function removes a coordinate from specific sets depending on the player's turn
 * @param pos is the coordinate to remove
 * @param R is the array of coordinates for player R
 * @param B is the array of coordinates for player B
 * @param S is the shared set of coordinates
 * @param T is the temporary set of coordinates
 * @param go is a flag indicating whose turn it is (1 for R, 0 for B)
 */
void Remove(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], int go)
{
    int i;

    for(i=0; i<MAX_SUBS; i++)
    {
        // go → (R = R − {pos})
        if(go)
        { 
            if(equal(R[i], pos))
            {
                R[i].x = 0;
                R[i].y = 0;
            }
        }
        // ¬go → (B = B − {pos})
        else 
        {
            if(equal(B[i], pos))
            {
                B[i].x = 0;
                B[i].y = 0;
            }
        }
        // S = S − {pos}
        if(equal(S[i], pos))
        {
            S[i].x = 0;
            S[i].y = 0;
        }
        // T = T − {pos}
        if(equal(T[i], pos))
        {
            T[i].x = 0;
            T[i].y = 0;
        }
    }
}

/**
 * This function replaces a coordinate in the player sets depending on the current turn
 * This handles capturing opponent pieces, updating shared sets, and triggers expansion
 * @param pos is the chosen coordinate input by the player
 * @param R is the array of coordinates for player R
 * @param B is the array of coordinates for player B
 * @param S is the shared set of coordinates
 * @param T is the temporary set of coordinates
 * @param go is a pointer to the current turn flag (1 for R, 0 for B)
 * @param found is a pointer to a flag indicating if the coordinate was found in opponent's set
 */
void Replace(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], int *go, int *found)
{
    int EqualR = 0, EqualB = 0, EqualS = 0, EqualT = 0, trigger = 0, i;

    //found = false
    *found = 0;

    if(*go)
    {
        for(i=0; i<MAX_SUBS; i++)
        {
            // (go ∧ pos ∈ B) → (B = B − {pos} ∧ found = true)
            if(equal(B[i], pos))
            {
                B[i].x = 0;
                B[i].y = 0;
                *found = 1;
            }
            // (go ∧ pos ∈ R) → found = true
            if(equal(R[i], pos))
            {
                *found = 1;
                EqualR = 1;
            }
        }

        // (go ∧ pos ̸∈ R) → (R = R ∪ {pos})
        if(!EqualR)
        {
            for(i=0; i<MAX_SUBS; i++)
            {
                if(R[i].x == 0 && R[i].y == 0)
                {
                    R[i] = pos;
                    i=MAX_SUBS;
                }
            }
        }
    }
    else 
    {
        for(i=0; i<MAX_SUBS; i++)
        {
            // (¬go ∧ pos ∈ R) → (R = R − {pos} ∧ found = true)
            if(equal(R[i], pos))
            {
                R[i].x = 0;
                R[i].y = 0;
                *found = 1;
            }
            // (¬go ∧ pos ∈ B) → found = true
            if(equal(B[i], pos))
            {
                *found = 1;
                EqualB = 1;
            }
        }

        // (¬go ∧ pos ̸∈ B) → (B = B ∪ {pos})
        if(!EqualB){
            for(i=0; i<MAX_SUBS; i++)
            {
                if(B[i].x == 0 && B[i].y == 0)
                {
                    B[i] = pos;
                    i=MAX_SUBS;
                }
            }
        }
    }

    // (found ∧ pos ̸∈ S) → (S = S ∪ {pos} ∧ found = false)
    if (*found)
    {
        // check if pos is equal to S
        for(i=0; i<MAX_SUBS; i++)
        {
            if(equal(S[i], pos))
                    EqualS = 1;
        }
        if(!EqualS)
        {
            for(i=0; i<MAX_SUBS; i++)
            {
                if(S[i].x == 0 && S[i].y == 0)
                {
                    S[i] = pos;
                    *found = 0;
                    i=MAX_SUBS;
                }
            }
        }
        //(found ∧ pos ∈ S ∧ pos ̸∈ T) → (T = T ∪ {pos} ∧ Expand(pos))
        else 
        {
            // check if pos is equal to T
            for(i=0; i<MAX_SUBS; i++)
            {
                if(equal(T[i], pos))
                    EqualT = 1;
            }
            if(!EqualT)
            {
                for(i=0; i<MAX_SUBS; i++)
                {
                    if(T[i].x == 0 && T[i].y == 0)
                    {
                        T[i] = pos;
                        trigger = 1;
                        i=MAX_SUBS;
                    }
                }
                if(trigger && *found)
                    Expand(pos, R, B, S, T, *go, found);
            }
        }
    }
}

/**
 * This function expands a coordinate to its adjacent positions and updates the player and shared sets
 * It removes the original coordinate from sets, then calls Replace function on neighboring coordinates
 * @param pos is the chosen coordinate input by the player
 * @param R is the array of coordinates for player R
 * @param B is the array of coordinates for player B
 * @param S is the shared set of coordinates 
 * @param T is the temporary set of coordinates
 * @param go is the current turn flag (1 for R, 0 for B)
 * @param found is a pointer to a flag indicating if a coordinate was captured during replacement
 */
void Expand(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], int go, int *found)
{
    int a, b;

    // (a, b) = pos
    a = pos.x;
    b = pos.y;

    // u, d, k, r ∈ M
    Coordinate u, d, k, r;

    // u = (a − 1, b)
    u.x = a-1;
    u.y = b;

    // d = (a + 1, b)
    d.x = a+1;
    d.y = b;

    // k = (a, b − 1)
    k.x = a;
    k.y = b-1;

    // r = (a, b + 1)
    r.x = a;
    r.y = b+1;

    //Remove(pos)
    Remove(pos, R, B, S, T, go);

    // (go) → Replace(u)
    if(go) 
    {
        if(valid(u))
            Replace(u, R, B, S, T, &go, found);
    }
    // (¬go) → Replace(d)
    else 
    {
        if(valid(d))
            Replace(d, R, B, S, T, &go, found);
    }

    //Replace(k)
    if(valid(k))
        Replace(k, R, B, S, T, &go, found);

    //Replace(r)
    if(valid(r))
        Replace(r, R, B, S, T, &go, found);
}

/** 
 * This function updates the state sets S and T based on the selected position 
 * @param pos is the chosen coordinate input by the player
 * @param R is the array of coordinates for player R 
 * @param B is the array of coordinates for player B 
 * @param S is the shared set of coordinates (visited positions)
 * @param T is the temporary set of coordinates (expanded positions)
 * @param go is the current turn flag (1 for R, 0 for B) 
 * @param found is a pointer to a flag indicating if replace condition was triggered
 * @param good is a pointer to indicate if the move is valid and accepted 
 */
void Update(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[], int go, int *found, int *good)
{
    int EqualS = 0, EqualT = 0, i;

    // good = false
    *good = 0;

    // (pos ̸∈ S) → (S = S ∪ {pos} ∧ good = ¬good)
    // check if pos is equal to S
    for(int i=0; i<MAX_SUBS; i++)
    {
        if(equal(S[i], pos))
            EqualS = 1;
    }
    if(!EqualS)
    {
        for(i=0; i<MAX_SUBS; i++)
        {
            if(S[i].x == 0 && S[i].y == 0)
            {
                S[i] = pos;
                *good = 1;
                i=MAX_SUBS;
            }
        }
    }

    // (¬good ∧ pos ∈ S ∧ pos ̸∈ T) → (T = T ∪ {pos} ∧ Expand(pos))
    // check if pos is equal to T
    for(i=0; i<MAX_SUBS; i++)
    {
        if(equal(T[i], pos))
            EqualT = 1;
    }
    if(!(*good) && EqualS && !EqualT) 
    {
        for(i=0; i<MAX_SUBS; i++)
        {
            if(T[i].x == 0 && T[i].y == 0)
            {
                T[i] = pos;
                Expand(pos, R, B, S, T, go, found);
                i=MAX_SUBS;
            }
        }
    }
}

/** 
 * This function processes the player's move and updates the overall game state
 * @param pos is the chosen coordinate input by the player
 * @param R is the array of coordinates for player R
 * @param B is the array of coordinates for player B
 * @param S is the shared set of coordinates (visited positions)
 * @param T is the temporary set of coordinates (expanded positions)
 * @param go is a pointer to the current turn flag (1 for R, 0 for B)
 * @param good is a pointer indicating if the move is valid and accepted
 * @param start is a pointer indicating if the game is still in the initial placement phase
 * @param val is a pointer to the number of valid moves made so far
 * @param over is a pointer indicating whether the game has ended
 * @param found is a pointer to a flag indicating if a replace condition was triggered
 */
void NextPlayerMove(Coordinate pos, Coordinate R[], Coordinate B[], Coordinate S[], Coordinate T[],
                        int *go, int *good, int *start, int *val, int *over, int *found)
{
    int i;

    if(!(*over)) 
    {
        if(*start) 
        {
            // (¬over ∧ start ∧ go) → (R = R ∪ {pos} ∧ S = S ∪ {pos} ∧ good = true)
            if(*go) 
            {
                for(i=0; i<MAX_SUBS; i++) 
                {
                    if(R[i].x == 0 && R[i].y == 0) 
                    {
                        R[i] = pos;
                        i=MAX_SUBS;
                    }
                }
            }
            // (¬over ∧ start ∧ ¬go) → (B = B ∪ {pos} ∧ S = S ∪ {pos} ∧ good = true)
            else {
                for(i=0; i<MAX_SUBS; i++) 
                {
                    if(B[i].x == 0 && B[i].y == 0) 
                    {
                        B[i] = pos;
                        i=MAX_SUBS;
                    }
                }    
            }

            for(i=0; i<MAX_SUBS; i++) 
            {
                if(S[i].x == 0 && S[i].y == 0) 
                {
                        S[i] = pos;
                        i=MAX_SUBS;
                }     
            }

            *good = 1;
        }
        // (¬over ∧ ¬start ∧ (go ∧ pos ∈ R ∨ ¬go ∧ pos ∈ B)) → (Update(pos) ∧ good = true)
        else
        {
            for(i=0; i<MAX_SUBS; i++) 
            {
                if((*go && equal(R[i], pos)) || (!(*go) && equal(B[i], pos))) 
                {
                    Update(pos, R, B, S, T, *go, found, good);
                    *good = 1;
                }
            }
        }
        // (¬over ∧ good) → (good = ¬good ∧ go = ¬go ∧ val = val + 1)
        if(*good) 
        {
            *good = 0;
            *go = !(*go);
            *val += 1;
        }
        else
            printf("Invalid Move, Try Again\n");
    }

    // (start ∧ |R| = 1 ∧ |B| = 1) → start = false
    if(*start)
    {
        int countR, countB;
    
        countR = cardinality(R);
        countB = cardinality(B);

        if(countR == 1 && countB == 1)
            *start = 0;
    }
}

/** 
 * This function determines the result of the game based on the final sets
 * @param R is the array of coordinates for player R
 * @param B is the array of coordinates for player B
 * @param result is a character array where the game outcome will be stored
 */
void GameOver(Coordinate R[], Coordinate B[], char result[])
{
    // result ∈ {“R wins”, “B wins”, “draw”}
    int countR, countB;

    countR = cardinality(R);
    countB = cardinality(B);

    // (over ∧ |R| > |B|) → result = “R wins”
    if(countR > countB) 
        strcpy(result, "R Wins!");
    // (over ∧ |R| < |B|) → result = “B wins”
    else if(countR < countB) 
        strcpy(result, "B Wins!");
    // (over ∧ |R| = |B|) → result = “draw”
    else
        strcpy(result, "draw");
           
}

/**
 * This function prints the current state of the board 
 * @param R is the array of coordinates for player R
 * @param B is the array of coordinates for player B
 * @param val is the number of valid moves made so far
 */
void PrintBoard(Coordinate R[], Coordinate B[], Coordinate S[], int val) 
{
    int i, j, k, nShared;
    char M[3][3];

    printf("Board (Move %d):\n\n", val);

    // initialize
    for(i = 0; i < 3; i++) 
    {
        for(j = 0; j < 3; j++) 
        {
            M[i][j] = 'x';
        }
    }

    // place R
    for(i = 0; i < MAX_SUBS; i++) 
    {
        if(R[i].x != 0 && R[i].y != 0) 
        {
            M[R[i].x - 1][R[i].y - 1] = 'R';
        }
    }

    // place B
    for(i = 0; i < MAX_SUBS; i++) 
    {
        if(B[i].x != 0 && B[i].y != 0) 
        {
            M[B[i].x - 1][B[i].y - 1] = 'B';
        }
    }

    // column label
    printf("   1   2   3\n");

    // print board
    for(i = 0; i < 3; i++) 
    {
        // row label
        printf("%d", i + 1);
        
        for(j = 0; j < 3; j++) 
        {
            nShared = 0;
            for(k = 0; k < MAX_SUBS; k++) 
            {
                if(S[k].x == i + 1 && S[k].y == j + 1) 
                {
                    nShared = 1;
                    k = MAX_SUBS; 
                }
            }
            if(M[i][j] == 'R') 
            {
                if(nShared)
                    printf("  %c ", M[i][j]);
                else
                    printf("  \033[31m%c\033[0m ", M[i][j]);
            } 
            else if(M[i][j] == 'B') 
            {
                if(nShared)
                    printf("  %c ", M[i][j]);
                else
                    printf("  \033[34m%c\033[0m ", M[i][j]);
            } 
            else 
            {
                printf("  %c ", M[i][j]); // default
            }
        }
        printf("\n");
    }
}

int main(){
    /* Variable Implementation */
    int val, good, go, start, over, found, sizeR, sizeB, sizeF, intersection, nMenu;
    Coordinate R[MAX_SUBS], B[MAX_SUBS], S[MAX_SUBS], T[MAX_SUBS], pos;
    char result[8];

    /* Variable Initialization */
    go = 1; //true
    start = 1; //true
    good = 0; //false
    found = 0; //false
    over = 0; //false
    val = 0;
    sizeR = 0;
    sizeB = 0; 
    sizeF = 0;
    nMenu = 0;
    // R, B, S, T = Ø
    for(int i=0; i<MAX_SUBS; i++)
    {
        R[i] = (Coordinate){0,0};
        B[i] = (Coordinate){0,0};
        S[i] = (Coordinate){0,0};
        T[i] = (Coordinate){0,0};
    }

    while(nMenu!=3)
    {
    printf("\e[1;1H\e[2J"); // clear screen
    printf(" . \\  :  /       \\  :  /       \\  :  /       \\  :  /       \\  :  /\n");
    printf("`. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .'\n");
    printf("_ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _\n");
    printf("   /_   _\\       /_   _\\       /_   _\\       /_   _\\       /_   _\\\n");
    printf(" .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.\n");
    printf("   /  |  \\       /  :  \\       /  :  \\       /  :  \\       /  |  \\\n");
    printf("      |                                                       |\n");
    printf("      |              Welcome to Chain Reaction!               |\n");
    printf("      |                                                       |\n");
    printf("      |                [1] Start Game                         |\n");
    printf("      |                [2] Learn How to Play                  |\n");
    printf("      |                [3] Exit                               |\n");
    printf("      |                                                       |\n");
    printf("   \\  |  /       \\  :  /       \\  :  /       \\  :  /       \\  |  /\n");
    printf("`. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .'\n");
    printf("_ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _\n");
    printf("   /_   _\\       /_   _\\       /_   _\\       /_   _\\       /_   _\\\n");
    printf(" .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.\n");
    printf("   /  :  \\       /  :  \\       /  :  \\       /  :  \\       /  :  \\\n\n\n");
    printf(">> ");
    scanf("%d", &nMenu); 
        if(nMenu==1)
        {
            printf("\e[1;1H\e[2J");
            printf("\nLegends:\n");
            printf(" | Red|Blue Colored Player Spaces -> cannot expand yet\n");
            printf(" | White Colored Player Spaces -> can expand\n");

            printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
            printf("Board:\n\n");
            // column label
            printf("   1   2   3\n");

            // print board
            for(int i = 0; i < 3; i++) {
                // row label
                printf("%d", i+1);
                
                for(int j = 0; j < 3; j++) {
                    printf("  x ");
                }
                printf("\n");
            }

            printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

            while(!over)
            {
                if(go)
                    printf("\n\033[31mPlayer R\033[0m, Choose a Position\n");
                else if (!go)
                    printf("\n\033[34mPlayer B\033[0m, Choose a Position\n");
                
                printf("Row: ");
                scanf("%d", &pos.x);
                printf("Column: ");
                scanf("%d", &pos.y);
                printf("\n");
                
                if(valid(pos))
                {
                    NextPlayerMove(pos, R, B, S, T, &go, &good, &start, &val, &over, &found);

                    sizeR = cardinality(R);
                    sizeB = cardinality(B);

                    //find intersection
                    intersection = 0;
                    for(int i=0; i<MAX_SUBS; i++){
                        for(int j=0; j<MAX_SUBS; j++){
                            if(equal(R[i], B[j]) && R[i].x != 0 && R[i].y != 0)
                                intersection++;
                        }
                    }

                    // M minus union of R and B
                    sizeF = MAX_SUBS - ((sizeR+sizeB)-intersection);

                    //check conditions to end game
                    if(sizeF==3 || val>=20 || (!start && ((sizeR>0 && sizeB==0) || (sizeR==0 && sizeB>0))))
                        over = 1;
                }
                else
                    printf("Enter a Valid Coordinate\n");
            
                PrintBoard(R, B, S, val);
                printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
            }

            // check which condition to end game was met
            if(sizeF==3)
                printCentered("- - - Only 3 Spaces Left Unoccupied - - -");
            else if(val>=20) 
                printCentered("- - - 20 Moves Reached - - -");
            else if(!start && (sizeR>0 && sizeB==0))
                printCentered("- - - Blue Player Eliminated - - -");
            else if (!start && (sizeR==0 && sizeB>0))
                printCentered("- - - Red Player Eliminated - - -");

            printCentered("- GAME OVER -");

            GameOver(R, B, result);
            if(strcmp(result, "R Wins!") == 0)
            {
                printf("\033[31m");
                printCentered(result);
                printf("\033[0m\n");
            }
            else if(strcmp(result, "B Wins!") == 0) 
            {
                printf("\033[94m");
                printCentered(result);
                printf("\033[0m\n");
            }
            else 
                printf("%s\n", result);

            printf("- - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

            printf("\nEnter Any Key to Go Back to Main Menu\n");
            getchar();
            getchar();

            // reset
            go = 1; 
            start = 1; 
            good = 0; 
            found = 0;
            over = 0;
            val = 0;
            sizeR = 0;
            sizeB = 0; 
            sizeF = 0;
            for(int i=0; i<MAX_SUBS; i++)
            {
            R[i] = (Coordinate){0,0};
            B[i] = (Coordinate){0,0};
            S[i] = (Coordinate){0,0};
            T[i] = (Coordinate){0,0};
            }
        }
        else if(nMenu==2)
        {
            printf("\e[1;1H\e[2J");
            printf(" . \\  :  /       \\  :  /       \\  :  /       \\  :  /       \\  :  /\n");
            printf("`. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .'\n");
            printf("_ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _\n");
            printf("   /_   _\\       /_   _\\       /_   _\\       /_   _\\       /_   _\\\n");
            printf(" .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.\n");
            printf("   /  |  \\       /  :  \\       /  :  \\       /  :  \\       /  |  \\\n");
            printf("      |                                                       |\n");
            printf("      |                                                       |\n");
            printf("      |      Chain Reaction is a tactical board game for      |\n"); 
            printf("      |    two players. Start by placing your first piece,    |\n"); 
            printf("      |    then take turns expanding your pieces to capture   |\n"); 
            printf("      |    your opponent\'s spots. The game ends when one of   |\n"); 
            printf("      |    these conditions are met: a player is eliminated,  |\n"); 
            printf("      |     only three spaces are left, or after 20 moves.    |\n"); 
            printf("      |     The goal is gain more spaces than your opponent!  |\n"); 
            printf("      |                      Ready to Play?                   |\n");
            printf("      |                                                       |\n");
            printf("      |                 Enter Any Key to Go Back              |\n");
            printf("      |                                                       |\n");
            printf("   \\  |  /       \\  :  /       \\  :  /       \\  :  /       \\  |  /\n");
            printf("`. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .' `. __/ \\__ .'\n");
            printf("_ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _ _ _\\     /_ _\n");
            printf("   /_   _\\       /_   _\\       /_   _\\       /_   _\\       /_   _\\\n");
            printf(" .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.   .'  \\ /  `.\n");
            printf("   /  :  \\       /  :  \\       /  :  \\       /  :  \\       /  :  \\\n\n\n");
            getchar();
            getchar();
        }
        else if(nMenu==3)
            printf("\nExiting Game...Thank You for Playing!\n");
    }
    return 0;
}
