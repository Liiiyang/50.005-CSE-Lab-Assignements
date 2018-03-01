#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <fcntl.h>

//#define MAX_INPUT 256
#define BUFFER_SIZE 100
#define NUM_FILES 100000
#define NAME_SIZE 100


struct statname {
	struct stat st;
	char name[NAME_SIZE];
	char relname[NAME_SIZE];
};

int timesort(const void * a, const void * b) {
	return (int) difftime(((struct statname *) a)->st.st_mtime, ((struct statname *) b)->st.st_mtime);
}

int sizesort(const void * a, const void * b) {
	return (int) ((struct statname *) a)->st.st_size - ((struct statname *) b)->st.st_size;
}


// from the linux kernel
// strips both sides of a string off whitespaces
char *strstrip(char *s) {
	size_t size;
	char *end;

	size = strlen(s);

	if (!size)
		return s;

	end = s + size - 1;
	// back strip
	while (end >= s && isspace(*end))
		end--;
	*(end + 1) = '\0';

	// front strip
	while (*s && isspace(*s))
		s++;

	return s;
}

// Check if a string is all digits
int digitcheck(char * str) {
	int i;
		for (i = 0; i < strlen(str); i++) {
		if (!isdigit(str[i])) {
			return 0;
		}
	}
	return 1;
}



int decalphasort(const struct dirent **a,const struct dirent **b){
	return alphasort(b,a);
}


static void recursive_find (const char *haystack, char *needle)
{
    DIR *dp;

    dp = opendir(haystack);

    if (! dp) {
        printf("Error: Cannot open directory.\n");
        exit (EXIT_FAILURE);
    }
    while (1) {
        struct dirent *dir;
        const char *name;
 
        dir = readdir(dp);
        if (!dir) {
           
            break;
        }

        name = dir->d_name;
        if (strstr(name, needle) != NULL)         
            printf ("%s/%s\n", haystack,name);

        if (dir->d_type & DT_DIR) {

            if (strcmp(name, "..") != 0 && strcmp(name, ".") != 0) {
                int path_length;
                char path[PATH_MAX];
 
                path_length = snprintf (path, PATH_MAX, "%s/%s", haystack, name);
                recursive_find(path, needle);
            }
	}
    }
    closedir(dp);
}

int tree_namesort(const struct dirent **a, const struct dirent **b){
	const char *first = (*a)->d_name;
	const char *second = (*b) -> d_name;
	return strcmp(first,second);
}

int tree_timesort(const struct dirent **a, const struct dirent **b){
	time_t time_one;
	time_t time_two;
	
	int i = -1;
	struct stat statbufa;
	struct stat statbufb;

	if (!(i = open((*a)->d_name, O_RDONLY))) {
		return 0;
	}

	fstat(i,&statbufa);
	time_one = (int unsigned long) statbufa.st_mtime;
	close(i);

	i = -1;
	if (!(i = open((*b)->d_name, O_RDONLY))) {
		return 0;
	}

	fstat(i,&statbufb);
	time_two = (int unsigned long) statbufb.st_mtime;

	return(difftime(time_one,time_two));

	close(i);

}

int tree_sizesort(const struct dirent **a, const struct dirent **b){

    int unsigned long a_size;
    int unsigned long b_size;

    int i = -1;
    struct stat statbufa;
    struct stat statbufb;

    if (!(i = open((*a)->d_name, O_RDONLY))) {
        return 0;
    }

    fstat(i, &statbufa);
    a_size = (int unsigned long) statbufa.st_size;

    close(i);

    i = -1;

    if (!(i= open((*b)->d_name, O_RDONLY))) {
        return 0;
    }

    fstat(i, &statbufb);
    b_size = (int unsigned long) statbufb.st_size;

    return (a_size - b_size);
    
    close(i);
	


}

int filter(const struct dirent *entry){
	return entry->d_name[0]!= '.';
}

