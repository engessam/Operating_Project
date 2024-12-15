#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int key_id,key_id2;
int msgq_id,shmid;
struct msgbuff
{
    long mtype;
    char mtext[70];
};
void  intialize_message_queue();
void intialize_shared_memory(); 
int main(int agrc, char *argv[])
{ 
    
    initClk();
        
    remainingtime=atoi(argv[1]);  
    //int rec_val;
    //printf("\nremaining_time of process %d = %d\n",getpid(),remainingtime);
    //struct msgbuff message1;
    int old_version=0;
        //remainingtime = ??;
        while (remainingtime > 0)
        {
          remainingtime--;
         printf("remaining time of process %d = %d after decrement at time %d\n",getpid(),remainingtime,getClk());
        old_version = getClk();
        while(old_version == getClk());
        
       
        //   *((int *)shmaddr) = remainingtime;
        // }
        
            // remainingtime = ??;
        //remaining_time_ptr=(int *)shmaddr;  
        //remainingtime=*remaining_time_ptr;
        
        }
         
        printf("Process %d terminated at time %d\n",getpid(),getClk());
        //printf("Current Time is %d\n", getClk());
        kill(getppid(),SIGUSR2);
        destroyClk(false);
        
        return 0;
}
void intialize_message_queue()
{
    key_id = ftok("pr_sch_file", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    printf("Message Queue ID = %d\n", msgq_id);
}
void intialize_shared_memory()
{
  key_id2 = ftok("pr_sch_file", 63);
  shmid = shmget(key_id2, 4096, IPC_CREAT | 0666);
  if (shmid == -1)
  {
    perror("Error in create");
    exit(-1);
  }
  else
     printf("\nShared memory ID = %d\n", shmid);
}