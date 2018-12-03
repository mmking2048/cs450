#include "spinlock.h"
#include "types.h"
#include "user.h"

#define SIZE 10

int IN, OUT, count, mutex, arr[SIZE];
void *iter;
struct semaphore *items, *spaces;

void consumer(void *);
void producer(void *);

int main()
{
    // 10 iterations
    iter = (void *)30;
    count = 0;

    // mutex
    mutex = mtx_create(0);
    // does buffer have item?
    items = malloc(sizeof(struct semaphore));
    sem_create(items, 0);
    // does buffer have space?
    spaces = malloc(sizeof(struct semaphore));
    sem_create(spaces, SIZE);

    // create producer thread
    thread_create(producer, malloc(4096), iter);
    // create consumer thread
    thread_create(consumer, malloc(4096), iter);

    void *stack1, *stack2;
    thread_join(&stack1);
    thread_join(&stack2);

    free(stack1);
    free(stack2);

    exit();
}

void producer(void* iterations)
{
    int i;

    for (i = 0; i < (int)iterations; i++)
    {
        // wait for event
        sleep(IN);

        // lock if previously unlocked (no space left)
        sem_acquire(spaces);

        mtx_lock(mutex);
            arr[IN] = i;
            printf(1, "Producer: %d, location: %d\n", i, IN);
            IN = (IN + 1) % SIZE;
            count++;
        mtx_unlock(mutex);

        // unlock if previously locked (there are items)
        sem_release(items);
    }

    exit();
}

void consumer(void *iterations)
{
    int i, j;

    for (i = 0; i < (int)iterations; i++)
    {
        // lock if previously unlocked (no items)
        sem_acquire(items);

        mtx_lock(mutex);
            j = arr[OUT];
            printf(1, "Consumer: %d, location: %d\n", j, OUT);
            OUT = (OUT + 1) % SIZE;
            count--;
        mtx_unlock(mutex);

        // unlock if previously locked (there is space)
        sem_release(spaces);

        // process event
        sleep(OUT);
    }

    exit();
}
