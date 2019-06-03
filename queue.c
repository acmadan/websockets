/*
MIT License

Copyright (c) 2019 Achal Madan   madanachal@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "websocket.h"
#include <stddef.h>
#include "queue.h"

static int length;
Queue *queue;

void queueInit()
{
   queue = (Queue *)malloc(sizeof(Queue));
   queue->front =NULL;
   queue->rear= NULL; 
   length =0;
}
int enqueue(ws_frame * pframe)
{
  if((queue->front == NULL) && ( queue->rear ==NULL))
  {

   Node *newNode = (Node *)malloc(sizeof(Node));
   newNode->frame = pframe;
   newNode->next = NULL;
  
   queue->front = newNode;
   queue->rear  = newNode;
   
   

  }else
  {

    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->frame = pframe;
    newNode->next = NULL;

    queue->rear->next  = newNode;
    queue->rear = queue->rear->next;

   

  }
  length++;
  return 1;


}


ws_frame *dequeue()
{

  if(queue->front == NULL)return NULL;


  
  Node *rNode = queue->front;   
 
  if(queue->front == queue->rear)
  {
    queue->front = NULL;
    queue->rear = NULL;

  }else
  {
     queue->front = queue->front->next;

  }
  length--;
  return rNode->frame;

}


int getLength()
{

  
  return length;
}
