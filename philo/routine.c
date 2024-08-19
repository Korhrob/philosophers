/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkorhone <rkorhone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:49:57 by rkorhone          #+#    #+#             */
/*   Updated: 2024/08/09 15:49:58 by rkorhone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	if (!get_philo_status(p))
		return ;
	if (!get_rt_status(p->rt))
		return ;
	pthread_mutex_lock(&p->rt->print_lock);
	if (get_philo_status(p) && get_rt_status(p->rt))
	{
		if (DEBUG)
			printf("%4d %d %s\n", get_cur_tick(p->rt), p->id + 1, msg);
		else
			printf("%d %d %s\n", get_cur_tick(p->rt), p->id + 1, msg);
	}
	pthread_mutex_unlock(&p->rt->print_lock);
}

/// @brief philos are thinking while waiting for forks
/// @param p pointer to philo
static void	philo_think(t_philo *p)
{
	if (!get_philo_status(p))
		return ;
	if (!get_rt_status(p->rt))
		return ;
	philo_print(p, MSG_THINK);
	p->l_fork = p->id;
	p->r_fork = (p->id + 1) % p->rt->data[PHILO_COUNT];
	if (p->id % 2 && p->eat_count == 0)
		usleep(p->rt->data_ms[TIME_TO_EAT] / 2);
	pthread_mutex_lock(&p->rt->forks[p->l_fork]);
	philo_print(p, MSG_FORK);
	if (p->l_fork == p->r_fork)
	{
		p->r_fork = -1;
		usleep(p->rt->data_ms[TIME_TO_DIE] + 1000);
		return ;
	}
	pthread_mutex_lock(&p->rt->forks[p->r_fork]);
	philo_print(p, MSG_FORK);
}

/// @brief eat and releast forks
/// @param p pointer to philo
static void	philo_eat(t_philo *p)
{
	if (!get_philo_status(p))
		return ;
	if (!get_rt_status(p->rt))
		return ;
	pthread_mutex_lock(&p->act);
	p->eat_count++;
	p->death_tick = p->rt->cur_tick + p->rt->data[TIME_TO_DIE];
	pthread_mutex_unlock(&p->act);
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
	if (!get_philo_status(p))
		return ;
	if (!get_rt_status(p->rt))
		return ;
	philo_print(p, MSG_SLEEP);
	usleep(p->rt->data_ms[TIME_TO_SLEEP]);
}

/// @brief philo routine
/// @param ptr pointer to philo
/// @return return 0
void	*routine(void *ptr)
{
	t_philo	*p;

	p = (t_philo *) ptr;
	if (!p)
		return (0);
	pthread_mutex_lock(&p->rt->ready_lock);
	pthread_mutex_unlock(&p->rt->ready_lock);
	pthread_mutex_lock(&p->act);
	p->death_tick = get_cur_tick(p->rt) + p->rt->data[TIME_TO_DIE];
	pthread_mutex_unlock(&p->act);
	while (get_philo_status(p) && get_rt_status(p->rt))
	{
		philo_think(p);
		philo_eat(p);
		philo_sleep(p);
	}
	if (p->l_fork != -1)
		pthread_mutex_unlock(&p->rt->forks[p->l_fork]);
	if (p->r_fork != -1)
		pthread_mutex_unlock(&p->rt->forks[p->r_fork]);
	pthread_mutex_lock(&p->act);
	p->thread_status = THREAD_CLEAN_EXIT;
	pthread_mutex_unlock(&p->act);
	return (0);
}
