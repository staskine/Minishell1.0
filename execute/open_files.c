/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_files.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emansoor <emansoor@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/12 12:59:06 by emansoor          #+#    #+#             */
/*   Updated: 2024/07/09 08:24:58 by emansoor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
checks for an empty file and saves its fd into first command, opens outfile
and saves its fd into last command in cmds
*/
static void	open_outfile(t_cmds *cmd)
{
	int	index;
	int	error;
	
	if (cmd->outfile_name == NULL)
		cmd->fd_outfile = 1;
	else if (cmd->fd_infile == -1)
		cmd->fd_outfile = -1;
	else
	{
		index = 0;
		error = 0;
		while (cmd->outfile_name[index])
		{
			cmd->fd_outfile = open(cmd->outfile_name[index], O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (cmd->fd_outfile < 0 && error < 1)
			{
				ft_putstr_fd("minishell: ", 2);
				perror(cmd->outfile_name[index]);
				error = 1;
			}
			if (cmd->outfile_name[index + 1] == NULL)
			{
				if (cmd->append)
				{
					close(cmd->fd_outfile);
					cmd->fd_outfile = open(cmd->outfile_name[index], O_WRONLY | O_CREAT | O_APPEND, 0666);
					if (cmd->fd_outfile < 0 && error < 1)
					{
						ft_putstr_fd("minishell: ", 2);
						perror(cmd->outfile_name[index]);
					}
				}
				break ;
			}
			close(cmd->fd_outfile);
			index++;
		}
	}
}

/*
checks if infile exists
*/
static void	open_infile(t_cmds *cmd)
{
	int	index;
	int error;
	
	if (cmd->infile_name == NULL)
		cmd->fd_infile = 0;
	else
	{
		index = 0;
		error = 0;
		while (cmd->infile_name[index])
		{
			if (ft_strcmp(cmd->infile_name[index], "heredoc") != 0)
			{
				cmd->fd_infile = open(cmd->infile_name[index], O_RDONLY, 0666);
				if (cmd->fd_infile < 0 && error < 1)
				{
					ft_putstr_fd("minishell: ", 2);
					perror(cmd->infile_name[index]);
					error = 1;
				}
				if (cmd->infile_name[index + 1] == NULL)
					break ;
				close(cmd->fd_infile);
			}
			index++;
		}
		if (ft_strcmp(cmd->infile_name[index - 1], "heredoc") == 0)
			cmd->fd_infile = 0;
	}
}

/*
closes any infiles and outfiles that might be open for each command
*/
void	close_files(t_cmds **cmds)
{
	t_cmds	*cmd;

	cmd = *cmds;
	while (cmd)
	{
		if (cmd->fd_infile > 1)
			close(cmd->fd_infile);
		if (cmd->fd_outfile > 1)
			close(cmd->fd_outfile);
		cmd = cmd->next;
	}
}

/*
opens any infiles and outfiles specified for each command
*/
void	open_files(t_cmds **cmds)
{
	t_cmds	*cmd;

	cmd = *cmds;
	while (cmd)
	{
		open_infile(cmd);
		open_outfile(cmd);
		cmd = cmd->next;
	}
}
