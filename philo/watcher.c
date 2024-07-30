#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>

static const char	*thrstate[6] = {
	"THREAD_NOT_STARTED",
	"THREAD_CREATE_FAIL",
	"THREAD_STARTED",
	"THREAD_JOIN_FAIL",
	"THREAD_JOINED",
	"THREAD_CLEAN_EXIT",
};

/// @brief flag all philos is_dead 1
/// @param rt runtime struct
static void	detach_philos(t_runtime *rt)
{
	int		i;
	t_philo	*p;

	rt->alive = 0;
	i = -1;
	while (++i < rt->data[PHILO_COUNT])
		rt->philos[i]->is_dead = 1;
	i = 0;
	usleep(1000000);
	while (i < rt->data[PHILO_COUNT])
	{
		p = rt->philos[i];
		if (p->thread_status != THREAD_CLEAN_EXIT)
		{
			if (DEBUG)
				printf(ERR_HANG, i, thrstate[p->thread_status]);
			if (p->thread_status == THREAD_JOIN_FAIL)
			{
				printf(ERR_DEATCH);
				pthread_detach(p->thread);
			}
		}
		i++;
	}
}

static int check_eat(t_runtime *rt, t_philo *philo)
{
	int		i;
	t_uint	l;

	i = 0;
	l = philo->eat_count;
	if (rt->data[MUST_EAT_X] == 0)
		return (0);
	while (i < rt->data[PHILO_COUNT])
	{
		if (rt->philos[i]->eat_count < l)
			l = rt->philos[i]->eat_count;
		i++;
	}
	if (l >= (t_uint)rt->data[MUST_EAT_X])
	{
		pthread_mutex_lock(&rt->print);
		rt->alive = 0;
		printf("eat count reached (%d)\n", rt->data[MUST_EAT_X]);
		pthread_mutex_unlock(&rt->print);
		return (1);
	}
	return (0);
}

static int	check_death(t_runtime *rt, t_philo *philo)
{
	if (philo->is_dead)
		return (0);
	if (rt->cur_tick > (philo->last_eat + rt->data[TIME_TO_DIE]))
	{
		philo->is_dead = 0;
		if (rt->alive)
		{
			pthread_mutex_lock(&rt->print);
			rt->alive = 0;
			printf("philo %d died on tick %d, expected death tick %d\n", philo->id, get_tick() - rt->start_tick, philo->last_eat + rt->data[TIME_TO_DIE]);
			pthread_mutex_unlock(&rt->print);
		}
		return (1);
	}
	return (0);
}

/// @brief watch all philos anSd quit program if one dies
/// @param ptr runtime struct
/// @return NULL
void	*watch_philos(void *ptr)
{
	t_runtime	*rt;
	t_philo		*philo;
	t_uint		i;

	rt = (t_runtime *)ptr;
	i = -1;
	pthread_mutex_lock(&rt->ready);
	pthread_mutex_unlock(&rt->ready);
	while (rt->alive)
	{
		philo = rt->philos[++i % rt->data[PHILO_COUNT]];
		if (check_death(rt, philo) || check_eat(rt, philo))
			break;
		usleep(1000000);
	}
	detach_philos(rt);
	return (0);
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