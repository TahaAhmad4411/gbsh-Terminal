#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>


static char* CD;
extern char** environ;



int countSignals;

pid_t pid;



void Start(){
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf("%s@%s %s > ", getenv("LOG"), hostn, getcwd(CD, 1024));
}


void fileIO(char * string[],int shellno, char* f2, int iter, char* f1){
	 	printf("\n In File IO\n");

	
	

	pid=fork();
	int fd; 
	if(pid==-1){
		printf("Fork Failed\n");
		return;
	}
	if(pid==0){
		
		if (iter == 0){
			fd = open(f2, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			dup2(fd, STDOUT_FILENO); 
			close(fd);
		}
		else if (iter == 1){
			fd = open(f1, O_RDONLY, 0600);  
			dup2(fd, STDIN_FILENO);
			close(fd);
			fd = open(f2, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fd, STDOUT_FILENO);
			close(fd);		 
		}
		 
	
		setenv("parent",getcwd(CD, 1024),1);
		
		if (execvp(string[0],string)==-1){
			printf("Error");
			kill(getpid(),SIGTERM);
		}		 
	}
	waitpid(pid,NULL,0);
}
int changeDirectory(char* args[]){
	
	if (args[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	else{ 
		if (chdir(args[1]) == -1) {
			printf(" %s: no such file or directory \n", args[1]);
            return -1;
		}
	}
	return 0;
}

void HandlingPipes(char * pipeCommands[]){
	// File descriptors
		printf("\nWOhoo ,I m in Pipe Handler\n");

	int fd1[2]; 
	int fd2[2];
	
	
	
	char *command[256];
	
	pid_t pid;
	
	int end = 0;
	
	
	
	int number=0;
	int i = 0;	
	int j = 0;
	int k = 0;
	int l = 0;

	while (pipeCommands[l] != NULL){
		if (pipeCommands[l]!="|"){
			number++;
			break;
		}
		l++;
	}
	number++;
	
	while (pipeCommands[j] != NULL && end != 1){
		k = 0;

		while (strcmp(pipeCommands[j],"|") != 0){
			command[k] = pipeCommands[j];
			j++;	
			if (pipeCommands[j] == NULL){

				end = 1;
				k++;
				break;
			}
			k++;
		}
		command[k] = NULL;
		j++;		
		
		switch(i%2)
		{
			case 0:
				pipe(fd2);
				
			case 1:
				pipe(fd1);
			
		}
		
		pid=fork();
		
		switch(pid)
		{
			case -1:
				if(i!=number-1)
				{
					switch(i%2)
					{
						case 1:
							close(fd1[1]);
						case 0:
							close(fd2[1]); 					
					}
				}
				printf("\nChild Process Failed\n");
				return;	
			case 0:
				if (i == 0){
				dup2(fd2[1], STDOUT_FILENO);
				}
				
				else if (i == number - 1){
					switch(number){
							
							case 0:
								dup2(fd2[0],STDIN_FILENO);
								break;
							default:
								dup2(fd1[0],STDIN_FILENO);						
						}
							}
				
				else{ 
					switch(i){
							case 1:
								dup2(fd2[0],STDIN_FILENO);
						    		dup2(fd1[1],STDOUT_FILENO);					
							case 0:						
								dup2(fd1[0],STDIN_FILENO);						
								dup2(fd2[1],STDOUT_FILENO);	
												
						}					
					
					}
			
			if (execvp(command[0],command)==-1){
				kill(getpid(),SIGTERM);
				}				
		}		

		if (i == 0){
			close(fd2[1]);
			}
		
		else if (i == number - 1){
			if (number % 2 != 0)
			{					
				close(fd1[0]);
			}
			else
			{					
				close(fd2[0]);
			}
					}
		else{
			if (i % 2 != 0){					
				close(fd2[0]);
				close(fd1[1]);
			}
			else{					
				close(fd1[0]);
				close(fd2[1]);
			}
		}
				
		waitpid(pid,NULL,0);
				
		i++;	
	}
}
void ErrorFunc()
{
	printf("Invalid Command");
}

void Execution(char **args, int unpiped){	 
	 
	 	printf("\nExecution Command \n");
	pid=fork();
	 if(pid==-1){
		 printf("\nFork Failed\n");
		 return;
	 }
	//Child Process
	if(pid==0){

		signal(SIGINT, SIG_IGN);
		

		setenv("parent",getcwd(CD, 1024),1);	
		int comm=execvp(args[0],args);
		if (comm==-1){
			ErrorFunc();
			kill(getpid(),SIGTERM);
		}
	 }
	 
	 if (unpiped == 0){
		 waitpid(pid,NULL,0);
	 }
	else{
		 printf("PID : %d\n",pid);
	 }	 
}


//Manage Environment Variables
int ManageEnvironmentVariables(char * args[], int iter){
	char **environment;
		printf("\n\nWOhoo ,I m in Environment Variables\n\n");

	if(iter==0){
		 	printf("\nWOhoo ,I m in Environment Variables option\n");

			for(environment = environ; *environment != 0; environment ++){
				printf("%s\n", *environment);
			}
			
			}
	else if(iter ==1)
	{ 
				printf("\nWOhoo ,I m in Environment Variables iter\n");

			if((args[1] == NULL) && args[2] == NULL){
				printf("%s","Incomplete Arguments\n");
				return -1;
			}
			
			if(getenv(args[1]) != NULL){
				printf("%s", "Overwritten Variable\n");
			}else{
				printf("%s", "Variable Created\n");
			}
			
			if (args[2] == NULL){
				setenv(args[1], "", 1);
			}else{
				setenv(args[1], args[2], 1);
			}
	}
	else if(iter ==2)
	{
			printf("\nWOhoo ,I m in Environment Variable ietr ==2\n");

			if(args[1] == NULL){
				printf("%s","Not enough input arguments\n");
				return -1;
			}
			if(getenv(args[1]) != NULL){
				unsetenv(args[1]);
				printf("%s", "Variable Erased\n");
			}else{
				printf("%s", "The variable does not exist\n");
			}
		
			
			
	}
	return 0;
}
 


			


int CommandHandling(char * command[]){
		printf("\n In Command Handler\n");
	
	int i = 0;
	int j = 0;
	
	int fd;
	int std_out;
	
	int a;
	int unpiped = 0;
	
	char *checker[1024];
	while ( command[i] != NULL){
		if ( (strcmp(command[i],">") == 0) || (strcmp(command[i],"<") == 0) || (strcmp(command[i],"&") == 0)){
			break;
		}
		checker[i] = command[i];
		i++;
	}
	
	
	if(strcmp(command[0],"exit") == 0) exit(0);
	
 	else if (strcmp(command[0],"pwd") == 0){

		if (command[i] != NULL){
			
			if ( (strcmp(command[i],">") == 0) && (command[i+1] != NULL) ){
					
				fd = open(command[i+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				
				std_out = dup(STDOUT_FILENO); 	
				dup2(fd, STDOUT_FILENO); 
				close(fd);
				printf("%s\n", getcwd(CD, 1024));
				dup2(std_out, STDOUT_FILENO);
			}
		}
		else{
			printf("%s\n", getcwd(CD, 1024));
		}
				} 
 	
	else if (strcmp(command[0],"clear") == 0) system("clear");
	
	else if (strcmp(command[0],"cd") == 0) changeDirectory(command);
	
	else if (strcmp(command[0],"environ") == 0)
	{
		if (command[j] != NULL){
		
			if ( (strcmp(command[i],">") == 0) && (command[i+1] != "\0") ){
				
				fd = open(command[i+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				std_out = dup(STDOUT_FILENO); 	
		
				dup2(fd, STDOUT_FILENO); 
				close(fd);
				ManageEnvironmentVariables(command,0);
				dup2(std_out, STDOUT_FILENO);
			}
		}
		
		else{
			ManageEnvironmentVariables(command,0);
		}
	}
	
	else if (strcmp(command[0],"setenv") == 0) ManageEnvironmentVariables(command,1);
	
	else if (strcmp(command[0],"unsetenv") == 0) ManageEnvironmentVariables(command,2);
	else{

		while (command[j] != NULL && unpiped == 0){
			
			if (strcmp(command[j],"&") == 0){
				unpiped = 1;
			}
			if (strcmp(command[j],"|") == 0){
				HandlingPipes(command);
				return 1;
			}
			if (strcmp(command[j],"<") == 0){
				a = j+1;
				if (command[a] == NULL || command[a+1] == NULL || command[a+2] == NULL ){
					
					return -1;
				}else{
					if (strcmp(command[a+1],">") != 0){
	
						return -2;
					}
				}
				fileIO(checker,1,command[j+3],1,command[j+3]);
				return 1;
			}
			if (strcmp(command[j],">") == 0){
				if (command[j+1] == NULL){
					printf("Not enough input arguments\n");
					return -1;
				}
				fileIO(checker,1,command[j+1],1,NULL);
				return 1;
			}
			j++;
		}

		checker[j] = NULL;
		Execution(checker,a);
		
	}
return 12;
}
int main(int argc, char *argv[], char ** envp) {
	char line[1024]; 
	char * command[256];
	int numTokens;
	pid_t shell_id;	
	int shell_available;
	countSignals = 0;
	pid = -10; 
	pid_t pid;

	

    	printf("Environment Variable %s\n",envp[1]);
	environ = envp;
	setenv("shell",getcwd(CD, 1024),1);
	
		for(int i=0;i>-1;i++){
		
		if (countSignals == 0) Start();
		countSignals = 0;

		memset ( line, '\0', 1024 );
		
		fgets(line, 1024, stdin);

		if((command[0] = strtok(line," \n\t")) == NULL) continue;
		
		numTokens = 1;
		while((command[numTokens] = strtok(NULL, " \n\t")) != NULL) numTokens++;
		

		CommandHandling(command);
		
	}          

	exit(0);
}
