#include "headers.h"
#include "datastructure.h"

int key_id, key_id2;
int msgq_id, shmid;
// struct msgbuff
// {
//     long mtype;
//     char mtext[70];
// };
void intialize_message_queue();
void intialize_shared_memory();
struct PCB *generate_process();
void round_robin(int Q);
void multilevel(int Q);
void tostring(char str[], int num);
void handler(int signum);
void increment_processes_terminated(int signum);
void forking(struct PCB *process);
void SJF();
void HPF();
int process_count = 0, finished = 0;
int processes_terminated = 0, lastid;
int algorithm_type, waiting_time_utili = 0, time = 0;
float AvgWTA = 0;
float Avg_waiting = 0;
FILE *file;
int main(int argc, char *argv[])
{
    // printf("schedular id =%d\n",getpid());
    signal(SIGUSR1, handler);
    signal(SIGUSR2, increment_processes_terminated);
    intialize_message_queue();
    // intialize_shared_memory();
    file = fopen("scheduler.log", "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1; // Exit if file cannot be opened
    }
    fprintf(file, "#At time\tx\tprocess\ty\tstate\tarr\tw\ttotal\tz\tremain\ty\twait\tk\n");
    fclose(file);
    initClk();
    algorithm_type = atoi(argv[1]);
    int Quantum = atoi(argv[2]);
    time = 1;
    if (algorithm_type == 3)
    {
        round_robin(Quantum);
    }
    else if (algorithm_type == 4)
    {
        multilevel(Quantum);
    }
    else if (algorithm_type == 1)
    {
        SJF();
    }
    else
        HPF();

    file = fopen("scheduler.perf", "w");

    // Check if file is successfully opened
    if (file == NULL)
    {
        printf("Error: Could not create file.\n");
    }
    // Write the performance data to the file
    // fprintf(file, "scheduler.perf example\n");
    printf("wait %d\n", waiting_time_utili);
    float clk = getClk();
    fprintf(file, "CPU utilization = %f%%\n", (clk - waiting_time_utili) * 100 / clk);
    fprintf(file, "AvgWTA = %.2f\n", AvgWTA / process_count);
    fprintf(file, "Avg Waiting = %.1f\n", Avg_waiting / process_count);

    // Close the file
    fclose(file);
    // printf("go away\n");
    //  TODO: implement the scheduler.
    //  TODO: upon termination release the clock resources.
    kill(getppid(), SIGINT);
    printf("destory me\n");
    exit(0);

    destroyClk(true);
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

int get_data_from_shared_memory()
{
    void *shmaddr = shmat(shmid, NULL, 0);
    int *remaining_time;
    if (shmaddr == (void *)-1)
    {
        perror("Error in attach ");
        exit(-1);
    }
    remaining_time = (int *)shmaddr;
    printf("\nremaining_time = %d\n", *remaining_time);
}
void attach(int shmid, int msg)
{
    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == (void *)-1)
    {
        perror("Error in attach ");
        exit(-1);
    }
    // else
    //{

    // strcpy((int *)shmaddr, msg);
    //}
}
struct PCB *generate_process()
{
    struct process_input_data msgprocess;

    int recieved = msgrcv(msgq_id, &msgprocess, sizeof(struct process_input_data), 0, IPC_NOWAIT); // remove the

    if (recieved == -1 || lastid == msgprocess.id)
    {
        // printf("Error in receiving schedular\n");
        return NULL;
    }
    else
    {
        process_count++;
        struct PCB *newpcb = malloc(sizeof(struct PCB));
        lastid = msgprocess.id;
        newpcb->id = msgprocess.id;
        newpcb->arrivalTime = msgprocess.arrivalTime;
        newpcb->runTime = msgprocess.runTime;
        newpcb->priority = msgprocess.priority;
        newpcb->waitingTime = 0;
        newpcb->remainingTime = msgprocess.runTime;
        newpcb->endTime = -1;
        newpcb->startTime = -1;
        newpcb->stoppedTime = -1;
        newpcb->state = "";
        // printing the process data at getclk()
        printf("at time %d process %d arrives with arrival %d\n", getClk(), newpcb->id, newpcb->arrivalTime);
        // int obj=getClk();
        // while(obj==getClk());
        return newpcb;
    }
}

