/*
THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
	A TUTOR OR CODE WRITTEN BY OTHER STUDENTS OR FOUND ONLINE 
	- SABRINA MARTINEZ
	- SMART57@EMORY.EDU / 928-285-3381
	- 2375865 / SMART57
*/


#include "tokens.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include<sys/wait.h>
#include <fcntl.h>
#include<unistd.h>


/*HELPER STRUCTS*/

//linked list for arguments
typedef struct arguments{
char * arg;//argument string
struct arguments * next;
}Argument;

//linked list for command line
typedef struct command{
struct arguments * arg;//argument linked list, first thing is cmd, last thing is NULL
char * cmd;
char * stdinf;//files
char * stdoutf;
int app;//booleans
int out;
int in;
int argnum;
struct command * next;

}Command;

/*HELPER FUNCTIONS*/

void setNULL(struct command * cmd){//set null so no tricky stuff happens
	cmd->next=NULL;
	cmd->arg=NULL;
	cmd->cmd= NULL;
	cmd->stdinf= NULL;
	cmd->stdoutf= NULL;
	cmd->in= 0;
	cmd->out=0;
	cmd->app=0;
}
void setargNULL(struct arguments * arg){
	arg->arg= NULL;
	arg->next= NULL;
}

void appendArgList(struct arguments * arguments, char * token){//add argument to list
	while(arguments!= NULL){


		if(arguments->arg==NULL){
			arguments->arg= token;
			break;
		}else if(arguments->arg != NULL && arguments->next!= NULL){
			arguments= arguments->next;
		}else{
			arguments->next=malloc(sizeof(struct arguments));
			arguments= arguments->next;
			setargNULL(arguments);
		}
	}
}
//FREEING THINGS
void freeArgList(struct arguments * args){
	Argument * tmp;
	Argument * arguments= args;

	while(arguments!= NULL){
		tmp= arguments;
		arguments= arguments->next;

		free(tmp);
	}
}

void freeCmdList(struct command * cmds){
	Command * tmp;
	Command * commands= cmds;

	while(commands!= NULL){
		freeArgList(commands->arg);

		tmp= commands;
		commands= commands->next;
		free(tmp);
	}
}
//CHECK IF THERE IS INPUT OR OUTPUT FILE
int finalCheck(struct command * commands){


	while(commands!= NULL){
		if(commands->in ==1){

			if(commands->stdinf==NULL){
				fprintf(stderr, "Error: %s\n", "Missing file name for input redirection");
				return 1;

			}

		}
		if(commands->out ==1){
			if(commands->stdoutf==NULL){
				fprintf(stderr, "Error: %s\n", "Missing file name for output redirection");
				return 1;
			}
		}

		if(commands->app ==1){
			if(commands->stdoutf==NULL){
				fprintf(stderr, "Error: %s\n", "Missing file name for output(append) redirection");
				return 1;
			}
		}
		commands= commands->next;
	}
}


void execCmd(Command * cmdCur, int background){//EXECUTE A SINGLE COMMAND
	int fd= -1;//file descriptor

	Argument * argCur;//pointer for list
	pid_t child, parentw;//child, parent, and error checkers
	int statuss=0;
	int dup2checker;
	int premature=0;

	int j=0;

	char * curArgs[cmdCur->argnum+1];//TURN ARGS LIST TO ARRAY
	curArgs[cmdCur->argnum]=NULL;//MAKE LAST ITEM NULL

	argCur=cmdCur->arg;


	//TURNING ARGS LIST TO ARRAY
	while(argCur!=NULL){
		curArgs[j]= argCur->arg;
		j++;
		argCur= argCur->next;

	}

	child=fork();
	if(child == -1){
		perror("fork");
	}

	if(child==0){//FOR THE CHILD

		if(cmdCur->out ==1){//output redirection


			fd= open(cmdCur->stdoutf, O_WRONLY|O_CREAT|O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			if(fd==-1){
				fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdoutf, strerror(errno));
				premature=1;
				exit(-1);
			}

			dup2checker= dup2(fd,1);//make output go to file
			
			if(dup2checker==-1){
				perror("dup2");
			}
		
			close(fd);
		}

		if(cmdCur->in==1){//input redirection
			fd= open(cmdCur->stdinf, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			if(fd==-1){
				fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdinf, strerror(errno));
				premature=1;
				exit(-1);
			}

			dup2checker=dup2(fd,0);

			if(dup2checker==-1){
				perror("dup2");
			}

			close(fd);
		}

		if(cmdCur->app ==1){//output redirection, ver app

			fd= open(cmdCur->stdoutf, O_WRONLY| O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			if(fd==-1){
				fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdoutf, strerror(errno));
				premature=1;
				exit(-1);
			}

			dup2checker= dup2(fd,1);//make output go to file

			if(dup2checker==-1){
				perror("dup2");
			}
			close(fd);
		}

		execvp(cmdCur->cmd, curArgs);
		premature=1;
		perror("execvp");

	}else{//FOR THE PARENT
		if(premature==1){
			return;
		}
		if(background!=1){
			statuss=0;


			while((parentw= wait(&statuss))!=-1);

			if(parentw==-1 && errno==EINTR){
				fprintf(stderr, "Error: %s\n", "Wait() failed, trying again..");
				while((parentw= wait(&statuss))!=-1);
			}
		}

	}
}


