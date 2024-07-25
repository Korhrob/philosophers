#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// most common place to get stuck waiting
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
	p->debug_state_count++;
}

// potential deadlock situation
// l or r fork locks, waits for philo_print, but never gets to print
// thinking is now locking fork until all prints are resolved
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
		ft_usleep(p->rt->data[TIME_TO_DIE] * 2, p->rt);
		return ;
	}
	p->debug_state = STATE_WAIT_R_FORK;
	pthread_mutex_lock(&p->rt->forks[p->r_fork]);
	philo_print(p, MSG_FORK);
	p->debug_state_count++;
}

// another potential deadlock sitation
// still holding on to l and r fork, but waiting for print
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
	p->is_eating = TRUE;
	p->eat_count++;
	philo_print(p, MSG_EAT);
	ft_usleep(p->rt->data[TIME_TO_EAT], p->rt);
	pthread_mutex_unlock(&p->rt->forks[p->l_fork]);
	pthread_mutex_unlock(&p->rt->forks[p->r_fork]);
	p->is_eating = FALSE;
	p->last_eat = p->rt->cur_tick;
	p->l_fork = -1;
	p->r_fork = -1;
	p->debug_state_count++;
}

static void	philo_sleep(t_philo *p)
{
	if (p->is_dead)
		return ;
	if (!p->rt->alive)
		return ;
	philo_print(p, MSG_SLEEP);
	ft_usleep(p->rt->data[TIME_TO_SLEEP], p->rt);
	p->debug_state_count++;
}

void	*philo_routine_new(void *ptr)
{
	t_philo	*p;

	p = (t_philo *) ptr;
	if (!p)
	{
		p->thread_status = THREAD_CLEAN_EXIT;
		pthread_exit(NULL);
	}
	p->last_eat = p->rt->cur_tick;
	if (!p->is_dead && p->thread_status == THREAD_STARTED)
	{
		philo_think(p);
		philo_eat(p);
		philo_sleep(p);
	}
	p->thread_status = THREAD_CLEAN_EXIT;
	p->debug_state = STATE_IDLE,
	pthread_exit(NULL);
}