void round_robin(int Q)
{
    int Finished_Process = -1;
    struct PCB *process = NULL;
    // int process_during_wait = 0;
    int pid, status;
    int count_Q = 0;
    file = fopen("scheduler.log", "a");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return; // Exit if file cannot be opened
    }

    while (Finished_Process != process_count || !finished || processes_terminated != process_count)
    {
        // printf("start again\n");

        process = generate_process();
        // printf("process id %p at time %d\n", process, getClk());
        // printf("process id %p at time %d\n", process, getClk());
        forking(process);
        if (front != NULL)
        {

            // i need a processing in from of a while loop that is equal to the quantum
            // or the remaining time if it's less than quantum ==> Q+currenttime=finishtime
            current = front;
            if (count_Q != Q)
            {
                printf("progress on process  %d at time %d \n", current->pcb->id, getClk());
                printList();
                if (count_Q == 0 && current->pcb->startTime == -1)
                {
                    // printf("progress\n");
                    current->pcb->startTime = getClk();
                    current->pcb->state = "started";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current->pcb->id, current->pcb->state, current->pcb->arrivalTime, current->pcb->runTime, current->pcb->remainingTime, current->pcb->waitingTime);
                }
                else if (count_Q == 0 && current->pcb->startTime != -1)
                {
                    current->pcb->waitingTime = current->pcb->waitingTime + getClk() - current->pcb->stoppedTime;
                    current->pcb->state = "resumed";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current->pcb->id, current->pcb->state, current->pcb->arrivalTime, current->pcb->runTime, current->pcb->remainingTime, current->pcb->waitingTime);
                }

                kill(current->pcb->pid, SIGCONT);

                count_Q++;
                current->pcb->remainingTime--;
                int obj = getClk();
                while (obj == getClk())
                {
                    // if (process_during_wait==0)
                    // {
                    process = generate_process();

                    // printf("process id %p at time %d\n", process, getClk());
                    forking(process);
                }
            }

            if (current->pcb->remainingTime == 0)
            {
                printf("process with id %d Finished  at time %d\n", current->pcb->id, getClk());
                current->pcb->state = "finished";
                count_Q = 0;
                current->pcb->endTime = getClk();
                if (Finished_Process == -1)
                {
                    Finished_Process = Finished_Process + 2;
                }
                else
                {
                    Finished_Process++;
                }
                int TA = current->pcb->endTime - current->pcb->arrivalTime;
               float WTA = (float) TA / current->pcb->runTime;
                AvgWTA += WTA;
                Avg_waiting += current->pcb->waitingTime;
                fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%f\n", getClk(), current->pcb->id, current->pcb->state, current->pcb->arrivalTime, current->pcb->runTime, current->pcb->remainingTime, current->pcb->waitingTime, TA, WTA);
                // int status;
                // int pid =current->pcb->pid;
                Dequeue();
                // printf("finished = %d && process count =%d && finished process =%d\n",finished,process_count,Finished_Process);
                // waitpid(pid,&status,0);
            }
            else if (count_Q == Q)
            {
                count_Q = 0;
                // printf("stopped\n");
                current->pcb->stoppedTime = getClk();
                current->pcb->state = "stopped";
                kill(current->pcb->pid, SIGSTOP);
                fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current->pcb->id, current->pcb->state, current->pcb->arrivalTime, current->pcb->runTime, current->pcb->remainingTime, current->pcb->waitingTime);
                Enqueue(current->pcb);
                Dequeue();
            }
            // printList();
        }
        else
        {

            if (time != getClk() && getClk() != 0 && (Finished_Process != process_count || !finished))
            {
                // fprintf(file, "current time %d time %d\n", getClk(), time);
                time = getClk();
                waiting_time_utili++;
            }
        }
    }
    fclose(file);
}
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;

    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}
