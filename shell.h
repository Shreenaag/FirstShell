#ifndef SHELL_H
#define SHELL_H
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>   // for pid_t
#include<sys/wait.h>    // for wait()
#include<string.h>      // for strerror()
#include<errno.h> 
# include <sysexits.h>
//////////////
#define RST   "\033[0m"   // Reset to default
#define RED     "\033[31m"  // Red
#define GREEN   "\033[32m"  // Green
#define YELLOW  "\033[33m"  // Yellow
#define CYAN    "\033[36m"  // Cyan
/////////
#define SPACE	"\t\n\v\f\r "
#define p(...) printf(__VA_ARGS__)
char* Getcwd(char *,size_t);
void	*Realloc(void *ptr, size_t size);
void *Malloc(size_t size);
void dbzSpinnerLoading(void); 
pid_t Fork(void);
pid_t	Wait(int *status);
pid_t	Waitpid(pid_t pid, int *status, int options);
void	Execvp(const char *file, char *const argv[]);
typedef struct s_bulitin
{
    const char *builtin_name;
    int (*foo)(char **av);
} t_builtin;
void	Getline(char **lineptr, size_t *n, FILE *stream);
void	Chdir(const char *path);
int	cell_env(char **args);   /* Environment variables display */
int	cell_exit(char **args);  /* Shell exit command */
int cell_echo(char **args);
#endif
