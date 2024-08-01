#include "philosophers.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/// @brief flag all philos as dead and detach any hanging threads
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
			if (p->thread_status == THREAD_JOIN_FAIL)
			{
				printf(ERR_DEATCH);
				pthread_detach(p->thread);
			}
		}
		i++;
	}
}

/// @brief check if every philo has eaten at least x meals
/// @param rt runtime struct
/// @param philo philo
/// @return return 1 on true and 0 on false
static int	check_eat(t_runtime *rt, t_philo *philo)
{
	int		i;
	t_uint	l;

	i = -1;
	l = philo->eat_count;
	if (rt->data[MUST_EAT_X] == 0)
		return (0);
	while (++i < rt->data[PHILO_COUNT])
	{
		if (rt->philos[i]->eat_count < l)
			l = rt->philos[i]->eat_count;
	}
	if (l >= (t_uint)rt->data[MUST_EAT_X])
	{
		pthread_mutex_lock(&rt->print);
		rt->alive = 0;
		if (DEBUG)
			printf("eat count reached (%d)\n", rt->data[MUST_EAT_X]);
		pthread_mutex_unlock(&rt->print);
		return (1);
	}
	return (0);
}

/// @brief check if specific philo has exceeded death_tick
/// @param rt runtime struct
/// @param philo philo
/// @return return 1 on true and 0 on false
static int	check_death(t_runtime *rt, t_philo *philo)
{
	if (philo->is_dead)
		return (0);
	if (rt->cur_tick >= philo->death_tick)
	{
		philo->is_dead = 0;
		if (rt->alive)
		{
			pthread_mutex_lock(&rt->print);
			rt->alive = 0;
			printf("%4d %d %s\n", rt->cur_tick, philo->id, MSG_DIE);
			pthread_mutex_unlock(&rt->print);
		}
		return (1);
	}
	return (0);
}

/// @brief watch all philos and quit program if one dies
/// @param ptr runtime struct
/// @return NULL
void	*watch_philos(void *ptr)
{
	t_runtime	*rt;
	t_philo		*philo;
	t_uint		i;

	rt = (t_runtime *)ptr;
	i = -1;
	pthread_mutex_lock(&rt->ready_flag);
	pthread_mutex_unlock(&rt->ready_flag);
	while (rt->alive)
	{
		usleep(1000);
		philo = rt->philos[++i % rt->data[PHILO_COUNT]];
		if (check_death(rt, philo) || check_eat(rt, philo))
			break ;
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
		printf(ERR_WATCHER);
	}
}
