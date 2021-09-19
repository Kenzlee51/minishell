/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talyx <talyx@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 21:08:17 by talyx             #+#    #+#             */
/*   Updated: 2021/09/19 19:28:45 by talyx            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	read_line(t_minishell *mini)
{
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	mini->line = readline("\033[32mminishell\033[0m \033[31m➢\033[0m ");
	if (!mini->line)
	{
		free(mini->line);
		ft_putstr_fd("\n", 2);
		exit(0);
		return (-1);
	}
	else if (ft_strlen(mini->line) == 0)
		return (0);
	else if(ft_strlen(mini->line) > 0)
		add_history(mini->line);
	return (1);
}
