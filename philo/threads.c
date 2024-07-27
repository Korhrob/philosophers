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
	pthread_mutex_init(&rt->ready, NULL);
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
	pthread_mutex_destroy(&rt->ready);
}

/// @brief create philo threads
/// @param rt runtime struct
static void	create_threads(t_runtime *rt)
{
	int	i;

	i = 0;
	while (i < rt->data[PHILO_COUNT])
	{
		if (pthread_create(&rt->philos[i]->thread, NULL, &philo_routine_new, rt->philos[i]))
		{
			printf(ERR_THREAD, i);
			rt->philos[i]->thread_status = THREAD_CREATE_FAIL;
			rt->eflag |= FLAG_PHILO;
			return ;
		}
		rt->philos[i]->thread_status = THREAD_STARTED;
		i++;
	}
}

/// @brief create and join all threads
/// @param rt runtime struct
void	philosophers(t_runtime *rt)
{
	int i;

	init_mutex(rt);
	pthread_mutex_lock(&rt->ready);
	create_timer(rt);
	create_watcher(rt);
	create_threads(rt);
	if (rt->eflag)
		rt->alive = FALSE;
	i = 0;
	while (i < rt->data[PHILO_COUNT])
	{
		if (rt->philos[i]->thread_status != THREAD_STARTED
			&& pthread_join(rt->philos[i]->thread, NULL))
		{
			printf(ERR_THREAD, i);
			rt->alive = FALSE;
			rt->philos[i]->thread_status = THREAD_JOIN_FAIL;
			rt->eflag |= (rt->eflag & FLAG_JOIN);
			break ;
		}
		i++;
	}
	pthread_mutex_unlock(&rt->ready);
	pthread_join(rt->timer, NULL);
	pthread_join(rt->watcher, NULL);
	destroy_mutex(rt);
}
