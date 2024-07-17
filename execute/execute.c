/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emansoor <emansoor@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 09:34:24 by emansoor          #+#    #+#             */
/*   Updated: 2024/07/17 11:32:57 by emansoor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	safe_to_run(t_cmds *cmds)
{
	printf("fd_infile: %d | validity: %d | fd_outfile: %d\n", cmds->fd_infile, cmds->valid, cmds->fd_outfile[0]);
	if (cmds->command == NULL
		|| ((cmds->fd_infile == -1 || cmds->valid < 0)
			&& cmds->heredoc == NULL) || cmds->fd_outfile[0] < 0)
		return (0);
	return (1);
}

/*
sets up pipes for command that's id is an even number
*/
static int	even_id_cmds(t_cmds *cmd, int *pipefds)
{
	close(pipefds[WRITE_END + 2]);
	if (cmd->fd_infile == 0)
		cmd->fd_infile = pipefds[READ_END + 2];
	if (dup2(cmd->fd_infile, STDIN_FILENO) < 0)
	{
		perror("minishell");
		return (1);
	}
	close(cmd->fd_infile);
	close(pipefds[READ_END]);
	if (cmd->fd_outfile[0] == 1)
		cmd->fd_outfile[0] = pipefds[WRITE_END];
	if (dup2(cmd->fd_outfile[0], STDOUT_FILENO) < 0)
	{
		perror("minishell");
		return (1);
	}
	close(cmd->fd_outfile[0]);
	return (0);
}

/*
sets up pipes for command that's id is an odd number
*/
static int	odd_id_cmds(t_cmds *cmd, int *pipefds)
{
	close(pipefds[WRITE_END]);
	if (cmd->fd_infile == 0)
		cmd->fd_infile = pipefds[READ_END];
	if (dup2(cmd->fd_infile, STDIN_FILENO) < 0)
	{
		perror("minishell");
		return (1);
	}
	close(cmd->fd_infile);
	close(pipefds[READ_END + 2]);
	if (cmd->fd_outfile[0] == 1)
		cmd->fd_outfile[0] = pipefds[WRITE_END + 2];
	if (dup2(cmd->fd_outfile[0], STDOUT_FILENO) < 0)
	{
		perror("minishell");
		return (1);
	}
	close(cmd->fd_outfile[0]);
	return (0);
}

/*
if the command to be executed is not the first or the last command
in a pipeline, sets up pipes according to even and odd numbered ids
*/
static void	set_pipes(t_mini *shell, t_cmds *cmd)
{
	if (cmd->id % 2 == 0)
	{
		if (even_id_cmds(cmd, shell->pipefds) > 0)
			panic(shell, 9);
	}
	else
	{
		if (odd_id_cmds(cmd, shell->pipefds) > 0)
			panic(shell, 9);
	}
}

/*
sets up pipes and redirections according to command type and runs
a builtin or execve accordingly
*/
void	execute(t_mini *shell, t_cmds *cmd)
{
	if (cmd->id == 0)
		first_command(shell, cmd);
	else if (cmd->id == cmd->commands - 1)
		last_command(shell, cmd);
	else
		set_pipes(shell, cmd);
	if (cmd->builtin == 1)
	{
		ft_freearray(shell->env_p);
		check_builtin(shell, cmd);
	}
	else
	{
		if (execve(cmd->path, cmd->command, shell->env_p) == -1)
			panic(shell, 126);
	}
}
