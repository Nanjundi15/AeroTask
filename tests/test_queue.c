#include "queue.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void){
    task_queue_t q;
    assert(queue_init(&q)==0);
    task_t a={0},b={0},out={0};
    strcpy(a.name,"low"); a.priority=1;
    strcpy(b.name,"high"); b.priority=10;
    assert(queue_push(&q,&a)==0);
    assert(queue_push(&q,&b)==0);
    assert(queue_pop(&q,&out)==1);
    assert(out.priority==10);
    assert(queue_pop(&q,&out)==1);
    assert(out.priority==1);
    queue_stop(&q);
    queue_destroy(&q);
    puts("test_queue: PASS");
    return 0;
}
