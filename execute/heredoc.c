/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emansoor <emansoor@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 10:00:25 by sataskin          #+#    #+#             */
/*   Updated: 2024/07/10 11:19:48 by emansoor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	join_rl(char *rl,  t_cmds *cmd, t_mini *shell)
{
    char    *temp;
    char    *hd;

	if (!cmd->heredoc)
		cmd->heredoc = ft_strdup("");
	if (!cmd->heredoc)
		free_and_exit(shell, "minishell: malloc fail");
    temp = ft_strjoin(rl, "\n");
    if (!temp)
	{
    	free_and_exit(shell, "minishell: malloc fail");
	}
	hd = cmd->heredoc;
    cmd->heredoc = ft_strjoin(cmd->heredoc, temp);
    if (!cmd->heredoc)
	{
		free(temp);
		free_and_exit(shell, "minishell: malloc fail");
	}
	free(temp);
	free(hd);
}

void	heredoc(t_mini *shell, t_cmds *cmd)
{
	char	*comp;
	char	*rl;

	comp = ft_strdup(cmd->heredoc);
	if (!comp)
		free_and_exit(shell, "minishell: malloc error");
	free(cmd->heredoc);
	cmd->heredoc = NULL;
	set_signal(2);
	while (1)
	{
		//rl = readline("> ");
		rl = readline(PINK HDOC RESET);
		if (!rl)
			break ;
		if (rl[0] == '\0')
			continue ;
		if (ft_strcmp(rl, comp) == 0)
			break ;
		else
			join_rl(rl, cmd, shell);
		free(rl);
	}
	set_signal(1);
	if (rl && cmd->heredoc == NULL && ft_strcmp(rl, comp) == 0)
	{
		cmd->heredoc = ft_strdup("");
		if (!cmd->heredoc)
			free_and_exit(shell, "minishell: malloc error");
	}
	if (rl)
		free(rl);
	else
	{
		ft_putstr_fd("minishell: heredoc interrupted by CTRL+D\n", 2);
		free(cmd->heredoc);
	}
	free(comp);
}
