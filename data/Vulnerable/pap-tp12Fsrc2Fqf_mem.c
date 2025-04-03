#include <stdio.h>
#include "FreeRTOS.h"
#include "qmpool.h"

void QMPool_init(QMPool * const me, void * const poolSto, uint_fast32_t poolSize, uint_fast16_t blockSize)
{
    QFreeBlock       *fb;
    uint_fast16_t nblocks;
    me->free_head = poolSto;
    me->blockSize = (QMPoolSize)sizeof(QFreeBlock); /* round up the blockSize to fit an integer free blocks, no division */
    nblocks       = (uint_fast16_t)1;               /* start with just one */
    while (me->blockSize < (QMPoolSize)blockSize) { /* #free blocks that fit in one memory block */
        me->blockSize += (QMPoolSize)sizeof(QFreeBlock);
        ++nblocks;
    }
    blockSize = (uint_fast16_t)me->blockSize;       /* round-up to nearest block */
    poolSize -= (uint_fast32_t)blockSize;           /* don't count the last block */
    me->nTot  = (QMPoolCtr)1;                       /* the last block already in the pool */
    fb        = (QFreeBlock *)me->free_head;        /* start at the head of the free list */
    while (poolSize >= (uint_fast32_t)blockSize) {  /* chain all blocks together in a free-list... */
        fb->next  = &fb[nblocks];                   /* point next link to next block */
        fb        = fb->next;                       /* advance to the next block */
        poolSize -= (uint_fast32_t)blockSize;       /* reduce available pool size */
        ++me->nTot;                                 /* increment the number of blocks so far */
    }
    fb->next  = (QFreeBlock *)0;                    /* the last link points to NULL */
    me->nFree = me->nTot;                           /* all blocks are free */
    me->nMin  = me->nTot;                           /* the minimum number of free blocks */
    me->start = poolSto;                            /* the original start this pool buffer */
    me->end   = fb;                                 /* the last block in this pool */
}

void QMPool_put(QMPool * const me, void *b) 
{
//   portENTER_CRITICAL();                                  // Enter on critical section
      ((QFreeBlock *)b)->next = (QFreeBlock *)me->free_head; /* link into list */
      me->free_head = b;                                     /* set as new head of the free list */
      ++me->nFree;                                           /* one more free block in this pool */
//   portEXIT_CRITICAL();                                   // Exit from critical section
}

void *QMPool_get(QMPool * const me, uint_fast16_t const margin) 
{
   QFreeBlock *fb;
   //portENTER_CRITICAL();                              // Enter on critical section
   if (me->nFree > (QMPoolCtr)margin) {               /* have more free blocks than the requested margin? */
      fb            = (QFreeBlock *)me->free_head;    /* get a free block */
      me->free_head = fb->next;                       /* set the head to the next free block */
      --me->nFree;                                    /* one less free block */
      if (me->nMin > me->nFree) me->nMin = me->nFree; /* remember the new minimum */
   }
   else
      fb = (QFreeBlock *)0;
   //portEXIT_CRITICAL();                               // Exit from critical section
   return fb;                                         /* return the block or NULL pointer to the caller */
}

uint_fast16_t QMPool_getMin(QMPool * const me)
{
   uint_fast16_t min;
   portENTER_CRITICAL ( ); // Enter on critical section
      min = me->nMin     ;
   portEXIT_CRITICAL  ( ); // Exit from critical section
   return min;
}

char* Print_Qmpool_Struct(QMPool* Q,char* S)
{
   sprintf( S, "mem pool\r\n----------\r\n"
               "free_head   =%d\r\n"
               "start       =%d\r\n"
               "end         =%d\r\n"
               "blockSize   =%d\r\n"
               "nTot        =%d\r\n"
               "nFree       =%d\r\n"
               "nMin        =%d\r\n",
               Q->free_head,
               Q->start,
               Q->end,
               Q->blockSize,
               Q->nTot,
               Q->nFree,
               Q->nMin);
   return S;
}


//						↓↓↓VULNERABLE LINES↓↓↓

// 69,3;69,10

