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

//Declare functions
void exit_prog(int status);
int string_breakdown(char *string);
void run_start(char *command);
int get_pid(char *command);
int get_pid_kill(char * command);
void event_handler(int idk);

//Declare global
int parent_pid;
int pids[1000] = {0};

int main(int argc, char *argv[]){
	//Declare variables and flags
	int i;
	int wpid;
	int status = 0;
	int to_wait = 0;
	int pid_to_wait_for = 0;
	int kill_pid = 0;
	//int pids[1000] = {0};
	int count = 0;
	int found;

	parent_pid = getpid();
	signal(SIGINT, event_handler);

	//Check there are no arguments inputted
	if(argc > 1){
		printf("Too many arguments\n");
		exit(1);
	}

	char command[1000];
	
	//Infinite loop waiting for user input
	while(1){
		sleep(1);
		//Wait for and get user input
		printf("ndshell> ");

		for(i = 0; i < 1000; i++)
			command[i] = '\0';

		
		fgets(command, 1000, stdin);

		//If they choose exit
		if(strcmp(command, "exit\n") == 0 || strcmp(command, "exit \n") == 0) {
			exit_prog(0);

		//If they choose to wait
		} else if(strcmp(command, "wait\n") == 0 || strcmp(command, "wait \n") == 0) {
			to_wait = 1;

		//If they choose waitfor get the pid they want to wait for
		} else if (strlen(command) > 6 && command[0] == 'w' && command[1] == 'a' && command[2] == 'i' && command[3] == 't' && command[4] == 'f' && command[5] == 'o' && command[6] == 'r' && command[7] == ' '){
			to_wait = 2;
			pid_to_wait_for = get_pid(command);

		//If they choose kill get the pid
		} else if (strlen(command) > 5 && command[0] == 'k' && command[1] == 'i' && command[2] == 'l' && command[3] == 'l' && command[4] == ' '){
			kill_pid = get_pid_kill(command);
			kill(kill_pid, 9);
			printf("ndshell: process %d exited abnormally with signal 9: Killed.\n", kill_pid);

		//If they choose quit
		} else if (strlen(command) > 3 && command[0] == 'q' && command[1] == 'u' && command[2] == 'i' && command[3] == 't'){
			//Clean up all processes
			for(i=0; i<1000; i++){
				if(pids[i] != 0){
					kill(pids[i], 9);
					printf("ndshell: process %d exited abnormally with signal 9: Killed.\n", pids[i]);
				}
			}
			
			printf("All child processes complete - exiting the shell.\n");
			exit(0);

		} else {
			// check that they input start or run
			char* jackson_command = malloc(1000*sizeof(char));
			strcpy(jackson_command, command);
			char* delim = " ";
			char* newer_command = strtok(jackson_command, delim);

			if ( strcmp(newer_command, "start") == 0 || strcmp(newer_command, "run") == 0){
				//int cpid = fork();
				//printf("\nTHIS: %s\n\n", command);
			}
			else{
				printf("Improper argument inputed\n");
				free(jackson_command);
				continue;
			}
			
			// Start a child process
			free(jackson_command);
			int cpid = fork();
			if(cpid < 0){
				printf("Error: fork failed\n");
				exit_prog(2);
			}

			if(cpid == 0){
				to_wait = string_breakdown(command);

				exit(0);
			}

			// If run inputted then wait
			if(strlen(command) > 4 && command[0] == 'r' && command[1] == 'u' && command[2] == 'n' && command[3] == ' '){
				to_wait = 2;
				pid_to_wait_for = cpid;
			}

			pids[count] = cpid;
			if (count >= 1000)
				count = -1;
			
			count++;



		}
		// If they input wait, then wait for the last process to finish.
		if(to_wait == 1){
			found = 0;

			for(i = 0; i < 1000; i++){
				if(pids[i] != 0)
					found = 1;
			}

			if(found == 0){
				printf("No child processes running\n");
				to_wait = 0;
				continue;
			}
			
			wpid = wait(NULL);

			//Change this if we want the array to be more than 1000 elements
			for(i=0; i<1000; i++){
				if (pids[i] == wpid){
					pids[i] = 0;
					break;
				}
			}
			
			// Check the status
			if(status == 0)
				printf("Process exited normally\n");

			else
				printf("Process %d failed with an exit status of %d\n", wpid, status);

			to_wait = 0;
		
		// If they chose run or waitfor, wait for the specified process
		} else if(to_wait == 2){
			found = 0;

			// Make sure the input is valid
			for(i = 0; i < 1000; i++){
				if(pids[i] == pid_to_wait_for)
					found = 1;
			}

			if(found == 0){
				printf("Specified pid not found\n");
				to_wait = 0;
				continue;
			}
			wpid = wait(pid_to_wait_for);
			
			for(i=0; i<1000; i++){
				if (pids[i] == pid_to_wait_for){
					pids[i] = 0;
					break;
				}
			}

			// Check the status
			if(status == 0)
				printf("Process %d exited normally with a status %d\n", pid_to_wait_for, status);

			else
				printf("Process %d failed with an exit status of %d\n", wpid, status);

			to_wait = 0;
		}
	
	}

}

