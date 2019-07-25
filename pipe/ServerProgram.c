#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
int main(int argc, char *argv[]){
        FILE *fp;
        //usage check
        if(argc == 4){
                fp = fopen(argv[1],"r");
                if(fp == NULL){
                        fprintf(stderr,"Error opening %s\n",argv[1]);
                        exit(1);
                }
                if(argv[2]!=0 && argv[3] !=0){
                        if(atoi(argv[2])>atoi(argv[3])){
                                fprintf(stderr,"%d <= %d required.\n",atoi(argv[2]),atoi(argv[3]));
                                exit(1);
                        }
                }
                else{
                        fprintf(stderr,"Please enter valid zipcodes\n");
                        exit(1);
                }
        }
        else{
                fprintf(stderr,"Usage: server <data-file-name> <low-zip> <high-zip>\n");
                exit(1);
        }

        //pipe
        int pipe1[2];
        int pipe2[2];
        int pipe3[2];
        int pipe4[2];
        int cid;
        int data=1;
        int lowzip = atoi(argv[2]);
        int highzip = atoi(argv[3]);
        char c;
        //pipe for client 1
        if(pipe(pipe1) < 0){
                perror("Pipe creation failed");
                exit(1);
        }
        if(pipe(pipe2) < 0){
                close(pipe1[0]);
                close(pipe1[1]);
                perror("pipe2 creaton failed");
                exit(1);
        }
        cid = fork();
        //client 1 does
        if(cid==0){
                close(pipe1[1]);
                close(pipe2[0]);
                dup2(pipe1[0],STDIN_FILENO);
                dup2(pipe2[1],STDOUT_FILENO);
                execlp("./cli","cli",NULL);
        }
        //server does
        else{

                close(pipe1[0]);
                close(pipe2[1]);
                write(pipe1[1],&lowzip,sizeof(int));
                write(pipe1[1],&highzip,sizeof(int));

                //seperating data file
                char chnumzip;
                int numzip;
                int halfdata;
                int spacecount;
                fscanf(fp, "%d\n", &numzip);
                halfdata = numzip/2;

                //sending data to client 1
                while((c = getc(fp)) != EOF){
                        if(c == ' '|| c == '\n'){
                                spacecount+=1;
                                if(spacecount ==  halfdata){
                                        break;
                                }
                        }
                        write(pipe1[1],&c,sizeof(char));

                }
                fprintf(stderr,"Client 1 was sent %d zipcodes\n",halfdata);
                wait(NULL);
                //pipes for client2
                if(pipe(pipe3) < 0){
                        close(pipe1[1]);
                        close(pipe1[0]);
                        close(pipe2[0]);
                        close(pipe2[1]);
                        perror("Pipe3 creation failed");
                }
                if(pipe(pipe4) < 0){
                        close(pipe1[1]);
                        close(pipe1[0]);
                        close(pipe2[0]);
                        close(pipe2[1]);
                        close(pipe3[0]);
                        close(pipe3[1]);
                        perror("pipe4 creaton failed");
                }
                int cli2;
                cli2 = fork();

                //cli 2 executes
                if(cli2 == 0){
                        close(pipe3[1]);
                        close(pipe4[0]);
                        dup2(pipe3[0],STDIN_FILENO);
                        dup2(pipe4[1],STDOUT_FILENO);
                        execlp("./cli","cli",NULL);


                }
                if(cli2>0){
                        close(pipe3[0]);
                        close(pipe4[1]);
                        write(pipe3[1],&lowzip,sizeof(int));
                        write(pipe3[1],&highzip,sizeof(int));
 //                     dup2(pipe4[0],STDIN_FILENO);

                        spacecount = 0;
                        //sending data to client 2

                        while((c = getc(fp)) != EOF){
                                if(c == ' '|| c == '\n'){
                                        spacecount+=1;
                                }
                                write(pipe3[1],&c,sizeof(char));
                        }
                        close(pipe3[1]);
                        fprintf(stderr,"client 2 was sent %d\n",spacecount);
                        wait(NULL);

                        //reading data from client 2
                        int count=0;
                        int countcli1 = 0;
                        int data2=1;
                        while(data2!=0){
                                read(pipe4[0],&data2,sizeof(int));
                                if(data2!=0){
                                        countcli1+=1;
                                        fprintf(stderr,"%d ",data2);
                                }
                                else{
                                        fprintf(stderr,"\n");
                                        break;
                                }
                                count+=1;
                                if(count == 8){
                                        fprintf(stderr,"\n");
                                        count = 0;
                                }

                        }
                        if(countcli1>0){
                                fprintf(stderr,"Client 2 found %d\n",countcli1);
                        }
                        else{
                                fprintf(stderr,"Client 2 found none\n");
                        }
                }





                //reading data from client 1
                int zipcount=0;
                int countcli2 = 0;
                while(data!=0){
                        read(pipe2[0],&data,sizeof(int));
                        if(data!=0){
                                fprintf(stderr,"%d ",data);
                                countcli2 +=1;
                        }
                        else{
                                fprintf(stderr,"\n");
                                break;
                        }
                        zipcount+=1;
                        if(zipcount == 8){
                                fprintf(stderr,"\n");
                                zipcount = 0;
                        }
                }
                if(countcli2>0){
                        fprintf(stderr,"Client 1 found %d\n",countcli2);
                }
                else{
                        fprintf(stderr,"Client 1 found none\n");
                }

        }
}



