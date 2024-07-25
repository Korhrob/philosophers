#include "philosophers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/// @brief flag all philos is_dead 1
/// @param rt runtime struct
static void	stop_philos(t_runtime *rt)
{
	int	i;

	printf("stop_philos\n");
	i = 0;
	rt->alive = 0;
	while (i < rt->data[PHILO_COUNT])
	{
		rt->philos[i]->is_dead = 1;
		i++;
	}
}

/// @brief watch all philos and quit program if one dies
/// @param ptr runtime struct
/// @return NULL
void	*watch_philos(void *ptr)
{
	t_runtime	*rt;
	t_philo		*philo;
	int			i;

	rt = (t_runtime *)ptr;
	philo = rt->philos[0];
	i = 0;
	while (rt->alive)
	{
		// not correct, should only break if all philos have eaten enough
		// if (rt->data[MUST_EAT_X] && ((i - 1) % rt->data[PHILO_COUNT]) &&
		// 	(philo->eat_count >= rt->data[MUST_EAT_X]))
		// 		break; 
		if (rt->cur_tick > (philo->last_eat + rt->data[TIME_TO_DIE]))
			if (philo->is_eating == 0)
			{
				philo->is_dead = 0;
				printf("philo %d died on tick %d\n", philo->id, get_tick() - rt->start_tick);
		 		break;
			}
		philo = rt->philos[++i % rt->data[PHILO_COUNT]];
		ft_usleep(1, rt);
	}
	stop_philos(rt);
	pthread_exit(NULL);
}

/// @brief create a watcher thread and set error flag if needed
/// @param rt runtime struct
void	create_watcher(t_runtime *rt)
{
	if (!pthread_create(&rt->watcher, NULL, &watch_philos, rt))
		rt->eflag |= FLAG_WATCHER;
}