static void print_tree(const char *path, int depths, char *command, int depth, char *p){
	int i=0;
	int n=0;
	struct dirent **dir = NULL;
	
	if((command!=NULL) && (command[0] != '\0')){
		if(strcmp(command, "time")==0){
			n = scandir(path, &dir,filter,tree_timesort); 
		}
		else if(strcmp(command, "size")==0){
			n = scandir(path, &dir,filter,tree_sizesort); 
		}
		else if(strcmp(command, "name")==0){
			n = scandir(path, &dir,filter,tree_namesort); 
		}
	}
	else{
		n = scandir(path, &dir,filter,NULL); 
	}

	if(n<0){
		printf("Error: Tree\n");
	}
	else{
		for(i = 0; i<n;i++){
			printf("%s",p);
			if(i==n-1){
				printf("L");
			}
			else{
				printf("|");
			}
			printf("-- ");

		    if (dir[i]->d_type & DT_DIR) {
		        printf("%s\n", dir[i]->d_name);

		        char newpath[PATH_MAX];
		        newpath[0] = '\0';
		        strcat(newpath, path);
		        strcat(newpath, "/");
		        strcat(newpath, dir[i]->d_name);
		    
		   
		        char np[128];
		        np[0] = '\0';
		        strcat(np, p);
		        if (i == n-1) {        
		            
		            strcat(np, "    "); 
		        } else{
		            strcat(np, "|   ");
		        }

		        if (depth < depths) 
		            print_tree(newpath, depths, command, depth+1, np);

		    } else {
		        printf("%s\n", dir[i]->d_name);

		    }
		    free(dir[i]);
        }
	free(dir);
	}
}