void execPipe(Command * cmdCur, int background){//EXEC A SINGLE PIPE(2 commands)
	int fd[2];//files
	int fdr;

	int dup2checker;//checkers/booleans
	int premature=0;

	pid_t child;

	int j=0;
	Argument * argCur;

	if(pipe(fd)==-1){
		perror("pipe");
	}

	if((child= fork())==-1){
		perror("fork");
	}

	char * curArgs[cmdCur->argnum+1];//TURN ARGS LIST TO ARRAY
	curArgs[cmdCur->argnum]=NULL;//MAKE LAST ITEM NULL

	argCur=cmdCur->arg;



	//TURNING ARGS LIST TO ARRAY
	while(argCur!=NULL){
		curArgs[j]= argCur->arg;
		j++;
		argCur= argCur->next;

	}

	if(child==0){//CHILD
		close(fd[0]);

		dup2checker= dup2(fd[1],1);

		if(dup2checker==-1){
			perror("dup2");
		}

		close(fd[1]);


		if(cmdCur->in==1){//input redirection
			fdr= open(cmdCur->stdinf, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			if(fdr==-1){
				fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdinf, strerror(errno));
				premature=1;
				exit(-1);
			}

			dup2checker= dup2(fdr,0);

			if(dup2checker==-1){
				perror("dup2");
			}


			close(fdr);
		}

		execvp(cmdCur->cmd,curArgs);
		premature=1;
		perror("execvp");
	}else{//PARENT

		if(premature==1){
			return;
		}

		//WAITING
		if(background!=1){
			int statuss=0;

			pid_t parentw;
			while((parentw= wait(&statuss))!=-1);

			if(parentw==-1 && errno==EINTR){
				fprintf(stderr, "Error: %s\n", "Wait() failed, trying again..");
				while((parentw= wait(&statuss))!=-1);
			}
		}

		//CHILD2
		cmdCur= cmdCur->next;

		j=0;

		char * curArgs2[cmdCur->argnum+1];//TURN ARGS LIST TO ARRAY
		curArgs2[cmdCur->argnum]=NULL;//MAKE LAST ITEM NULL

		argCur=cmdCur->arg;


		//TURNING ARGS LIST TO ARRAY
		while(argCur!=NULL){
			curArgs2[j]= argCur->arg;
			j++;
			argCur= argCur->next;

		}


		pid_t child2;

		if((child2= fork())==-1){
			perror("fork");
		}

		if(child2==0){//CHILD 2

			close(fd[1]);

			dup2checker= dup2(fd[0],0);

			if(dup2checker==-1){
				perror("dup2");
			}

			close(fd[0]);
			

			if(cmdCur->out ==1){//output redirection

				fdr= open(cmdCur->stdoutf, O_WRONLY|O_CREAT|O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				if(fdr==-1){
					fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdoutf, strerror(errno));
					premature=1;
					exit(-1);
				}

				dup2checker= dup2(fdr,1);//make output go to file

				if(dup2checker==-1){
					perror("dup2");
				}
			
				close(fdr);
			}

			if(cmdCur->app ==1){//output redirection, ver app

				fdr= open(cmdCur->stdoutf, O_WRONLY| O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				if(fdr==-1){
					fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdoutf, strerror(errno));
					premature=1;
					exit(-1);
				}

				dup2checker= dup2(fdr,1);//make output go to file

				if(dup2checker==-1){
					perror("dup2");
				}

				close(fdr);
			}


			execvp(cmdCur->cmd, curArgs2);
			premature=1;
			perror("execvp");

		}else{//PARENT

			if(background!=1){
				int status=0;
				close(fd[0]);
				close(fd[1]);
				pid_t parentx;
				while((parentx= wait(&status))!=-1);

				if(parentx==-1 && errno==EINTR){
					fprintf(stderr, "Error: %s\n", "Wait() failed, trying again..");
					while((parentx= wait(&status))!=-1);
				}
				close(fd[0]);
				close(fd[1]);

			}else{
				close(fd[0]);
				close(fd[1]);

			}
		
		}
		close(fd[0]);
		close(fd[1]);

	}
}


