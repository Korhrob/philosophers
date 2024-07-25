#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/// @brief flag all philos is_dead 1
/// @param rt runtime struct
static void	detach_philos(t_runtime *rt)
{
	int	i;

	rt->alive = 0;
	i = -1;
	while (++i < rt->data[PHILO_COUNT])
		rt->philos[i]->is_dead = 1;
	i = 0;
	pthread_mutex_unlock(&rt->print);
	usleep(1000000);
	while (i < rt->data[PHILO_COUNT])
	{
		if (!rt->philos[i]->removed)
		{
			printf("detaching philo %d... \n", i);
			pthread_detach(rt->philos[i]->thread);
		}
		i++;
	}
}

int check_eat(t_runtime *rt, t_philo *philo)
{
	int	i;
	t_uint	lowest;

	i = 0;
	lowest = 99;
	while (i < rt->data[PHILO_COUNT])
	{
		if (philo->eat_count < lowest)
			lowest = philo->eat_count;
		i++;
	}
	if (lowest >= (t_uint)rt->data[MUST_EAT_X])
	{
		printf("eat count reached\n");
		return (1);
	}
	return (0);
}

int	is_dead(t_runtime *rt, t_philo *philo)
{
	if (philo->is_dead)
		return (1);
	if (rt->cur_tick > (philo->last_eat + rt->data[TIME_TO_DIE]))
	{
		if (philo->is_eating == 0)
		{
			philo->is_dead = 0;
			if (rt->alive)
				printf("philo %d died on tick %d\n", philo->id, get_tick() - rt->start_tick);
			return (0);
		}
	}
	return (1);
}

/// @brief watch all philos anSd quit program if one dies
/// @param ptr runtime struct
/// @return NULL
void	*watch_philos(void *ptr)
{
	t_runtime	*rt;
	t_philo		*philo;
	int			i;

	rt = (t_runtime *)ptr;
	philo = rt->philos[0];
	i = 0;
	while (rt->body_count < rt->data[PHILO_COUNT])
	{
		if (!is_dead(rt, philo) || check_eat(rt, philo))
		{
			pthread_mutex_lock(&rt->print);
			rt->alive = 0;
			break;
		}
		philo = rt->philos[++i % rt->data[PHILO_COUNT]];
		usleep(1000);
	}
	detach_philos(rt);
	printf("exit watcher\n");
	
	pthread_exit(NULL);
}

/// @brief create a watcher thread and set error flag if needed
/// @param rt runtime structSS
void	create_watcher(t_runtime *rt)
{
	if (pthread_create(&rt->watcher, NULL, &watch_philos, rt))
	{
		rt->eflag |= FLAG_WATCHER;
		printf("watcher thread failed\n");
	}
}