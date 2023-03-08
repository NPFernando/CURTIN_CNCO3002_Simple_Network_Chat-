#include	"acc.h"

void	*copyto(void *);

static int	sockfd;		/* global for both threads to access */
static FILE	*fp;

pthread_mutex_t   counter_mutex = PTHREAD_MUTEX_INITIALIZER;

//stdout print Student ID 20897525 (working)
void overwrite_stdout() {
    printf("\r%s", "20897525 > ");
    fflush(stdout);
}

void
str_cli(FILE *fp_arg, int sockfd_arg)
{
	char		recvline[MAXLINE];
	pthread_t	tid;

	sockfd = sockfd_arg;	/* copy arguments to externals */
	fp = fp_arg;

	printf("20897525 > ");//fist Id output(1 time)
		
	/* does no pass argument */
	Pthread_create(&tid, NULL, copyto, NULL);
	//Pthread_create(&tid, NULL, idle_time, NULL);

	while (Readline(sockfd, recvline, MAXLINE) > 0){
		pthread_mutex_lock(&counter_mutex);

		Fputs(recvline, stdout);			
        overwrite_stdout();//print ID>(working)	

        pthread_mutex_unlock(&counter_mutex);
	}
}

void *
copyto(void *arg)
{
	char	sendline[MAXLINE];

	while ((void *) Fgets(sendline, MAXLINE, fp) != NULL){	
		pthread_mutex_lock(&counter_mutex);

		//to quit from client(working)
		if (strncasecmp(sendline, "QUIT",4) == 0){
			sleep(1);//sleep process for run other code without sudden forcefull termination 			
			close(sockfd);				
			exit(1);
		}

		Writen(sockfd, sendline, strlen(sendline));			

		pthread_mutex_unlock(&counter_mutex);
	}

	Shutdown(sockfd, SHUT_WR);	/* EOF on stdin, send FIN */

	return(NULL);
	/* 4return (i.e., thread terminates) when end-of-file on stdin */
}

