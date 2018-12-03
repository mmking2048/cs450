#include "types.h"
#include "user.h"

#define SIZE 10

int IN, OUT;
int arr[SIZE];
int mutex, items, spaces;
void *iter;

void consumer(void *);
void producer(void *);

int main()
{
    // 10 iterations
    iter = (void *)10;

    // mutex
    mutex = mtx_create(0);
    // does buffer have item?
    items = mtx_create(1);
    // does buffer have space?
    spaces = mtx_create(0);

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
        // lock if previously unlocked (no space left)
        mtx_lock(spaces);

        mtx_lock(mutex);
            arr[IN] = i;
            IN = (IN + 1) % SIZE;
            printf(1, "Producer: %d, location: %d\n", i, IN);
        mtx_unlock(mutex);

        // unlock if previously locked (there are items)
        mtx_unlock(items);
    }

    exit();
}

void consumer(void *iterations)
{
    int i, j;

    for (i = 0; i < (int)iterations; i++)
    {
        // lock if previously unlocked (no items)
        mtx_lock(items);

        mtx_lock(mutex);
            j = arr[OUT];
            OUT = (OUT + 1) % SIZE;
            printf(1, "Consumer: %d, location: %d\n", j, OUT);
        mtx_unlock(mutex);

        // unlock if previously locked (there is space)
        mtx_unlock(spaces);
    }

    exit();
}
