#include <stdio.h>
#include<stdlib.h>
#include<math.h>

void main() {//(double *arrayValues, int arrayDimension, int threadCount, double precision)
	/*Declare variables*/
	int arrayDimension = 10;	// Dimensions of the array
	double precision = 0.01;	// precision of every iteration
	int N = arrayDimension;		// for mallocing (redundant)
	int M = arrayDimension;		// for mallocing (redundant)
	int i, j, q, p;				// loop counter variables
	double **arrayValues;		// 2D array declaration
	double oldValue;			// the old value, used in relaxation
	double newValue;			// the value to replace oldValue
	double comparePrecision;	// the comparison between Old & NewValue
	int areThingsHappening;		// checks the comparison between precisions
	/*Malloc the array into the memory to ensure larger arrays don't crash*/
    arrayValues=(double **) malloc(N*sizeof(double *));
    for(i=0;i<N;i++){
    arrayValues[i]=(double *) malloc(M*sizeof(double));
    }
    /*Put some random values into the array determined by rand math library*/
	for (i = 0; i<arrayDimension; i++) {
		for (j = 0; j<arrayDimension; j++) {
			arrayValues[i][j] = (double)rand() / (double)RAND_MAX;
		}
	}
	/*Print the input file into a csv file called 
	inputSingle.csv for future reference*/
	FILE *fIn = fopen("inputSingle.csv", "w"); 
	for (q = 0; q < arrayDimension; ++q) {
		for (p = 0; p < arrayDimension; ++p) {
		    fprintf(fIn, "%lf ", arrayValues[q][p]); 
		}
		if(q != arrayDimension-1){ //last line doesn't need ;
		fprintf(fIn,";\n");
		}
    }
    fclose(fIn);
	
	/* ---------- average neighbouring values, relaxation method ---------- */
	do {
		/*at the start of every loop iteration, set the variable that 
		checks whether to keep going back to zero*/
		areThingsHappening = 0;
		/*Loop through the height and width of the array*/
		for ( q = 0; q < arrayDimension; ++q) {
			for ( p = 0; p < arrayDimension; ++p) {
				/* if column & row is start or end, skip*/
				if (q != 0 && q != (arrayDimension - 1) &&
					p != 0 && p != (arrayDimension - 1)) {
					/*The current position gets saved to OldValue
					so that it can later be used for comparsion*/
					oldValue = arrayValues[q][p];
					/*here newValue gets set to be equal to the average
					 of its 4 surrounding values*/
					newValue = (arrayValues[q - 1][p] + arrayValues[q][p - 1] +
					arrayValues[q + 1][p] + arrayValues[q][p + 1]) / 4;
					/*newValue is subtracted from oldValue, as the 
					difference between these two signifies whether
					to keep looping or not. The absolute is taken
					as it could be a negative number.*/
					comparePrecision = fabs(oldValue - newValue);
					/*newValue gets inserted into the main array*/
					arrayValues[q][p] = newValue;
					/*if the difference between old and new is 
					greater than the precision value, ensure the
					do while loop check is notified*/
					if (comparePrecision > precision) {
						/*Using the variable here as "notifier"*/
						areThingsHappening = 1;
					}
				}
			}
		}
	} while (areThingsHappening == 1); // as long as precision isn't reached
	/* ---END---- average neighbouring values, relaxation method ----END--- */
	/*Print the output file into a csv file called 
	inputSingle.csv for future reference*/
	FILE *fOut = fopen("outputSingle.csv", "w"); 
	for (q = 0; q < arrayDimension; ++q) {
		for (p = 0; p < arrayDimension; ++p) {
		    fprintf(fOut, "%lf ", arrayValues[q][p]); 
		}
		if(q != arrayDimension-1){ //last line doesn't need ;
		fprintf(fOut,";\n");
		}
    }
    fclose(fOut);
}