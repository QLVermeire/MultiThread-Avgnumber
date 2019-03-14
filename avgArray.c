#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include<stdlib.h>
#include<math.h>

/*Global variables are not desired, but are used for passing 
from main to avgArray threads, this is to be improved*/
int arrayDimension;			//The dimensions of the main array
int rowsPerThread;			//The amount of rows to be done in a thread
double precision;			//Precision parameter 
int areThingsHappening = 0; //signalising parameter on precision activity

void *avgArray(void *startpoint){
	/** This function is explained more in-depth in the main, with more comments etc*/
	double *arrayPointer = (double *)startpoint;
    int endpoint = arrayDimension *(rowsPerThread+1);
    double oldValue;
	double newValue;
	double comparePrecision;
	double oneUp;
	double oneLeft;
	double oneDown;
	double oneRight;
	int loopCounter = 0;
	/*Initialise he start point of the pointer*/
	double *pointerStartPosition = arrayPointer;
	loopCounter = 0;
	/*Set the pointer back to its starting point*/
	arrayPointer = pointerStartPosition; 
	/* As long as things are happening, signal for another iteration */
	areThingsHappening = 0;
	/*As long as it hasn't reached the end of th vector, continue*/
	while(loopCounter < endpoint){
		/*If the loopcounter is at its first or last position in the column
			(found by using the modulus operator), skip this iteration*/
		if((loopCounter)%arrayDimension == 0||(loopCounter+1)%arrayDimension == 0){
		 //do nothing
		}else{
			/*If it is before the endpoint and after at least one iteration
				(as it skips the first row after the endpoint due to the padding*/
			if(loopCounter <= endpoint && loopCounter > arrayDimension){
				/*Shift the pointer around to find its neighbouring values*/
				oldValue = *arrayPointer;
				arrayPointer = arrayPointer -1;
				oneLeft = *arrayPointer;
				arrayPointer = arrayPointer +2;
				oneRight = *arrayPointer;
				arrayPointer = arrayPointer + arrayDimension -1;
				oneDown =  *arrayPointer;
				arrayPointer = arrayPointer - arrayDimension*2;
				oneUp = *arrayPointer;
				arrayPointer = arrayPointer + arrayDimension;
    			/* set newValue equal to average of 4 surroundings*/
				newValue = (oneUp + oneDown + oneLeft + oneRight) / 4;

				/*compare oldValue and newValue to check whether
				it is in precision limits - the absolute is taken
				in case the difference is a negative number*/
				comparePrecision = fabs(oldValue - newValue);	
				*arrayPointer = newValue;
				if (comparePrecision > precision) {
				/*If the difference still exceeds precision, loop around again*/
					areThingsHappening = 1;// things are still happening			
				}
			}
		}
		/*Increment array and loop pointer after its position checks*/
		arrayPointer++;
		loopCounter++;
	}
	/*Return thread*/
	return NULL;
}
/*Most of my code, averaging et cetera is done in the main function,
I am aware that this does affect the legibility of the code, hence 
why I will try my best to ensure it is properly commented and legible.*/
int main(int argc, char *argv[]){
	/*Check whether there are 4 arguments present*/
	if (argc != 5) {
		printf("Incorrect input! Please use avgArray -file.txt -precision -threadcount -arraysize.");
		exit(1);
	}
	/*Declare variables*/
	char * fileName = argv[1];
	precision = atof(argv[2]);	// precision of every iteration
	int threadCount = atoi(argv[3]);// count of threads/tasks
	int arrayDimension = atoi(argv[4]);	//size of array
	int rowsFinalThread;		// amount of rows the final thread does
	int threadStartPoint;		// arrayDimension * 1;
	int loopCounter;       		// a counter for a while loop used in relaxation
    int finalStart; 			// The pointer position where the final thread starts
	int printvalue;				// used as loop counter to print vectors
	int i,j,q,p;				// loop counter variables
	double oldValue;			// the old value, used in relaxation
	double newValue;			// the value to replace oldValue
	double comparePrecision;	// the comparison between Old & NewValue
	double oneUp;				// the value of [x][y-1], easier with pointers
	double oneLeft;				// [x-1][y]
	double oneDown;				// [x][y+1]
	double oneRight;			// [x+1][y]
	double value;
	
	/*Malloc the array into the memory to ensure larger arrays don't crash*/
	double **arrayValues;
    arrayValues=(double **) malloc(arrayDimension*sizeof(double *));
    for(i=0;i<arrayDimension;i++){
        arrayValues[i]=(double *) malloc(arrayDimension*sizeof(double));
    }
	printf("Attempting to read file: %s\n", argv[1]);
	/*Read in the text file with the array in it*/
	FILE* MyFile;
	MyFile = fopen(fileName, "r");
	if (NULL != MyFile){
		for (i = 0; i < arrayDimension; i++) {
			for (j = 0; j < arrayDimension; j++) {
				//printf("did it work?");
				fscanf(MyFile, "%lf", &value);
				arrayValues[i][j] = value;
			//	printf("",value);
			}
		}
		fclose(MyFile);
	}
	else{
		printf("File does not exist.");
		while (1);
	}
	
	/*If there is more than one thread, find out how many rows
	each thread will do, and how many the final one will do*/
	if(threadCount > 1){
	/* The rows per thread is simply the array length divided
	   by the amount of threads there are*/
	rowsPerThread = arrayDimension/threadCount;
	/*The final thread however uses the modulus, to ensure
	that where division is not an integer number it still does
	the last few rows that would normally not be covered*/
	rowsFinalThread = rowsPerThread+arrayDimension%threadCount;
	}
	

	
	/** for debug v------------------------------------------------
    //put values into 2D array
	for (i = 0; i<arrayDimension; i++) {
		for (j = 0; j<arrayDimension; j++) {
			arrayValues[i][j] = (double)rand() / (double)RAND_MAX;
		}
	}
		for debug ^------------------------------------------------*/
	/*Print the input file into a csv file called 
	inputSingle.csv for future reference*/	
	FILE *fIn = fopen("input.csv", "w"); 
	for (q = 0; q < arrayDimension; ++q) {
		for (p = 0; p < arrayDimension; ++p) {
		    fprintf(fIn, "%lf ", arrayValues[q][p]); 
		}
		if(q != arrayDimension-1){ //last line doesn't need ;
		fprintf(fIn,";\n");
		}
    }
    fclose(fIn);
	
	/*Here the array is vectorized, this is due to the fact that 
	it is a lot easier to control a 1D array with known dimensions
	when passing it onto a seperate thread with pointers*/
	/*Initialise the total amount of values as x*y so that we
	can form a vector of the right size.*/
	int totalAmountOfValues = arrayDimension * arrayDimension;
	/*Create the vector of appropriate size*/
	double vectorValues[totalAmountOfValues];
	/*Loops through the arrayValues and place them into the
	new vector vectorValues, starting at position 0*/
	int vectorPosition = 0;
	for(q = 0;q < arrayDimension;++q){
    	for(p = 0;p < arrayDimension;++p){
        	vectorValues[vectorPosition] = arrayValues[q][p];
        	vectorPosition++;
    	}
	}
	
    do{
    
        /*Create an array of threads*/
        pthread_t threadPointer[threadCount];
		/*For each thread that exists, enter this loop, in essence
		it fires off a bunch of threads at different pointer positions*/
        for( i=0; i < threadCount; i ++){
			
            if (i == 0){
				/*For the first row, start at point 0*/
				threadStartPoint = 0; 
                pthread_create(&threadPointer[i], NULL, avgArray, &vectorValues[threadStartPoint]);
            }else if (i < threadCount-1){
				/*Otherwise, if it's not the final row, start at:
				Thread# + (rows per thread * dimensions of the array)
				Essentially shift the pointer up to where it needs to go */
                threadStartPoint = (rowsPerThread*i*arrayDimension); 
                pthread_create(&threadPointer[i], NULL, avgArray, &vectorValues[threadStartPoint]);
            }else if (i == threadCount -1){
				/*The final thread will start off in another location outside this loop,
				in the main thread, the main thread handling the final parts was 
				a design choice that I built off the example in averagePthreads.c*/
                finalStart = i; 
            }
            
        }
		/*The average on the last rows used an irregular length of
		 iterations, hence why I opted to have it run in the main
		 thread, this is possibly not the best and is subject
		 to improvement, but it works*/
		 
        /*The start point for the final row is the same, only it iterates until
		the end using the modulus-calculated row counter*/
        threadStartPoint = (rowsPerThread*finalStart*arrayDimension); 
		/*Set the pointer to the address of the start point of the thread 
		minus a row, the row subtraction is due to the fact that the loop below
		skips the very first row - hence it just goes back one to prevent this*/
        double *arrayPointer = &vectorValues[threadStartPoint-arrayDimension];
        /*This variable is declared here as it is used exclusively in this
		thread to signify the point at which the array pointer should stop*/
		int endpoint = arrayDimension *rowsFinalThread;
		/* Set the loopCounter to 0 */
		loopCounter = 0;
		/* As long as things are happening, signal for another iteration */
		areThingsHappening = 0;
		/* Final thread has launched! */       
		while(loopCounter < endpoint){
			/*If the loopcounter is at its first or last position in the column
			(found by using the modulus operator), skip this iteration*/
		    if((loopCounter)%arrayDimension == 0||(loopCounter+1)%arrayDimension == 0){
				//do nothing
		    }else{
				/*If it is before the endpoint and after at least one iteration
				(as it skips the first row after the endpoint due to the padding*/
		       if(loopCounter <= endpoint && loopCounter > arrayDimension){
    					/*Shfit the pointer around to find its neighbouring values*/
    					oldValue = *arrayPointer;
    					arrayPointer = arrayPointer -1;
    					oneLeft = *arrayPointer;
    					arrayPointer = arrayPointer +2;
    					oneRight = *arrayPointer;
    					arrayPointer = arrayPointer + arrayDimension -1;
    					oneDown =  *arrayPointer;
    					arrayPointer = arrayPointer - arrayDimension*2;
    					oneUp = *arrayPointer;
    					arrayPointer = arrayPointer + arrayDimension;
    					/* set newValue equal to average of 4 surroundings*/
    					newValue = (oneUp + oneDown + oneLeft + oneRight) / 4;
						/*Set the current position to be newValue*/
       				 	*arrayPointer = newValue;
    					/*compare oldValue and newValue to check whether
						it is in precision limits - the absolute is taken
						in case the difference is a negative number*/
    					comparePrecision = fabs(oldValue - newValue);

        				if (comparePrecision > precision) {
						/*If the difference still exceeds precision, loop around again*/
        					areThingsHappening = 1;
        				}
    	        }
		    }
			/*Increment array and loop pointer after its position checks*/
            arrayPointer++;
			loopCounter++;
		}
    /*All threads get collected at the end here, and potentially sent
	out again if the precision has not been met*/
        for( i=0; i < threadCount-1; i ++){
            pthread_join(threadPointer[i], NULL);
            }

	}while(areThingsHappening == 1);
	
    
	/*Used for debugging: print the output vector to the console log */
	/*printvalue = 0;	
	for (q = 0; q < arrayDimension; ++q) {
		for (p = 0; p < arrayDimension; ++p) {
			printf("%lf ", vectorValues[printvalue]);
			printvalue++;
		}
		printf("\n");
	}
	printf("\n");
	*/
	printvalue = 0;	
	FILE *fOut = fopen("output.csv", "w"); 
	for (q = 0; q < arrayDimension; ++q) {
		for (p = 0; p < arrayDimension; ++p) {
		    fprintf(fOut, "%lf ", vectorValues[printvalue]); 
			printvalue++;
		}
		if(q != arrayDimension-1){ //last line doesn't need ;
		/*Note that I add an ; after every row - this is due to
		the fact that it made number crunching easier */
		fprintf(fOut,";\n");
		}
}
    fclose(fOut);
    return 0;
}
