#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static const char	*tconst[6] = {
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
	usleep(3000000);
	while (i < rt->data[PHILO_COUNT])
	{
		p = rt->philos[i];
		if (p->thread_status != THREAD_CLEAN_EXIT)
		{
			if (DEBUG)
				printf(ERR_HANG, i, tconst[p->thread_status]);
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
	int	i;
	t_uint	lowest;

	i = 0;
	lowest = -1;
	if (rt->data[MUST_EAT_X] == 0)
		return (0);
	while (i < rt->data[PHILO_COUNT])
	{
		if (philo->eat_count < lowest)
			lowest = philo->eat_count;
		i++;
	}
	if (lowest >= (t_uint)rt->data[MUST_EAT_X])
	{
		pthread_mutex_lock(&rt->print);
		rt->alive = 0;
		printf("eat count %d reached\n", rt->data[MUST_EAT_X]);
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
		if (philo->is_eating == 0)
		{
			philo->is_dead = 0;
			if (rt->alive)
			{
				pthread_mutex_lock(&rt->print);
				rt->alive = 0;
				printf("philo %d died on tick %d\n", philo->id, get_tick() - rt->start_tick);
				pthread_mutex_unlock(&rt->print);
			}
			return (1);
		}
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
	int			i;

	rt = (t_runtime *)ptr;
	philo = rt->philos[0];
	i = 0;
	while (rt->alive)
	{
		if (check_death(rt, philo) || check_eat(rt, philo))
			break;
		philo = rt->philos[++i % rt->data[PHILO_COUNT]];
		usleep(1000);
	}
	detach_philos(rt);
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