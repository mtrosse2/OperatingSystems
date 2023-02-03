#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <dirent.h>

//Function to return the file size
int get_file_size(FILE *fp);
void print_regular(char *fo_name, char *buf);
void print_reverse(char *fo_name, char *buf);
void usage();
void print_reverse_noflip(char *fo_name, char *buf);
void dir_main(char* dir_name, int max, int r_and_b, int reverse);

int main(int argc, char *argv[]){
	int i;
	int changed = 0;
	int max = 25000;
	int r_and_b = 0;
	int reverse = 0;
	int found = 0;
	int infile_idx = -1;
	int outfile_idx = -1;
	int dir_inputed = 0;
	int dir_idx = -1;

	//Check command line input
	if(argc < 2){
		printf("Wrong number of command line arguments\n");
		return(3);
	}

	for(i = 1; i < argc; i++){
		if (strcmp(argv[i], "-help") == 0){
			usage();
			return(1);
		
		} else if (strcmp(argv[i], "-o") == 0){
			i++;
			
			if(i >= argc){
				usage();
				return(1);
			}

			outfile_idx = i;
			
			changed = 1;

			FILE *fp_temp = fopen(argv[outfile_idx], "r");

			if(fp_temp == NULL){
				//fclose(fp_temp);

			} else if(fp_temp != NULL){
				printf("Output file already exists\n");
				fclose(fp_temp);
				return(7);
			}


		} else if (strcmp(argv[i], "-maxsize") == 0) {
			i++;

			if(i >= argc){
				usage();
				return(1);
			}
			
			max = atoi(argv[i]);

			if(max == 0){
				printf("Invalid input for maxsize\n");
				return(8);
			}
		
		} else if (strcmp(argv[i], "-r") == 0){
			reverse = 1;

		} else if (strcmp(argv[i], "-bfr") == 0) {
			r_and_b = 1; 
		
		} else if (strlen(argv[i]) > 4 && argv[i][strlen(argv[i])-4] == '.' && argv[i][strlen(argv[i])-3] == 't' && argv[i][strlen(argv[i])-2] == 'x' && argv[i][strlen(argv[i])-1] == 't') {

			infile_idx = i;

			if(changed == 0)
				outfile_idx = i;

			found = 1;
			
		}
		else {
			DIR *dir = opendir(argv[i]);
			if(dir != NULL){
				dir_inputed = 1;
				dir_idx = i;
				closedir(dir);
			}
		}
		

	}
	if(dir_inputed == 1){
		dir_main(argv[dir_idx], max, r_and_b, reverse);
		return(0);
	}

	if(found != 1){
		printf("Input a file name\n");
		return(5);
	}
	
	//Open the file
	FILE *fp = fopen(argv[infile_idx], "rb");


	if(fp == NULL){
		printf("File not found\n");
		//fclose(fp);
		return(4);
	}


	// Check the size
	// printf("Input: %s ", argv[infile_idx]);
	int size = get_file_size(fp);
	
	//printf("3\n");
	
	if(size > max){
		printf("File is too large\n");
		return(2);
	}

	//Creates a buffer to read in the contents of the file.
	char *buf = (char *)malloc((size+1) * sizeof(char));

	for(i=0; i<size; i++){
		fread(*&buf+i, 1, 1, fp);
	}
	
	
	//change this to file)name

	
	if(r_and_b == 0 && reverse == 0) {
		print_regular(argv[outfile_idx], buf);
		printf("Input: %s is %d bytes\n", argv[infile_idx],size);	
		printf("Output: %s.bf was output successfully (bitflip)\n", argv[outfile_idx]);
	}


	if(reverse == 1) {
		print_reverse_noflip(argv[outfile_idx], buf);	
		printf("input: %s is %d bytes\n", argv[infile_idx],size);
		printf("Output: %s.r was output successfully (reverse, no bitflip)\n ", argv[outfile_idx]);
	}

	if(r_and_b == 1) {
		print_reverse(argv[outfile_idx], buf);	
		printf("input: %s is %d bytes\n", argv[infile_idx],size);
		printf("Output: %s.bfr was output successfully (reverse, bitflip)\n ",argv[outfile_idx]);
	}
	// printf("Input: %s is %d bytes\n", argv[infile_idx], size);	
	fclose(fp);
	free(buf);


	return(0);
}

//Function to get the file size
int get_file_size(FILE *fp){
	// make sure we get right file
	
	int begin = ftell(fp);
	fseek(fp, 0, SEEK_END);

	int size = ftell(fp);
	fseek(fp, begin, SEEK_SET);

	// printf("was %d bytes\n", size);

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
	
	// check to see output file already exists without -o flag
	FILE *ftemp = fopen(names, "r");
	if(ftemp != NULL){
			printf("Error: %s already exists!\n", names);
			fclose(ftemp);
			exit(9);
	}
	FILE *fn = fopen(names, "w");
	char temp;

	for(i=0; i<strlen(buf); i++){
		temp = buf[i];
		temp = temp ^ 0xFF;
		fwrite(&temp, sizeof(temp), 1, fn);
	}

	fclose(fn);	
	// printf("Output: %s was output successfully (bitflip)\n", names);
	free(names);
}



