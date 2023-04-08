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
#define DEBUG 0
#define DEBUG_FINDALL 1
int parse_echo=1;
int extract_echo=1;


int recursive = 0;
char *filtering_options = "";
int filtering = 0;
int perm_exe = 0;
char *dir_path="";
int parse_section_size[100]={0};
int parse_section_offset[100]={0};
int nr_of_section=0;
int nr_of_line_in_section=0;

//-------------------- Listing-----------------------------
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
//------------------- Listing end---------------------------
//------------------- Parse ---------------------------------
int parse(char* filename) {

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
		if(parse_echo)
        printf("Error opening file: \n");
        return -10;
    }

    off_t end_offset = lseek(fd, 0, SEEK_END);
    if (end_offset < 0) {
		if(parse_echo)
        printf("Error seeking to end of file:\n");
        close(fd);
        return -1;
    }

    off_t offset = end_offset - 4;
    if (offset < 0) {
        offset = 0;
		if(parse_echo)
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
		if(parse_echo)
        printf("Error reading file: %d, %d \n",red,red2);
        return -1;
    }

    if (red == 0|| red2==0) {
		if(parse_echo)
        printf("File is empty/not big enough\n");
        close(fd);
        return -1;
    }
	if(strcmp(magic,"wT")!=0)
	{
		if(parse_echo)
		printf("ERROR\n");
		if(parse_echo)
		printf("wrong magic\n"); //ERROR \n wrong magic|version|sect_nr|sect_types \n
		return -2;
	}
	offset= end_offset- header_size;    
	if (offset < 0) {
        offset = 0;
		if(parse_echo)
		printf("Error reading file: \n");
        return -1;
    }

	int variation=0;
	pread(fd, &variation,2,offset);
	offset+=2;
	if(variation>=152 || variation<=100){
		if(parse_echo)
		printf("ERROR\n");
		if(parse_echo)
		printf("wrong version\n"); //ERROR \n wrong magic|version|sect_nr|sect_types \n
		return -2;
	}

	int nrOfSections=0;
	pread(fd, &nrOfSections,1,offset);
	offset++;
	if( nrOfSections<8 || nrOfSections>13) {
		if(parse_echo)
		printf("ERROR\n");
		if(parse_echo)
		printf("wrong sect_nr\n"); //ERROR \n wrong magic|version|sect_nr|sect_types \n
		return -2;
	}

	char output[100][50]={0};

	for(int i=1; i<=nrOfSections; i++){
	
		char name[16]={0};
		int type=0,size_sect=0,offset_sect=0;
		int size_name=pread(fd, &name,16,offset);
		name[size_name]=0;
		offset= offset+16; 
		
		pread(fd,&type,1,offset);
		offset++;

		pread(fd,&offset_sect,4,offset);
		offset+=4;

		pread(fd,&size_sect,4,offset);
		offset+=4;
		
		if (type != 31 && type != 66 && type != 32 && type != 73 && type != 91){
			//31 66 32 73 91
			if(parse_echo)
			printf("ERROR\n");
			if(parse_echo)
			printf("wrong sect_types\n"); //ERROR \n wrong magic|version|sect_nr|sect_types \n
			return -2;
		}
		sprintf(output[i],"section%d: %s %d %d",i,name,type,size_sect);
		parse_section_size[i]=size_sect;
		parse_section_offset[i]=offset_sect;
	}
	nr_of_section=nrOfSections;
	if(parse_echo){
	printf("SUCCESS\n");
	printf("version=%d\n", variation);
	printf("nr_sections=%d\n",nrOfSections);
	int i=1;
	while(output[i][0]!=0){
	printf("%s \n",output[i]);
	i++;
	}
	}
	offset=0;
	if(parse_echo)
    close(fd);
    return 1;
}
//-----------------------Parse end --------------------------
//----------------------- Extract  -------------------------
int extract(char* file_path, int line, int section){
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
		if(extract_echo)
        printf("Error opening file: \n");
		close(fd);
        return -10;
    }
	parse_echo=0;
	int readed=parse(file_path);
	parse_echo=1;
	if(DEBUG)for(int i=0;i<14;i++)
		printf("i:%d, offset:%d,size %d\n",i,parse_section_offset[i],parse_section_size[i]);
	if(readed == -10 || readed== -2){
			if(extract_echo){
			printf("ERROR\n");
			printf("invalid file\n"); //invalid file|section|line
		}
		 close(fd);
		return -2;
	}
	if(section>nr_of_section){
		if(extract_echo){
			printf("ERROR\n");
			printf("invalid section\n"); //invalid file|section|line
		}
		return -1;
	}
    off_t offset = lseek(fd, 0, SEEK_END);
    if (offset < 0) {
		if(parse_echo)
        printf("Error seeking to end of file:\n");
        close(fd);
        return -1;
    }
	int sizeSection=parse_section_size[section];
	offset= parse_section_offset[section];
	if(DEBUG)
		printf("malloc size %d, offset:%ld, offsetSection %d\n",sizeSection+2+100,offset,parse_section_offset[section]);
	char* sectionText= malloc(sizeSection+2);
	//-----------READ-------------
	pread(fd, sectionText,sizeSection,offset);
	//------------READ------------
	//sectionText[strlen(sectionText)+1]='\n';
	int strlenSectionText=strlen(sectionText);
	for(int i=0; i<=strlenSectionText/2; i++ ){
		// reverse the orders of the letters
		char aux=sectionText[i];
		sectionText[i]=sectionText[strlenSectionText-i-1];
		sectionText[strlenSectionText-i-1]=aux;
	}
	if(DEBUG)printf("exit while: %s\n", sectionText);
	if(DEBUG)printf("s-a invartit! \n" );
	sectionText[strlenSectionText+1]=0;
	int nrLines=1;
	char *auxPointer=strchr(sectionText,'\n');
	// number all the lines
	while(nrLines<=sizeSection){
		nrLines++;
		auxPointer=strchr(auxPointer+1, '\n');
	if(auxPointer==NULL){
		break;
	}
	}
	nr_of_line_in_section = nrLines;
	if(DEBUG)printf("%d\n", nr_of_line_in_section);
	if(extract_echo){
		// check the nr of lines >= the wanted line
		// a line ends with x"0A"
		//nrLines++;
		int lastLineOffset=-1;
		nrLines=1;
		if(nr_of_line_in_section>line){
			for(int i=0;i<=strlenSectionText;i++){
				if(nrLines==line)
				break;
				if(sectionText[i]=='\n'){

					lastLineOffset=i;
					nrLines++;
					if(DEBUG){
					//printf("nr de lini %d,%d,%d\n", nrLines,line,i);
					printf("%d:%c\n",i,sectionText[i+1]);
					}
				}
			}
		}
		else{
			printf("ERROR\n");
			printf("invalid line\n"); //invalid file|section|line
		free(sectionText);
		close(fd);
		return -1;
		}
		printf("SUCCESS\n");
		int i=1;
		char *point= sectionText+lastLineOffset;
		while(point[i]!='\n'&&(point+i!=NULL && point+i+1!=NULL)){
			printf("%c",point[i]);
			i++;
		}
		printf("\n");

	}
	free(sectionText);
	close(fd);
	return 0;
}
//---------------------- Extract End  ----------------------
//----------------------- Findall  -------------------------
int findallbool(char*dirName, int line, int section){

	int result = extract(dirName,line,section);
	if(result == 0)
		return 1;
	return 0;
}

