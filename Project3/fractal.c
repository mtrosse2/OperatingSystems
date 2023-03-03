/*
fractal.c - Mandelbrot fractal generation
Starting code for CSE 30341 Project 3 - Spring 2023
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>
#include <stdbool.h>

#include "bitmap.h"
#include "fractal.h"
#include <ctype.h>

/*
Compute the number of iterations at point x, y
in the complex space, up to a maximum of maxiter.
Return the number of iterations at that point.

This example computes the Mandelbrot fractal:
z = z^2 + alpha

Where z is initially zero, and alpha is the location x + iy
in the complex plane.  Note that we are using the "complex"
numeric type in C, which has the special functions cabs()
and cpow() to compute the absolute values and powers of
complex values.
*/

// Create row-thread info
struct rowthread_info {
	struct bitmap * pBitmap;
	struct FractalSettings * pSettings;
	int nIndex;
	int rowMin;
	int rowMax;
	pthread_t ThreadID;
};



static int compute_point( double x, double y, int max )
{
	double complex z = 0;
	double complex alpha = x + I*y;

	int iter = 0;

	while( cabs(z)<4 && iter < max ) {
		z = cpow(z,2) + alpha;
		iter++;
	}

	return iter;
}

/*
Compute an entire image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax).

HINT: Generally, you will want to leave this code alone and write your threaded code separately

*/

void compute_image_singlethread ( struct FractalSettings * pSettings, struct bitmap * pBitmap)
{
	int i,j;

	// For every pixel i,j, in the image...

	for(j=0; j<pSettings->nPixelHeight; j++) {
		for(i=0; i<pSettings->nPixelWidth; i++) {

			// Scale from pixels i,j to coordinates x,y
			double x = pSettings->fMinX + i*(pSettings->fMaxX - pSettings->fMinX) / pSettings->nPixelWidth;
			double y = pSettings->fMinY + j*(pSettings->fMaxY - pSettings->fMinY) / pSettings->nPixelHeight;

			// Compute the iterations at x,y
			int iter = compute_point(x,y,pSettings->nMaxIter);

			// Convert a iteration number to an RGB color.
			// (Change this bit to get more interesting colors.)
			int gray = 255 * iter / pSettings->nMaxIter;

            // Set the particular pixel to the specific value
			// Set the pixel in the bitmap.
			bitmap_set(pBitmap,i,j,gray);
		}
	}
}

//
void compute_image_rowthread (void *pData1)
{
	int i, j;

	struct rowthread_info * pData  = (struct rowthread_info *) pData1;

	struct FractalSettings * pSettings;
	pSettings = pData->pSettings;

	struct bitmap * pBitmap;
	pBitmap = pData->pBitmap;

	for(i=pData->rowMin; i<pData->rowMax; i++){
		for(j=0; j<pSettings->nPixelWidth; j++) {

			// Scale from pixels i,j to coordinates x,y
			double x = pSettings->fMinX + i*(pSettings->fMaxX - pSettings->fMinX) / pSettings->nPixelWidth;
			double y = pSettings->fMinY + j*(pSettings->fMaxY - pSettings->fMinY) / pSettings->nPixelHeight;

			// Compute the iterations at x,y
			int iter = compute_point(x,y,pSettings->nMaxIter);

			// Convert a iteration number to an RGB color.
			// (Change this bit to get more interesting colors.)
			int gray = 255 * iter / pSettings->nMaxIter;

      // Set the particular pixel to the specific value
			// Set the pixel in the bitmap.
			bitmap_set(pBitmap,i,j,gray);
		}
	}

}


/* Process all of the arguments as provided as an input and appropriately modify the
   settings for the project 
   @returns 1 if successful, 0 if unsuccessful (bad arguments) */
bool is_number (char number[]){

		// Found from Stack Overflow
		int i = 0;

  	//checking for negative numbers
    if (number[0] == '-') {
        i = 1;
		}
    for (; number[i] != 0; i++) {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
		return true;
}


char processArguments (int argc, char * argv[], struct FractalSettings * pSettings)
{
	int i = 0;
	bool result;
		for(i = 1; i < argc; i++){

			if(strcmp(argv[i], "-help") == 0){
				printf("-help					Display the help information\n");
				printf("-xmin X				New value for x min\n");
				printf("-xmax X				New value for x max\n");
				printf("-ymin Y				New value for y min\n");
				printf("-ymax Y				New Value for y max\n");
				printf("-maxiter N		New Value for the maximum number of iterations (must be an integer)\n");
				printf("-width W			New width for the output image\n");
				printf("-height H			New height for the output image\n");
				printf("-output F			New name for the output file\n");
				printf("-threads N		Number of threads to use for processing (default is 1)\n");
				printf("-row					Run using a row-based approach\n");
				printf("-task					Run using a thread-based approach\n");
				return 0;
			}

			else if(i < argc - 1 && strcmp(argv[i],"-xmin") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					pSettings->fMinX = atof(argv[i+1]);
					i++;
				}
			}

			else if(i < argc - 1 && strcmp(argv[i],"-xmax") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					pSettings->fMaxX = atof(argv[i+1]);
					i++;
				}
			}

			else if(i < argc - 1 && strcmp(argv[i],"-ymin") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					pSettings->fMinY = atof(argv[i+1]);
					i++;
				}
			}

			else if(i < argc - 1 && strcmp(argv[i],"-ymax") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					pSettings->fMaxY = atof(argv[i+1]);
					i++;
				}
			}

			else if(i < argc - 1 && strcmp(argv[i],"-maxiter") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					pSettings->nMaxIter = atoi(argv[i+1]);
					i++;
				}
			}

			else if(i < argc - 1 && strcmp(argv[i],"-width") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					pSettings->nPixelWidth = atoi(argv[i+1]);
					i++;
				}
			}

			else if(i < argc - 1 && strcmp(argv[i],"-height") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					pSettings->nPixelHeight = atoi(argv[i+1]);
					i++;
				}
			}
			
			else if(i < argc - 1 && strcmp(argv[i], "-output") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					strcpy(pSettings->szOutfile,argv[i+1]);
					i++;
				}
			}

			else if( i < argc - 1 && strcmp(argv[i],"-threads") == 0){
				result = is_number(argv[i+1]);
				if(result == true){
					pSettings->nThreads = atoi(argv[i+1]);
					i++;
				}
			}

			else if(strcmp(argv[i],"-row") == 0){
				pSettings->theMode = MODE_THREAD_ROW;
			}

			else if(strcmp(argv[i], "-task") == 0){
				pSettings->theMode = MODE_THREAD_TASK;
			}

			else{
				continue;
			}
		}
	// RETURN SOMETHING DIFFERENT HERE LATER
	return 1;
}




