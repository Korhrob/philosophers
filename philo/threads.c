#include "philosophers.h"
#include <stdio.h>
#include <pthread.h>

/// @brief initialize mutexes
/// @param rt runtime struct
static void	init_mutex(t_runtime *rt)
{
	int	i;

	i = -1;
	pthread_mutex_init(&rt->print, NULL);
	pthread_mutex_init(&rt->tick, NULL);
	while (++i < rt->data[PHILO_COUNT])
		pthread_mutex_init(&rt->forks[i], NULL);
}

/// @brief destroy mutexes
/// @param rt runtime struct
static void	destroy_mutex(t_runtime *rt)
{
	int	i;

	i = -1;
	while (++i < rt->data[PHILO_COUNT])
		pthread_mutex_destroy(&rt->forks[i]);
	pthread_mutex_destroy(&rt->print);
	pthread_mutex_destroy(&rt->tick);
}

/// @brief create philo threads
/// @param rt runtime struct
static void	create_threads(t_runtime *rt)
{
	int			i;

	i = -1;
	pthread_mutex_lock(&rt->ready_flag);
	create_timer(rt);
	create_watcher(rt);
	while (++i < rt->data[PHILO_COUNT])
	{
		if (pthread_create(&rt->philos[i]->thread, NULL, &philo_routine_new, rt->philos[i]))
		{
			printf(ERR_THREAD, i);
			rt->philos[i]->thread_status = THREAD_CREATE_FAIL;
			rt->eflag |= FLAG_PHILO;
			break ;
		}
		printf("create thread %p\n", &rt->philos[i]->thread);
		rt->philos[i]->thread_status = THREAD_STARTED;
	}
	pthread_mutex_unlock(&rt->ready_flag);
}

/// @brief join all threads and set flags
/// @param rt runtime struct
static void	join_threads(t_runtime *rt)
{
	int	i;

	i = -1;
	while (++i < rt->data[PHILO_COUNT])
	{
		if (pthread_join(rt->philos[i]->thread, NULL))
		{
			printf(ERR_THREAD, i);
			rt->alive = FALSE;
			rt->philos[i]->thread_status = THREAD_JOIN_FAIL;
			rt->eflag |= (rt->eflag & FLAG_JOIN);
			//detach
			break;
		}
		printf("join %d\n", i);
		printf("join thread %p\n", &rt->philos[i]->thread);
	}
	if (!pthread_join(rt->watcher, NULL))
		rt->eflag |= FLAG_JOIN_W;
	if (!pthread_join(rt->timer, NULL))
		rt->eflag |= FLAG_JOIN_T;
}

/// @brief create and join all threads
/// @param rt runtime struct
void	philosophers(t_runtime *rt)
{
	init_mutex(rt);
	create_threads(rt);
	if (rt->eflag)
		rt->alive = FALSE;
	join_threads(rt);
	destroy_mutex(rt);
}
