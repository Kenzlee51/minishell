/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talyx <talyx@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 21:08:17 by talyx             #+#    #+#             */
/*   Updated: 2021/09/10 17:29:15 by talyx            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	read_line(t_minishell *mini)
{
	char	*buff;

	buff = "";
	while (!ft_strequ(buff, "exit"))
	{
		signal(SIGINT, sig_handler);
		signal(SIGQUIT, sig_handler);
		buff = readline("\033[32mminishell\033[0m \033[31m➢\033[0m ");
		if (!buff)
		{
			ft_putstr_fd("exit\n", 1);
			break;
		}
		else if (ft_strlen(buff) == 0)
			continue ;
		else if(ft_strlen(buff) > 0)
			add_history(buff);
	}
	(void)mini;
}
