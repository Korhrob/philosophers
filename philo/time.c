#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

/// @brief get currect tick with gettimeofday
/// @return tick in ms
t_uint	get_tick()
{
	struct timeval time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

/// @brief tick runtime timer
/// @param ptr pointer to t_runtime
/// @return null
void	*timer_tick(void *ptr)
{
	struct timeval	time;
	t_runtime		*rt;

	rt = (t_runtime *)ptr;
	rt->start_tick = get_tick();
	rt->cur_tick = 0;
	while (rt->alive)
	{
		ft_usleep(1, rt);
		gettimeofday(&time, NULL);
		rt->cur_tick = (time.tv_sec * 1000) + (time.tv_usec / 1000) - rt->start_tick;
	}
	pthread_exit(NULL);
	return (NULL);
}

/// @brief sleep at least until x ms
/// @param ms millisecond
/// @param start_tick start tick
void	ft_usleep(t_uint ms, t_runtime *rt)
{
	t_uint	sleep_till;
	int		sleep_diff;

	sleep_till = rt->cur_tick + ms;
	while (get_tick() - rt->start_tick < sleep_till)
		usleep(1000);
	// usleep(ms * 1000);
	sleep_diff = sleep_till - rt->cur_tick;
	if (sleep_diff > 1)
		usleep(sleep_diff * 1000);
}