void handler(int signum)
{
    // printf("finished sending\n");
    finished = 1;
}
void increment_processes_terminated(int signum)
{
    processes_terminated++;
}
void forking(struct PCB *process)
{
    int pid;
    if (process != NULL)
    {
        pid = fork();

        if (pid == -1)
            perror("error in fork");

        else if (pid == 0)
        {
            printf("run process id %d\n", process->id);
            int remainingtime = (process->remainingTime);
            char remainingtime_str[10];
            tostring(remainingtime_str, remainingtime);
            execl("process.out", "./process.out", remainingtime_str, NULL);
        }
        else
        {
            process->pid = pid;

            printf("process id %d is forked with process id %d\n", process->id, process->pid);
            if (algorithm_type == 3)
                Enqueue(process);
            else if (algorithm_type == 4)
                enqueue(process, process->priority);
            else if (algorithm_type == 1)
            {
                process->stoppedTime = getClk();

                // if (pq_front == NULL)
                //{
                enqueueprio(process, process->remainingTime);
                //}
                // else
                //{
                // Enqueue(process);
                //}
            }
            else
            {
                process->stoppedTime = getClk();

                // if (pq_front == NULL)
                //{
                enqueueprio(process, process->priority);
                //}
                // else
                //{
                //    Enqueue(process);
                //}
            }

            // printList();
            kill(pid, SIGSTOP);
            // continue;
            printf("Parent\n");
        }
    }
}
void multilevel(int Q)
{
    int Finished_Process = -1;
    struct PCB *process = NULL;
    int current_level = -50;
    // int process_during_wait = 0;
    int pid, status;
    int count_Q = 0;
    int count_empty_levels = 0;
    file = fopen("scheduler.log", "a");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return; // Exit if file cannot be opened
    }
    intializeQueue();
    while (Finished_Process != process_count || !finished || processes_terminated != process_count)
    {
        // printf("start again\n");
        process = generate_process();
        // printf("process id %p at time %d\n", process, getClk());
        // printf("process id %p at time %d\n", process, getClk());
        forking(process);
        count_empty_levels = 0;
        for (int i = 0; i <= 10; i++)
        {

            while (prio[i].queue->front != NULL)
            {

                if (i - 1 > current_level && current_level >= 0)
                {
                    i = current_level;
                    current_level = -50;
                }
                current_queue = prio[i].queue->front;
                PrintQueue();

                if (count_Q != Q)
                {
                    printf("progress on process  %d at time %d \n", current_queue->pcb->id, getClk());
                    // PrintQueue();
                    if (count_Q == 0 && current_queue->pcb->startTime == -1)
                    {

                        current_queue->pcb->startTime = getClk();
                        current_queue->pcb->state = "started";
                        fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_queue->pcb->id, current_queue->pcb->state, current_queue->pcb->arrivalTime, current_queue->pcb->runTime, current_queue->pcb->remainingTime, current_queue->pcb->waitingTime);
                    }
                    else if (count_Q == 0 && current_queue->pcb->startTime != -1)
                    {
                        current_queue->pcb->waitingTime = current_queue->pcb->waitingTime + getClk() - current_queue->pcb->stoppedTime;
                        current_queue->pcb->state = "resumed";
                        fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_queue->pcb->id, current_queue->pcb->state, current_queue->pcb->arrivalTime, current_queue->pcb->runTime, current_queue->pcb->remainingTime, current_queue->pcb->waitingTime);
                    }
                    kill(current_queue->pcb->pid, SIGCONT);
                    // printf("i arrivied at level %d \n", i);
                    count_Q++;
                    current_queue->pcb->remainingTime--;
                    int obj = getClk();
                    while (obj == getClk())
                    {
                        // if (process_during_wait==0)
                        // {
                        process = generate_process();

                        // printf("process id %p at time %d\n", process, getClk());
                        forking(process);
                        if (process != NULL)
                        {
                            if (process->priority < i)
                            {

                                current_level = process->priority;
                            }
                        }
                    }
                }
                if (current_queue->pcb->remainingTime == 0)
                {

                    printf("process with id %d Finished  at time %d\n", current_queue->pcb->id, getClk());

                    count_Q = 0;
                    current_queue->pcb->state = "finished";
                    current_queue->pcb->endTime = getClk();
                    if (Finished_Process == -1)
                    {
                        Finished_Process = Finished_Process + 2;
                    }
                    else
                    {
                        Finished_Process++;
                    }
                    int TA = current_queue->pcb->endTime - current_queue->pcb->arrivalTime;
                     float WTA = (float) TA / current_queue->pcb->runTime;
                    AvgWTA += WTA;
                    Avg_waiting += current_queue->pcb->waitingTime;
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%f\n", getClk(), current_queue->pcb->id, current_queue->pcb->state, current_queue->pcb->arrivalTime, current_queue->pcb->runTime, current_queue->pcb->remainingTime, current_queue->pcb->waitingTime, TA, WTA);

                    dequeue(current_queue->pcb, i);
                }
                if (count_Q == Q)
                {

                    count_Q = 0;
                    current_queue->pcb->state = "stopped";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_queue->pcb->id, current_queue->pcb->state, current_queue->pcb->arrivalTime, current_queue->pcb->runTime, current_queue->pcb->remainingTime, current_queue->pcb->waitingTime);
                    current_queue->pcb->stoppedTime = getClk();
                    kill(current_queue->pcb->pid, SIGSTOP);
                    if (i != 10)
                    {
                        enqueue(current_queue->pcb, i + 1);
                        dequeue(current_queue->pcb, i);
                    }
                    else
                    {
                        enqueue(current_queue->pcb, current_queue->pcb->priority);
                        dequeue(current_queue->pcb, i);
                    }
                }
            }
            if (prio[i].queue->front == NULL)
            {
                count_empty_levels++;
            }
        }
        if (count_empty_levels == 11)
        {
            if (time != getClk() && getClk() != 0 && (Finished_Process != process_count || !finished))
            {
                // fprintf(file, "current time %d time %d\n", getClk(), time);
                time = getClk();
                waiting_time_utili++;
            }
        }
    }
    fclose(file);
}
void SJF()
{
    int Finished_Process = -1;
    struct PCB *process = NULL;
    struct PCB *current_process = NULL;
    int firsttime=0;
    // int process_during_wait = 0;
    int pid, status;
    // int count_Q = 0;
    file = fopen("scheduler.log", "a");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return; // Exit if file cannot be opened
    }
    while (Finished_Process != process_count || !finished || processes_terminated != process_count)
    {
        // printf("start again\n");

        process = generate_process();
        // printf("process id %p at time %d\n", process, getClk());
        // printf("process id %p at time %d\n", process, getClk());
        forking(process);
        if (pq_front != NULL || current_process != NULL)
        {
            if (current_process == NULL)
            {
                current_process = dequeueprio();
            }
            printprioQueue();
            // current_ptr = pq_front;
            printf("Hello %d", current_process->id);
            if (current_process->remainingTime != 0)
            {
                printf("progress on process  %d at time %d \n", current_process->id, getClk());
                printprioQueue();
                if (current_process->startTime == -1)
                {

                    // printf("progress\n");
                    current_process->startTime = getClk();
                    current_process->waitingTime = current_process->startTime - current_process->stoppedTime;
                    printf("start %d stopped %d\n", current_process->startTime, current_process->stoppedTime);
                    current_process->state = "started";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_process->id, current_process->state, current_process->arrivalTime, current_process->runTime, current_process->remainingTime, current_process->waitingTime);
                    kill(current_process->pid, SIGCONT);
                }
                if (current_process->state == "stopped")
                {
                    current_process->waitingTime = current_process->waitingTime + getClk() - current_process->stoppedTime;
                    current_process->state = "resumed";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_process->id, current_process->state, current_process->arrivalTime, current_process->runTime, current_process->remainingTime, current_process->waitingTime);
                    kill(current_process->pid, SIGCONT);
                }
                // count_Q++;
                current_process->remainingTime--;
                int obj = getClk();
                while (obj == getClk())
                {
                    // if (process_during_wait==0)
                    // {
                    process = generate_process();

                    // printf("process id %p at time %d\n", process, getClk());
                    forking(process);
                }
            }

            if (current_process->remainingTime == 0)
            {
                printf("process with id %d Finished  at time %d\n", current_process->id, getClk());
                current_process->state = "finished";
                // count_Q = 0;
                current_process->endTime = getClk();
                if (Finished_Process == -1)
                {
                    Finished_Process = Finished_Process + 2;
                }
                else
                {
                    Finished_Process++;
                }
                int TA = current_process->endTime - current_process->arrivalTime;
                 float WTA = (float)  TA / current_process->runTime;
                AvgWTA += WTA;
                Avg_waiting += current_process->waitingTime;
                // printf("thankyoo\n");
                fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%f\n", getClk(), current_process->id, current_process->state, current_process->arrivalTime, current_process->runTime, current_process->remainingTime, current_process->waitingTime, TA, WTA);
                // int status;
                // int pid =current->pcb->pid;

                // dequeueprio();
                //  while (front != NULL)
                //  {
                //      enqueueprio(front->pcb, front->pcb->remainingTime);
                //      Dequeue();
                //  }
                current_process = NULL;
                printprioQueue();
                // printf("finished = %d && process count =%d && finished process =%d\n",finished,process_count,Finished_Process);
                // waitpid(pid,&status,0);
            }
            if (current_process != NULL && pq_front != NULL && firsttime==0)
            {
                if (current_process->remainingTime > pq_front->pcb->remainingTime)
                {
                    current_process->stoppedTime = getClk();
                    current_process->state = "stopped";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_process->id, current_process->state, current_process->arrivalTime, current_process->runTime, current_process->remainingTime, current_process->waitingTime);
                    kill(current_process->pid, SIGSTOP);
                    enqueueprio(current_process, current_process->remainingTime);
                    current_process = NULL;
                }
                firsttime=1;
            }
        }
        if (pq_front == NULL && current_process == NULL)
        {
            if (time != getClk() && getClk() != 0 && (Finished_Process != process_count || !finished))
            {
                // fprintf(file, "current time %d time %d\n", getClk(), time);
                time = getClk();
                waiting_time_utili++;
            }
        }
    }
    fclose(file);
}
void HPF()
{

    int Finished_Process = -1;
    struct PCB *process = NULL;
    struct PCB *current_process = NULL;
    // int process_during_wait = 0;
    int pid, status;
    int current_id = 0;
    // int count_Q = 0;
    file = fopen("scheduler.log", "a");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return; // Exit if file cannot be opened
    }
    while (Finished_Process != process_count || !finished || processes_terminated != process_count)
    {

        // printf("start again\n");

        process = generate_process();

        //        // printf("process id %p at time %d\n", process, getClk());
        //         // printf("process id %p at time %d\n", process, getClk());
        forking(process);

        if (pq_front != NULL || current_process != NULL)
        {

            if (current_process == NULL)
            {
                current_process = dequeueprio();
            }
            printprioQueue();
            //         //             current_ptr = pq_front;
            if (current_process->remainingTime != 0)
            {
                printf("progress on process  %d at time %d \n", current_process->id, getClk());
                // printprioQueue();
                if (current_process->startTime == -1)
                {
                    // printf("progress\n");
                    current_process->startTime = getClk();
                    current_process->waitingTime = current_process->startTime - current_process->stoppedTime;
                    // printf("start %d stopped %d\n",current_process->startTime,current_process->stoppedTime);
                    current_process->state = "started";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_process->id, current_process->state, current_process->arrivalTime, current_process->runTime, current_process->remainingTime, current_process->waitingTime);
                    kill(current_process->pid, SIGCONT);
                }
                if (current_process->state == "stopped")
                {
                    current_process->waitingTime = current_process->waitingTime + getClk() - current_process->stoppedTime;
                    current_process->state = "resumed";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_process->id, current_process->state, current_process->arrivalTime, current_process->runTime, current_process->remainingTime, current_process->waitingTime);
                    kill(current_process->pid, SIGCONT);
                }

                current_process->remainingTime--;
                int obj = getClk();
                while (obj == getClk())
                {
                    process = generate_process();
                    forking(process);
                }
            }
            printf("busyyyyyyyyy\n");
            if (current_process->remainingTime == 0)
            {
                printf("process with id %d Finished  at time %d\n", current_process->id, getClk());
                current_process->state = "finished";
                //                 // count_Q = 0;
                current_process->endTime = getClk();
                if (Finished_Process == -1)
                {
                    Finished_Process = Finished_Process + 2;
                }
                else
                {
                    Finished_Process++;
                }
                int TA = current_process->endTime - current_process->arrivalTime;
                float WTA = (float)TA / current_process->runTime;
                AvgWTA += WTA;
                Avg_waiting += current_process->waitingTime;
                // printf("thankyoo\n");
                fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%f\n", getClk(), current_process->id, current_process->state, current_process->arrivalTime, current_process->runTime, current_process->remainingTime, current_process->waitingTime, TA, WTA);
                current_process = NULL;
            }
            if (current_process != NULL && pq_front != NULL)
            {
                if (current_process->priority > pq_front->pcb->priority)
                {
                    current_process->stoppedTime = getClk();
                    current_process->state = "stopped";
                    fprintf(file, "At time %d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), current_process->id, current_process->state, current_process->arrivalTime, current_process->runTime, current_process->remainingTime, current_process->waitingTime);
                    kill(current_process->pid, SIGSTOP);
                    enqueueprio(current_process, current_process->priority);
                    current_process = NULL;
                }
            }

            printprioQueue();
        }
        if (pq_front == NULL && current_process == NULL)
        {
            if (time != getClk() && getClk() != 0 && (Finished_Process != process_count || !finished))
            {
                //    fprintf(file, "current time %d time %d\n", getClk(), time);
                time = getClk();
                waiting_time_utili++;
            }
        }
    }
    fclose(file);
}
