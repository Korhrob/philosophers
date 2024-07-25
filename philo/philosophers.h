#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

#include <pthread.h>

#define ERR_INVALID_ARGC "error: invalid argument count\n"
#define ERR_INVALID_ARGV "error: invalid argument '%s'\n"
#define HINT_FORMAT "./philosopher [count] [time_to_die] [time_to_eat] [time_to_sleep] (must_eat_x)\n"
#define	HINT_NUMBER "argument must be numerical value above 0\n"

#define MSG_THINK "is thinking"
#define MSG_FORK "has taken a fork"
#define MSG_EAT "is eating"
#define MSG_SLEEP "is sleeping"
#define MSG_DIE "died"

typedef struct s_runtime t_runtime;
typedef struct s_philo t_philo;
typedef unsigned int t_uint;
typedef pthread_mutex_t t_mutex;

typedef enum e_data
{
	PHILO_COUNT = 0,
	TIME_TO_DIE,
	TIME_TO_EAT,
	TIME_TO_SLEEP,
	MUST_EAT_X,
	DATA_MAX,
}	t_data;

typedef struct	s_philo
{
	t_uint		id;
	t_uint		is_dead;
	t_uint		last_eat;
	t_uint		eat_count;
	t_uint		is_eating;
	t_uint		l_fork;
	t_uint		r_fork;
	t_runtime	*rt;
	pthread_t	thread; // could move to rt
}	t_philo;

typedef struct	s_runtime
{
	t_uint		data[6];
	t_uint		start_tick;
	t_uint		cur_tick;
	t_uint		alive;
	t_philo		**philos;
	int			sleep_adjust;
	pthread_t	timer;
	pthread_t	watcher;
	t_mutex		*forks;
	t_mutex		tick;
	t_mutex		mutex; // for print
}	t_runtime;

// mini libft
int		ft_atoi(char *str);
void	*ft_calloc(int size, int count);

// philosophers
void	philosophers(t_runtime *rt);

// routine
void	*philo_routine(void *ptr);

// time
t_uint	get_tick();
void	*timer_tick(void *ptr);
void	ft_usleep(t_uint ms, t_runtime *rt);

#endif