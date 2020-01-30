#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_LEN 100
#define THREAD_NUM 3

// initializing global variable
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static int avergae_value;
static int min;
static int max;
static int size = 0;

int fd1[2];
int fd2[2];
int fd3[2];

pthread_t averageThread;
pthread_t minThread;
pthread_t maxThread;

void average()
{
    int num;
    int i;
    int sum = 0;

    printf("\n%d\n", size);

    for (i = 0; i < size; i++)
    {
        // output, input, length
        read(fd1[0], &num, sizeof(int));
        sum = sum + num;
        printf("+%d", num);
    }
    printf("%d", sum);
    pthread_mutex_lock(&mut);
    avergae_value = sum / size;

    pthread_mutex_unlock(&mut);
}

int main()
{

    pipe(fd1);
    pipe(fd2);
    pipe(fd3);

    int i;

    // creating the two children
    pthread_t child[THREAD_NUM];
    int num = scanf("%d", &num);
    while (num != 0)
    {
        write(fd1[1], &num, sizeof(int));
        write(fd2[1], &num, sizeof(int));
        write(fd3[1], &num, sizeof(int));
        size++;
        num = scanf("%d", &num);
    }

    pthread_create(&avergae_value, NULL, average, NULL);
    pthread_join(avergae_value, NULL);
    //print result
    printf("average value is: %d\n", avergae_value);

    pthread_exit(NULL);
    return 0;
}
