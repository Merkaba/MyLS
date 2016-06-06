#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

/*char * printSize(int intInput){
    double input = (double) intInput;
    char * result = malloc(1000);
    if(intInput / 1024000000 > 0) 
    {
        sprintf(result, "%fG", input / 1024000000);
	}
    else if(intInput / 1024000 > 0) 
    {
        sprintf(result, "%fM", input / 1024000);
	}
    else if(intInput / 1024 > 0)
    {
	    sprintf(result, "%fK", input / 1024);
	}
    else
    {
	    sprintf(result, "%f", input);
    }
    return result;
}
*/

char * getTimeModified(time_t mtime){
    char * printTime = malloc(100);
    strftime(printTime, 100, "%b %d %R", localtime(&mtime));
   
    return printTime;
}


char * getFilePermissions(struct stat mystat){
    printf( (S_ISDIR(mystat.st_mode)) ? "d" : "-");
    printf( (mystat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (mystat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (mystat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (mystat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (mystat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (mystat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (mystat.st_mode & S_IROTH) ? "r" : "-");
    printf( (mystat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (mystat.st_mode & S_IXOTH) ? "x" : "-");
    return "";//just returns a char * so i can use it inside my printf
} 

void newDir(struct dirent *entry, struct stat mystat, char * newStr, int depth){
    sprintf(newStr, "%s/", newStr); //add a slash to the end for recursion purposes 
    workFile(entry, mystat, newStr, depth); //need to display folder before recurisng into it
    //printf("trying to recurse on file name %s\n", newStr);
    ls(newStr, depth + 1);
}

void workFile(struct dirent *entry, struct stat mystat, char * newStr, int depth){
    stat(newStr, &mystat);
    struct passwd *uidVal = getpwuid(mystat.st_uid);
    struct group *gidVal = getgrgid(mystat.st_gid);
    addTabsForDepth(depth); 
    printf("%s %d %s %s %d %s %s\n", getFilePermissions(mystat), mystat.st_nlink, uidVal->pw_name, gidVal->gr_name, mystat.st_size, getTimeModified(mystat.st_mtime) ,entry->d_name);
}

void addTabsForDepth(int depth){
    int i = 0;
    for(; i < depth; i++){
        printf("    ");
    }
}

void checkForDirectory(struct dirent* entry, struct stat mystat, char * newStr, int depth){
    if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ 
        newDir(entry, mystat, newStr, depth); 
    }  
}

void checkForFile(struct dirent* entry, struct stat mystat, char * newStr, int depth){
    if(entry->d_type == DT_REG){ 
        workFile(entry, mystat, newStr, depth); 
    }
}

void entryWork(struct dirent *entry, struct stat mystat, const char *name, int depth){
    char newStr[1024];
    sprintf(newStr, "%s%s", name, entry->d_name);
    checkForDirectory(entry, mystat, newStr, depth);
    checkForFile(entry, mystat, newStr, depth);
}

void iterateDir(struct dirent *entry, DIR *dir, struct stat mystat, const char *name, int depth){
    while(entry = readdir(dir)){ 
        entryWork(entry, mystat, name, depth);
    }   
}

void ls(const char *name, int depth)
{
    DIR *dir = opendir(name);
    struct dirent *entry;
    struct stat mystat;
    if(dir != NULL)
    {
        iterateDir(entry, dir, mystat, name, depth);
    }
    closedir(dir);

}

int main(int argc, char* argv[])
{
    int i;
    if(argc > 0){
        for(i = 0; i < argc; i++){ ls(argv[i], 0); }
    }
    else{ ls("./", 0); } 
    return 0;
}