void print_reverse(char *fo_name, char *buf){
		
	char *names = malloc(strlen(fo_name)+4*sizeof(char));
	
	int i;
	for(i=0; i<strlen(fo_name); i++){
		names[i] = fo_name[i];
	}

	names[strlen(fo_name)] = '.';
	names[strlen(fo_name)+1] = 'b';
	names[strlen(fo_name)+2] = 'f';
	names[strlen(fo_name)+3] = 'r';

	// check to see output file already exists without -o flag
	FILE *ftemp = fopen(names, "r");
		if(ftemp != NULL){
			printf("Error: %s already exists!\n", names);
			fclose(ftemp);
			exit(9);
		}


	FILE *fn = fopen(names, "w");
	char temp;

	for(i=strlen(buf)-1; i>=0; i--){
		temp = buf[i];
		temp = temp ^ 0xFF;
		fwrite(&temp, sizeof(temp), 1, fn);
	}

	fclose(fn);	
	// printf("Output: %s was output successfully (bitflip, reverse)\n", names); 
	free(names);
}




/*
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
	char *name = (char *)malloc(strlen(fo_name)+2*sizeof(char));
	
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

*/

void print_reverse_noflip(char *fo_name, char *buf){
		
	char *names = malloc(strlen(fo_name)+2*sizeof(char));
	
	int i;
	for(i=0; i<strlen(fo_name); i++){
		names[i] = fo_name[i];
	}

	names[strlen(fo_name)] = '.';
	names[strlen(fo_name)+1] = 'r';
	
	// check to see output file already exists without -o flag
	FILE *ftemp = fopen(names, "r");

	if(ftemp != NULL){
			printf("Error: %s already exists!\n", names);
			fclose(ftemp);
			exit(9);
	}

	FILE *fn = fopen(names, "w");
	char temp;

	for(i=strlen(buf)-1; i >= 0; i--){
		temp = buf[i];
		fwrite(&temp, sizeof(temp), 1, fn);
	}

	fclose(fn);
	// printf("Output: %s was output successfully (reverse, no bitflip)\n ",names);
	free(names);

}


void dir_main(char* dir_name, int max, int r_and_b, int reverse){
	struct dirent *pDirent;
	int size;
	FILE* fp;
	DIR* dir = opendir(dir_name);
	char* dir_name_and_file;
	int i, count=0;
	char* buf;

	if(dir == NULL) {
		printf("Cannot open directory '%s'\n", dir_name);
	}
	while((pDirent = readdir(dir)) != NULL) {
		if (pDirent->d_name[0] == '.'){
			continue;
		} else if (strlen(pDirent->d_name) > 4 && pDirent->d_name[strlen(pDirent->d_name)-4] != '.' && pDirent->d_name[strlen(pDirent->d_name)-3] != 't' && pDirent->d_name[strlen(pDirent->d_name)-2] != 'x' && pDirent->d_name[strlen(pDirent->d_name)-1] != 't') {
			continue;
		}

	
		// size = get_file_size();
		int sum = strlen(dir_name) + strlen(pDirent->d_name) + 3;
		printf("SUM: %d\n", sum);
		dir_name_and_file = (char*)calloc(sum,sizeof(char));
		

		// debugging section
		//printf(" size of malloc %ld, dir_name len %ld, file_name len %ld\n", sizeof(dir_name_and_file), strlen(dir_name), strlen(pDirent->d_name));
		//printf("%s\n",dir_name_and_file);
		// debugging section
		dir_name_and_file[0] = '.';
		dir_name_and_file[1] = '/';
		count = 2;

		for(i=0; i<strlen(dir_name); i++){
			dir_name_and_file[count] = dir_name[i];
			count++;
		}
		dir_name_and_file[count] = '/';
		count++;
		
		for(i=0; i<strlen(pDirent->d_name); i++){
			dir_name_and_file[count] = pDirent->d_name[i];
			// printf("%s\n", dir_name_and_file);
			count++;
		}
		
		// printf("%s\n", dir_name_and_file);		

		// have path as a file pointer
		fp = fopen(dir_name_and_file, "rb");
		size = get_file_size(fp);
		fclose(fp);
		free(dir_name_and_file);
		if(size >= max){
			continue;
		}
		// just do the bit flip
		
	  buf = (char *)malloc((size+1) * sizeof(char));
		if(r_and_b == 0 && reverse == 0){
			printf("sending file from dirrectory to printregular: %s\n", dir_name_and_file);
			print_regular(dir_name_and_file, buf);
		}

		// just do the reverse
		else if(reverse == 1){
			print_reverse_noflip(dir_name_and_file, buf);
		}
		// do both reverse and bit flip
		else if(r_and_b == 1){
			print_reverse(dir_name_and_file, buf);
		}
		printf("[%s], size: %d\n", pDirent->d_name, size);	
	}
	closedir(dir);
	
}
