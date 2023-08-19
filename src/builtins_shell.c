/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_shell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhayri <ykhayri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/21 11:55:49 by abouabra          #+#    #+#             */
/*   Updated: 2023/08/19 10:32:57 by ykhayri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	the_search_built(t_env *search, char *old_path)
{
	while (search)
	{
		if (!ft_strncmp(search->env_id, "PWD", -1))
		{
			char *tmp_search  = getcwd(NULL, 0);
			garbage_collector(tmp_search, 0);
			search->env_data = tmp_search;
		}
		else if (!ft_strncmp(search->env_id, "OLDPWD", -1))
			search->env_data = old_path;
		search = search->next;
	}
}

void	cd(t_command *command)
{
	t_env	*search;
	char	*data;
	int		ofsset;
	char	*new;

	if (!command->command_args[1])
		command->command_args[1] = get_env_data( "HOME");
	if (!command->command_args[1])
	{
		ft_dprintf(2, "minishell: cd: HOME not set\n");
		g_vars->ex_status = 1;
		return ;
	}
	else if (ft_strchr(command->command_args[1], '~'))
	{
		data = get_env_data( "HOME");
		ofsset = ft_strchr_num(command->command_args[1], '~');
		new = ft_substr(command->command_args[1], ofsset + 1,
				ft_strlen(command->command_args[1]));
		command->command_args[1] = ft_strjoin(data, new);
	}
	else if (access(command->command_args[1], F_OK))
	{
		ft_dprintf(2, "minishell: cd: %s: No such file or directory\n",
			command->command_args[1]);
		g_vars->ex_status = 1;
		return ;
	}
	char *old_path = getcwd(NULL, 0);
	garbage_collector(old_path, 0);
	chdir(command->command_args[1]);
	search = g_vars->env_head;
	the_search_built(search,old_path);
	g_vars->ex_status = 0;
}

void	echo(t_command *command)
{
	int	is_arg;
	int	i;

	is_arg = 1;
	i = 0;
	if (!command->command_args[1])
	{
		ft_dprintf(1, "\n");
		return ;
	}
	while (!ft_strncmp("-n", command->command_args[i +1], -1)
		|| !ft_strncmp("-nn", command->command_args[i +1], -1))
	{
		is_arg = 0;
		i++;
	}
	while (command->command_args[++i])
	{
		ft_dprintf(1, "%s", command->command_args[i]);
		if (command->command_args[i + 1])
			ft_dprintf(1, " ");
	}
	if (is_arg)
		ft_dprintf(1, "\n");
}

void					my_exit(t_command *command)
{
	int	status = 0;

	if (!command->command_args[1])
		status = g_vars->ex_status;
	else if (command->command_args[1] && command->command_args[2])
	{
		if ((is_arg_number(command->command_args[1]) && !is_arg_number(command->command_args[2]))
		|| (is_arg_number(command->command_args[1]) && is_arg_number(command->command_args[2])))
		{
			status = 255;
			ft_dprintf(2, "minishell: exit: %s: numeric argument required\n",are_two_args_number(command->command_args));
		}
		else
		{
			status = 1;
			ft_dprintf(2, "minishell: exit: too many arguments\n");
		}
	}
	else
	{
		if (is_arg_number(command->command_args[1]))
		{
			ft_dprintf(2, "minishell: exit: %s: numeric argument required\n",is_arg_number(command->command_args[1]));
			status = 255;
		}
		else
			status = ft_atoi(command->command_args[1]);
	}
	custom_exit(status);
}

void	pwd(void)
{
	char *path = getcwd(NULL, 0);
	garbage_collector(path, 0);
	ft_dprintf(1, "%s\n", path);
	g_vars->ex_status = 0;
}