int main( int argc, char *argv[] )
{
    struct FractalSettings theSettings;

	// The initial boundaries of the fractal image in x,y space.
    theSettings.fMinX = DEFAULT_MIN_X;
    theSettings.fMaxX = DEFAULT_MAX_X;
    theSettings.fMinY = DEFAULT_MIN_Y;
    theSettings.fMaxY = DEFAULT_MAX_Y;
    theSettings.nMaxIter = DEFAULT_MAX_ITER;

    theSettings.nPixelWidth = DEFAULT_PIXEL_WIDTH;
    theSettings.nPixelHeight = DEFAULT_PIXEL_HEIGHT;

    theSettings.nThreads = DEFAULT_THREADS;
    theSettings.theMode  = MODE_THREAD_SINGLE;
    
    strncpy(theSettings.szOutfile, DEFAULT_OUTPUT_FILE, MAX_OUTFILE_NAME_LEN);

    /* TODO: Adapt your code to use arguments where the arguments can be used to override 
             the default values 

        -help         Display the help information
        -xmin X       New value for x min
        -xmax X       New value for x max
        -ymin Y       New value for y min
        -ymax Y       New value for y max
        -maxiter N    New value for the maximum number of iterations (must be an integer)     
        -width W      New width for the output image
        -height H     New height for the output image
        -output F     New name for the output file
        -threads N    Number of threads to use for processing (default is 1) 
        -row          Run using a row-based approach        
        -task         Run using a thread-based approach

        Support for setting the number of threads is optional

        You may also appropriately apply reasonable minimum / maximum values (e.g. minimum image width, etc.)
    */


   /* Are there any locks to set up? */


   if(processArguments(argc, argv, &theSettings))
   {
        /* Dispatch here based on what mode we might be in */
        if(theSettings.theMode == MODE_THREAD_SINGLE)
        {
            /* Create a bitmap of the appropriate size */
            struct bitmap * pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

            /* Fill the bitmap with dark blue */
            bitmap_reset(pBitmap,MAKE_RGBA(0,0,255,0));

            /* Compute the image */
            compute_image_singlethread(&theSettings, pBitmap);

            // Save the image in the stated file.
            if(!bitmap_save(pBitmap,theSettings.szOutfile)) {
                fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));
                return 1;
            }            
        }
        else if(theSettings.theMode == MODE_THREAD_ROW)
        {
            /* A row-based approach will not require any concurrency protection */
            struct bitmap * pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

            bitmap_reset(pBitmap,MAKE_RGBA(0,0,255,0));
            
						struct rowthread_info thread_info[theSettings.nThreads];

						int num_rows_per_thread = theSettings.nPixelHeight/theSettings.nThreads;

						int i;
						for(i=0; i<theSettings.nThreads; i++){
								thread_info[i].nIndex = i;
								thread_info[i].rowMin = i * num_rows_per_thread;
								thread_info[i].rowMax = (i+1) * num_rows_per_thread - 1;
								thread_info[i].pBitmap = pBitmap;
								thread_info[i].pSettings = &theSettings;
						}

						for(i=0; i<theSettings.nThreads; i++)
								pthread_create(&thread_info[i].ThreadID, NULL, compute_image_rowthread, (void *)&thread_info[i]);

						for(i=0; i<theSettings.nThreads; i++)
								pthread_join(thread_info[i].ThreadID, NULL);
						
						
            if(!bitmap_save(pBitmap,theSettings.szOutfile)) {
                fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));
                return 1;
            }            
						
						/* Could you send an argument and write a different version of compute_image that works off of a
               certain parameter setting for the rows to iterate upon? */
        }
        else if(theSettings.theMode == MODE_THREAD_TASK)
        {
            /* For the task-based model, you will want to create some sort of a way that captures the instructions
               or task (perhaps say a startX, startY and stopX, stopY in a struct).  You can have a global array 
               of the particular tasks with each thread attempting to pop off the next task.  Feel free to tinker 
               on what the right size of the work unit is but 20x20 is a good starting point.  You are also welcome
               to modify the settings struct to help you out as well.  
               
               Generally, it will be good to create all of the tasks into that array and then to start your threads
               with them in turn attempting to pull off a task one at a time.  
               
               While we could do condition variables, there is not really an ongoing producer if we create all of
               the tasks at the outset. Hence, it is OK whenever a thread needs something to do to try to access
               that shared data structure with all of the respective tasks.  
               */
        }
        else 
        {
            /* Uh oh - how did we get here? */
        }
   }
   else
   {
        /* Probably a great place to dump the help */

        /* Probably a good place to bail out */
        exit(-1);
   }

    /* TODO: Do any cleanup as required */

	return 0;
}
