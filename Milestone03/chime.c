#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

int return_threadNum(char *command);
float return_sleepTime(char *command);

char g_bKeepLooping = 1;
#define MAX_THREADS 5


struct ChimeThreadInfo
{
	int        nIndex;
  float      fChimeInterval;
  char       bIsValid;
  pthread_t  ThreadID;
};


struct ChimeThreadInfo  TheThreads[MAX_THREADS];


void *ThreadChime (void *pData)
{
    struct ChimeThreadInfo  *pThreadInfo;
    
		/* Which chime are we? */
    pThreadInfo = (struct ChimeInfo *) pData;
    
		while(g_bKeepLooping)
    {
    	sleep(pThreadInfo->fChimeInterval);
    	printf("Ding - Chime %d with an interval of %f s!\n", pThreadInfo->nIndex, pThreadInfo->fChimeInterval);
    }
    
		return NULL;
}


#define BUFFER_SIZE 1024
int main (int argc, char *argv[])
{
	int threadNum;
	float sleepTime;

	char szBuffer[BUFFER_SIZE];
	
	/* Set all of the thread information to be invalid (none allocated) */
	for(int j=0; j<MAX_THREADS; j++)
	{
        TheThreads[j].bIsValid = 0;
  }

  while(1)
  {
    /* Prompt and flush to stdout */
		printf("CHIME> ");
		fflush(stdout);


    
		/* Wait for user input via fgets */
    fgets(szBuffer, BUFFER_SIZE, stdin);
    
		char *command = malloc(1000 * sizeof(char));
		strcpy(command, szBuffer);
		char *delim = " ";
		char *new_command = strtok(command, delim);
		
		
		/* If the command is quit - join any active threads and finish up gracefully */
		if(strcmp(new_command, "exit\n") == 0 || strcmp(new_command, "exit") == 0)
		{	
			// Update global
			g_bKeepLooping = 0;
			int i;

			// Loop through and join each chime if it was started
			for(i=0; i<MAX_THREADS; i++){
				if(TheThreads[i].bIsValid == 1){
					pthread_join(TheThreads[i].ThreadID, NULL);
					printf("Thread %ld was joined\n", TheThreads[i].ThreadID);
				}
			}

			// Leave
			printf("Exit 	chime program...\n");
			break;
		}
				
		/* If the command is chime, the second argument is the chime number (integer) and the 
		 * third number is the new interval (floating point). If necessary, start the thread as needed */
		else if (strcmp(new_command, "chime") == 0)
		{
			// Get inputted times
			char *temp = malloc(1000 * sizeof(char));
			strcpy(temp, szBuffer);

			threadNum = return_threadNum(szBuffer);
	
			if (threadNum == -1)
				continue;

			sleepTime = return_sleepTime(temp);

			memset(temp, '\0', 1000);
			free(temp);

			if(sleepTime == -1.0)
				continue;

			// If the chime is already running then update it
			if(TheThreads[threadNum].bIsValid == 1)
			{
				printf("Chime %d's sleep time was changed to %f\n", threadNum, sleepTime);
				TheThreads[threadNum].fChimeInterval = sleepTime;
				continue;
			}

			// Update values in the struct
			TheThreads[threadNum].nIndex = threadNum;
			TheThreads[threadNum].fChimeInterval = sleepTime;
			TheThreads[threadNum].bIsValid = 1;
			
			// Create the thread
			pthread_create(&TheThreads[threadNum].ThreadID, NULL, ThreadChime, (void *) &TheThreads[threadNum]);
			printf("Starting thread %ld for chime %d, interval is %f s\n", TheThreads[threadNum].ThreadID, TheThreads[threadNum].nIndex, TheThreads[threadNum].fChimeInterval);
		}
		
		else
		{
			printf("Unknown command/Invalid input: %s\n", new_command);
		}

		memset(command, '\0', 1000);
		memset(new_command, '\0', 1000);

		free(command);
				
		/* Optionally, provide appropriate protection against changing the
       chime interval and reading it in a thread at the same time by using a
       mutex.  Note that it is not strictly necessary to do that */
	
	}

	return 0;
}


// Function to get the chime number that they want
int return_threadNum(char *command)
{
	// Use strtok to get the inputted integer value
	char* new_command = strtok(command, "\n");
	strcpy(command, new_command);
	char* arguments[1000];
	for(int i = 0; i < 1000; i++){
		arguments[i] = NULL;
	}

	char * result;
	result = strtok(command, " ");

	int i = 0;
	while(result != NULL)
	{
		arguments[i] = strdup(result);
		result = strtok(NULL, " ");
		i = i + 1;
	}
	
	// Check that there are two arguments
	if(arguments[2] == NULL){
		printf("Invalid number of arguments\n");
		return -1;
	}
	
	// Check the user inputted a number
	for(i=0; i<strlen(arguments[1]); i++ )
	{
		if(isdigit(arguments[1][i]) == 0)
		{
			printf("Number value not inputted\n");
			return -1.0;
		}
	}

	// Return the inputted integer value
	int val = atoi(arguments[1]);

	if(val>=0 && val<MAX_THREADS)
		return val;

	// Return -1 if something invalid was inputted
	printf("Invalid thread number inputted\n");
	return -1;
}


// Function to get the inputted sleep time
float return_sleepTime(char *command)
{
	// Use strtok to get the float value
	char* new_command = strtok(command, "\n");
	strcpy(command, new_command);
	char* arguments[1000];
	for(int i = 0; i < 1000; i++){
		arguments[i] = NULL;
	}

	char * result;
	result = strtok(command, " ");

	int i = 0;
	while(result != NULL)
	{
		arguments[i] = strdup(result);
		result = strtok(NULL, " ");
		i = i + 1;
	}

	// Check the number of arguments
	if(arguments[2] == NULL){
		printf("Invalid number of arguments\n");
		return -1.0;
	}
	
	// Check that the user inputted a number
	int decimal_count = 0;
	for(i=0; i<strlen(arguments[2]); i++)
	{
		if(arguments[2][i] == '.')
			decimal_count++;

		if(decimal_count >= 2){
			printf("Invalid number input\n");
			return -1.0;
		}

		if(isdigit(arguments[2][i]) == 0 && arguments[2][i] != '.')
		{
			printf("Number value not inputted\n");
			return -1.0;
		}
	}
	
	//Check if the float value is valid and then return it
	float val = strtof(arguments[2], NULL);
	
	if(val > 0)
		return val;

	//Return -1.0 if it is bad input
	printf("Invalid sleep time inputted\n");
	return -1.0;
}