// Alert the user and exit program
void exit_prog(int status){
	
	if(status == 0){
		printf("Exiting shell immediately\n");
		exit(status);
	
	} else {
		exit(status);
	}


}

//Function to execute the start or run command 
int string_breakdown(char *string){
	int i;
	int start;
	char newCommand[1000];
	
	// Initialize to null
	for(i = 0; i < 1000; i++)
		newCommand[i] = '\0';

	
	// Set the start of the command to the correct index
	if (strlen(string) > 6 && string[0] == 's' && string[1] == 't' && string[2] == 'a' && string[3] == 'r' && string[4] == 't' && string[5] == ' '){
		start = 6;

	} else if (strlen(string) > 4 && string[0] == 'r' && string[1] == 'u' && string[2] == 'n' && string[3] == ' '){
		start = 4;

	// Exit if there is invalid input
	} else {
		printf("ndshell: Invalid command: %s", string);
		exit(0);
	}

	// Get the system command the user inputted
	for(i = start; i < strlen(string); i++){
		newCommand[i-start] = string[i];
	}

	// Inform the user and start the system call for start
	printf("ndshell: process %d started\n", getpid());

	if(start == 6){
		run_start(newCommand);
		printf("Process %d completed\n", getpid());
		return 0;
	}

	// Start the system call for run
	if(start == 4){
		run_start(newCommand);
		printf("Process %d completed\n", getpid());
		return 0;
	}

	return 0;
}


// Run the system command that the user inputed 
void run_start(char *command){
	char* new_command = strtok(command, "\n");
	strcpy(command, new_command);
	char* arguments[1000];
	for(int i = 0; i < 1000; i++){
		arguments[i] = NULL;
	}

	char * result;
	result = strtok(command, " ");

	int i = 0;
	while(result != NULL){
		arguments[i] = strdup(result);
		result = strtok(NULL, " ");
		i = i + 1;
	}

	// execture the mofo
	execvp(arguments[0], arguments);
	printf("Got here\n");
	exit(3);
}

// Get the pid the user inputed for waitfor
int get_pid(char *command){
	int i;
	char pid_str[1000];
	int result;

	for(i=8; i<strlen(command); i++){
		pid_str[i-8] = command[i];
	}

	result = atoi(pid_str);

	return result;
}

// Get the pid the user inputed for the kill command
int get_pid_kill(char * command){
	int i;
	char pid_str[1000];
	int result;

	for(i=5; i<strlen(command); i++){
		pid_str[i-5] = command[i];
	}

	result = atoi(pid_str);

	return result;
}

// Handle control-c input
void event_handler(int sig) {
	int i;
	int found = 0;

	// Check that there is a child process running and kill the most recent one
	if(sig == SIGINT){

		for(i=999; i>=0; i--){
			if(pids[i] != 0){
				found = 1;
				kill(pids[i], 9);
				pids[i] = 0;
				printf("\nndshell: Control-c pressed: %d exited abnormally with signal 9: Killed.\n", pids[i]);
				break;
			}
		}
	} 
	
	// Inform the user if there is no child process
	if(found == 0 && getpid() == parent_pid && sig == SIGINT)
		printf("\nndshell: No current child processes running\n");
	
	return;

}




