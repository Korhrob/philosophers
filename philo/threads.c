#include "philosophers.h"
#include <stdio.h>
#include <pthread.h>

/// @brief initialize mutexes
/// @param rt runtime struct
static void	init_mutex(t_runtime *rt)
{
	int	i;

	i = -1;
	pthread_mutex_init(&rt->print_lock, NULL);
	pthread_mutex_init(&rt->timer_lock, NULL);
	pthread_mutex_init(&rt->watch_lock, NULL);
	while (++i < rt->data[PHILO_COUNT])
	{
		pthread_mutex_init(&rt->forks[i], NULL);
		pthread_mutex_init(&rt->philos[i]->act, NULL);
	}
}

/// @brief destroy mutexes
/// @param rt runtime struct
static void	destroy_mutex(t_runtime *rt)
{
	int	i;

	i = -1;
	while (++i < rt->data[PHILO_COUNT])
		pthread_mutex_destroy(&rt->forks[i]);
	pthread_mutex_destroy(&rt->print_lock);
	pthread_mutex_destroy(&rt->timer_lock);
	pthread_mutex_destroy(&rt->watch_lock);
}

/// @brief create philo threads
/// @param rt runtime struct
static void	create_threads(t_runtime *rt)
{
	int			i;

	i = -1;
	pthread_mutex_lock(&rt->ready_lock);
	create_timer(rt);
	create_watcher(rt);
	while (++i < rt->data[PHILO_COUNT])
	{
		if (pthread_create(&rt->philos[i]->thread, NULL, &philo_routine_new, rt->philos[i]))
		{
			printf(ERR_THREAD, i);
			rt->run = FALSE;
			rt->philos[i]->thread_status = THREAD_CREATE_FAIL;
			rt->eflag |= FLAG_PHILO;
			break ;
		}
		rt->philos[i]->thread_status = THREAD_STARTED;
	}
	pthread_mutex_unlock(&rt->ready_lock);
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
			rt->philos[i]->thread_status = THREAD_JOIN_FAIL;
			rt->eflag |= (rt->eflag & FLAG_JOIN);
			//detach this thread
			break;
		}
	}
	if (!pthread_join(rt->watcher, NULL))
		rt->eflag |= FLAG_JOIN_W;
	if (!pthread_join(rt->timer, NULL))
		rt->eflag |= FLAG_JOIN_T;
}

/// @brief create and join all threads RENAME
/// @param rt runtime struct
void	run(t_runtime *rt)
{
	init_mutex(rt);
	create_threads(rt);
	if (rt->eflag)
		rt->run = FALSE;
	join_threads(rt);
	destroy_mutex(rt);
}

	// while (++i < rt->data[PHILO_COUNT])
	// {
	// 	p = rt->philos[i];
	// 	if (p->thread_status != THREAD_CLEAN_EXIT)
	// 	{
	// 		if (p->thread_status == THREAD_JOIN_FAIL)
	// 		{
	// 			printf(ERR_DEATCH);
	// 			pthread_detach(p->thread);
	// 		}
	// 	}
	// }