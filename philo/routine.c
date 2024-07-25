#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static void	philo_write(t_mutex *mutex, int tick, int id, int msg_id)
{
	const char	*str;

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
	pthread_mutex_lock(mutex);
	printf("%4d %d %s\n", tick, id, str);
	pthread_mutex_unlock(mutex);
}

static int	philo_think(t_philo *p)
{
	if (p->is_dead)
		return (1);
	philo_write(&(p->rt->mutex), p->rt->cur_tick, p->id, 1);
	p->l_fork = p->id;
	p->r_fork = (p->id + 1) % p->rt->data[PHILO_COUNT];
	pthread_mutex_lock(&p->rt->forks[p->l_fork]);
	philo_write(&p->rt->mutex, p->rt->cur_tick, p->id, 2);
	pthread_mutex_lock(&p->rt->forks[p->r_fork]);
	philo_write(&p->rt->mutex, p->rt->cur_tick, p->id, 2);
	return (p->is_dead);
}

static int	philo_eat(t_philo *p)
{
	if (p->is_dead)
		return (1);
	p->is_eating = 1;
	philo_write(&(p->rt->mutex), p->rt->cur_tick, p->id, 3);
	p->eat_count++;
	ft_usleep(p->rt->data[TIME_TO_EAT], p->rt);
	p->last_eat = p->rt->cur_tick;
	p->is_eating = 0;
	pthread_mutex_unlock(&(p->rt->forks[p->l_fork]));
	pthread_mutex_unlock(&(p->rt->forks[p->r_fork]));
	return (p->is_dead);
}

static int	philo_sleep(t_philo *p)
{
	if (p->is_dead)
		return (1);
	philo_write(&(p->rt->mutex), p->rt->cur_tick, p->id, 4);
	ft_usleep(p->rt->data[TIME_TO_SLEEP], p->rt);
	return (p->is_dead);
}

void	*philo_routine(void *ptr)
{
	t_philo *p;

	p = (t_philo *)ptr;
	if (!p)
	{
		printf("null philo\n");
		pthread_exit(NULL);
		return (NULL);
	}
	p->last_eat = p->rt->cur_tick;
	while (!p->is_dead)
	{
		if (!p->is_dead)
			p->is_dead = philo_think(p);
		if (!p->is_dead)
			p->is_dead = philo_eat(p);
		if (!p->is_dead)
			p->is_dead = philo_sleep(p);
	}
	pthread_exit(NULL);
	return (NULL);
}