#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
		
		// get info about the directory's element
		lstat (name, &inode);

		// test the type of the directory's element
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
					
				if(aux==2)
					printf("File found");
					return 10;

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



int main(int argc, char **argv){

	//listDir("/");
	if (argc != 3) {
		printf ("USAGE: %s dir_name wanted_file_name\n", argv[0]);
		exit(1);
	}

	if(argc==3){
			// Check if realy a directory
	struct stat fileMetadata;
	if (stat(argv[1], &fileMetadata) < 0) {  // get info 
		printf("ERROR (getting info about the file)");
		exit(2);
	}
	if (S_ISDIR(fileMetadata.st_mode)) { // it is a directory
		// list directory's contents
		
		search_tree(argv[1],argv[2]);

	} else {
		printf("%s is not a directory!\n", argv[1]);
		exit(3);
	}
	}
	return 0;
}