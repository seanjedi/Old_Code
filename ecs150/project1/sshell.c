#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

//input struct
struct argLine {
	char *args[15];
	int direction;
	int status;
	char *path;
	char *file;
	char *command;

};


struct fullCommand {
	struct argLine commands[20];
	char buf[20][512];
	int cmdnumber;
	int status;
};



//Function to parse input
struct argLine parseLine(char *in_arg)
{
	struct argLine out_args;
	out_args.status = 0;
	char *temp_arg = in_arg;
	//determine if it is a file redirection or a pipe, if it is, then parse it differently
	//Input
	if(strchr(temp_arg, '<') != NULL)
	{
		char *token = strtok(temp_arg, "<"); //find the arguments before the input redirection
		char *argument = token;
		//Find the file to have input from
		token = strtok(NULL, " ");
		out_args.file = token;
		//Parse the lines before the input redirection
		char *temp = strtok(argument, " "); 
		out_args.args[0] = temp;
		int i = 1;
		while(temp != NULL){
			temp = strtok(NULL, " ");
			out_args.args[i] = temp;
			i++;
		}
		out_args.direction = 1;
		return out_args;
	}

	//Ouput
	if(strchr(temp_arg,'>') != NULL)
	{
		char *token = strtok(temp_arg, ">"); //find the arguments before the input redirection
		char *argument = token;
		//Find the file to output
		token = strtok(NULL, " ");
		out_args.file = token;
		//Parse the lines before the input redirection
		char *temp = strtok(argument, " "); 
		out_args.args[0] = temp;
		int i = 1;
		while(temp != NULL){
			temp = strtok(NULL, " ");
			out_args.args[i] = temp;
			i++;
		}
		out_args.direction = 2;
		return out_args;
	}

	//piplining
	if(strchr(temp_arg,'|') != NULL)
	{
		out_args.direction = 3;
		return out_args;
	}

	if(strchr(temp_arg,'&'))
	{
		out_args.direction = 4;
		return out_args;
	}

	//normal parsing
	
		//check if it is empty
		
		//get the first
		char *token = strtok(temp_arg, " ");
		if(token == NULL)
		{
			out_args.status = 1;
			return out_args;
		}

		out_args.args[0] = token;
		
		//then walk through other tokens, learned from https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
		int i = 1;
		while(token != NULL) {
			token = strtok(NULL, " ");
			out_args.args[i] = token;
			i++;
		}
		out_args.direction = 0;
		return out_args;
}

struct fullCommand parsePipe(char *in_arg)
{
	struct argLine cmd1, cmd2;
	struct fullCommand cmdlist;
	char *temp_arg = in_arg;
	char *buf1, *buf2;
	int i, ex;
	i = 0;
	cmd2.direction = 3;
	ex =1;
	//if there is more pipes, do this again, and rewrite some values
	while(ex == 1) 
	{

		ex = 0;
		char *token = strtok(temp_arg, "|"); //find the command before the pipe
		char *argument = token;
		token = strtok(NULL,"");
		//Find the command input

		buf1 = argument;
		

		cmd1 = parseLine(argument);
		
		if(cmd1.status == 1)
		{
			printf("Error: invalid command line\n");
			cmdlist.status = -1;
		}		

		printf("cmd1: %s\n" ,argument);
		strcpy(cmdlist.buf[i], buf1);
		cmdlist.commands[i] = cmd1;
		i++;
		cmdlist.cmdnumber++;

		if(strchr(token,'|') != NULL)
		{
			ex = 1;
			continue;
		}
		buf2 = token;
		printf("got here2 %s\n", buf2);
		cmd2 = parseLine(token);
		printf("cmd2: %s\n", token);

		//if commands empty, send back an error
		if(cmd2.status == 1)
		{
			printf("Error: invalid command line\n");
			cmdlist.status = -1;
		}		

		strcpy(cmdlist.buf[i], buf2);
		cmdlist.commands[i] = cmd2;
		
	}
	return cmdlist;
}



