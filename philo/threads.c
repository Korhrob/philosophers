#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/// @brief initialize mutexes
/// @param rt runtime struct
static void init_mutex(t_runtime *rt)
{
	int	i;

	i = 0;
	pthread_mutex_init(&rt->print, NULL);
	pthread_mutex_init(&rt->tick, NULL);
	pthread_mutex_init(&rt->counter, NULL);
	while (i < rt->data[PHILO_COUNT])
	{
		pthread_mutex_init(&rt->forks[i], NULL);
		i++;
	}
}

/// @brief destroy mutexes
/// @param rt runtime struct
static void destroy_mutex(t_runtime *rt)
{
	int	i;

	i = 0;
	while (i < rt->data[PHILO_COUNT])
	{
		pthread_mutex_destroy(&rt->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&rt->print);
	pthread_mutex_destroy(&rt->tick);
	pthread_mutex_destroy(&rt->counter);
}

/// @brief create philo threads
/// @param rt runtime struct
static void	create_threads(t_runtime *rt)
{
	int	i;

	i = 0;
	while (i < rt->data[PHILO_COUNT])
	{
		if (pthread_create(&rt->philos[i]->thread, NULL, &philo_routine, rt->philos[i]))
			rt->eflag |= FLAG_PHILO;
		usleep(1);
		i++;
	}
}

/// @brief create and join all threads
/// @param rt runtime struct
void	philosophers(t_runtime *rt)
{
	int i;

	init_mutex(rt);
	create_timer(rt);
	create_watcher(rt);
	create_threads(rt);
	if (rt->eflag)
		rt->alive = 0;
	pthread_join(rt->timer, NULL);
	pthread_join(rt->watcher, NULL);
	i = 0;
	while (i < rt->data[PHILO_COUNT])
	{
		if (pthread_join(rt->philos[i]->thread, NULL))
		{
			printf("thread join failed at %d\n", i);
			rt->alive = 0;
		}
		i++;
	}
	destroy_mutex(rt);
}
