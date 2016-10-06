/*
########################################
Name:		John Walter
Date:		10/1/2016
Class:		ECE 3220
Assignment:	Lab 5
########################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Global variables 
int length = 0;
double offset,scale;
int max_value;

// This function reads in the data from a file to an integer array and returns a pointer to the array
int *Readfile(int file){
 	int cell, i;
 	int array[2000];
 	int *arrayPtr = array;
	char file_name[20];

	//Formats the file name based on the number inputted by the user
	if(file<9){ // includes a zero in the name
		sprintf(file_name,"Raw_data_0%d.txt",file);
	}else{ //doesn't include a zero in the name
		sprintf(file_name,"Raw_data_%d.txt",file);
	}
	FILE *fp = fopen(file_name,"r");
	if (fp == NULL){
		return NULL;
	}
	fscanf(fp,"%d %d",&length,&max_value);
	for(i=0;i<length;i++){
		fscanf(fp,"%d",&cell);
		array[i] = cell;
	}

	fclose(fp);
	return arrayPtr;
}

// This function displays the help menu
void disp_help(){
	printf("The options/parameters that need to be handled are:\n");
	printf("\t-n: File Number\n");
	printf("\t-o: Offset Signal\n");
	printf("\t-s: Scale Signal\n");
	printf("\t-S: Create Statistics File\n");
	printf("\t-C: Center Signal\n");
	printf("\t-N: Normalize Signal\n");
	printf("\t-r: Rename File\n");
	printf("\t-h: Display help menu\n");
}

// This function creates a new file with the new filename given and writes the contents to it
void renameFile(char *new_filename, int oldfilenum){
	int cell, i;
	char file_name[20];
	//Formats the file name based on the number inputted by the user
	if(oldfilenum<9){ // includes a zero in the name
		sprintf(file_name,"Raw_data_0%d.txt",oldfilenum);
	}else{ //doesn't include a zero in the name
		sprintf(file_name,"Raw_data_%d.txt",oldfilenum);
	}

	FILE *fp = fopen(file_name,"r");
	FILE *out = fopen(new_filename,"w");
	fscanf(fp,"%d %d",&length,&max_value);
	fprintf(out,"%d %d\n",length,max_value);
	for(i=0;i<length;i++){
		fscanf(fp,"%d",&cell);
		fprintf(out,"%d\n",cell);
	}

	fclose(fp);
	fclose(out);
}

// This function writes the values in a double array to a file 
void WriteFile(char file_name[20], double *array, double x){ // the x represents the offset or scale factor used in the transformations 
	int i;
	
	FILE *fp = fopen(file_name,"w");
	if(fp == NULL){
		printf("Error opening file\n");
	}
	fprintf(fp,"%d %.4f\n",length,x);
	for(i=0;i<length;i++){
		fprintf(fp,"%.4f\n",array[i]);
	}
	fclose(fp);
}

// This function calculates the mean (average) of an integer array
double mean(int *array){
	double total = 0.0;
	int i=0;
	for(i=0;i<length;i++){
		total = total+(double)array[i];
	}
	return total/(double)length; //length is type int
}

// This function returns the maximum possible value of the signal
double max(int *array){
	// Originally thought that this function needed to actually find the maximum value of the signal
	// but then realized it just needs to return the max value given in the file, which is declared as
	// a global variable above and found the the Readfile() function
	/*int i=0;
	double max = (double)array[i];
	for(i=0;i<length;i++){
		if(array[i] > max){max = array[i];}
	}
	return max;
	*/
	return (double)max_value;
}

// This function generates a file containing the mean and max of each raw file
void createStatisticsFile(int *array, int file){
	char file_name[20];
	if(file<10){
		sprintf(file_name,"Statistics_data_0%d.txt",file);
	}else{
		sprintf(file_name,"Statistics_data_%d.txt",file);
	}
	
	FILE * fp = fopen(file_name,"w");
	fprintf(fp,"%.4f %.0f",mean(array),max(array));
	fclose(fp);
}

// This function offsets the signal based on an offset value inputted
void offsetSignal(int *array, double offset_value, char file_name[20]){
	int i;
	double arrayD[2000];
	for(i=0;i<length;i++){
		arrayD[i] = (double)array[i] + offset_value;
	}
	//Writes the new array to a file
	WriteFile(file_name, arrayD, offset_value);
}

//This function scales the signal based on a scale factor inputted
void scaleSignal(int *array, double scale_factor, char file_name[20]){
	int i;
	//printf("%f\n",scale_factor);
	double arrayD[2000];
	for(i=0;i<length;i++){
		arrayD[i] = (double)array[i] * scale_factor;
	}
	//Writes the new array to a file
	WriteFile(file_name, arrayD, scale_factor);
}

//This function centers the signal 
void centerSignal(int *array, char file_name[20]){
	// Since centering is the same as offsetting the signal by a factor
	// of -mean, the offsetSignal can be used with an offset value of -mean
	offsetSignal(array, (-mean(array)), file_name);
}

// This function normalizes the singal
void normalizeSignal(int *array, char file_name[20]){
	// Since normalizing is the same as scaling the signal by a factor
	// of 1/max, the scaleSignal() function can be used with a scale value of 1/max
	scaleSignal(array, (1/max(array)), file_name);
}








