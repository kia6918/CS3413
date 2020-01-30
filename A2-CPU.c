/* @author Yan Qiu #3537527 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/* limitations */
#define MAX_USER_LENGTH 10
#define MAX_PROCESS_LENGTH 5
#define MAX_LENGTH_PERLINE 128
#define MAX_RECORD_NUMBER 128

typedef struct person_info
{
    char user[MAX_USER_LENGTH];
    char process[MAX_PROCESS_LENGTH];
    int arrival;
    int duration;
    int priority;
    bool active; /* if current_tick >= arrival */
} person_info_t;

typedef struct person_summary
{
    char user[MAX_USER_LENGTH];
    int time;
} person_summary_t;

/* simulate current CPU time*/
static int current_tick = 0;

/* file name can be readed from cmdline */
static char input_filename[] = "a2-input.txt";
/* arrays to read all tasks info from input file */
static person_info_t infos[MAX_RECORD_NUMBER];
static int info_numbers;
static int num_of_user = 0;
static int num_of_cpu = 2;
static bool start = false;

static person_summary_t summary[MAX_RECORD_NUMBER];

/* return index of next node-to-add. -1 if there's nothing available */
static int choose_next_thread(person_info_t *p_infos, int info_numbers)
{
    int next_chosen = -1;
    int count = 0;

    for (count = 0; count < info_numbers; count++)
    {
        if ((p_infos[count].active) && (p_infos[count].duration > 0))
        {
            if (next_chosen < 0)
            {
                next_chosen = count;
            }
            else
            {
                bool same_priority = (p_infos[count].priority == p_infos[next_chosen].priority);
                bool same_arrival = (p_infos[count].arrival == p_infos[next_chosen].arrival);
                if ((p_infos[count].priority < p_infos[next_chosen].priority) || (same_priority && (p_infos[count].arrival < p_infos[next_chosen].arrival)) || (same_priority && same_arrival && (p_infos[count].duration < p_infos[next_chosen].duration)))
                {
                    next_chosen = count;
                }
            }
        }
    }

    return next_chosen;
}

/* return index of next node-to-add. -1 if there's nothing available */
static void update_active_status()
{
    int count = 0;
    for (count = 0; count < info_numbers; count++)
    {
        if (infos[count].arrival <= current_tick)
        {
            start = true;
            infos[count].active = true;
        }
    }
}

static void tick_run(int tick_time)
{

    int current_cpu;
    if (start)
    {
        printf("%d\t\t", current_tick);
        for (current_cpu = 0; current_cpu < num_of_cpu; current_cpu++)
        {
            int index = choose_next_thread(infos, info_numbers);
            if (index != -1)
            {
                printf("%s\t\t", infos[index].process);
            }
            else
            {
                printf("IDLE\t");
            }

            infos[index].active = false;
            infos[index].duration -= 1;
            if (infos[index].duration == 0)
            {
                int i;
                for (i = 0; i < num_of_user; i++)
                {
                    if (strcmp(summary[i].user, infos[index].user) == false)
                    {
                        summary[i].time = current_tick;
                    }
                }
            }
            if (current_cpu == num_of_cpu - 1)
            {
                printf("\n");
            }
        }
        update_active_status();
    }
}

int main(int argc, char *argv[])
{

    FILE *fp;
    char line_record[MAX_LENGTH_PERLINE];
    int record_index = 0, info_index = 0, count = 0;
    int total_duration = 0;
    int temp_index = 0;

    /**
     * NOTE: Please run the program using ./a a1-input.txt 2 for the formatting
     * If you don't specify the num_of_cpu AND file_name, it will use the default value automatically
     * If you want to include the num_of_cpu then you have to include the file_name in the comand line
    */
    if (argc == 2)
    {
        printf("The formatting is wrong, please include file_name & num_of_cpu: %s %s\n", argv[1], argv[2]);
        return -1;
    }

    if (argc == 3)
    {
        strcpy(input_filename, argv[1]);
        num_of_cpu = atoi(argv[2]);
    }

    fp = fopen(input_filename, "r");
    if (NULL == fp)
    {
        printf("cannot find input file %s\n", input_filename);
        return -1;
    }

    while (!feof(fp))
    {
        if (NULL == fgets(line_record, MAX_LENGTH_PERLINE, fp))
        {
            break;
        }
        if (record_index >= 1) /* ignore first row in input file */
        {
            info_index = record_index - 1;
            sscanf(line_record, "%s %s %d %d %d", infos[info_index].user, infos[info_index].process, &infos[info_index].arrival, &infos[info_index].duration, &infos[info_index].priority);
            infos[info_index].active = false;

            bool duplicated = false;
            if (info_numbers > 0)
            {
                int i;
                for (i = 0; i < num_of_user; i++)
                {
                    duplicated = !strcmp(summary[i].user, infos[info_index].user);
                    if (duplicated == true)
                    {
                        break;
                    }
                }
            }
            if (duplicated == false)
            {
                strcpy(summary[info_index].user, infos[info_index].user);
                num_of_user++;
            }
        }
        record_index++;
    }
    info_numbers = info_index + 1;
    fclose(fp);

    int i = 0;
    printf("%s\t", "Time");
    while (i < num_of_cpu)
    {
        i++;
        printf("CPU%d\t", i);
    }
    printf("\n");

    while (true)
    {
        total_duration = 0;
        for (count = 0; count < info_index; count++)
        {
            total_duration += infos[count].duration;
        }
        current_tick++;
        if (0 == total_duration)
        {
            int num = 0;
            printf("%d\t", current_tick);
            while (num < num_of_cpu)
            {
                num++;
                printf("\tIDLE");
            }
            printf("\n\nSummary\n");
            int i;
            for (i = 0; i < num_of_user && summary[i].time != 0; i++)
            {
                printf("%s  \t\t%d\n", summary[i].user, summary[i].time);
            }
            break;
        }

        update_active_status();
        tick_run(current_tick);
    }
    return 0;
}
