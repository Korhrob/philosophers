#include "philosophers.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/// @brief flag all philos as dead and detach any hanging threads
/// @param rt runtime struct
static void	stop_program(t_runtime *rt)
{
	int		i;

	pthread_mutex_lock(&rt->watch_lock);
	rt->run = 0;
	pthread_mutex_unlock(&rt->watch_lock);
	i = -1;
	while (++i < rt->data[PHILO_COUNT])
	{
		pthread_mutex_lock(&rt->philos[i]->act);
		rt->philos[i]->alive = 0;
		pthread_mutex_unlock(&rt->philos[i]->act);
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
	l = get_philo_eat(philo);
	if (rt->data[MUST_EAT_X] == 0)
		return (0);
	while (++i < rt->data[PHILO_COUNT])
	{
		if (get_philo_eat(rt->philos[i]) < l)
			l = get_philo_eat(rt->philos[i]);
	}
	if (l >= (t_uint)rt->data[MUST_EAT_X])
	{
		pthread_mutex_lock(&rt->print_lock);
		pthread_mutex_lock(&rt->watch_lock);
		rt->run = 0;
		if (DEBUG)
			printf("eat count reached (%d)\n", rt->data[MUST_EAT_X]);
		pthread_mutex_unlock(&rt->watch_lock);
		pthread_mutex_unlock(&rt->print_lock);
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
	if (!get_philo_status(philo))
		return (0);
	if (get_cur_tick(rt) >= get_philo_death(philo))
	{
		pthread_mutex_lock(&philo->act);
		philo->alive = 0; // use setter
		pthread_mutex_unlock(&philo->act);
		if (get_rt_status(rt))
		{
			pthread_mutex_lock(&rt->print_lock);
			pthread_mutex_lock(&rt->watch_lock);
			rt->run = 0;
			printf("%4d %d %s\n", get_cur_tick(rt), philo->id, MSG_DIE);
			pthread_mutex_unlock(&rt->watch_lock);
			pthread_mutex_unlock(&rt->print_lock);
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
	pthread_mutex_lock(&rt->ready_lock);
	pthread_mutex_unlock(&rt->ready_lock);
	while (get_rt_status(rt))
	{
		usleep(1000);
		philo = rt->philos[++i % rt->data[PHILO_COUNT]];
		if (check_death(rt, philo) || check_eat(rt, philo))
			break ;
	}
	stop_program(rt);
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
