#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/// @brief printing with mutex
/// @param mutex mutex
/// @param tick current tick
/// @param id philo id
/// @param msg_id message id
static void	philo_write(t_philo *p, int msg_id)
{
	const char	*str;

	if (p->is_dead)
		return ;
	if (msg_id == 1)
		str = MSG_THINK;
	else if (msg_id == 2)
		str = MSG_FORK;
	else if (msg_id == 3)
		str = MSG_EAT;
	else if (msg_id == 4)
		str = MSG_SLEEP;
	else if (msg_id == 5)
		str = MSG_DIE;
	else
		return ;
	if (!p->is_dead)
	pthread_mutex_lock(&p->rt->print);
	if (!p->is_dead)
		printf("%4d %d %s\n", p->rt->cur_tick, p->id, str);
	pthread_mutex_unlock(&p->rt->print);
}

/// @brief philosopher think logic
/// @param p philo
/// @return is_dead
static int	philo_think(t_philo *p)
{
	if (p->is_dead)
		return (1);
	philo_write(p, 1);
	p->l_fork = p->id;
	p->r_fork = (p->id + 1) % p->rt->data[PHILO_COUNT];
	if (p->l_fork == p->r_fork)
	{
		p->is_dead = 1;
		ft_usleep(p->rt->data[TIME_TO_DIE], p->rt);
		return (p->is_dead);
	}
	if (!p->is_dead)
		pthread_mutex_lock(&p->rt->forks[p->l_fork]);
	philo_write(p, 2);
	if (!p->is_dead)
		pthread_mutex_lock(&p->rt->forks[p->r_fork]);
	philo_write(p,  2);
	return (p->is_dead);
}

/// @brief philosopher eat logic
/// @param p philo
/// @return is_dead
static int	philo_eat(t_philo *p)
{
	if (p->is_dead) {
		pthread_mutex_unlock(&(p->rt->forks[p->l_fork]));
		pthread_mutex_unlock(&(p->rt->forks[p->r_fork]));
		return (1);
	}
	p->is_eating = 1;
	p->eat_count++;
	philo_write(p, 3);
	ft_usleep(p->rt->data[TIME_TO_EAT], p->rt);
	p->last_eat = p->rt->cur_tick;
	p->is_eating = 0;
	pthread_mutex_unlock(&(p->rt->forks[p->l_fork]));
	pthread_mutex_unlock(&(p->rt->forks[p->r_fork]));
	return (p->is_dead);
}

/// @brief philosopher sleep logic
/// @param p philo
/// @return is_dead
static int	philo_sleep(t_philo *p)
{
	if (p->is_dead)
		return (1);
	philo_write(p, 4);
	ft_usleep(p->rt->data[TIME_TO_SLEEP], p->rt);
	return (p->is_dead);
}

/// @brief main logic for single philosopher
/// @param ptr philo
/// @return NULL
void	*philo_routine(void *ptr)
{
	t_philo *p;

	p = (t_philo *)ptr;
	if (!p)
	{
		printf("null philo\n");
		pthread_exit(NULL);
	}
	p->last_eat = p->rt->cur_tick;
	while (!p->is_dead && p->rt->alive)
	{
		philo_think(p);
		philo_eat(p);
		philo_sleep(p);
	}
	pthread_mutex_lock(&p->rt->counter);
	p->removed = 1;
	pthread_mutex_unlock(&p->rt->counter);
	pthread_exit(NULL);
}
