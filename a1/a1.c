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
#define search_ECHO 0

int recursive = 0;
char *filtering_options = "";
int filtering = 0;
int perm_exe = 0;
char *dir_path="";
//-------------------- Listing-----------------
int has_exec_permission(const char* filepath) {
    struct stat filestat;

    if (stat(filepath, &filestat) == -1) {
        perror("Error getting file status");
        //exit(2);
		return 0;
    }

    // Check if the owner has execute permission
    if (filestat.st_mode & S_IXUSR ) {
        return 1;
    } else {
        return 0;
    }
}
void listDir(char *dirName){

	DIR *dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];

	dir = opendir(dirName);
	if (dir == 0)
	{
		//printf("Error opening directory");
		exit(4);
	}

	while ((dirEntry = readdir(dir)) != 0)
	{
		snprintf(name, MAX_PATH_LEN, "%s/%s", dirName, dirEntry->d_name);

		lstat(name, &inode);
		if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0){
			continue;
		}

		if(perm_exe==0){
		printf("%s\n",name);
		}
		else {
			if(has_exec_permission(name))
				printf("%s\n",name);
		}
	}

	closedir(dir);
}
void listDirRecusive(char *dirName)
{
	DIR *dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];

	dir = opendir(dirName);
	if (dir == 0)
	{
		//perror("Error opening directory");
		closedir(dir);
		
		return;
	}
	listDir(dirName);

	while ((dirEntry = readdir(dir)) != 0)
	{

		snprintf(name, MAX_PATH_LEN, "%s/%s", dirName, dirEntry->d_name);

		lstat(name, &inode);

		if (S_ISDIR(inode.st_mode))
		{
			if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
			{
				listDirRecusive(name);
			}
		}
	}

	closedir(dir);
}
int search_dir(char *dir_name, char *searched_name){


	int gasit = 0;
	DIR *dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];
	dir = opendir(dir_name);
	if (dir == 0)
	{
		if (search_ECHO)printf("Error opening directory %s\n", dir_name);
		closedir(dir);
		return 0;
	}

	while ((dirEntry = readdir(dir)) != 0)
	{
		snprintf(name, MAX_PATH_LEN, "%s/%s", dir_name, dirEntry->d_name);
		lstat(name, &inode);

		if (strstr(dirEntry->d_name, searched_name))
		{

			if (S_ISDIR(inode.st_mode))
			{
				if (search_ECHO)
					printf("dir ");
				gasit = 1;
			}
			else if (S_ISREG(inode.st_mode))
			{
				if (search_ECHO)
					printf("file ");
				gasit = 2;
			}
			else if (S_ISLNK(inode.st_mode))
			{
				if (search_ECHO)
					printf("lnk ");
				gasit = 3;
			}
			lstat(name, &inode);
			//if (search_ECHO)
				//printf("%s\n", dirEntry->d_name);
			//if (search_ECHO)
				printf("%s\n", name);
				closedir(dir);
			return gasit;
		}
	}

	closedir(dir);
	if (search_ECHO)
		printf("nope \n");
	return 0;
}
int search_tree(char *dir_name, char *searched_name){
	int search=0;
	if(filtering==0)
	listDirRecusive(dir_path);
	else
	search=search_dir(dir_path,filtering_options);
	if (search != 0)
	{
		return 1;
	}

	DIR *dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];
	dir = opendir(dir_name);
	if (dir != 0)
		while ((dirEntry = readdir(dir)) != 0)
		{
			// build the complete path to the element in the directory
			snprintf(name, MAX_PATH_LEN, "%s/%s", dir_name, dirEntry->d_name);

			// get info about the directory's element
			lstat(name, &inode);

			if (S_ISDIR(inode.st_mode))
			{
				if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
				{
					int aux = 0;
						if(filtering==0){
						listDirRecusive(name);
						//search_tree(name,filtering_options);
						//aux=1;
						}
						else
						search=search_dir(name,filtering_options);
					if (aux == 1)
					{
						//printf("Directory founded: %s\n", name);
						return 10;
					}

					if (aux == 2)
					{
						//printf("File found");
						return 10;
					}
					if (aux == 10)
					{
						return 10;
					}
				}
			}
		}

	closedir(dir);
	return 0;
}
//------------------- Listing end---------------
//------------------- Parse --------------------

