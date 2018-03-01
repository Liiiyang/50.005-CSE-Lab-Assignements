#include <stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#define MAX_INPUT 8192
void main(){//start main

char command[MAX_INPUT];//to store users command
int check_dir;
char *path;
char *folder;
char **history = (char **) malloc(15* sizeof(char)); // Create an History array to store previous commands
int index = 0; //History Pointer
int sys;
//while loop to keep asking user for more inputs
while(1){
	//Q1 Creating External Process
	printf("csh>");
	fgets(command,MAX_INPUT,stdin);//take input from user
	//printf("command %s\n",command);
	//system(command);
	//Q3 History Features
	strtok(command, "\n");
	//Adds commands into array, without history and !! and integers
	if(strcmp(command,"history") != 0 && atoi(command)==0 && strcmp(command , "!!") != 0){
		history[index] = malloc(sizeof(command));
		strcpy(history[index], command);
		index+=1;
	}
	// if "!!" is entered, show previous command, if index is 0, show error msg
	if(strcmp(command , "!!") == 0){
		if(index == 0){
			printf("No previous command\n");
		}
		else{
			strcpy(command, history[index-1]);
		}
	}
	//if the index is selected, return the particular command from history
	if((atoi(command) > 0 && atoi(command)<=index)||strcmp(strtok(command, "\n") , "0") == 0){
		strcpy(command, history[atoi(command)]);
	}
	if(atoi(command)>=index){
		printf("The number must be 1 to %d, inclusive\n",index);
	}
	//Q2 Changing Directories
	//Returns to Home if only cd is entered
	if(strcmp(command,"cd")==0){
		chdir(getenv("HOME"));
	}
	//check if "cd" exists in the command string
	else if(strncmp(command,"cd",2) == 0){ 
		path = strtok(command," \n");
		path = strtok(NULL, " \n");
		folder = strtok(path, " \n");
		check_dir = chdir(folder);
		if(strcmp(folder,"~")==0){
			if(chdir(getenv("HOME"))==-1){
				perror(getenv("HOME"));
			}
		}
		else if(check_dir == -1){ //Check if directory is invalid
			printf("error: new directory is invalid\n");
		}
		
	}
	//Q3 Displays all the commands in History, in reverse order
	else if(strcmp(command,"history") == 0){
		int j =1;
		for(int i = index;i>0;i--){
			printf("%d %s\n",j,history[i-1]);
			j+=1;
		}
	}
	else{
		sys = system(command);
		if(sys == 32512){ //32512 -> system(command) integer when there is an error
			printf("Cannot run program \"%s\", No such file or directory.\n",command);
		}
	}

	}//end while1
}// end maint
