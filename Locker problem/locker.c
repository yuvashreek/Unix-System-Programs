/*
*  Name: Yuvashree
*  Login: SP_19_CPS536_09

*  Bug report: “No bugs, and all the features have been implemented”  */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc,  char *argv[ ]) {
   pid_t childpid;             /* indicates process should spawn another     */
   int error;                  /* return value from dup2 call                */
   int fd[2];                  /* file descriptors returned by pipe          */
   int i;                      /* number of this process (starting with 1)   */
   int nprocs;                 /* total number of processes in ring          */
   int mlockers;
   int ringnum;
   int roundnum = 0;
   int *lockerArr;
   lockerArr =malloc((mlockers*sizeof(int)));
   int j;
   int temp;
   int flipcount = 0;
           /* check command line for a valid number of processes to generate */
   if ( (argc != 3) || ((nprocs = atoi (argv[1])) <= 0)){
        fprintf(stderr,"Usage: lockers <n>  <m> (and n divides m)\n");
        return 1;
   }
   if ( ((mlockers = atoi(argv[2])) %nprocs) !=0){
        fprintf(stderr,"Usage: lockers <n>  <m> (and n divides m)\n");
        return 1;
   }


   if (pipe (fd) == -1) {      /* connect std input to std output via a pipe */
      perror("Failed to create starting pipe");
      return 1;
   }
   if ((dup2(fd[0], STDIN_FILENO) == -1) ||
       (dup2(fd[1], STDOUT_FILENO) == -1)) {
      perror("Failed to connect pipe");
      return 1;
   }
   if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) {
      perror("Failed to close extra descriptors");
      return 1;
   }

   for (i = 1; i < nprocs;  i++) {         /* create the remaining processes */
      if (pipe (fd) == -1) {
         fprintf(stderr, "[%ld]:failed to create pipe %d: %s\n",
                (long)getpid(), i, strerror(errno));
         return 1;
      }
      if ((childpid = fork()) == -1) {
         fprintf(stderr, "[%ld]:failed to create child %d: %s\n",
                 (long)getpid(), i, strerror(errno));
         return 1;
      }

      if (childpid > 0){               /* for parent process, reassign stdout */
          error = dup2(fd[1], STDOUT_FILENO);
      }
      else{                              /* for child process, reassign stdin */
          error = dup2(fd[0], STDIN_FILENO);
      }
      if (error == -1) {
         fprintf(stderr, "[%ld]:failed to dup pipes for iteration %d: %s\n",
                 (long)getpid(), i, strerror(errno));
         return 1;
      }
      if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) {
         fprintf(stderr, "[%ld]:failed to close extra descriptors %d: %s\n",
                (long)getpid(), i, strerror(errno));
         return 1;
      }
      if (childpid)
         break;
   }
   //process 1 does
   if(i==1){
        ringnum = mlockers/nprocs;
        //initialize locker
        for(j=0; j < mlockers; j++) {
                lockerArr[j] = 1;
        }
        j = 1;
        int ring;
        for(ring = 1; ring<=ringnum+1; ring++){
                roundnum++;
                temp = roundnum;
				//flipping locker
                for(j=temp;j<=mlockers;j++){
                        if(j%roundnum == 0){
                                if(lockerArr[j-1]==0){
                                        lockerArr[j-1]=1;
                                }
                                else{
                                        lockerArr[j-1]=0;
                                }
                                flipcount++;
                        }
                }

                //wrintg round num and locker to process 2
                write(STDOUT_FILENO, &roundnum, sizeof(roundnum));
                for(j=0;j < mlockers; j++) {
                        write(STDOUT_FILENO,&lockerArr[j], sizeof(int));
                }

                //reading round num and locker details from last process
                read(STDIN_FILENO,&roundnum,sizeof(roundnum));
                for(j=0; j < mlockers; j++) {
                        read(STDIN_FILENO,&lockerArr[j], sizeof(int));
                }

                //shutdown - process 1 gives details of open lockers
                if(roundnum>mlockers){
                        fprintf(stderr,"Following lockers are open at the end:\n");
                        for(j=0; j<mlockers;j++){
                                if(lockerArr[j]==0){
                                        fprintf(stderr,"%d ",j+1);
                                }
                        }
                        fprintf(stderr,"\n");
                        return(0);
                }

        }
   }
   else{
        ringnum = mlockers/nprocs;
        int ring;
        for(ring = 1; ring<=ringnum+1; ring++){
                //shutdown - gives number of time flipped
                if(ring>ringnum){
                        fprintf(stderr,"Proc %d flipped %d lockers.\n",i,flipcount);
                }
                //reading round number and locker info
                read(STDIN_FILENO, &roundnum, sizeof(roundnum));
                roundnum++;
                for(j=0; j < mlockers; j++) {
                        read(STDIN_FILENO,&lockerArr[j], sizeof(int));
                }
                j=0;

                //flipping locker
                temp = roundnum;
                for(j=temp;j<=mlockers;j++){
                        if(j%roundnum == 0){
                                if(lockerArr[j-1]==0){
                                        lockerArr[j-1]=1;
                                }
                                else{
                                        lockerArr[j-1]=0;
                                }
                                flipcount++;
                        }
                }

                //wrinting round num and locker info to next process
                write(STDOUT_FILENO, &roundnum, sizeof(roundnum));
                for(j=0;j < mlockers; j++) {
                        write(STDOUT_FILENO,&lockerArr[j], sizeof(int));
                }



        }
   }
   return 0;
}