void execMultiPipes(Command * cmdCur, int background, int cmdCount){//MULTIPLES PIPES (PIPES= NUM OF CMDS -1);

	int status=0;
	int j=0;
	int curCmdIndex=0;
	int pipes= cmdCount-1;
	int dup2checker;
	int premature=0;

	int fd; //input or output file
	int fds[pipes *2];// one space for each end of pipe

	pid_t parentw, child;

	Argument * argCur;

	//PIPING
	for(int i=0; i< pipes;i++){
		if(pipe(fds+ (i*2))<0){
			perror("pipe");
		}
	}

	//start process
	while(cmdCur != NULL){//as long as cmd is not empty
		j= 0;

		//TURN ARG LIST INTO ARRAY
		char * curArgs[cmdCur->argnum+1];//TURN ARGS LIST TO ARRAY
		curArgs[cmdCur->argnum]=NULL;//MAKE LAST ITEM NULL

		argCur=cmdCur->arg;

		while(argCur!=NULL){
			curArgs[j]= argCur->arg;
			j++;
			argCur= argCur->next;

		}

		curArgs[cmdCur->argnum]=NULL;//MAKE LAST ITEM NULL

		child=fork();//fork and error check
		if(child == -1){
			perror("fork");
		}

		if(child==0){//CHILD

			if(cmdCur->next!=NULL){//NO NEED TO REDIRECT OUTPUT IF WE'RE AT THE LAST CMD
				dup2checker= dup2(fds[(curCmdIndex * 2) +1],1);//output goes to cmds own output
				if(dup2checker==-1){
					perror("dup2");
				}

			}

			//GET OUTPUT AFTER FIRST CMD AS INPUT
			if(curCmdIndex!=0){
				dup2checker= dup2(fds[(curCmdIndex-1)*2],0);//get input from output of prev cmd
				if(dup2checker==-1){
					perror("dup2");
				}

			}
			
			//NO WORRY ABOUT REDIRECTION SINCE PARSING CATCHES ANY SYNTAX/INPUT MISTAKES
			if(cmdCur->out ==1){//output redirection


				fd= open(cmdCur->stdoutf, O_WRONLY|O_CREAT|O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				if(fd==-1){
					fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdoutf, strerror(errno));
					premature=1;
					exit(-1);
				}

				dup2checker= dup2(fd,1);//make output go to file

				if(dup2checker==-1){
					perror("dup2");
				}
			
				close(fd);
			}

			if(cmdCur->in==1){//input redirection
				fd= open(cmdCur->stdinf, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				if(fd==-1){
					fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdinf, strerror(errno));
					premature=1;
					exit(-1);
				}

				dup2checker= dup2(fd,0);

				if(dup2checker==-1){
					perror("dup2");
				}

				close(fd);
			}

			if(cmdCur->app ==1){//output redirection, ver app

				fd= open(cmdCur->stdoutf, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				if(fd==-1){
					fprintf(stderr, "Error: open(%s): %s\n", cmdCur->stdoutf, strerror(errno));
					premature=1;
					exit(-1);
				}

				dup2checker= dup2(fd,1);//make output go to file

				if(dup2checker==-1){
					perror("dup2");
				}

				close(fd);
			}

			//CLOSE PIPES ONCE WE DON'T NEED THEM
			for(int i=0; i<pipes *2;i++){
				close(fds[i]);
			}

			execvp(cmdCur->cmd, curArgs);
			premature=1;
			perror("execvp");


		}
		
		cmdCur= cmdCur->next;
		curCmdIndex++;

		if(cmdCount==curCmdIndex){
			for(int i=0; i<pipes *2;i++){//CLOSE ALL THE PIPES WHEN NOT IN USE
			close(fds[i]);
			}
			break;
		}
	}

	//PARENT

	if(premature==1){
		return;
	}
	

	if(background!=1){//WAITING



		while((parentw= wait(&status))!=-1);

		if(parentw==-1 && errno==EINTR){
			fprintf(stderr, "Error: %s\n", "Wait() failed, trying again..");
			while((parentw= wait(&status))!=-1);
		}

	}

	for(int i=0; i<pipes *2;i++){//ONE FINAL CLOSE
		close(fds[i]);
	}
}


int main(int argc, char const *argv[])
{
	char input[1024];//command line can only be 1024 or less, buffer
	char **tokens=NULL; //tokens thingy
	Command * cmdCur; //current command, also acts as linked list for commands
	Command * cmdCurHead;



	int premature=0;
	int i;
	int background=0;
	char * prevOp= NULL;
	int isOp=0;
	int cmdCount=0;

	pid_t waiting;
	int status = 0;
	
	/*PARSE*/

	while (1){//loop
		cmdCount=0;

		if(tokens != NULL){
		free_tokens(tokens);
		tokens= NULL;
		}

		premature=0;
		background=0;
		prevOp=NULL;
		isOp=0;

		cmdCurHead=NULL;
		cmdCur= NULL;



		/*CHECK FOR PROMPT AND PRINT OR THROW ERROR ACCORDINGLY*/
		if(argv[1]!=NULL){
			if(argv[2]!=NULL){
				fprintf(stderr, "Error: Usage: %s\n", "mysh [prompt]" );
				break;
			}else{
				printf("\n%s:", argv[1]);
			}

		}else{
			printf("\nmysh: ");
		}

		/*GET COMMAND LINE AND CHECK IF FGETS WAS DONE CORRECTLY OR EXITING*/
		if(fgets(input, sizeof(input), stdin)==NULL){//error or goodbye?
			if(errno == EINTR){//error
				continue;
			}else{//ctrl d goodbye
				printf("\nCTRL+D \nGoodbye! :)\n\n");
				break;
			}

		}else if(strncmp(input, "exit\n", sizeof(input))==0){//exiting
			printf("\nGoodbye! :)\n\n");
			break;
		}

		if(strncmp(input, "\n", sizeof(input))==0){
			continue;
		}


		/*TOKENIZING COMMAND LINE*/
		tokens= get_tokens(input);

		if(tokens==NULL){
			continue;
		}


		/*ITERATING OVER TOKENS*/

		for(i=0; tokens[i];i++){ //iterating over tokens for errors

			//NEW COMMAND
			if(i==0||cmdCurHead==NULL){ 

				cmdCurHead= malloc(sizeof(Command));
				cmdCur= cmdCurHead;
				setNULL(cmdCur);
				cmdCur->argnum=0;
				cmdCount+=1;

			}else if(strncmp(tokens[i], "|", sizeof(tokens[i]))==0){

				cmdCur->next= malloc(sizeof(Command));
				cmdCur= cmdCur->next;
				setNULL(cmdCur);
				cmdCur->argnum=0;
				cmdCount+=1;
			}
			

			//CASE: & IS NOT LAST TOKEN
			if(tokens[i+1]!=NULL && strncmp(tokens[i], "&", sizeof(tokens[i]))==0){
				fprintf(stderr, "Error: \"%s\" must be last token on command line\n", "&");
				premature= 1;
				break;
			}

			if(strncmp(tokens[i], "&", sizeof(tokens[i]))==0){
				continue;
			}
			

			
			if(strncmp(tokens[i], "|", sizeof(tokens[i]))==0 || strncmp(tokens[i], "<", sizeof(tokens[i]))==0 || strncmp(tokens[i], ">", sizeof(tokens[i]))==0 || strncmp(tokens[i], ">>", sizeof(tokens[i]))==0 ){

				//CASE: NULL COMMAND
				if(i==0 && cmdCur->cmd==NULL){
					fprintf(stderr, "Error: %s\n", "Invalid NULL command" );
					premature= 1;
					break;
				}else if(strncmp(tokens[i], ">", sizeof(tokens[i]))!=0 &&strncmp(tokens[i], ">>", sizeof(tokens[i]))!=0 && strncmp(tokens[i], "<", sizeof(tokens[i]))!=0 &&strncmp(tokens[i], "|", sizeof(tokens[i]))!=0 && tokens[i+1]==NULL){
					fprintf(stderr, "Error: %s\n", "Invalid NULL command" );
					premature= 1;
					break;
				}

				//CASE: AMBIGUITY
				if(tokens[i+1]!= NULL){

					if(strncmp(tokens[i+1], "<", sizeof(tokens[i+1]))==0){

						fprintf(stderr, "Error: Ambiguous %s\n", "input redirection");
						premature= 1;
						break;

					}else if(strncmp(tokens[i+1], ">", sizeof(tokens[i+1]))==0 || strncmp(tokens[i+1], ">>", sizeof(tokens[i+1]))==0 ||strncmp(tokens[i+1], "|", sizeof(tokens[i+1]))==0 ){
						fprintf(stderr, "Error: Ambiguous %s\n", "output redirection");
						premature= 1;
						break;

					}

				}

				if(prevOp!=NULL){//rare weird case
					if(strncmp(prevOp, "|", sizeof(prevOp))==0 && strncmp(tokens[i], "<", sizeof(tokens[i]))==0 ){
						fprintf(stderr, "Error: Ambiguous %s\n", "input redirection");
						premature= 1;
						break;
					}
					if((strncmp(prevOp, ">", sizeof(prevOp))==0 || strncmp(prevOp, ">>", sizeof(prevOp))==0)&& strncmp(tokens[i], "|", sizeof(tokens[i]))==0 ){
						fprintf(stderr, "Error: Ambiguous %s\n", "output redirection");
						premature= 1;
						break;
					}
				}


				//set prevop
				if(strncmp(tokens[i], "|", sizeof(tokens[i]))==0 || strncmp(tokens[i], ">", sizeof(tokens[i]))==0 || strncmp(tokens[i], ">>", sizeof(tokens[i]))==0) {
					prevOp= tokens[i];
				}


				if(strncmp(tokens[i], ">", sizeof(tokens[i]))==0){
					cmdCur->out= 1;
				}else if(strncmp(tokens[i], "<", sizeof(tokens[i]))==0){
					cmdCur->in= 1;
				}else if(strncmp(tokens[i], ">>", sizeof(tokens[i]))==0){
					cmdCur->app= 1;
				}




			}else{

				//TO ARG LIST OR TO NOT ARG LIST


				if(cmdCur->cmd==NULL){//command
					cmdCur->cmd= tokens[i];

						if(cmdCur->arg==NULL){
							//MALLOC SPACE FOR ARG
							cmdCur->arg= malloc(sizeof(Argument));

							setargNULL(cmdCur->arg);
							appendArgList(cmdCur->arg, tokens[i]);
							cmdCur->argnum+=1;
							
						}

				}else if(cmdCur->in==1 && cmdCur->stdinf==NULL){//in file, don't put in arg list
					cmdCur->stdinf= tokens[i];

				}else if(cmdCur->out==1 && cmdCur->stdoutf==NULL){//out file, don't put in arg list

					cmdCur->stdoutf= tokens[i];

				}else if(cmdCur->app==1 && cmdCur->stdoutf==NULL){

					cmdCur->stdoutf= tokens[i];

				}else{//other args


					appendArgList(cmdCur->arg, tokens[i]);
					cmdCur->argnum+=1;

				}
			}


		}

		i-=1;

		if(premature!=1){
			premature= finalCheck(cmdCurHead);
		}

		if(premature==1){//if encountered something that shouldn't happen, start while loop again
			freeCmdList(cmdCurHead);
			cmdCurHead= NULL;
			cmdCur= NULL;
			continue;
		}



		/*BACKGROUND CHECK*/
		if(strncmp(tokens[i], "&", sizeof(tokens[i]))==0){
			background= 1; 
		}


		/////////////EXECUTION/////////////////////

		if(cmdCurHead!=NULL){


			cmdCur= cmdCurHead;


			if(cmdCount==2 && cmdCur->next->cmd!=NULL){//ONE PIPE
				execPipe(cmdCur, background);


			}else if (cmdCount >1 && cmdCur->next->cmd!=NULL){//MULTIPLE PIPES
				execMultiPipes(cmdCur, background, cmdCount);

			}else{//NO PIPES
				execCmd(cmdCur, background);
			}

		}

		freeCmdList(cmdCurHead);
		cmdCurHead=NULL;
		cmdCur=NULL;

	}

	
	while((waiting= wait(&status))!=-1);//one final wait to avoid zombies

	if(waiting==-1 && errno==EINTR){
		fprintf(stderr, "Error: %s\n", "Wait() failed, trying again..");
		while((waiting= wait(&status))!=-1);
	}

	return 0;
}