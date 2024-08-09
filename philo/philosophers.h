/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkorhone <rkorhone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:49:30 by rkorhone          #+#    #+#             */
/*   Updated: 2024/08/09 15:49:32 by rkorhone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>

# ifndef DEBUG
#  define DEBUG 0
# endif

# define ERR_THREAD "critical error: creating or joing thread %d failed\n"
# define ERR_HANG "hanging thread %d, thread status: %s\n"
# define ERR_DEATCH "thread was joined, detaching...\n"
# define ERR_INVALID_ARGC "error: invalid argument count\n"
# define ERR_INVALID_ARGV "error: invalid argument '%s'\n"
# define ERR_WATCHER "watcher thread failed\n"
# define ERR_TIMER "timer thread failed\n"
# define ERR_MALLOC "malloc failed"

# define DEBUG_EATCOUNT "eat count reached (%d)\n"

# define HINT_FORMAT "./philosopher [count] [time_to_die] \
[time_to_eat] [time_to_sleep] (must_eat_x)\n"
# define HINT_NUMBER "argument must be numerical value above 0\n"
# define HINT_DEBUG "program in debug mode\n"

# define MSG_THINK "is thinking"
# define MSG_FORK "has taken a fork"
# define MSG_EAT "is eating"
# define MSG_SLEEP "is sleeping"
# define MSG_DIE "died"

# ifndef TRUE
#  define TRUE 1
# endif
# ifndef FALSE
#  define FALSE 0
# endif

typedef struct s_runtime	t_runtime;
typedef struct s_philo		t_philo;
typedef unsigned int		t_uint;
typedef unsigned int		t_bool;
typedef pthread_mutex_t		t_mutex;

typedef enum e_data
{
	PHILO_COUNT,
	TIME_TO_DIE,
	TIME_TO_EAT,
	TIME_TO_SLEEP,
	MUST_EAT_X,
	DATA_MAX,
}	t_data;

typedef enum e_error
{
	FLAG_WATCHERT	= 1,
	FLAG_TIMERT		= 2,
	FLAG_PHILOT		= 4,
	FLAG_JOIN_P		= 8,
	FLAG_JOIN_W		= 16,
	FLAG_JOIN_T		= 32,
}	t_error;

typedef enum e_thread_status
{
	THREAD_NOT_STARTED,
	THREAD_CREATE_FAIL,
	THREAD_STARTED,
	THREAD_JOIN_FAIL,
	THREAD_JOINED,
	THREAD_CLEAN_EXIT
}	t_tstatus;

typedef struct s_philo
{
	t_uint		id;
	t_uint		alive;
	t_uint		death_tick;
	t_uint		eat_count;
	int			l_fork;
	int			r_fork;
	t_runtime	*rt;
	pthread_t	thread;
	t_tstatus	thread_status;
	t_mutex		act;
}	t_philo;

typedef struct s_runtime
{
	int			eflag;
	int			data[6];
	int			data_ms[6];
	t_uint		start_tick;
	t_uint		cur_tick;
	t_uint		run;
	t_philo		**philos;
	pthread_t	timer;
	pthread_t	watcher;
	t_mutex		*forks;
	t_mutex		timer_lock;
	t_mutex		print_lock;
	t_mutex		ready_lock;
	t_mutex		watch_lock;
}	t_runtime;

// mini libft
int		ft_atoi(char *str);
void	*ft_calloc(int size, int count);

// philosophers
void	run(t_runtime *rt);

// routine
void	*routine(void *ptr);

// timer
t_uint	get_tick(void);
void	*timer_tick(void *ptr);
void	ft_usleep(t_uint ms, t_runtime *rt);
void	create_timer(t_runtime *rt);

// watcher
void	*watch_philos(void *ptr);
void	create_watcher(t_runtime *rt);

// get
int		get_rt_status(t_runtime *rt);
int		get_philo_status(t_philo *philo);
t_uint	get_philo_death(t_philo *philo);
t_uint	get_philo_eat(t_philo *philo);
t_uint	get_cur_tick(t_runtime *rt);

#endif
