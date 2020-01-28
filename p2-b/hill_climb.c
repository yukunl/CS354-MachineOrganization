/*Author: yukunl**/

#include <stdio.h>   
#include <stdlib.h>     
#include <string.h>  
char * COMMA = ",";

/*
 * USE THIS:
 * Structure representing the Matrix
 * numRows: number of rows
 * numCols: number of columns
 * matrix: 2D heap allocated array of integers
 */
typedef struct {    
    int numRows;
    int numCols;
    int **matrix;
} Matrix;

/* TODO:
 * Get the dimensions of the matrix that are on
 * the first line of the input file.
 * 
 * infp: file pointer for input file
 * nRows: pointer to number of rows in the matrix   
 * nCols: pointer to number of columns in the matrix
 */
void get_dimensions(FILE *infp, int *nRows, int *nCols) {         
    // get line 
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, infp) == -1) {
        printf("Error in reading the file\n");
        exit(1);
    }
    // dimension in Row 
    char *token = NULL;
    token = strtok(line, COMMA);
    *nRows = atoi(token);
    // dimension in Col 
    token = strtok(NULL,COMMA );
    *nCols  = atoi(token);
    free(line);
     line = NULL; 
}          

/*
 * Recursive method that search for the highest number
 *  in the square of nine and change the pointer to the 
 *  highest number. 
 *  board: pointer to the game board that store the numbers 
 *  ncol: pointer to number of col in board
 *  nrow: pointer to number of row in board 
 *  currentRow: pointer to  index of the current number(row)
 *  currentCol: pointer to  index of the current number(col)
 *  outfp: pointer to the output file  */
void recursive ( int ** board,int ncol, int nrow,  int currentRow, int currentCol, FILE *outfp ){
    // first number is the index 00 number on the board
     int maxNum = *(*(board + currentRow) + currentCol);
    // number holder so no pointer changed 
     int rowHolder = 0; 
     int colHolder = 0; 
     int maxcount = 0; // tracker to see if there are any number that is bigger than the current state aroudn it 
     // search for the 9 boxes around 
       	for(int  i = currentRow -1; i <= currentRow + 1; i++){
		for ( int j = currentCol - 1; j <=  currentCol + 1; j++){
           // if out of bound do not consider 
          	if(i >= 0 && i < nrow && j >= 0 && j < ncol ){
                  // if the new number on the board is bigger than the current max, change that to max 
		       	if(maxNum < *(*(board + i) + j)){
			   maxcount ++;
			   maxNum = *(*(board + i) + j);
		           rowHolder = i; // change pointer of row 
			   colHolder = j; // change pointer of col
		   }
	    }}}
	// if there are no other numbers bigger than the current one, return 
          if(maxcount  == 0){
		  return;
	  }
	  // print the maxNum out everytime 
	  fprintf(outfp," %d",maxNum);
          // call recursive 
	  recursive(board , ncol, nrow,rowHolder , colHolder, outfp);

}




/* TODO:
 * Continually find and print the largest neighbor
 * that is also larger than the current position
 * until nothing larger is found.       
 * 
 * outfp: file pointer for output file  
 * mstruct: pointer to the Matrix structure
 */
void hill_climb(FILE *outfp, Matrix *mstruct) {  
     // Write the intial starting number to outfp
     fprintf(outfp, "%d", **(mstruct -> matrix)   ); 
     // call recursive method to hillClimb
     recursive ( mstruct -> matrix, mstruct -> numCols, mstruct -> numRows, 0, 0, outfp );
     fprintf(outfp, "\n");
   // Find the largest neighbor that is also larger than the current position
    // Move to that position  
    // Write that number to outfp

    // Repeat until you can't find anything larger (you may use any loop)
}    

/* TODO:
 * This program reads an m by n matrix from the input file
 * and outputs to a file the path determined from the
 * hill climb algorithm.    
 *
 * argc: CLA count
 * argv: CLA values
 */
int main(int argc, char *argv[]) {
    // Check if number of command-line arguments is correct

        if(argc != 3 )
        {
	printf("Usage: ./hill_climb <input_filename> <output_filename>\n");
        return 0;
        }
    //Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL)
    {
        printf("Cannot open file for reading.\n");
        exit(1);
    }
    // Declare local variables including the Matrix structure 
   int nRows, nCols; 
   get_dimensions(fp,&nRows,&nCols);
    // Call the function get_dimensions

    // Dynamically allocate a 2D array in the Matrix structure
 int  ** myboard =(int **) malloc(nRows * sizeof(int *));
    // check if malloc return Null
      if(myboard == NULL)
        {
               exit(1);
        }
   for(int i = 0; i < nRows; i++){
         *( myboard + i) = (int *) malloc(nCols * sizeof(int));
      // check if malloc return Null
      if((myboard + i) == NULL)
             {
               exit(1);
              }

           }

	   // Read the file line by line base on the number of rows
       char *line = NULL;
       size_t len = 0;
       char *token = NULL;

   for(int i = 0 ; i <  nRows; i++){
      if(getline(&line,&len, fp) == -1){
	      exit(1);
      }
      token = strtok(line, COMMA);
   for(int j = 0; j < nCols; j++){
    *(*(myboard + i) + j) = atoi(token);
    token = strtok(NULL, COMMA);
	   }
   }

    // Tokenize each line wrt comma to store the values in the matrix
    Matrix *board = malloc(sizeof(Matrix));
    // check if malloc return Null
   if(board == NULL)
        {
               exit(1);
        }
    board -> numRows = nRows;
    board -> numCols = nCols;
    board -> matrix = myboard; 
    // Open the output file  
   FILE *outFile = fopen(argv[2], "w");
    if(outFile == NULL){
	    fprintf(stderr, "Can't open output file %s!\n", argv[2]);
	    exit(1);
    }

    // Call the function hill_climb
   hill_climb(outFile, board); 

    // Free the dynamically allocated memory
   for(int i = 0; i < nRows; i++){
	   free( *((board -> matrix) + i));
   }
   free(board -> matrix);
   free(board);
    // Close the input file
   fclose(fp);  
    // Close the output file
   fclose(outFile);
    return 0; 
}  
