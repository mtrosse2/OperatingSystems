#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

//Declare global parent id
pid_t pid;

//Define functions
void event_handler(int idk);

int main(int argc, char* argv[]) {
	if(argc > 1){
		printf("Error: too many arguments\n");
		return 1
	}
	//Get the parent id
	pid = getpid();
	char command[1000];

	//Handle control-c
	signal(SIGINT, event_handler);

	//Tread the process
	int cpid = fork();

	//Have the child ask for input and execute the command so the parent function can handle control-c
	if (cpid == 0){
		printf("Execute? ");
		fgets(command, 1000, stdin);
		system(command);
		return 0;
	}

	//Wait for the child to finish
	wait(NULL);
	
	//Notify that execution was complete and return success
	printf("Execution complete!\n");
	return(0);
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
		kill(getpid() == SIGINT);
		exit(0)
	
	}


}



