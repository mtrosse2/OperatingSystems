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


void exit_prog(int status);
int string_breakdown(char *string);
void run_start(char *command);

int main(int argc, char *argv[]){
	int i;
	int wpid;
	int status = 0;
	int to_wait = 0;

	if(argc > 1){
		exit(1);
	}

	char command[1000];
	
	while(1){
		sleep(1);
		printf("ndshell> ");

		for(i = 0; i < 1000; i++)
			command[i] = '\0';

		fgets(command, 1000, stdin);

		if(strcmp(command, "exit\n") == 0) {
			exit_prog(0);

		} else {
			to_wait = string_breakdown(command);
		}

		if(to_wait == 1){
			wpid = wait(NULL);
			//waitpid(wpid, &status, 0);
			
			if(status == 0)
				printf("Process exited normally\n");

			else
				printf("Process %d failed with an exit status of %d\n", wpid, status);
		}

			
	}


}

void exit_prog(int status){
	
	if(status == 0){
		printf("Exiting shell immediately\n");
		exit(status);
	
	} else {
		exit(status);
	}


}


int string_breakdown(char *string){
	int i;
	int start;
	char newCommand[1000];

	for(i = 0; i < 1000; i++){
		//string[i] = '\0';
		newCommand[i] = '\0';
	}


	if (strlen(string) > 6 && string[0] == 's' && string[1] == 't' && string[2] == 'a' && string[3] == 'r' && string[4] == 't' && string[5] == ' '){
		start = 6;

	} else if (strlen(string) > 3 && string[0] == 'w' && string[1] == 'a' && string[2] == 'i' && string[3] == 't'){
		start = 5;
		return 1;

	} else if (strlen(string) > 6 && string[0] == 'w' && string[1] == 'a' && string[2] == 'i' && string[3] == 't' && string[4] == 'f' && string[5] == 'o' && string[6] == 'r' && string[7] == ' '){
		start = 8;

	} else if (strlen(string) > 4 && string[0] == 'r' && string[1] == 'u' && string[2] == 'n' && string[3] == ' '){
		start = 4;

	} else if (strlen(string) > 3 && string[0] == 'q' && string[1] == 'u' && string[2] == 'i' && string[3] == 't'){
		exit_prog(0);

	} else if (strlen(string) > 4 && string[0] == 'k' && string[1] == 'i' && string[2] == 'l' && string[3] == 'l'){
		exit_prog(0);

	} else {
		printf("Invalid command\n");
		return 0;
	}

	for(i = start; i < strlen(string); i++){
		newCommand[i-start] = string[i];
	}


	int cpid = fork();

	if(cpid < 0){
		printf("Error: fork failed\n");
		exit_prog(2);
	}

	if (cpid != 0){
		if(start == 6)
			return 0;
		else
			return 1;

	} else {

		printf("ndshell: process %d started\n", getpid());
		if(start == 6)
			run_start(newCommand);
		
		printf("Process %d completed\n", getpid());
		exit(0);
	}

}



void run_start(char *command){
	system(command);
}

