/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_phase.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/21 11:08:47 by abouabra          #+#    #+#             */
/*   Updated: 2023/08/19 09:42:37 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <stdio.h>
#include <unistd.h>

void	execute_built_in(t_command *command)
{
	if (!ft_strncmp("cd", command->command_args[0], -1))
		cd(command);
	else if (!ft_strncmp("exit", command->command_args[0], -1))
		my_exit(command);
	else if (!ft_strncmp("export", command->command_args[0], -1))
		env_export(command);
	else if (!ft_strncmp("unset", command->command_args[0], -1))
		unset( command);
}

int	builtin_should_execute_in_child(t_command *tmp)
{
	if (!ft_strncmp("echo", tmp->command_args[0], -1))
	{
		echo(tmp);
		custom_exit(0);
	}
	else if (!ft_strncmp("env", tmp->command_args[0], -1))
	{
		env();
		custom_exit(0);
	}
	else if (!ft_strncmp("export", tmp->command_args[0], -1))
	{
		env_export(tmp);
		custom_exit(0);
	}
	else if (!ft_strncmp("pwd", tmp->command_args[0], -1))
	{
		pwd();
		custom_exit(0);
	}
	else if (!ft_strncmp("cd", tmp->command_args[0], -1))
	{
		cd(tmp);
		custom_exit(0);
	}
	return 0;
}

static void	handle_child3(t_command *tmp)
{
	if(!builtin_should_execute_in_child(tmp))
	{
		execve(tmp->command_path, tmp->command_args,
			convert_env_to_arr(g_vars->env_head));
		custom_exit(1);
	}
}

void do_redirections(t_cmd_redir *head)
{

	// t_cmd_redir * tmp = g_vars->command_head->redir;
	// while(tmp)
	// {
	// 	printf("type: %d   file name: %s\n", tmp->type, tmp->file);
	// 	tmp = tmp->next;
	// }
	
	t_cmd_redir *redir = head;
	int fd;
	while(redir)
	{
		if(redir->type == INPUT)
		{
			fd = open(redir->file, O_RDONLY);
			if (fd == -1)
				custom_exit(1);
			dup2(fd, 0);
			close(fd);
		}
		else if(redir->type == OUTPUT)
		{
			fd = open(redir->file, O_WRONLY | O_TRUNC | O_CREAT, 0644);
			if (fd == -1)
				custom_exit(1);
			dup2(fd, 1);
			close(fd);
		}
		else if(redir->type == APPEND)
		{
			fd = open(redir->file, O_RDWR | O_APPEND | O_CREAT, 0644);
			if (fd == -1)
				custom_exit(1);
			dup2(fd, 1);
			close(fd);
		}
		else if(redir->type == HEREDOC)
		{
			fd = open("/tmp/herdoc_data", O_RDONLY);
			if (fd == -1)
				custom_exit(1);
			dup2(fd, 0);
			close(fd);
		}
		redir = redir->next;
	}
}

void	handle_child(t_command *tmp, int i)
{
	if ((g_vars->pipe == 2 || g_vars->pipe == 3)||  (i > 0 && (!g_vars->op[0] || (g_vars->op[0] && g_vars->op[(i - 1) * 2] == '1'))))
	{
		// printf("handle child pipe: 2 cmd: %s\n",tmp->command_args[0]);

		// if(g_vars->pipe == 2)
		// {

		// char line[101];
		// int r =read(g_vars->prev_pipefd[0], line, 100);
		// line[r] = 0;
		// write(2,line,ft_strlen(line));

		// }

		dup2(g_vars->prev_pipefd[0], 0);



		close(g_vars->prev_pipefd[0]);
		close(g_vars->prev_pipefd[1]);
	}
	if ((g_vars->pipe == 1  || g_vars->pipe == 3)|| (i < g_vars->command_count - 1 && (!g_vars->op[0] || (g_vars->op[0] && g_vars->op[i * 2] == '1'))))
	{
		// printf("handle child pipe: 1 cmd: %s\n",tmp->command_args[0]);
		dup2(g_vars->next_pipefd[1], 1);
		close(g_vars->next_pipefd[0]);
		close(g_vars->next_pipefd[1]);
	}
	do_redirections(tmp->redir);
	if (tmp->is_valid_command == 0)
		custom_exit(127);
	if (tmp->is_valid_command == 69)
		custom_exit(0);
	handle_child3(tmp);
}
