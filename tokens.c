#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define p(...) printf(__VA_ARGS__)
#define DEL " "
void cell_spilt_line(char *line)
{
    char *tokens[6]={0};
    int position=0;
    for(char *token=strtok(line,DEL); token!=NULL; token=strtok(NULL,DEL))
    {  
        tokens[position++]=token;
    }
    position=0;
    while(tokens[position]!=NULL)
    {
        p("%s\n", tokens[position++]);
    }
}
int main(void)
{
    char s[]="ls -l /home/user";
    cell_spilt_line(s);
    return EXIT_SUCCESS;
}