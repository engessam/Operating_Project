#include "headers.h"

#include <ctype.h>
#include <string.h>
#include "datastructure.h"

void clearResources(int);
int get_num_processes(char *file);
void put_processes_in_PCB(struct process_input_data *pcb, int size, char *file);
void process_string(const char *input_string, int *output_array);
void tostring(char str[], int num);
void intialize_message_queue();
void intialize_shared_memory();
int key_id, key_id2;
int msgq_id, shmid;
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);

    intialize_message_queue();

    //intialize_shared_memory();

    // TODO Initialization

    int process_count = get_num_processes(argv[1]);
    struct process_input_data process[process_count];
    // // 1. Read the input files.
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    int algorithim_type = atoi(argv[3]);
    int quantam = 0;
    if (algorithim_type == 3 || algorithim_type == 4)
    {
        quantam = atoi(argv[5]);
    }
    // 3. Initiate and create the scheduler and clock processes.

    char process_count_str[10];
    char algorithim_type_str[10];
    char quantam_str[10];
    tostring(algorithim_type_str, algorithim_type);
    tostring(quantam_str, quantam);

    int sch_id, clk_id;
    printf("\nmy pid = %d\n", getpid());

    clk_id = fork();
    if (clk_id == -1)
        perror("error in fork");

    else if (clk_id == 0)
    {
        printf("run clock\n");
        execl("clk.out", "./clk.out", NULL);
    }
    sch_id = fork();
    //system("./scheduler.out");
    if (sch_id == -1)
        perror("error in fork");

    else if (sch_id == 0)
    {
        printf("run sch\n");
        execl("scheduler.out", "./scheduler.out", algorithim_type_str, quantam_str, NULL);
    }

    // 4. Use this function after creating the clock process to initialize clock.
    printf("inti clk process generator\n");
    initClk();
   // printf("Mohamed\n");

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    put_processes_in_PCB(process, process_count, argv[1]);
    // 6. Send the information to the scheduler at the appropriate time.
    int send_val, count = 0;
    printf("Current Time is %d\n", getClk());
    while (count < process_count)
    {
        // To get time use this function.
        int x = getClk();
        
        
        if (process[count].arrivalTime == x)
        {
            send_val = msgsnd(msgq_id, &process[count], sizeof(struct process_input_data), IPC_NOWAIT);
            if (send_val == -1)
            {
                perror("Errror in send");
            }

            else
            {
                count++;
               // printf("%d vs %d\n", count, x);
            }
            printf("Current Time is %d\n", getClk());
        }

        // printf("%d vs %d at %d\n",process_count,count,x);
    }
    //printf("schedular id =%d\n",sch_id);
    kill(sch_id,SIGUSR1);
    int status;
    waitpid(sch_id, &status, 0);
    printf("\nschedular terminated  with status %d", WEXITSTATUS(status)); 
    // 7. Clear clock resources
    
    destroyClk(true);
}

void clearResources(int signum)
{
    printf("Destroy Resources\n");
    // TODO Clears all resources in case of interruption
    msgctl(msgq_id, IPC_RMID, NULL);
    //shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
    exit(0);
    // killpg(getpgrp(),SIGKILL);
}
int get_num_processes(char *file)
{
    int process_count = 0;
    char line[256];
    FILE *var_file;
    // Open the file in read mode
    var_file = fopen(file, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    while (fgets(line, sizeof(line), var_file))
    {
        // Remove newline character at the end of the line if present
        line[strcspn(line, "\n")] = '\0';

        // // Check if the line starts with '#' after stripping leading spaces
        char *trimmed_line = line;
        // while (*trimmed_line == ' ') {
        //     trimmed_line++;
        // }

        if (*trimmed_line != '#')
        {
            // Print or process the line
            process_count++;
            // printf("%s\n", line);
        }
    }

    fclose(var_file);
    return process_count;
}
void put_processes_in_PCB(struct process_input_data *pcb, int size, char *file)
{

    char line[256];
    int output_file[4];
    FILE *var_file;
    int index = 0;
    // Open the file in read mode
    var_file = fopen(file, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    while (fgets(line, sizeof(line), var_file))
    {
        // Remove newline character at the end of the line if present
        line[strcspn(line, "\n")] = '\0';

        // // Check if the line starts with '#' after stripping leading spaces
        char *trimmed_line = line;
        // while (*trimmed_line == ' ') {
        //     trimmed_line++;
        // }

        if (*trimmed_line != '#')
        {
            process_string(line, output_file);
            pcb[index].id = output_file[0];
            pcb[index].arrivalTime = output_file[1];
            pcb[index].runTime = output_file[2];
            pcb[index].priority = output_file[3];
            index++;
        }
    }
    fclose(var_file);
}
void process_string(const char *input_string, int *output_array)
{
    int index = 0;
    char number_str[20]; // Temporary buffer to store numbers as strings
    int num_index = 0;

    // Loop through each character in the input string
    for (int i = 0; i < strlen(input_string); i++)
    {
        if (isdigit(input_string[i]))
        {
            // If it's a digit, append it to the number buffer
            number_str[num_index++] = input_string[i];
        }
        else if ((input_string[i] == ' ' || input_string[i] == '\t') && num_index > 0)
        {
            // If we encounter a space (or tab), and there's a number in the buffer
            number_str[num_index] = '\0';             // Null-terminate the number string
            output_array[index++] = atoi(number_str); // Convert to int and store in the output array
            num_index = 0;                            // Reset the number buffer
        }
    }

    // If the string ends with a number, add the last one to the array
    if (num_index > 0)
    {
        number_str[num_index] = '\0';
        output_array[index] = atoi(number_str);
    }
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

void intialize_message_queue()
{
   // printf("heloooooooooooooooo\n");
    key_id = ftok("pr_sch_file", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    //printf("heloooooooooooooooo\n");
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