/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkorhone <rkorhone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:49:03 by rkorhone          #+#    #+#             */
/*   Updated: 2024/08/09 15:49:05 by rkorhone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/// @brief allocate and initialize t_philo
/// @param rt runtime struct
/// @param id id
/// @return t_philo instance
static t_philo	*create_philo(t_runtime *rt, int id)
{
	t_philo	*p;

	p = ft_calloc(sizeof(t_philo), 1);
	if (!p)
		return (NULL);
	p->id = id;
	p->rt = rt;
	p->l_fork = -1;
	p->r_fork = -1;
	p->alive = 1;
	return (p);
}

/// @brief initialize runtime struct
/// @param rt runtime struct
/// @param philos number of philosophers
/// @return 0 = malloc fail, 1 = success
static int	initialize_struct(t_runtime *rt, int philos)
{
	int	i;

	rt->eflag = 0;
	rt->cur_tick = 0;
	rt->start_tick = 0;
	rt->run = 1;
	rt->philos = ft_calloc(sizeof(t_philo *), philos);
	rt->forks = ft_calloc(sizeof(t_mutex), (philos));
	if (!rt->philos || !rt->forks)
		return (0);
	i = 0;
	while (i < philos)
	{
		rt->philos[i] = create_philo(rt, i);
		if (!rt->philos[i])
		{
			printf(ERR_MALLOC);
			return (0);
		}
		i++;
	}
	return (1);
}

/// @brief clean the runtime struct
/// @param rt runtime struct
static void	clean_struct(t_runtime *rt)
{
	int	i;

	if (rt->philos)
	{
		i = 0;
		while (i < rt->data[PHILO_COUNT])
		{
			if (rt->philos[i])
				free(rt->philos[i]);
			i++;
		}
		free(rt->philos);
	}
	if (rt->forks)
		free(rt->forks);
	free(rt);
}

/// @brief parse the program arguments and print related errors
/// @param argc argument arg count
/// @param argv arguments arguments
/// @param rt runtime struct
/// @return 0 = error, 1 = no errors
static int	parse_input(int argc, char **argv, t_runtime *rt)
{
	int	i;
	int	j;

	i = 0;
	if (argc < 5 || argc > 6)
	{
		printf(ERR_INVALID_ARGC);
		printf(HINT_FORMAT);
		return (0);
	}
	memset(rt->data, 0, DATA_MAX);
	while (i < DATA_MAX && i < (argc - 1))
	{
		j = ft_atoi(argv[i + 1]);
		if (j <= 0)
		{
			printf(ERR_INVALID_ARGV, argv[i + 1]);
			printf(HINT_NUMBER);
			return (0);
		}
		rt->data[i] = j;
		rt->data_ms[i] = j * 1000;
		i++;
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_runtime	*rt;

	if (DEBUG)
		printf(HINT_DEBUG);
	rt = NULL;
	rt = malloc(sizeof(t_runtime));
	if (!rt)
		return (1);
	if (!parse_input(argc, argv, rt))
		return (1);
	if (initialize_struct(rt, rt->data[PHILO_COUNT]))
		run(rt);
	clean_struct(rt);
	return (0);
}
