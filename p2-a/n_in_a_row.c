/*Author yukunl**/

#include <stdio.h>  
#include <stdlib.h>
#include <string.h>     

char *COMMA = ",";  

/*       
 * Retrieves from the first line of the input file,
 * the size of the board (number of rows and columns).
 * 
 * fp: file pointer for input file
 * size: pointer to size
 */
void get_dimensions(FILE *fp, int *size) {      
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, fp) == -1) {
        printf("Error in reading the file\n");
        exit(1);
    }

    char *token = NULL;
    token = strtok(line, COMMA);
    *size = atoi(token);
}   


/* 
 * Returns 1 if and only if the board is in a valid state.
 * Otherwise returns 0.
 * 
 * board: heap allocated 2D board
 * size: number of rows and columns
 */
int n_in_a_row(int **board, int size) {

 int numX = 0; // total num of X
 int numO = 0; // total num of Y
 int a[size*2 + 2]; // int array that keep track of multiple winning
 int diag1x = 0; // num of diagonal left top to bottom right of X
 int diag1O = 0; // num of diagonal left top to bottom right of O
 int diag2x = 0; // num of diagonal bottom left to left right of X
 int diag2O = 0; // num of diagonal bottom left to left right of O

 // loop through the array
 for (int i=0;i< size; i++ )
 {
        // number of row trackers 
         int numXrow = 0; 
         int numOrow = 0;
	
	  for(int j=0; j< size; j++)
      {
	        
	//count number of X and O that showed up 
		 if(*(board[i] + j)  == 1) // count X
         {   //left top to bottom right
			 if(i == j)
             {
			  diag1x = diag1x + 1;
			 }
             //bottom left to left right
			 if(i + j == size - 1)
             {
				 diag2x = diag2x + 1;
			 }
			 numX = numX +1;
			 numXrow = numXrow + 1;  
		 }
		 if(*(board[i] + j)  == 2) // count O
         {   //left top to bottom right
			 if(i == j)
             {
				diag1O = diag1O + 1;
			 }
             //bottom left to left right
			 if(i + j == size - 1)
             {
                diag2O = diag2O + 1;
             }
			  numO = numO +1;
              numOrow = numOrow + 1;
		 }
	     }
       // if row is full of x, store
	   if(numXrow == size)
       {
                a[i] = 1;
        }
       // if row is full of O, store
        if(numOrow == size)
        {
                a[i] = 2;
        }
        }
        // count column
       	int numXCol = 0;
        int numOCol = 0;
        // loop through array with column
        for(int j = 0 ; j < size ; j++)
          {
             for (int i = 0; i < size; i ++)
             {
                 // x column count
                 if( *(board[i] + j)  == 1)
                 {
                      numXCol = numXCol + 1;
                     }
                 // O column count
                 if(*(board[i] + j)  == 2)
                 {
                      numOCol = numOCol + 1;
                     }   
	    }
       // if col filled with x
       if(numXCol == size)
       {
            a[2*j] = 1;
        }
        // if col filled with O
        if(numOCol == size)
        {
            a[2*j] = 2;
        }
	  
	  }
	   
    // if diagnol are full, record it in array
	if(diag1x == size)
    {
        a[size -1] = 1;
	}
	if(diag1O == size)
    {
        a[size - 1] = 2;
	}
    if(diag2x == size)
     {
        a[size -2] = 1;
        }
    if(diag2O == size)
    {
        a[size - 2] = 2;
        }
// count number of wins for both, for only x and only y	
     int wincount = 0; // total win count
     int xwin = 0; // number of win by X
     int owin = 0; // number of win by O
    for (int i = 0 ; i < 2* size + 2; i++ )
    {
		if(a[i] == 1 )
        {
			xwin = xwin + 1;
		}
		if(a[i] == 2)
        {
			owin = owin + 1;
		}
        if(a[i]  == 1 || a[i]  == 2)
        {
            wincount  = wincount  + 1;
	       }
       }
	// if both xwin and owin are both winning, false
         if(wincount > 1 && xwin > 0 && owin > 0 )
         {
           return 0; 
          }
          if((owin > 0 && numX > numO) || (xwin > 0 && numO == numX))
          {
	  return 0;
	      }
 
 // check if it was 1 and 2 in the array instead of other characters
       if(size != 0 && numX == 0 && numO == 0)
       {
		return 0;
	 	}
       if(numX != numO && numX -1 != numO)
        {
          return 0; 
        }


    return 1;   
}     


/*
 * This program prints Valid if the input file contains
 * a game board with either 1 or no winners; and where
 * there is at most 1 more X than O.
 * 
 * argc: CLA count
 * argv: CLA value
 */
int main(int argc, char *argv[]) {              
   
        int argnum = 0;
	for(int i = 0; i< argc; i++){
            argnum = argnum + 1;
	}
        if(argnum != 2 ){
	printf("Usage: ./n_in_a_row <input_filename>\n");
	return 0;
        }
    //Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL) {
        printf("Cannot open file for reading\n");
        exit(1);
    }
    //Declare local variables.
    int size;
    int *sizepointer = &size;

   
    get_dimensions(fp,sizepointer );
   
      int **myboard = malloc(sizeof(int) * size);
        if(myboard == NULL){
	       exit(1);
	}	       
      for(int i = 0; i < size; i++){
	myboard[i] = malloc(sizeof(int) * size);
        }

    //Read the file line by line.
    //Tokenize each line with respect to comma to store the values in your 2D array.
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    for (int i = 0; i < size; i++) {

        if (getline(&line, &len, fp) == -1) {
            printf("Error while reading the file\n");
            exit(1);
        }

        token = strtok(line, COMMA);
        for (int j = 0; j < size; j++){
	    *(j+ *(myboard+i)) = atoi(token);
            token = strtok(NULL, COMMA);
        }
    }

   
    //output depending on the function's return value.
int result =  n_in_a_row(myboard, size);
if(result == 0){
	printf("invalid\n");}
if(result == 1){
	printf("valid\n");}
    //TODO: Free all dynamically allocated memory.
for(int k =0; k< size; k++){
	free(*(myboard + k));
}
free(myboard);
    myboard = NULL; 

    //Close the file.
    if (fclose(fp) != 0) {
        printf("Error while closing the file\n");
        exit(1);
    } 

    return 0;       
}       

