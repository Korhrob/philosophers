#include "philosophers.h"
#include <pthread.h>

// return 1 if running
int	get_rt_status(t_runtime *rt)
{
	int	r;

	r = 0;
	pthread_mutex_lock(&rt->watch_lock);
	r = rt->run;
	pthread_mutex_unlock(&rt->watch_lock);
	return (r);
}

// return 1 if alive
int	get_philo_status(t_philo *philo)
{
	int	r;

	r = 0;
	pthread_mutex_lock(&philo->act);
	r = philo->alive;
	pthread_mutex_unlock(&philo->act);
	return (r);
}

// return philo death tick
t_uint	get_philo_death(t_philo *philo)
{
	t_uint	r;

	r = 0;
	pthread_mutex_lock(&philo->act);
	r = philo->death_tick;
	pthread_mutex_unlock(&philo->act);
	return (r);
}

// return philo meals eaten
t_uint	get_philo_eat(t_philo *philo)
{
	t_uint	r;

	r = 0;
	pthread_mutex_lock(&philo->act);
	r = philo->eat_count;
	pthread_mutex_unlock(&philo->act);
	return (r);
}

// return current tick
t_uint	get_cur_tick(t_runtime *rt)
{
	t_uint	tick;

	pthread_mutex_lock(&rt->timer_lock);
	tick = rt->cur_tick;
	pthread_mutex_unlock(&rt->timer_lock);
	return (tick);
}
