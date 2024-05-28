#include "lib.h"
#include "types.h"

int uEntry(void)
{
	/*
	// For lab4.1
	// Test 'scanf'
	int dec = 0;
	int hex = 0;
	char str[6];
	char cha = 0;
	int ret = 0;
	while (1)
	{
		printf("Input:\" Test %%c Test %%6s %%d %%x\"\n");
		ret = scanf(" Test %c Test %6s %d %x", &cha, str, &dec, &hex);
		printf("Ret: %d; %c, %s, %d, %x.\n", ret, cha, str, dec, hex);
		if (ret == 4)
			break;
	}


	// For lab4.2
	// Test 'Semaphore'
	int i = 4;

	sem_t sem;
	printf("Father Process: Semaphore Initializing.\n");
	ret = sem_init(&sem, 2);
	if (ret == -1)
	{
		printf("Father Process: Semaphore Initializing Failed.\n");
		exit();
	}

	ret = fork();
	if (ret == 0)
	{
		while (i != 0)
		{
			i--;
			printf("Child Process: Semaphore Waiting.\n");
			sem_wait(&sem);
			printf("Child Process: In Critical Area.\n");
		}
		printf("Child Process: Semaphore Destroying.\n");
		sem_destroy(&sem);
		exit();
	}
	else if (ret != -1)
	{
		while (i != 0)
		{
			i--;
			printf("Father Process: Sleeping.\n");
			sleep(128);
			printf("Father Process: Semaphore Posting.\n");
			sem_post(&sem);
		}
		printf("Father Process: Semaphore Destroying.\n");
		sem_destroy(&sem);
		exit();
	}
*/
	// For lab4.3
	// TODO: You need to design and test the problem.
	// Note that you can create your own functions.
	// Requirements are demonstrated in the guide.
	sem_t mutex;
	sem_t full;
	sem_t empty;
	int ret = 1;
	sem_init(&mutex, 1);
	sem_init(&full, 0);
	sem_init(&empty, 3);
	int idx;
	for (int i = 0; i < 4; ++i)
	{
		if (ret > 0)
		{
			ret = fork();
			idx = i;
		}
	}
	while (1)
	{
		if (ret == 0) // producer
		{
			sem_wait(&empty);
			sem_wait(&mutex);
			printf("Producer : produce\n");
			printf("pid : %d\n", idx);
			sleep(128);
			sem_post(&mutex);
			sem_post(&full);
		}
		else if (ret <= 5 && ret > 0) // comsumer
		{
			sem_wait(&full);
			sem_wait(&mutex);
			printf("Consumer : consume\n");
			sleep(128);
			sem_post(&mutex);
			sem_post(&empty);
		}
	}

	sem_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);
	return 0;
}
