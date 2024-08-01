#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/// @brief print what while is currently doing
/// @param p pointer to philo
/// @param msg message
static void	philo_print(t_philo *p, const char *msg)
{
	if (p->is_dead)
		return ;
	if (!p->rt->alive)
		return ;
	p->debug_state = STATE_WAIT_PRINT;
	pthread_mutex_lock(&p->rt->print);
	if (!p->is_dead && p->rt->alive)
		printf("%4d %d %s\n", p->rt->cur_tick, p->id, msg);
	pthread_mutex_unlock(&p->rt->print);
}

/// @brief philos are thinking while waiting for forks
/// used to swap forks like this, but it caused issues
/// if (p->id % 2 == 1)
/// {
/// 	p->l_fork = p->l_fork ^ p->r_fork;
/// 	p->r_fork = p->l_fork ^ p->r_fork;
/// 	p->l_fork = p->l_fork ^ p->r_fork;
/// }
/// @param p pointer to philo
static void	philo_think(t_philo *p)
{
	if (p->is_dead)
		return ;
	if (!p->rt->alive)
		return ;
	philo_print(p, MSG_THINK);
	p->l_fork = p->id;
	p->r_fork = (p->id + 1) % p->rt->data[PHILO_COUNT];
	p->debug_state = STATE_WAIT_L_FORK;
	pthread_mutex_lock(&p->rt->forks[p->l_fork]);
	philo_print(p, MSG_FORK);
	if (p->l_fork == p->r_fork)
	{
		usleep(p->rt->data_ms[TIME_TO_DIE] + 1000);
		return ;
	}
	p->debug_state = STATE_WAIT_R_FORK;
	pthread_mutex_lock(&p->rt->forks[p->r_fork]);
	philo_print(p, MSG_FORK);
}

/// @brief eat and releast forks
/// @param p pointer to philo
static void	philo_eat(t_philo *p)
{
	if (p->is_dead && (p->l_fork != -1 || p->r_fork != -1))
	{
		if (p->l_fork != -1)
			pthread_mutex_unlock(&p->rt->forks[p->l_fork]);
		if (p->r_fork != -1)
			pthread_mutex_unlock(&p->rt->forks[p->r_fork]);
		return ;
	}
	if (!p->rt->alive)
		return ;
	p->eat_count++;
	p->death_tick = p->rt->cur_tick + p->rt->data[TIME_TO_DIE];
	p->debug_state = STATE_EAT;
	philo_print(p, MSG_EAT);
	usleep(p->rt->data_ms[TIME_TO_EAT]);
	pthread_mutex_unlock(&p->rt->forks[p->l_fork]);
	pthread_mutex_unlock(&p->rt->forks[p->r_fork]);
	p->l_fork = -1;
	p->r_fork = -1;
}

/// @brief sleep
/// @param p poitner to philo
static void	philo_sleep(t_philo *p)
{
	if (p->is_dead)
		return ;
	if (!p->rt->alive)
		return ;
	p->debug_state = STATE_SLEEP;
	philo_print(p, MSG_SLEEP);
	usleep(p->rt->data_ms[TIME_TO_SLEEP]);
}

/// @brief philo routine
/// @param ptr pointer to philo
/// @return return 0
void	*philo_routine_new(void *ptr)
{
	t_philo	*p;

	p = (t_philo *) ptr;
	if (!p)
		return (0);
	pthread_mutex_lock(&p->rt->ready_flag);
	pthread_mutex_unlock(&p->rt->ready_flag);
	p->death_tick = p->rt->cur_tick + p->rt->data[TIME_TO_DIE];
	while (!p->is_dead && p->rt->alive)
	{
		philo_think(p);
		philo_eat(p);
		philo_sleep(p);
	}
	p->thread_status = THREAD_CLEAN_EXIT;
	p->debug_state = STATE_ENDED;
	return (0);
}
