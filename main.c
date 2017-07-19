/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntrancha <ntrancha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/02/07 23:13:16 by ntrancha          #+#    #+#             */
/*   Updated: 2016/03/02 23:17:01 by ntrancha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minilibft/includes/libft.h"
#define SIZE_OF_STACK		4096
#define SIZE_OF_REGISTER	12

typedef struct			s_memcase
{
	char			*data;
	int			player_owner_id;
	struct s_memcase	*next;
}				t_memcase;

typedef struct			s_memory
{
	int			size;
	int			used;
	int			free;
	struct s_memcase	*first_case;
}				t_memory;

void		memory_mod(t_memory *mem, int addr, char *val, int id_player)
{
	t_memcase	*memcase;
	int		count;

	if (val)
	{
		count = 0;
		addr = addr % SIZE_OF_STACK;
		memcase = mem->first_case;
		while (memcase)
			if (addr == count)
			{
				memcase->data[0] = val[0];
				memcase->data[1] = val[1];
				memcase->data[2] = val[2];
				memcase->data[3] = val[3];
				memcase->player_owner_id = id_player;
				memcase = NULL;
			}
			else
			{
				count++;
				memcase = memcase->next;
			}
	}
}


void		case_kill(t_memcase *memcase)
{
	t_memcase	*next;

	ft_strdel(&(memcase->data));
	next = memcase->next;
	ft_memdel((void*)&memcase);
	if (next)
		case_kill(next);
}

void		memory_kill(t_memory *memory)
{
	case_kill(memory->first_case);
	ft_memdel((void*)&memory);
}

t_memcase	*case_init(int size)
{
	t_memcase	*mem;
       
	if (size > 0)
	{
		mem = ft_memalloc(sizeof(t_memcase));
		mem->data = ft_strdup("0000");
		mem->data[0] = '\0';
		mem->data[1] = '\0';
		mem->data[2] = '\0';
		mem->data[3] = '\0';
		mem->player_owner_id = -1;
		mem->next = case_init(size - 1);
		return (mem);
	}
	return (NULL);

}

t_memory	*memory_init(int size)
{
	t_memory	*mem;

	mem = ft_memalloc(sizeof(t_memory));
	mem->size = size;
	mem->used = 0;
	mem->free = size;
	mem->first_case = case_init(size);
}

void		debug_case(t_memcase *memcase)
{
	char	*tmp;

	ft_putnbr(memcase->data[0]);
	ft_putstr(":");
	ft_putnbr(memcase->data[1]);
	ft_putstr(":");
	ft_putnbr(memcase->data[2]);
	ft_putstr(":");
	ft_putnbr(memcase->data[3]);
	ft_putstr(" ");
	if (memcase->next)
		debug_case(memcase->next);
}

void		debug_memory(t_memory *memory)
{
	ft_putendl("\n\nMEMORY :\n");
	debug_case(memory->first_case);
	ft_putendl("\n");
}

typedef struct			s_game
{
	struct s_player		*players;
	int			n_player;
	int			cycle;
	t_memory		*memory;
}				t_game;

typedef struct			s_player
{
	char			*name;
	char			*file_asm;
	int			id;
	struct s_process	*process;
	struct s_player		*next;
}				t_player;

typedef struct			s_register
{
	char			*data;
	int			num;
	struct s_register	*next;
}				t_register;

typedef struct			s_process
{
	int			player_id;
	int			carry;
	int			adress;
	int			pid;
	int			alive;
	struct s_register	*r0;
	struct s_process	*next;
}				t_process;


void		register_kill(t_register *reg)
{
	t_register	*next;

	ft_strdel(&reg->data);
	next = reg->next;
	ft_memdel((void*)&reg);
	if (next)
		register_kill(next);
}

void		debug_player(t_player *player)
{
	t_player	*tmp;
	t_process	*proc;

	tmp = player;
	while (tmp->next != NULL)
	{
		ft_putstr("Nom:");
		ft_putendl(tmp->name);
		ft_putstr("Fichier:");
		ft_putendl(tmp->file_asm);
		ft_putstr("ID:");
		ft_putnbr_endl(tmp->id);
		ft_putendl("\nPROCESS :");
		proc = tmp->process;
		while (proc->next != NULL)
		{
			ft_putstr("ID:");
			ft_putnbr_endl(proc->player_id);
			ft_putstr("Carry:");
			ft_putnbr_endl(proc->carry);
			ft_putstr("Adress:");
			ft_putnbr_endl(proc->adress);
			ft_putstr("PID:");
			ft_putnbr_endl(proc->pid);
			ft_putstr("Alive:");
			ft_putnbr_endl(proc->alive);
			ft_putendl("");
			proc = proc->next;
		}
		ft_putendl("");
		ft_putendl("");
		tmp = tmp->next;
	}
}

void		debug_game(t_game *game)
{
	ft_putstr("cycle:");
	ft_putnbr_endl(game->cycle);
	ft_putstr("n_joueurs:");
	ft_putnbr_endl(game->n_player);
	ft_putendl("________________\nJOUEURS:\n");
	debug_player(game->players);
	debug_memory(game->memory);
}

t_player	*player_init(void)
{
	t_player	*player;

	player = ft_memalloc(sizeof(t_player));
	if (!player)
		return (NULL);
	player->name = NULL;
	player->file_asm = NULL;
	player->id = 0;
	player->process = NULL;
	player->next = NULL;
	return (player);
}

t_register	*register_add(t_register *r1, int count)
{
	t_register	*tmp;
	t_register	*new;

	if (r1 && count < SIZE_OF_REGISTER)
	{
		tmp = r1;
		while (tmp->next)
			tmp = tmp->next;
		new = ft_memalloc(sizeof(t_register));
		tmp->next = new;
		new->data = ft_strdup("0000");
		new->num = tmp->num + 1;
		new->next = NULL;
		register_add(r1, count + 1);
		return (new);
	}
	return (NULL);
}

t_register	*register_init()
{
	t_register	*first;
	int		count;

	count = 0;
	first = ft_memalloc(sizeof(t_register));
	first->num = count++;
	first->data = ft_strdup("0000");
	first->next = register_add(first, count);
	return (first);
}

t_process	*process_init(void)
{
	t_process	*process;

	process = ft_memalloc(sizeof(t_process));
	if (!process)
		return (NULL);
	process->next = NULL;
	process->r0 = register_init();
	process->pid = 0;
	process->player_id = 0;
	process->carry = 0;
	process->alive = 0;
	process->adress = 0;
	return (process);
}

t_game		*game_init(void)
{
	t_game	*game;

	game = ft_memalloc(sizeof(t_game));
	game->players = player_init();
	game->n_player = 0;
	game->cycle = 0;
	game->memory = memory_init(SIZE_OF_STACK);
	return (game);
}

int		process_create(t_process *process, int player_id, int adress);

int			player_create(t_player *player, char *name, char *file_asm, int addr)
{
	t_player	*tmp;
	t_player	*next;
	int		id;

	if (!player || !name || !file_asm)
		return (0);
	tmp = player;
	id = tmp->id;
	while (tmp->next != NULL)
	{
		id = tmp->id;
		tmp = tmp->next;
	}
	tmp->name = ft_strdup(name);
	tmp->file_asm = ft_strdup(file_asm);
	tmp->id = id + 1;
	tmp->process = process_init();
	tmp->next = player_init();
	process_create(tmp->process, id, addr);
	return (id);
}

int		process_create(t_process *process, int player_id, int adress)
{
	t_process	*tmp;
	t_process	*next;
	int		pid;

	if (!process)
		return (0);
	tmp = process;
	pid = tmp->pid + 1;
	while (tmp->next != NULL)
	{
		pid = tmp->pid + 1;
		tmp = tmp->next;
	}
	next = process_init();
	if (!next)
		return (0);
	tmp->player_id = player_id;
	tmp->adress = adress;
	tmp->pid = pid;
	tmp->alive = 1;
	tmp->next = next;
	return (pid);	
}

void	process_killall(t_process *process)
{
	t_process	*tmp;
	t_process	*next;

	tmp = process;
	while (tmp != NULL)
	{
		next = tmp->next;
		register_kill(tmp->r0);
		ft_memdel((void*)&tmp);
		tmp = next;
	}
}

int	process_kill(t_process *process, int pid)
{
	t_process	*tmp;

	tmp = process;
	while (tmp->next != NULL)
	{
		if (tmp->pid == pid)
		{
			tmp->alive = 0;
			return (1);
		}
		tmp = tmp->next;
	}
	return (0);
}

void		game_kill(t_game *game)
{
	t_player	*tmp;
	t_player	*next;

	memory_kill(game->memory);
	tmp = game->players;
	while (tmp != NULL)
	{
		next = tmp->next;
		ft_strdel(&(tmp->name));
		ft_strdel(&(tmp->file_asm));
		process_killall(tmp->process);
		ft_memdel((void*)&tmp);
		tmp = next;
	}
	ft_memdel((void*)&game);
}

char	*get_bit(void *memory, int adress)
{
	char	*ret;

	if (!(adress >= 0 && adress < SIZE_OF_STACK))
		return (NULL);
	ret = ft_strnew(5);
	ret[0] = ((char *)memory)[adress * 4];
	ret[1] = ((char *)memory)[(adress * 4) + 1];
	ret[2] = ((char *)memory)[(adress * 4) + 2];
	ret[3] = ((char *)memory)[(adress * 4) + 3];
	ret[4] = '\0';
	return (ret);
}

int	set_bit(void *memory, int adress, char *val)
{
	if (adress >= 0 && adress < SIZE_OF_STACK && ft_strlen(val) == 4)
	{
		((char *)memory)[adress * 4] = val[0];
		((char *)memory)[(adress * 4) + 1] = val[1];
		((char *)memory)[(adress * 4) + 2] = val[2];
		((char *)memory)[(adress * 4) + 3] = val[3];
		return (1);
	}
	return (0);
}

int	add_player(t_game *game, char *name, char *file_asm)
{
	int	id;
	
	id = player_create(game->players, name, file_asm, -1);
	game->n_player++;
	return (id);
}

int	decode_instruc(char *instruction)
{
	char	*tolower;
	int	ret;

	tolower = ft_strdup(instruction);
	ft_strtolower(tolower);
	ret = 1;
	if (ft_strcmp(tolower, "live") == 0)
		ret = 1;
	else if (ft_strcmp(tolower, "ld") == 0)
		ret = 2;
	else if (ft_strcmp(tolower, "st") == 0)
		ret = 3;
	else if (ft_strcmp(tolower, "add") == 0)
		ret = 4;
	else if (ft_strcmp(tolower, "sub") == 0)
		ret = 5;
	else if (ft_strcmp(tolower, "and") == 0)
		ret = 6;
	else if (ft_strcmp(tolower, "or") == 0)
		ret = 7;
	else if (ft_strcmp(tolower, "xor") == 0)
		ret = 8;
	else if (ft_strcmp(tolower, "zjump") == 0)
		ret = 9;
	else if (ft_strcmp(tolower, "ldi") == 0)
		ret = 10;
	else if (ft_strcmp(tolower, "sti") == 0)
		ret = 11;
	else if (ft_strcmp(tolower, "fork") == 0)
		ret = 12;
	else if (ft_strcmp(tolower, "lld") == 0)
		ret = 13;
	else if (ft_strcmp(tolower, "lldi") == 0)
		ret = 14;
	else if (ft_strcmp(tolower, "lfork") == 0)
		ret = 15;
	else if (ft_strcmp(tolower, "aff") == 0)
		ret = 16;
	ft_strdel(&tolower);
	return (ret - 1);
}

int	decode_targ(char *arg)
{
	if (!arg)
		return (0);
	if (arg[0] == '#')
		return (0);
	if (arg[0] == '@')
		return (2);
	if (arg[0] == 'r' || arg[0] == 'R')
		return (3);
	return (1);
}

int	decode_arg(char *arg)
{
	if (arg && (arg[0] == '@' || arg[0] == '#'))
		return (ft_atoi(&arg[1]));
	else if (arg)
		return (ft_atoi(arg));
	return (0);
}

char	*create_bin(int inst, int t1, int t2, int arg1, int arg2)
{
	char	*octet;
	int	ret;
	int	tmp;

	ret = ft_dectobin(inst) * 10000 + ft_dectobin(t1) * 100 + ft_dectobin(t2);
	octet = ft_strdup("0000");
	octet[0] = ret;
	ret = ft_dectobin(arg1) / 10000;
	tmp = arg1 - (ret * 10000);
	octet[1] = ret;
	ret = ft_dectobin(arg2) / 10000;
	octet[2] = (tmp * 10000) + ret;
	octet[3] = arg2 - (ret * 10000);
	return (octet);
}

char	*add_instruction(char *instruction)
{
	char	**tab;
	int	count;
	int	index;
	int	inst;
	int	targ1;
	int	targ2;
	int	arg1;
	int	arg2;

	tab = ft_strsplit((const char *)instruction, ' ');
	count = 0;
	while (tab && tab[count])
		count++;
	index = 0;
	inst = decode_instruc(tab[0]);
	targ1 = decode_targ(tab[1]);
	targ2 = decode_targ(tab[2]);
	arg1 = decode_arg(tab[1]);
	arg2 = decode_arg(tab[2]);
	ft_tabstrdel(tab);
	return (create_bin(inst, targ1, targ2, arg1, arg2));
}

void	parse_file_asm(char *file, t_player *player, t_game *game)
{
	char	**tab;
	char	*instruction;
	int	count;
	int	start;

	start = player->process->adress;
	tab = ft_strsplit(file, '\n');
	count = 0;
	while (tab && tab[count])
	{
		instruction = add_instruction(tab[count++]);
		memory_mod(game->memory, start++, instruction, player->id);
		ft_strdel(&instruction);
	}
	ft_tabstrdel(tab);
}

void	file_load(t_player *player, t_game *game)
{
	char	*file;

	file = ft_get_file(player->file_asm);
	parse_file_asm(file, player, game);
	ft_strdel(&file);
}

void	start_game(t_game *game)
{
	t_player	*player;
	t_process	*process;

	player = game->players;
	while (player->next)
	{
		player->process->adress = SIZE_OF_STACK / (game->n_player + 1) * player->id;
		file_load(player, game);
		player = player->next;
	}
}

int	main(void)
{
	t_game	*game;

	game = game_init();
	add_player(game, "toto", "code.asm");
	process_create(game->players->process, 42, 24);
	start_game(game);
	debug_game(game);
	game_kill(game);
	return 1;
}
