#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef PATH_MAX
#define PATH_MAX 255
#endif
int count = 0;
void dir_details(char * d_path,int size){
        //initialization
        DIR *dirp;
        struct dirent *direntp;
        struct stat statbuf;

        //open directory
        if((dirp = opendir(d_path)) == NULL){
                fprintf(stderr,"Could not open %s directory : %s\n",d_path,strerror(errno));
                exit(1);
        }
        //reading directory
        while((direntp=readdir(dirp))!=NULL){

                //chcking for directory or not
                if(direntp->d_type != DT_DIR && strcmp(direntp->d_name,".")!=0 && strcmp(direntp->d_name,"..")!=0 ){

                        char filepath[PATH_MAX];
                        sprintf(filepath, "%s/%s", d_path, direntp->d_name);
                        //obtain status of file and error check
                        if(stat(filepath,&statbuf) == -1){
                                fprintf(stderr,"stat failed on %s\n",filepath);
                                closedir(dirp);
                                exit(1);
                        }
                        //size comparison
                        if(statbuf.st_size >= size){
                                printf("\t%u %s\n",statbuf.st_size,filepath);
                                count++;

                        }

                }
                else if(direntp->d_type == DT_DIR){
                        //if directory reading further by recurrsion
                        char path[PATH_MAX];
                        sprintf(path, "%s/%s", d_path, direntp->d_name);
                        dir_details(path,size);

                }

        }
        closedir(dirp);
}



int main(int argc, char *argv[]){
        //initialization
        char  mycwd[PATH_MAX];
        int size;
        char cwd[PATH_MAX];
        char d_path[PATH_MAX];
        char root[PATH_MAX];
        //error check
        if(argc !=3){
                fprintf(stderr,"Usage : <program-name>  <root> <limit>");
                exit(1);
        }
        //size initialization
        size = atof(argv[2]) * 1024;
        printf("Links with size >= %d bytes:\n",size);

        //checking for absolute path
        if((argv[1][0])=='/'){
                strcpy(root,argv[1]);
                dir_details(root,size);
        }
        else{
                strcpy(root,argv[1]);
                getcwd(mycwd, PATH_MAX);
                sprintf(d_path, "%s/%s", mycwd, root);
                dir_details(d_path,size);

        }
        printf("Total number of links found = %d\n",count);

}

