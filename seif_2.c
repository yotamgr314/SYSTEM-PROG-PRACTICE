// Global semaphore for the printer
sem_t printer_sem;

// PRINT function simulating a print operation
void PRINT(void)
{
  // For example, display a message and pause briefly to simulate printing time
  printf("Printing...\n");
}

// Thread function: each thread takes the semaphore, calls PRINT, and then releases the semaphore
void* thread_func(void* arg)
{
  sem_wait(&printer_sem);  // Acquire a token (blocks if no tokens are available)
  PRINT();                 // Execute printing
  sem_post(&printer_sem);  // Release the token back to the semaphore
  return NULL;
}

int main(void)
{
  const int NUM_THREADS = 5;
  pthread_t threads[NUM_THREADS];

  // Initialize the semaphore with the printer capacity
  sem_init(&printer_sem, 3);

  // Create 5 threads
  for (int i = 0; i < NUM_THREADS; i++) {
    if (pthread_create(&threads[i], NULL, thread_func, NULL) != 0) {
      perror("Error creating thread");
      exit(EXIT_FAILURE);
    }
  }

  // Wait for all threads to complete
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}