int main( int argc, char *argv[]){
	int i, statistics = 0, center = 0, normalize = 0, rename = 0, filenumber = 0, help = 0, offset_check = 0, scale_check = 0;
	float offset_value = 0, scale_factor = 0;
	char *filename = NULL;
	char file_name[20];
	
	/*
	printf("%d\n",argc);
	for(i=1;i<=argc-1;i++){
		printf("%s\n",argv[i]);
	}
	*/
	
	// This for loop loops through the arguments looking for the correct arguments
	for(i=1;i<argc;i++){
		// if "-n" in argument
		if(strcmp(argv[i],"-n") == 0){

			filenumber = atoi(argv[i+1]);
			if(filenumber != 0){
				//skip over the file number to the next argument
				i++;
			}
		}

		// if "-o" in argument
		else if(strcmp(argv[i],"-o") == 0){
			offset_check = 1;
			
			offset_value = atof(argv[i+1]);
			if(offset_value != 0){
				// skip over the offset value to the next argument
				i++;
			}
		}

		// if "-s" in argument
		else if(strcmp(argv[i],"-s") == 0){
			scale_check = 1;
			scale_factor = atof(argv[i+1]);
			if(scale_factor != 0){
				// skip over the scale factor to the next argument
				i++;
			}
		}
		// if "-S" in argument
		else if(strcmp(argv[i],"-S") == 0){
			statistics = 1;
		}
		// if "-C" in argument
		else if(strcmp(argv[i],"-C") == 0){
			center = 1;
		}
		// if "-N" in argument
		else if(strcmp(argv[i],"-N") == 0){
			normalize = 1;
		}
		// if "-r" in argument
		else if(strcmp(argv[i],"-r") == 0){
			filename = argv[i+1];
			// check that a newfilename was given
			if(filename == NULL){
				printf("No new filename given\n");
			}
			else if(strcmp(filename,"-n") == 0){
				printf("No new filename given\n");
			}
			else if(strcmp(filename,"-o") == 0){
				printf("No new filename given\n");
			}
			else if(strcmp(filename,"-s") == 0){
				printf("No new filename given\n");
			}
			else if(strcmp(filename,"-S") == 0){
				printf("No new filename given\n");
			}
			else if(strcmp(filename,"-C") == 0){
				printf("No new filename given\n");
			}
			else if(strcmp(filename,"-N") == 0){
				printf("No new filename given\n");
			}
			else if(strcmp(filename,"-r") == 0){
				printf("No new filename given\n");
			}
			else if(strcmp(filename,"-h") == 0){
				printf("No new filename given\n");
			}
			else{
				rename = 1;
				// skip over the new file name to the next argument
				i++;
			}
		}
		// if "-h" in argument
		else if(strcmp(argv[i],"-h") == 0){
			help = 1;
		}
		
	}
	
	/*
	printf("Filename: %s \n",filename);
	printf("Offset value: %f \n",offset_value);
	printf("Scale factor: %f \n",scale_factor);
	printf("Statistics: %d \n",statistics);
	printf("Center: %d \n",center);
	printf("Normalize: %d \n",normalize);
	printf("Rename: %d \n",rename);
	printf("Filenumber: %d \n",filenumber);
	*/
	
		// if the help flag is set
		if(help == 1){
			disp_help();
			return 0;
		}

		// if no file number was given
		if(filenumber == 0){
			printf("No file number given\n");
			return -1;
		}else{
			if((filenumber < 1) | (filenumber > 10)){
				printf("Invalid file number\n");
				return -1;
			}
		}

		// read from the file
		int *array = Readfile(filenumber);
		if(array == NULL){
			printf("Could not find file\n");
			return -1;
		}

		// if the offset flag is set
		if(offset_check == 1){
			if(offset_value == 0){
				printf("No offset value\n");
			}else{
				printf("Doing offset\n");
				if(filenumber<10){
					sprintf(file_name,"Offset_data_0%d.txt",filenumber);
				}else{
					sprintf(file_name,"Offset_data_%d.txt",filenumber);
				}
				offsetSignal(array, offset_value, file_name);
			}
		}
		
		// if the scale flag is set
		if(scale_check == 1){
			if(scale_factor == 0){
				printf("No scale factor\n");
			}else{
				printf("Doing scale\n");
				if(filenumber<10){
					sprintf(file_name,"Scaled_data_0%d.txt",filenumber);
				}else{
					sprintf(file_name,"Scaled_data_%d.txt",filenumber);
				}
				scaleSignal(array, scale_factor, file_name);
			}
		}
		
		// if the rename flag is set
		if(rename == 1){
			printf("Doing rename\n");
			renameFile(filename,filenumber);
		}
		
		// if the statistics flag is set
		if(statistics == 1){
			printf("Doing statistics\n");
			createStatisticsFile(array,filenumber);
		}

		// if the center flag is set
		if(center == 1){
			printf("Doing center\n");
			if(filenumber<10){
				sprintf(file_name,"Centered_data_0%d.txt",filenumber);
			}else{
				sprintf(file_name,"Centered_data_%d.txt",filenumber);
			}
			centerSignal(array,file_name);
		}

		// if the normalize flag is set
		if(normalize == 1){
			printf("Doing normalize\n");
			if(filenumber<10){
				sprintf(file_name,"Normalized_data_0%d.txt",filenumber);
			}else{
				sprintf(file_name,"Normalized_data_%d.txt",filenumber);
			}
			normalizeSignal(array,file_name);
		}
	return 0;
}
