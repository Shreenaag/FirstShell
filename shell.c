#include "shell.h"
t_builtin g_builtin[]={
{.builtin_name = "env", .foo=cell_env},     /* Print environment */
{.builtin_name = "echo", .foo=cell_echo},     /* Print environment */
	{.builtin_name = "exit", .foo=cell_exit},  /* Exit the shell */
	{.builtin_name = NULL},                    /* Sentinel */

};
int status=0;
void cell_launch(char **args)
{
   if(Fork()==0)
   {
    Execvp(args[0],args);
   }
   else{
    Wait(&status);
   }
}
void cell_exec(char **args)
{
    int i;
    const char *cur;
    i=0;
    while(cur=g_builtin[i].builtin_name)
    {
        if(!strcmp(cur,args[0]))
        {
            status=g_builtin[i].foo(args);
            return;
        }
        i++;
    }
    cell_launch(args); //fork ->exec
}
char** cell_split_line(char *line)
{
      char **tokens;
      unsigned long postion=0;
      size_t bufSize=BUFSIZ;
      tokens=Malloc(bufSize*sizeof *tokens);
      for(char *token=strtok(line,SPACE);token;token=strtok(NULL,SPACE))
      {
        tokens[postion++]=token;
        if(postion>=bufSize)
        {
            bufSize*=2;
            tokens=Realloc(tokens,bufSize*sizeof*tokens);
        }
      }
      tokens[postion]=NULL;//sentinal value
      return tokens;
}
char* cell_read_line(void)
{
   char	*line;
	size_t	bufsize;
	char	cwd[BUFSIZ];

	line = NULL;
	bufsize = BUFSIZ;
	if (isatty(fileno(stdin)))
	{
		if (status)
			p("🦠"CYAN"[%s]"RED"[%d]"RST"🦠 > ", Getcwd(cwd, bufsize),status);
		else
			p("🦠"CYAN"[%s]"RST"🦠 > ", 
					Getcwd(cwd, bufsize));
	}

	Getline(&line, &bufsize, stdin);
	return (line);
}
int main(void)
{
    //REPL==read->eval->Print->Loop
    char *line;
    char **args;
    while(line=cell_read_line())
    {  
        //1 get line
        //2 get tokens
       
        args= cell_split_line(line);
         //3 get exec
         cell_exec(args);
         free(line);
         free(args);

    }
    return EXIT_SUCCESS;

}