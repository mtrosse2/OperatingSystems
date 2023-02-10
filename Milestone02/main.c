#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>

// Declare global parent id

//Define functions
void event_handler(int idk);

int main(int argc, char* argv[]) {
	if(argc > 1){
		printf("Error: too many arguments!!!!!!\n");
		return 1;
	}

	//Handle control-c
	signal(SIGINT, event_handler);

	char command[1000];
	printf("Execute?????");
	fgets(command, 1000, stdin);


	//Tread the process
	int cpid = fork();
	
	if(cpid < 0){
		printf("Error: Fork Failed\n");
		return 2;
	}
	
	//Have the child ask for input and execute the command so the parent function can handle control-c
	else if (cpid == 0){
		char* new_command = strtok(command, "\n");
		strcpy(command, new_command);
		// need to initialize to NULL
		char* arguments[1000];
		for(int i = 0; i < 1000; i++){
			arguments[i] = NULL;
		}
		

		char* token;
		token = strtok(command, " ");

		int i = 0;
		while(token != NULL){
			arguments[i] = strdup(token);
			token = strtok(NULL, " ");
			i = i + 1;
		}	
		
		execvp(arguments[0], arguments);
		printf("Error: Execvp failed! Please enter valid command\n");
		exit(3);

	}
	else {
		int status;
		if ( waitpid(cpid, &status, 0) == -1 ) {
			perror("wait() failed");
			exit(EXIT_FAILURE);
		}
		if ( status != 0 ) {
			printf("Error: child program exited because execvp failed\n");
		}
		else {
			printf("Execution Complete!\n");
		}
	}

	
	return 0;
}


//Function to print to the screen and exit when control-c is pressed
//
/*
void event_handler(int idk) {
	if(getpid() == pid){
		printf("\nControl-c was pressed .. exiting\n");
		exit(1);
	}
}
*/

void event_handler(int sig) {
	if(sig == SIGINT){
		printf("\nControl-c was pressed . . . exiting\n");
		kill(getpid(), SIGINT);
		exit(0);
	
	}


}