int parse(char* filename) {

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file: \n");
        return -1;
    }

    off_t end_offset = lseek(fd, 0, SEEK_END);
    if (end_offset < 0) {
        printf("Error seeking to end of file:\n");
        close(fd);
        return -1;
    }

    off_t offset = end_offset - 4;
    if (offset < 0) {
        offset = 0;
		printf("Error reading file: \n");
        return -1;
    }

    int header_size=0;
	char magic[3]={0};

    int red=pread(fd, &header_size,2,offset);
	offset+=2;
	int red2=pread(fd,magic,2,offset);
	offset+=2;

    if (red==-1||red2==-1) {
        printf("Error reading file: %d, %d \n",red,red2);
        return -1;
    }

    if (red == 0|| red2==0) {
        printf("File is empty/not big enough\n");
        close(fd);
        return -1;
    }

	if(strcmp(magic,"wt")==0)
	{
		printf("ERROR\n");
		printf("wrong magic\n"); //ERROR \n wrong magic|version|sect_nr|sect_types \n
		return -2;
	}

	
	offset= end_offset- header_size;    
	if (offset < 0) {
        offset = 0;
		printf("Error reading file: \n");
        return -1;
    }

	int variation=0;
	pread(fd, &variation,2,offset);
	offset+=2;
	if(variation>=152 || variation<=100){
		printf("ERROR\n");
		printf("wrong version\n"); //ERROR \n wrong magic|version|sect_nr|sect_types \n
		return -2;
	}

	int nrOfSections=0;
	pread(fd, &nrOfSections,1,offset);
	offset++;
	if( nrOfSections<8 || nrOfSections>13) {
		printf("ERROR\n");
		printf("wrong sect_nr\n"); //ERROR \n wrong magic|version|sect_nr|sect_types \n
		return -2;
	}

	
	printf("SUCCESS\n");
	printf("version=%d\n", variation);
	printf("nr_sections=%d\n",nrOfSections);
	for(int i=1; i<=nrOfSections; i++){
	
		char aux[4],buffer[25]={0};

		pread(fd, &buffer,25,offset);
		offset= offset+25;

		char name[16];
		for(int i=0; i<16;i++)
			name[i]=buffer[i];

		int type= (int)buffer[16];

		for(int i=0; i<16;i++)
			name[i]=buffer[i];
		int offset= atoi()

		printf("section%d: ", i);
		printf("%s\n",buffer);
	}
	offset=0;
    close(fd);
    return 0;
}

//-----------------------Parse end ---------------------------
int readDirections(int argc, char **argv)
{
	if (strcmp(argv[1], "variant") == 0)
	{
		printf("22794");
	}
	else if (strcmp(argv[1], "list") == 0)
	{
		if (argc < 2)
		{
			printf("Usage: %s list path=<dir_path> [recursive] [<filtering_options>]\n", argv[0]);
			return 0;
		}

		for(int i = 2; i < argc; i++)
		{
			if (strncmp(argv[i], "path=", 5) == 0)
			{
				dir_path = &argv[i][5];
			}
			else if (strcmp(argv[i], "recursive") == 0)
			{
				recursive = 1;
			}
			else if (strncmp(argv[i], "name_ends_with=", 15) == 0)
			{
				filtering_options = &argv[i][15];
				filtering=1;
			}
			if (strcmp(argv[i], "has_perm_execute") == 0)
			{
				perm_exe = 1;
			}
		}
		printf("SUCCESS\n");
		//printf("recusrive:%d, perm_exe:%d, filtering:%s, path=%s \n",recursive,perm_exe,filtering_options, dir_path);
		
		if(recursive==1)
		{
			listDirRecusive(dir_path);
		}
		else {
			//search_dir(dir_path,filtering_options);
			if(filtering==0){
			listDir(dir_path);
			}
			else{
			search_dir(dir_path,filtering_options);
			}
		}
	}
	else if(strcmp(argv[1],"parse")==0){

		if (argc != 3)
		{
			printf("Usage: %s parse path=<dir_path>\n", argv[0]);
			return 0;
		}
		if (strncmp(argv[2], "path=", 5) == 0)
		{
				dir_path = &argv[2][5];
				//printf("%s\n",dir_path);
				parse(dir_path);

		}
		else {
			printf("Usage: %s parse path=<dir_path>\n", argv[0]);
			return 0;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 1 || argv == NULL)
	{
		printf("Invalid arguments\n");
		return -1;
	}
	else
	{
		readDirections(argc, argv);
	}

	return 0;
}
