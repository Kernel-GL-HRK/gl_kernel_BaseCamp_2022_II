#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main (void)
{
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    char *ptr3 = NULL;
    char *ptr4 = NULL;
    int *ptr = NULL;
    ptr1 = malloc(1);
    if (ptr1 != NULL) {
        printf("Heap1 starts at %p\n", ptr1);
    }
    ptr = ptr1;
    ptr--;
    ptr--;
    printf("Value before ptr: %p %d\n", ptr, *ptr);
    ptr2 = malloc(3);
    if (ptr2 != NULL) {
        printf("Heap2 starts at %p\n", ptr2);
    }
    ptr = ptr2;
    ptr--;
    ptr--;
    printf("Value before ptr: %p %d\n", ptr, *ptr);
    ptr3 = malloc(133*1024);
    if (ptr3 != NULL) {
        printf("Heap3 starts at %p\n", ptr3);
    }
    ptr = ptr3;
    ptr--;
    ptr--;
    printf("Value before ptr: %p %d\n", ptr, *ptr);
    ptr4 = malloc(134000);
    if (ptr4 != NULL) {
        printf("Heap3 starts at %p\n", ptr4);
    }
    ptr = ptr4;
    ptr--;
    ptr--;
    printf("Value before ptr: %p %d\n", ptr, *ptr);

    free(ptr1);
    free(ptr2);
    free(ptr3);
    free(ptr4);

    return 0;
}