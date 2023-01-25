#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_file_size(FILE *fp);

int main(int argc, char *argv[]){
	//Check command line input
	if(argc != 3){
		printf("Wrong number of command line arguments\n");
		return(3);
	}

	if( argv[2][0] != '0' || argv[2][1] != 'x'){
		printf("Input a hexadecimal value to find in format 0x...\n");
		return(4);
	}

	
	//Open the file
	FILE *fp = fopen(argv[1], "rb");

	if(fp == NULL){
		printf("File not found\n");
		return(1);
	}

	// Check the size
	int size = get_file_size(fp);
	
	if(size > 25000){
		printf("File is too large\n");
		return(2);
	}

	int i;

	char *buf = (char *)malloc((size+1) * sizeof(char));

	for(i=0; i<size; i++){
		fread(*&buf+i, 1, 1, fp);
	}


	int val = (int)strtol(argv[2], NULL, 0);

	int temp;
	int counter = 0;

	for(i=0; i<strlen(buf); i++){
		temp = (int) buf[i];

		if(temp == val)
			counter++;

	}

	printf("Your binary value was found %d time(s).\n", counter);

	fclose(fp);
	free(buf);


	return(0);


}

int get_file_size(FILE *fp){
	int begin = ftell(fp);
	fseek(fp, 0, SEEK_END);

	int size = ftell(fp);
	fseek(fp, begin, SEEK_SET);

	return size;

}