int main(int argc, char *argv[])
{
	struct argLine a_Line;
	struct fullCommand cmdlist;

	char buf[512], run[512];
	int status;
	pid_t pid;
	while(1){
		printf("sshell$ ");
		fgets(buf, sizeof(buf), stdin);
		if(strcmp(buf, "\n") == 0)
		{
			continue;
		}

		buf[strcspn(buf,"\n")] = 0;

		strcpy(run, buf);
		a_Line = parseLine(buf);
		//Handle exit, pwd, and cd
		if((strcmp(a_Line.args[0], "exit")) == 0)
		{
			printf("Bye...\n");
			exit(0);
		}
		else if((strcmp(a_Line.args[0], "pwd")) == 0)
		{	
			char wd[100];
			getcwd(wd, sizeof(wd));
			printf("%s\n", wd);
			fprintf(stderr,"+ completed '%s'[0]\n", buf);
		}
		else if(strcmp(a_Line.args[0], "cd") == 0)
		{
			int ret;
			ret = chdir(a_Line.args[1]);
			if(ret != 0){
			printf("Error: no such directory\n");	
			}
			fprintf(stderr,"+ completed '%s'[%d]\n", buf, ret);
		}
		else{
			pid = fork();
			if(pid == 0){
				if(a_Line.status == 1)
				{
					exit(-2);
				}
				//include redirection in child, so that when the child dies, 
				//the parent won't read from the wrong location
				// if there is a redirection, change the output
				//INPUT REDIRECTION
				if(a_Line.direction == 1) 
				{
					//If there is no file specified
					if(a_Line.file == NULL)
					{
						printf("Error: no input file\n");
						exit(-2);
					}
					int fd;
					fd = open(a_Line.file, O_RDWR);
					//if open fails
					if(fd == -1)
					{
						printf("Error: cannot open input file\n");
						exit(-2);
					}

					dup2(fd, STDIN_FILENO);
					close(fd);
				}

				//OUTPUT REDIRECTION
				if(a_Line.direction == 2)
				{
					//If there is no file specified
					if(a_Line.file == NULL)
					{
						printf("Error: no output file\n");
						exit(-2);
					}
					int fd;
					fd = open(a_Line.file, O_CREAT | O_RDWR, 0644);
					//if open fails
					if(fd == -1)
					{
							printf("Error: cannot open output file\n");
							exit(-2);
					}

					dup2(fd, STDOUT_FILENO);
					close(fd);
				}

				//PIPLINING
				if(a_Line.direction == 3)
				{
					cmdlist = parsePipe(buf);
					int cmdID = cmdlist.cmdnumber;
					int i = 0;
					int fd[2];
					fprintf(stderr,"+ completed '%s\n", run);
					//fork to be able to handle all the processes without dying, wait until child is done
					pid = fork();
					if(pid == 0)
					{
						while(i < cmdID)//deal with all the pipes
						{
							pid = fork();

							pipe(fd);
							//finish this command first
							if(pid == 0)
							{
								close(fd[0]);
								dup2(fd[1], STDOUT_FILENO);
								close(fd[1]);
								execvp(cmdlist.buf[i],cmdlist.commands[i].args);
							}
							//the one getting piped
							if(pid > 0)
							{
								//wait for child to execute
								waitpid(-1,&status, 0);
								fprintf(stderr, "[%d]", WEXITSTATUS(status));
								close(fd[1]);
								close(STDIN_FILENO);
								dup(fd[0]);
								close(fd[0]);
								execvp(cmdlist.buf[i],cmdlist.commands[i].args);

							}			
						}
					}
					else if(pid > 0)
					{
						waitpid(-1, &status, 0);
						fprintf(stderr, "[%d]\n", WEXITSTATUS(status));
						exit(-2);
					}
				}
			
				if(a_Line.direction == 4)
				{
					
				}




					//normal execution
					execvp(buf, a_Line.args); //child should die here
					printf("error: command not found\n");
					exit(1);
			} else if (pid > 0) {
				//parent
				waitpid(-1, &status, 0); //wait for child to die
				// if a file redirection fails, we need to not print out the exit status, so this is my workaround to that! 
				if(WEXITSTATUS(status) == 0) 
				{
					fprintf(stderr,"+ completed '%s'[%d]\n", run, WEXITSTATUS(status));
				}
			} else {
				perror("fork");
				exit(1);
			} 
		}
	}
	return 0;
}
