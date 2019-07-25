/*
*  Name: Yuvashree
*  Login: SP_19_CPS536_09
*  Purpose:The goal is to become familiar with the basic ideas of processes in UNIX.

*  Bug report: “No bugs, and all the features have been implemented”  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){

        //initializations
        int i, n, myid;
        pid_t childpid, ppid;
        myid = 1;
        int eo ;
        int odd = 0;
        int even = 0;
        int rand_num, val = 0;

        //usage check
        if(argc !=2){
                fprintf(stderr,"Usage: %s <nprocs>\n",argv[0]);
                exit(1);
        }
        else{
                n = atoi(argv[1]);
        }

        //n range check
        if (!(n>=1 && n<=10)){
                fprintf(stderr,"nprocs must be in [1 ... 10]\n");
                exit(1);
        }
        //if input is 1 then random number for current process is created.
        if(n == 1){
                srand(getpid());
                rand_num = rand();
                val = rand_num % 50000;
                fprintf(stderr,"myid = %d  pid = %d  ppid = %d  val = %d\n",myid, getpid(),getppid(),val);
                if(val%2 == 0){
                        even = 1;
                }
                else{
                        odd = 1 ;
                }
                fprintf(stderr,"FINAL RESULTS: #even =  %d #odd =  %d \n",even,odd);

        }

        //loop for creating n process
        for(i=1; i<n; i++){

                childpid = fork();

                //checks for fork failure
                if(childpid <= -1){
                        printf("fork failed\n");
                        exit(1);
                }

                //generating random number for each  process
                srand(getpid());
                rand_num = rand();
                val = rand_num % 50000;

                //child does
                if(childpid == 0){
                        myid+=1;
                        fprintf(stderr,"myid = %d  pid = %d  ppid = %d  val = %d\n",myid, getpid(),getppid(),val);

                        //assigning eo = 1 for odd random number and 10 for even random number
                        //this is only of last process in chain and exiting it
                        if(i == (n-1)){
                                eo=1;
                                if(val%2 == 0){
                                        eo = 10;
                                }
                                exit(eo);
                        }
                }

                //parent does
                else{

                        int oddOrEven;
                        if(i == 1){
                                myid=1;
                                fprintf(stderr,"myid = %d  pid = %d  ppid = %d  val = %d\n",myid, getpid(),getppid(),val);
                        }

                        wait(&oddOrEven);
                        int newOddOrEven = oddOrEven/256;

                        //if exited successfully
                        if(WIFEXITED(oddOrEven)){
                                //first digit is for even
                                //second digit is for odd
                                even =newOddOrEven/10;
                                odd =newOddOrEven%10;
                                fprintf(stderr,"myid is %d  received #even = %d  #odd = %d \n",myid,even,odd);
                        }
                        //even and odd becomes 0 for for unsuccessfull termination
                        else{
                                even = 0;
                                odd = 0;
                                fprintf(stderr,"myid is %d, abnormal exit of child, therefore received #even = %d  #odd = %d \n",myid,even,odd);
                        }
                        //condition to find odd or even and updating the variable
                        if(val%2 == 0){
                                newOddOrEven+=10;
                        }
                        else if(val%2 !=0){
                                newOddOrEven+=1;
                        }

                        //first process prints the final result
                        if(myid == 1){
                                even =newOddOrEven/10;
                                odd =newOddOrEven%10;
                                fprintf(stderr,"\nFINAL RESULTS: #even =  %d #odd =  %d \n",even,odd);
                        }

                        exit(newOddOrEven);
                }

        }

}

