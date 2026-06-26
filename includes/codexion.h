/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/26 01:37:03 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define HEAP_CAP 2
# define SCHED_EDF 0
# define SCHED_FIFO 1

typedef struct s_request
{
	int				ticket;
	long long		deadline;
}					t_request;

typedef struct s_dongle
{
	int				is_taken;
	long long		released_at;
	int				next_ticket;
	int				heap_size;
	t_request		*heap;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
}					t_dongle;

typedef struct s_coder
{
	int				id;
	int				dongle_left;
	int				dongle_right;
	long long		last_compile_start;
	int				compiles_done;
	pthread_mutex_t	data_lock;
	struct s_env	*env;
}					t_coder;

typedef struct s_env
{
	int				nb_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				nb_compiles_req;
	int				dongle_cooldown;
	int				scheduler;
	int				stop;
	long long		start_time;
	pthread_mutex_t	stop_lock;
	pthread_mutex_t	log_lock;
	pthread_t		monitor_thread;
	int				nb_mutex_data;
	int				nb_mutex_dongles;
	int				nb_mutex_cond;
	int				nb_mutex_lock;
	int				dongles_initialized;
	t_dongle		*dongles;
	t_coder			*coders;
}					t_env;

int					run_init_parsing(char **argv, int *setting);
int					run_init_settings(t_env *env, int *setting);
void				run_destroy(t_env *env);
int					init_settings(t_env *env, int *setting);
void				init_coders(t_env *env);
void				start_clock(t_env *env);
int					init_dongles(t_env *env);
int					init_mutex_lock(t_env *env);
int					init_mutex_data(t_env *env);
int					init_mutex_dongles(t_env *env);
int					init_mutex_cond(t_env *env);
int					run_t_manager(t_env *env);
int					create_t_coders(t_env *env, pthread_t *thread_ids);
int					create_t_monitor(t_env *env);
void				wait_t_coders(t_env *env, pthread_t *thread_ids);
void				wait_t_monitor(t_env *env);
int					parse_scheduler(char *arg, int *value);
void				error_log_parse(char *arg);

void				destroy_mutex_lock(t_env *env);
void				destroy_all_mutex_data(t_env *env);
void				destroy_all_mutex_dongles(t_env *env);
void				destroy_all_cond(t_env *env);

int					ft_atoi(const char *nptr);
int					is_number(char *str);
int					is_length_number(char *str, int nb);
long long			get_time_in_ms(void);
void				wait_action(t_env *env, int ms);
void				cooldown_sleep(long long released_at, int cooldown);
void				log_event(t_env *env, int coder_id, char *msg);
void				log_burnout(t_env *env, int coder_id);

int					take_dongle(t_dongle *dongle, t_coder *coder);
void				release_dongle(t_dongle *dongle);
void				grant_dongle(t_dongle *dongle, int scheduler, int cooldown);
int					request_is_priority(t_request *a, t_request *b,
						int scheduler);
void				heap_push(t_dongle *dongle, t_request req, int scheduler);
t_request			heap_pop(t_dongle *dongle, int scheduler);
int					should_stop(t_env *env);
void				set_stop(t_env *env);
void				choose_dongle(t_coder *coder, t_dongle **first,
						t_dongle **second);
void				compile(t_coder *coder, t_dongle *first, t_dongle *second);
void				debug_and_refactor(t_coder *coder);
void				*run_action_coders(void *arg);
void				*run_monitor_coders(void *arg);
int					check_burnout(t_coder *coder);
int					check_all_c_done(t_env *env);

#endif
