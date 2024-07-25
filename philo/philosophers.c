#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static void destroy_mutex(t_runtime *rt)
{
	int	i;

	i = 0;
	while (i < rt->data[PHILO_COUNT])
	{
		pthread_mutex_destroy(&rt->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&rt->mutex);
	pthread_mutex_destroy(&rt->tick);
	pthread_exit(NULL);
}

void	stop_threads(t_runtime *rt)
{
	int	i;

	printf("stop_threads\n");
	rt->alive = 0;
	i = 0;
	while (i < rt->data[PHILO_COUNT])
	{
		rt->philos[i]->is_dead = 1;
		if (rt->philos[i] && rt->philos[i]->thread)
			pthread_detach(rt->philos[i]->thread);
		i++;
	}
	pthread_detach(rt->timer);
	pthread_detach(rt->watcher);
	destroy_mutex(rt);
}

void	*watch_philos(void *ptr)
{
	t_runtime	*rt;
	t_philo		*philo;
	int			i;

	rt = (t_runtime *)ptr;
	philo = rt->philos[0];
	while (rt->alive)
	{
		// not correct, should only break if all philos have eaten enough
		// if (rt->data[MUST_EAT_X] && ((i - 1) % rt->data[PHILO_COUNT]) &&
		// 	(philo->eat_count >= rt->data[MUST_EAT_X]))
		// 		break; 
		if (rt->cur_tick > (philo->last_eat + rt->data[TIME_TO_DIE]))
			if (philo->is_eating == 0)
			{
				philo->is_dead = 1;
				printf("philo %d died on tick %d\n", philo->id, get_tick() - rt->start_tick);
		 		break;
			}
		philo = rt->philos[++i % rt->data[PHILO_COUNT]];
		ft_usleep(1, rt);
	}
	stop_threads(rt);
}

void	philosophers(t_runtime *rt)
{
	int	i;

	pthread_mutex_init(&rt->mutex, NULL);
	pthread_mutex_init(&rt->tick, NULL);
	pthread_create(&rt->timer, NULL, &timer_tick, rt);
	i = -1;
	while (++i < rt->data[PHILO_COUNT])
		pthread_mutex_init(&rt->forks[i], NULL);
	i = -1;
	while (++i < rt->data[PHILO_COUNT])
	{
		pthread_create(&rt->philos[i]->thread, NULL, &philo_routine, rt->philos[i]);
		usleep(1);
	}
	pthread_create(&rt->watcher, NULL, &watch_philos, rt);
	pthread_join(rt->timer, NULL);
	pthread_join(rt->watcher, NULL);
	i = -1;
	while (++i < rt->data[PHILO_COUNT])
		pthread_join(rt->philos[i]->thread, NULL);
	destroy_mutex(rt);
}
