#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

//Function to return the file size
int get_file_size(FILE *fp);
void print_regular(char *fo_name, char *buf);
void print_reverse(char *fo_name, char *buf);
void usage();
void print_reverse_noflip(char *fo_name, char *buf);

int main(int argc, char *argv[]){
	int i, j;
	int changed = 0;
	int max = 25000;
	int r_and_b = 0;
	int reverse = 0;
	char *file_name;
	int found = 0;
	char *infile;

	//Check command line input
	if(argc < 2){
		printf("Wrong number of command line arguments\n");
		return(3);
	}

	for(i = 1; i < argc; i++){
		printf("%c\n", argv[i][strlen(argv[i]) - 3]);
		if (strcmp(argv[i], "-help") == 0){
			usage();
			return(1);
		
		} else if (strcmp(argv[i], "-o") == 0){
			i++;
			file_name = (char *)malloc(strlen(argv[i]) * sizeof(char));
			//COPY IT HERE.....
			
			changed = 1;

		} else if (strcmp(argv[i], "-maxsize") == 0) {
			i++;
			max = atoi(argv[i]);
		
		} else if (strcmp(argv[i], "-r") == 0){
			reverse = 1;

		} else if (strcmp(argv[i], "-bfr") == 0) {
			r_and_b = 1; 
		
		}else if (strlen(argv[i]) > 4 && argv[i][strlen(argv[i])-4] == '.' && argv[i][strlen(argv[i])-3] == 't' && argv[i][strlen(argv[i])-2] == 'x' && argv[i][strlen(argv[i])-1] == 't') {

			infile = (char *)malloc(strlen(argv[i]) * sizeof(char));

			for(j=0; j<strlen(argv[i]); j++)
				infile[j] = argv[i][j];
			
			//if (changed == 0){
			//	file_name = (char *)malloc(strlen(argv[i]) * sizeof(char));
			//	file_name = argv[i];
			//}
			file_name = "test.txt";

			found = 1;

		}

	}


	if(found != 1){
		printf("Input a file name\n");
		return(5);
	}
	
	//Open the file
	FILE *fp = fopen(infile, "rb");


	//printf("1\n");

	if(fp == NULL){
		printf("File not found\n");
		return(4);
	}

	//printf("2\n");

	// Check the size
	int size = get_file_size(fp);
	
	//printf("3\n");
	
	if(size > max){
		printf("File is too large\n");
		return(2);
	}

	//printf("");
	//Creates a buffer to read in the contents of the file.
	char *buf = (char *)malloc((size+1) * sizeof(char));

	for(i=0; i<size; i++){
		fread(*&buf+i, 1, 1, fp);
	}
	
	
	//change this to file)name
	print_regular(argv[1], buf);

	if(r_and_b == 1)
		print_reverse(argv[1], buf);
	if(reverse == 1)
		print_reverse_noflip(argv[1], buf) ;
			

	fclose(fp);
	free(buf);
	free(infile);
	free(file_name);


	return(0);


}

//Function to get the file size
int get_file_size(FILE *fp){
	int begin = ftell(fp);
	fseek(fp, 0, SEEK_END);

	int size = ftell(fp);
	fseek(fp, begin, SEEK_SET);

	return size;

}

//Usage Function
void usage(){
	printf("Filename	Ending in .txt\n");
	printf("-o XXX		Change output filename\n");
	printf("-maxsize XXX	Change max file size\n");
	printf("-r		Reverse the byte order without doing bitflip\n");
}


void print_regular(char *fo_name, char *buf){
		
	char *names = malloc(strlen(fo_name)+3*sizeof(char));
	
	int i;
	for(i=0; i<strlen(fo_name); i++){
		names[i] = fo_name[i];
	}

	names[strlen(fo_name)] = '.';
	names[strlen(fo_name)+1] = 'b';
	names[strlen(fo_name)+2] = 'f';

	FILE *fn = fopen(names, "w");
	char temp;

	for(i=0; i<strlen(buf); i++){
		temp = buf[i];
		temp = temp ^ 0xFF;
		fwrite(&temp, sizeof(temp), 1, fn);
	}

	fclose(fn);	

	//free( names );
	free(names);

	//printf("exit\n");
}


void print_reverse(char *fo_name, char *buf){

	printf("Enter");
	char *name = malloc(strlen(fo_name)+4*sizeof(char));
	
	int i;
	for(i=0; i<strlen(fo_name); i++){
		name[i] = fo_name[i];
	}
	name[strlen(fo_name)] = '.';
	name[strlen(fo_name)+1] = 'b';
	name[strlen(fo_name)+2] = 'f';
	name[strlen(fo_name)+3] = 'r';

	FILE *fp = fopen(name, "w");
	char temp;

	for(i=strlen(buf)-1; i>=0; i++){
		temp = buf[i];
		temp = temp ^ 0xFF;
		fwrite(&temp, sizeof(temp), 1, fp);
	}


	fclose(fp);
	free(name);

}


void print_reverse_noflip(char *fo_name, char *buf){

	printf("Enter");
	char *name = malloc(strlen(fo_name)+2*sizeof(char));
	
	int i;
	for(i=0; i<strlen(fo_name); i++){
		name[i] = fo_name[i];
	}
	name[strlen(fo_name)] = '.';
	name[strlen(fo_name)+1] = 'r';


	FILE *fp = fopen(name, "w");
	char temp;

	for(i=strlen(buf)-1; i>=0; i++){
		temp = buf[i];
		fwrite(&temp, sizeof(temp), 1, fp);
	}


	fclose(fp);
	free(name);

}



