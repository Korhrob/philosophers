#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int	ft_isnum(char c)
{
	return (c >= '0' && c <= '9');
}

int	ft_atoi(char *str)
{
	int	i;

	i = 0;
	while (*str != 0 && ft_isnum(*str))
	{
		i *= 10;
		i += *str - '0';
		str++;
		if (i < 0)
			return (-1);
	}
	return (i);
}

void	*ft_calloc(int size, int count)
{
	void	*ptr;

	ptr = NULL;
	ptr = malloc(size * count);
	if (ptr)
		memset(ptr, 0, size * count);
	return (ptr);
}
