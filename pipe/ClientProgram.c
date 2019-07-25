#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main (){

        //initialization
        char temp[2090];
        int data;
        char newtemp[200];
        int arrdata[200];
        int lowzip=0;
        int highzip=0;

        //reading from server through STDIN
        read(STDIN_FILENO,&lowzip,sizeof(int));
        read(STDIN_FILENO,&highzip,sizeof(int));
        read(STDIN_FILENO,&temp,sizeof(temp));

        int i;
        int j = 0;
        //k is number of integers in arrdata
        int k = 0;
        //loop for converting char to int and putting in array
        for(i=0;i<strlen(temp);i++){

                if(temp[i]!=' '&& temp[i]!='\n'){
                        newtemp[j] = temp[i];
                        j+=1;
                }
                else{
                        data = atoi(newtemp);
                        arrdata[k]=data;
                        k+=1;
                        j=0;
                }
        }
        int m = 0;
        int n = 0;
        int newarr[k];
        //loop for writing integers in specified range
        for(m;m<k;m++){
                if(arrdata[m]>=lowzip && arrdata[m]<=highzip){
                        newarr[n] = arrdata[m];
                        write(STDOUT_FILENO,&newarr[n],sizeof(newarr[n]));
                        n+=1;
                }

        }
        int eof = 0;
        write(STDOUT_FILENO,&eof,sizeof(int));
        exit(1);
}