void printFindall(char*dirName){
	int result=findallbool(dirName,1,1);
	int ok=0;
	for(int sections=2; sections<=nr_of_section;sections++){
		result=findallbool(dirName,1,sections);
		if(result==1&&nr_of_line_in_section==14) ok++;
		if(ok>=2){
			printf("%s\n",dirName);
			break;
		}
	}
}

void findall(char *dirName){
	DIR *dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];

	dir = opendir(dirName);
	if (dir == 0)
	{
		//printf("Error opening directory");
		closedir(dir);
		return;
	}

	while ((dirEntry = readdir(dir)) != 0)
	{
		snprintf(name, MAX_PATH_LEN, "%s/%s", dirName, dirEntry->d_name);

		lstat(name, &inode);
		if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0){
			continue;
		}
		if (S_ISREG(inode.st_mode)) 
			printFindall(name);
	}
	closedir(dir);
}

void findallRecusive(char *dirName){
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
	findall(dirName);

	while ((dirEntry = readdir(dir)) != 0)
	{

		snprintf(name, MAX_PATH_LEN, "%s/%s", dirName, dirEntry->d_name);

		lstat(name, &inode);

		if (S_ISDIR(inode.st_mode))
		{
			if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
			{
				findallRecusive(name);
			}
		}
	}

	closedir(dir);
	free(dir);
	free(dirEntry);
	return;
} 
//---------------------  Findall end -----------------------
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
	else if(strcmp(argv[1],"extract")==0){
		if (argc != 5)
		{
			printf("Usage: %s extract path=<dir_path> section=<int> line=<int>\n", argv[0]);
			return 0;
		}
		int section=-1, line=-1;
		for (int i = 1; i < argc; i++) {
		 if (strncmp(argv[i], "path=", 5) == 0) {
            dir_path = &argv[i][5];
			//if(DEBUG)printf("dir_path: %s\n",dir_path);
        } else if (strncmp(argv[i], "section=", 8) == 0) {
            section=atoi(argv[i]+8);
			//if(DEBUG)printf("section: %d\n",section);
        } else if (strncmp(argv[i], "line=", 5) == 0) {
            line=atoi(argv[i]+5);
			//if(DEBUG)printf("line: %d\n",line);
        	}
   		}
		if(section!=-1&& line!=-1 && strlen(dir_path)>2){
			extract(dir_path,line,section);
		} else {
			printf("WRONG INPUT! \nUsage: %s extract path=<dir_path> section=<int> line=<int>\n", argv[0]);
			return 0;
		}
	}
	else if(strcmp(argv[1],"findall")==0){
		printf("SUCCESS\n");
		if (argc != 3)
		{
			printf("Usage: %s findall path=<dir_path>\n", argv[0]);
			return 0;
		}
		if (strncmp(argv[2], "path=", 5) == 0)
		{		
				parse_echo=0;
				extract_echo=0;
				dir_path = &argv[2][5];
				findallRecusive(dir_path);
		}
		else {
			printf("Usage: %s findall path=<dir_path>\n", argv[0]);
			return 0;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	//if(DEBUG_FINDALL)printf("SUCCESS\n");
	//printFindall("test_root/gYV5TqCoI/WUQAYeE3p/OzMXbM/TY9orYa2F/uggfOjKYuZ/LHdeb/ZjTd2Z.5sh");
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