int main(){

//variables
char command[100];//to store users command

//for create option
char *create;
create="create";//be careful about \n

//for delete
char *delete;
delete="delete";//be careful about \n

//for display
char *display;
display="display";//be careful about \n

//for list
char *list;
list="list";//be careful about \n

char *property;
property="property";//be careful about \n

char *propertytime;
propertytime="property time";//be careful about \n

char *propertysize;
propertysize="property size";//be careful about \n

char *propertyname;
propertysize="property name";//be careful about \n

char *find;
find="find";//be careful about \n

char *tree;
tree="tree";//be careful about \n

char *treetime;
treetime=" time";//be careful about " " 

char *treesize;
treesize=" size";//be careful about " "

char *treename;
treename=" name";//be careful about " "

char *fileName;//to store fileName entered by user
int ret;//check return value


//pointers
char *ptr;
char *ptrTime;//sorting by time
char *ptrName;//sorting by name
char *ptrSize;//sorting by size

while(1){
	//Q1
	printf("csh>");
	fgets(command,MAX_INPUT,stdin);//take input from user and save it in command
	printf("command %s\n",command);

//--------------------------------------------------------------------
//Create, Case #1
	ptr=strstr(command,create);
	if(ptr!=NULL){
		FILE*fp;
		fileName = strstrip(ptr + strlen(create) + 1);
		if((fp = fopen(fileName,"w+"))!=NULL){
			printf("%s is created\n",fileName);
			fclose(fp);
		}
		else{
			printf("Error: File cannot be created.\n");
		}
	
	}

//--------------------------------------------------------------------
//Delete, Case #2
 ptr=strstr(command,delete);
	if(ptr!=NULL){
		FILE*fp;
		fileName = strstrip(ptr + strlen(delete) + 1);
		if((remove(fileName)==0)){
			printf("%s is deleted\n",fileName);
		}
		else{
			printf("Error:File cannot be deleted.\n");
		}
	}

//--------------------------------------------------------------------
//Display, Case #3
 ptr=strstr(command,display);
	if(ptr!=NULL){
		FILE*fp;
		char ch;
		fileName = strstrip(ptr + strlen(display) +1);
		if((fp = fopen(fileName,"r"))!=NULL){
			while((ch=fgetc(fp))!=EOF){
				printf("%c",ch);
			}
		}
		else{
			printf("File cannot be displayed\n");
		}
		fclose(fp);
	
	}

//--------------------------------------------------------------------
//list, Case #4
 ptr=strstr(command,list);
	if(ptr!=NULL)
	{
/*
list - list file according to names 
list property - list files according to names with properties
list property time - list files according to time
list property size - list files according to size
*/

		DIR * dir;
		struct dirent * dp;
		char *prop;
		char *p;
		char datestring[256];
		int numFiles = NUM_FILES;
		prop = strtok(ptr +strlen(list) + 1, " ");
		p = strtok(ptr +strlen(list) +strlen(property) + 2, " ");
		dir = opendir(".");
		int count = 0;
		if(prop!=NULL){
			if (digitcheck(strstrip(prop))) {
				numFiles = atoi(prop);
			}
			if(p!=NULL){
				struct statname * buffers;
				buffers = malloc(sizeof(struct statname) * BUFFER_SIZE);
				int current_buffer_size = BUFFER_SIZE;
				struct statname * current_buffer = buffers;
				while ((count <= numFiles) && (dp = readdir(dir))) {
					if (count >= BUFFER_SIZE) {
						buffers = realloc(buffers, sizeof(buffers) + sizeof(struct statname) * BUFFER_SIZE);
					}
					if (stat(dp->d_name, &current_buffer->st) < 0) {
						fprintf(stderr, "Cannot read file %s\n", dp->d_name);
						return EXIT_FAILURE;
					}
					strcpy(current_buffer->name, dp->d_name);
					count++;
					current_buffer++;
				}
				if(strncmp(p,"time",4)==0){
					printf("Time sorting....\n");
					qsort(buffers, count, sizeof(struct statname), timesort);
					int i;
					for (i = 0; i < count; i++) {
						char buff[40];
						struct tm * timeinfo;
						timeinfo = localtime (&buffers[i].st.st_mtime);
						strftime(buff, sizeof(buff), nl_langinfo(D_T_FMT), timeinfo);
						printf("%-15s Size:%-15jd Last Modified: %s\n", buffers[i].name, buffers[i].st.st_size, buff);
					}

					free(buffers);
				}
				else if(strncmp(p,"size",4)==0){
					printf("Size sorting...\n");
					qsort(buffers, count, sizeof(struct statname), sizesort);
					int i;
					for (i = 0; i < count; i++) {
						char buff[40];
						struct tm * timeinfo;
						timeinfo = localtime (&buffers[i].st.st_mtime);
						strftime(buff, sizeof(buff), nl_langinfo(D_T_FMT), timeinfo);
						printf("%-15s Size:%-15jd Last Modified: %s\n", buffers[i].name, buffers[i].st.st_size, buff);
					}

					free(buffers);
				}
			}
			else if(strncmp(prop,"property",8)==0){
				struct dirent **namelist;
				struct tm *tm;
				char datestring[256];
				struct stat statbuf;
				int n;
				n = scandir(".", &namelist, 0, decalphasort);
				if (n < 0) {
					perror("scandir\n");
				} else {
					while((dp=readdir(dir))!=NULL&&n--){
						if(stat(dp->d_name, &statbuf) == -1)
							continue;

						tm = localtime(&statbuf.st_mtime);
						strftime(datestring,sizeof(datestring),nl_langinfo(D_T_FMT),tm);
						printf("%-15s Size:%-15jd Last Modified: %s\n", namelist[n]->d_name, (intmax_t)statbuf.st_size, datestring);
						free(namelist[n]);
					}
				}
				free(namelist);	
			}
		}
		else if(strstr(command,property)==NULL){
			struct dirent **namelist;
			int i,n;
			n = scandir(".", &namelist,0,alphasort);
			if(n<0){
				printf("scandir");
			}
			else{
				for(i = 0; i<n;i++){
					printf("%s\n",namelist[i]->d_name);
					free(namelist[i]);
				}
			}
			free(namelist);
		}
		closedir(dir);
	}

//--------------------------------------------------------------------
//Find, Case #5
 ptr=strstr(command,find);
	if(ptr!=NULL)
	{

		char *stringPtr;
		char *filename;
            	stringPtr = command; 
            	strsep(&stringPtr, " "); 
            	filename = strsep(&stringPtr, "\n");
            	if (!filename) {
                	printf("ERROR: Command: find needs another argument.\n");
                	continue;
            	}

            	char path[PATH_MAX];
            	if (!getcwd(path, sizeof(path))) {
                	printf("ERROR: Cannot open directory.\n");
                	continue;
           	 }
            
            	recursive_find((const char *) path, filename);
			continue;
	}
//--------------------------------------------------------------------
//tree Case #6
 ptr=strstr(command,tree);
	if(ptr!=NULL)
	{
	//Implement your code to handle tree here
	/*There are many cases to handle in this part*/

	/*
	//tree
	//tree 1 
	//tree 1 time
	//tree size
	//tree name
	//etc..
	*/
	 char * stringPtr;
         char path[PATH_MAX];
            if (!getcwd(path, sizeof(path))) {
                printf("ERROR: Cannot open directory.\n");
                continue;
            }

            printf(".\n");
            char *argbuf;            
            int depths;            
            char sort[20];

            stringPtr = command; 
            strsep(&stringPtr, " "); 

            argbuf = strsep(&stringPtr, "\n"); 

            if (!argbuf) {
                print_tree(path, 999, "", 1, "");
                continue;
            }

            depths = atoi(argbuf);

            if (depths == 0) {
               	depths = 999;
            }

            int i = 0;
            int j = 0;

            while (i != strlen(argbuf)) {
                if ((argbuf[i] >= 97) && (argbuf[i] <= 122)) {
                    sort[j] = argbuf[i];
                    i++;
                    j++;
                } else {
                    i++;
                }
            }
            sort[j] = '\0';

            print_tree(path, depths, sort, 1, "");
	    continue;
	}
//--------------------------------------------------------------------
//Wrong command, Case #7
		/*	
		printf("You entered wrong command\n");
	
		*/


}//end while
}//end main
