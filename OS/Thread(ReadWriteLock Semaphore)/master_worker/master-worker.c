#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce, curr_buf_size, consumed_item=0;
int total_items, max_buf_size, num_workers, num_masters;
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond;
int *buffer;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
    {
	  pthread_mutex_lock(&mutex);

	  if(item_to_produce >= total_items) {
		  pthread_mutex_unlock(&mutex);
		  pthread_cond_broadcast(&cond);
		  return 0;
	  }


      if(curr_buf_size >= max_buf_size)
      {
    	  pthread_cond_signal(&cond);
    	  pthread_cond_wait(&cond,&mutex);
    	  pthread_mutex_unlock(&mutex);
    	  continue;
      }



      buffer[curr_buf_size++] = item_to_produce;

      print_produced(item_to_produce, thread_id);
      item_to_produce++;

      pthread_mutex_unlock(&mutex);
    }
  return 0;
}

void *consume_loop(void *data)
{
	int thread_id = *((int *)data);
	int i;

	while(1)
	{
		pthread_mutex_lock(&mutex);

		if(consumed_item >= total_items)
		{
			pthread_mutex_unlock(&mutex);
			 pthread_cond_broadcast(&cond);
			return 0;
		}
		if(curr_buf_size<=0)
		{
			 pthread_cond_signal(&cond);
			 pthread_cond_wait(&cond,&mutex);
			 pthread_mutex_unlock(&mutex);
			 continue;
		}


		curr_buf_size--;
		print_consumed(buffer[curr_buf_size],thread_id);
		consumed_item++;

		pthread_mutex_unlock(&mutex);

	}
}
//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
  int *master_thread_id;
  pthread_t *master_thread;

  int *worker_thread_id;
  pthread_t *worker_thread;
  item_to_produce = 0;
  consumed_item=0;
  curr_buf_size = 0;
  
  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
    

   buffer = (int *)malloc (sizeof(int) * max_buf_size);

   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  worker_thread_id = (int *)malloc(sizeof(int)* num_workers);
  worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for(i=master_thread_id[num_masters-1]+1;i<num_workers;i++)
	  worker_thread_id[i]=i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads
  for(i=0;i <num_workers;i++)
  {
	pthread_create(&worker_thread[i],NULL,consume_loop,(void*)&worker_thread_id[i]);
  }
  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
    {
	  void *status;
      pthread_join(master_thread[i], status);
      printf("master %d joined\n", i);
    }

  for(i=0;i<num_workers;i++)
  {
	  void *status;
	  pthread_join(worker_thread[i],status);
	  printf("worker %d joined\n",i);
  }
  pthread_mutex_destroy(&mutex);
  pthread_exit(NULL);

  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(master_thread_id);
  free(master_thread);
  free(worker_thread_id);
  free(worker_thread);
  
  return 0;
}