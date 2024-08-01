#include "philosophers.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

/// @brief get currect tick with gettimeofday
/// @return tick in millisecond
t_uint	get_tick(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

/// @brief try to sleep x milliseconds
/// @param ms millisecond
/// @param rt runtime struct
void	ft_usleep(t_uint ms, t_runtime *rt)
{
	t_uint	sleep_till;

	sleep_till = rt->cur_tick + ms;
	if (ms > 1)
		usleep(1000 * (ms - 1));
	while (get_tick() - rt->start_tick < sleep_till)
		;
}

/// @brief tick runtime timer
/// @param ptr runtime struct
/// @return null
void	*timer_tick(void *ptr)
{
	t_runtime	*rt;

	rt = (t_runtime *)ptr;
	pthread_mutex_lock(&rt->ready_flag);
	pthread_mutex_unlock(&rt->ready_flag);
	pthread_mutex_lock(&rt->tick);
	rt->start_tick = get_tick();
	rt->cur_tick = 0;
	pthread_mutex_unlock(&rt->tick);
	while (rt->alive)
	{
		ft_usleep(1, rt);
		pthread_mutex_lock(&rt->tick);
		rt->cur_tick = get_tick() - rt->start_tick;
		pthread_mutex_unlock(&rt->tick);
	}
	return (0);
}

/// @brief create the timer thread
/// @param rt runtime struct
void	create_timer(t_runtime *rt)
{
	if (pthread_create(&rt->timer, NULL, &timer_tick, rt))
	{
		rt->eflag |= FLAG_TIMER;
		printf(ERR_TIMER);
	}
}
