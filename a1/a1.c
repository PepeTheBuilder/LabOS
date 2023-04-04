#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define MAX_PATH_LEN 4096
#define search_ECHO 1


void listDir(char *dirName){

	DIR* dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];

	dir = opendir(dirName);
	if (dir == 0) {
		printf("Error opening directory");
		exit(4);
	}

	// iterate the directory contents
	while ((dirEntry=readdir(dir)) != 0) {
		// build the complete path to the element in the directory
		snprintf(name, MAX_PATH_LEN, "%s/%s",dirName,dirEntry->d_name);
		
		lstat (name, &inode);

		if (S_ISDIR(inode.st_mode))
			printf("dir ");
		else 
			if (S_ISREG(inode.st_mode))
				printf ("file ");
			else
				if (S_ISLNK(inode.st_mode))
					printf ("lnk ");
				else;

		printf(" %s\n", dirEntry->d_name);
	}

	closedir(dir);
}

int search_dir(char *dir_name, char *searched_name){

	int gasit=0;
	DIR* dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];
	dir = opendir(dir_name);
	if (dir == 0) {

		printf("Error opening directory %s\n", dir_name);
		//exit(4);
		return 0;
	}

	// iterate the directory contents
	while ((dirEntry=readdir(dir)) != 0) {
		// build the complete path to the element in the directory
		snprintf(name, MAX_PATH_LEN, "%s/%s",dir_name,dirEntry->d_name);
		
		// get info about the directory's element
		lstat (name, &inode);

		// test the type of the directory's element

		if (strstr(dirEntry->d_name,searched_name)){
		
		if (S_ISDIR(inode.st_mode)){
			if(search_ECHO)printf("dir ");
			gasit=1;
		}
		else 
			if (S_ISREG(inode.st_mode)){
				if(search_ECHO)printf ("file ");
				gasit=2;
			}
			else
				if (S_ISLNK(inode.st_mode)){
					if(search_ECHO)printf ("lnk ");
					gasit=3;
				}
		lstat (name, &inode);
		if(search_ECHO)printf("%s\n", dirEntry->d_name);
		if(search_ECHO)printf("%s\n", name);
			return gasit;
		}
	}

	closedir(dir);
	if(search_ECHO)printf("nope \n");
	return 0;
}

int search_tree(char *dir_name, char *searched_name){
	int search=search_dir(dir_name,searched_name);
	if(search!=0){
		return 1;
	}
	
	DIR* dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];
	dir = opendir(dir_name);
	if(dir!=0)
	while ((dirEntry=readdir(dir)) != 0) {
		// build the complete path to the element in the directory
		snprintf(name, MAX_PATH_LEN, "%s/%s",dir_name,dirEntry->d_name);
		
		// get info about the directory's element
		lstat (name, &inode);

		if (S_ISDIR(inode.st_mode)){
			if(strcmp(dirEntry->d_name,".")!=0 && strcmp(dirEntry->d_name,"..")!=0){
				int aux =search_tree(name,searched_name);
				if(aux==1){
					printf("Directory founded: %s\n", name);
					return 10;
				}
					
				if(aux==2){
					printf("File found");
					return 10;
                }
				if(aux==10){
					return 10;
				}
			}
			continue;
		}
	}

	closedir(dir);
	return 0;
}
/*
int readDirections(int argc, char **argv){
   

   if(strcmp(argv[1],"variant")==0){
    printf("22794");
   }
       
    if(strcmp(argv[1],"list")==0){


        if(argc<=4 && argc>2){
        printf("%d\n",argc);
        int recusive=0; 
        int  permision=0; 
        int end=0; 
        char end_with[100];
        char path[4096];
         
        for(int i=2; i<=argc;i++){
            
        if(strcmp(argv[i],"recusive")==0){
            recusive=1;
            break;
        }
        }
           printf("aici3\n");

        for(int i=2; i<=argc;i++){
            printf("aici i=%d\n",i);
        if(strcmp(argv[i],"has_perm_execute")==0){
            permision=1;
            break;
        }
        }
         printf("aici4\n");
        for(int i=2; i<=argc;i++){
            if(strcmp(argv[i],"name_ends_with=")==0){
                end=1;
                strcpy(end_with,argv[i]+15);
                printf("%s, ",end_with);
                break;
            }
        }
        for(int i=2; i<=argc;i++){
            if(strstr(argv[i],"path=")==0){
                strcpy(path,argv[i]+5);
                printf("%s, ",path);
                break;
            }
        }
        printf("recusive %d, permision =%d, end=%d",recusive,permision,end);
        }

    }
    return 0;

}
*/

int readDirections(int argc, char **argv) {
    if(strcmp(argv[1], "variant") == 0) {
        printf("22794");
    } else if(strcmp(argv[1], "list") == 0) {
        if(argc <= 4 && argc > 2) {
            int recursive = 0;
            int permission = 0;
            int end = 0;
            char end_with[100];
            char path[4096];

            for(int i = 2; i < argc; i++) {
                if(strcmp(argv[i], "recursive") == 0) {
                    printf("aici");
                    recursive = 1;
                    break;
                }
            }

            for(int i = 2; i < argc; i++) {
                if(strcmp(argv[i], "has_perm_execute") == 0) {
                    permission = 1;
                    break;
                }
            }


            for(int i = 2; i < argc; i++) {
                if(strncmp(argv[i], "name_ends_with=", 15) == 0) {
                    end = 1;
                    strcpy(end_with, argv[i] + 15);
                    printf("%s, ", end_with);
                    break;
                }
            }

            for(int i = 2; i < argc; i++) {
                if(strstr(argv[i], "path=") != NULL) {
                    strcpy(path, argv[i] + 5);
                    printf("%s, ", path);
                    break;
                }
            }

            printf("recursive=%d, permission=%d, end=%d\n", recursive, permission, end);
        }
    }
    return 0;
}

int main(int argc, char **argv){
    if(argc < 1 || argv == NULL) {
        printf("Invalid arguments\n");
        return -1;
    }
    else {
         readDirections(argc, argv);
    }

    return 0;
}