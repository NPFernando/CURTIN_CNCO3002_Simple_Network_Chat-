#include	"acc.h"

static void	*doit(void *);		/* each thread executes this function */
static _Atomic unsigned int cli_count = 0;
//static int max_idle_time ; //Maximum time the server waits
static int m ; //Maximum number of Clients

static char connfd_c[10][100], nickname[10][100],realname[10][100],buffer[256];//array creation and buffer for string storing

pthread_mutex_t	counter_mutex = PTHREAD_MUTEX_INITIALIZER;

//----------------------------------------------------------------------------------------
union val {
  int				i_val;
  long				l_val;
  char				c_val[10];
  struct linger		linger_val;
  struct timeval	timeval_val;
} val;

static char	*sock_str_flag(union val *, int);
static char	*sock_str_int(union val *, int);
static char	*sock_str_linger(union val *, int);
static char	*sock_str_timeval(union val *, int);

struct sock_opts {
  char	   *opt_str;
  int		opt_level;
  int		opt_name;
  char   *(*opt_val_str)(union val *, int);
} sock_opts[] = {
/*
//(get sock details - removing comment can run this part)---------------------------------------------------------------------------------------
	"SO_BROADCAST",		SOL_SOCKET,	SO_BROADCAST,	sock_str_flag,
	"SO_DEBUG",			SOL_SOCKET,	SO_DEBUG,		sock_str_flag,
	"SO_DONTROUTE",		SOL_SOCKET,	SO_DONTROUTE,	sock_str_flag,
	"SO_ERROR",			SOL_SOCKET,	SO_ERROR,		sock_str_int,
	"SO_KEEPALIVE",		SOL_SOCKET,	SO_KEEPALIVE,	sock_str_flag,
	"SO_LINGER",		SOL_SOCKET,	SO_LINGER,		sock_str_linger,
	"SO_OOBINLINE",		SOL_SOCKET,	SO_OOBINLINE,	sock_str_flag,
	"SO_RCVBUF",		SOL_SOCKET,	SO_RCVBUF,		sock_str_int,
	"SO_SNDBUF",		SOL_SOCKET,	SO_SNDBUF,		sock_str_int,
	"SO_RCVLOWAT",		SOL_SOCKET,	SO_RCVLOWAT,	sock_str_int,
	"SO_SNDLOWAT",		SOL_SOCKET,	SO_SNDLOWAT,	sock_str_int,
	"SO_RCVTIMEO",		SOL_SOCKET,	SO_RCVTIMEO,	sock_str_timeval,
	"SO_SNDTIMEO",		SOL_SOCKET,	SO_SNDTIMEO,	sock_str_timeval,
	"SO_REUSEADDR",		SOL_SOCKET,	SO_REUSEADDR,	sock_str_flag,
#ifdef	SO_REUSEPORT
	"SO_REUSEPORT",		SOL_SOCKET,	SO_REUSEPORT,	sock_str_flag,
#else
	"SO_REUSEPORT",		0,			0,				NULL,
#endif
	"SO_TYPE",			SOL_SOCKET,	SO_TYPE,		sock_str_int,
#ifdef	SO_USELOOPBACK
	"SO_USELOOPBACK",	SOL_SOCKET,	SO_USELOOPBACK,	sock_str_flag,
#else
	"SO_USELOOPBACK",		0,			0,				NULL,
#endif
	"IP_TOS",			IPPROTO_IP,	IP_TOS,			sock_str_int,
	"IP_TTL",			IPPROTO_IP,	IP_TTL,			sock_str_int,
	"TCP_MAXSEG",		IPPROTO_TCP,TCP_MAXSEG,		sock_str_int,
	"TCP_NODELAY",		IPPROTO_TCP,TCP_NODELAY,	sock_str_flag,
	NULL,				0,			0,				NULL
*/};

// *INDENT-ON* 

int* start_timer(void *secs);

//(messag sending and queue handling )-------------------------------------------------------------------------------------

//Remove clients to queue (function is not used)
void queue_remove(int cli_count,char *nickname){
	pthread_mutex_lock(&counter_mutex);
	//null values that belog to quited client
	for(int i=0; i<m; ++i){
		if(strcasecmp(nickname[i], nickname[cli_count-1])){
			strcpy(nickname[i],NULL);
			strcpy(realname[i],NULL);
			strcpy(connfd_c[i],NULL);
		}		
	}

	pthread_mutex_unlock(&counter_mutex);
}

//Send message to all clients except sender(code or structure error)
void send_message_all(char *nickname, int cli_count, char conn){
	pthread_mutex_lock(&counter_mutex);

	for(int i=0; i<m; ++i){
		if(strcasecmp(nickname[i], nickname[cli_count-1])){
			if(strcmp(connfd_c[i],conn) != 0){
				bzero(buffer,sizeof(buffer));
				sprintf(buffer, "Server : hi 3!\n");
				write(conn, buffer, sizeof(buffer));
				if(write(connfd_c[i], nickname, strlen(nickname)) < 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}		
	}

	pthread_mutex_unlock(&counter_mutex);
}

//Send message to client using nickname(code or structure error)
void send_message_client(char *nickname, int cli_count, char *msg){
	pthread_mutex_lock(&counter_mutex);

	int conn;
	for(int i=0; i<m; ++i){
		//cheak nickname
		if(strcasecmp(nickname[i], nickname)){
			//conn need to replce to nickname client connfd
			if(strcmp(connfd_c[i],conn) == 0){
				bzero(buffer,sizeof(buffer));
				sprintf(buffer, "%s : %s\n",nickname, msg);//message save in buffer
				write(conn, buffer, sizeof(buffer));//write buffer to client
				if(write(connfd_c[i], nickname, strlen(nickname)) < 0)
				{
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}		
	}

	pthread_mutex_unlock(&counter_mutex);
}

//(timeout function-not tested/not working properly)-----------------------------------------------------------------------------------------
static char	strres[128];

static char	*
sock_str_flag(union val *ptr, int len){
/* *INDENT-OFF* */
	if (len != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
	else
		snprintf(strres, sizeof(strres),
				 "%s", (ptr->i_val == 0) ? "off" : "on");
	return(strres);
/* *INDENT-ON* */
}
/* end checkopts3 */

static char	*
sock_str_int(union val *ptr, int len){
	if (len != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
	else
		snprintf(strres, sizeof(strres), "%d", ptr->i_val);
	return(strres);
}

static char	*
sock_str_linger(union val *ptr, int len){
	struct linger	*lptr = &ptr->linger_val;

	if (len != sizeof(struct linger))
		snprintf(strres, sizeof(strres),
				 "size (%d) not sizeof(struct linger)", len);
	else
		snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d",
				 lptr->l_onoff, lptr->l_linger);
	return(strres);
}

static char	*
sock_str_timeval(union val *ptr, int len){
	struct timeval	*tvptr = &ptr->timeval_val;

	if (len != sizeof(struct timeval))
		snprintf(strres, sizeof(strres),
				 "size (%d) not sizeof(struct timeval)", len);
	else
		snprintf(strres, sizeof(strres), "%ld sec, %ld usec",
				 tvptr->tv_sec, tvptr->tv_usec);
	return(strres);
}
//(main code)-------------------------------------------------------------------------------------
int
main(int argc, char **argv){

	int	 listenfd, *iptr;
	pthread_t	tid;
	socklen_t	addrlen, len;
	struct sockaddr_in	 servaddr;
	struct sockaddr	*cliaddr;
	struct sock_opts	*ptr;

	void	sig_chld(int);		

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	/*for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
		printf("%s: ", ptr->opt_str);
		if (ptr->opt_val_str == NULL)
			printf("(undefined)\n");
		else {
			len = sizeof(val);
			if (getsockopt(listenfd, ptr->opt_level, ptr->opt_name,
						   &val, &len) == -1) {
				perror("getsockopt error");
				//err_ret("getsockopt error");
			} else {
				printf("default = %s\n", (*ptr->opt_val_str)(&val, len));
			}
		}
	}*/
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_TCP_PORT);
	
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	//check argument count and assigned argument value to variable
	if (argc == 3){
		m  = atoi(argv[1]);
		max_idle_time = atoi(argv[2]);	
	}else{
		err_quit("usage: server_snc argument error\n");
	}

	//  maximum time the server waits for other clients. This time is started once any Server thread receives an Client connection; valid numbers: 1 to 300 seconds.
	if (1>max_idle_time || max_idle_time>300){
		printf("Maximum idle time server gives for other clients should be between 1 to 300 seconds \n");
		exit(1);
	} 
	//  maximum number of Clients at each session; valid numbers: 1 to 10.
	if (1>m || m>10){
		printf("Maximum number of Clients at each session of the game 1 to 10 \n");
		exit(1);
	} 
	
	printf("\nMaximum idle time, clients: %d \n" , max_idle_time);
	printf("Maximum number of Clients : %d \n" , m);

	printf(" _______________________________________________________________________________\n");
	printf("| \n");
	printf("|                               Welcome to SNC!\n");
	printf("|                                   Server \n");
	printf("|_______________________________________________________________________________\n\n");
	
	while(1) {
	
		len = sizeof (cliaddr);
		iptr = (int *) Malloc(sizeof(int));
		iptr = Accept(listenfd, (SA *) &cliaddr, &len);

		//The sending maximum clients reached warning to the clients
		char maxCli_warn[] = "\nSorry! Maximum client count reached. Try next time\n";

		//handling the number of clients
		if((cli_count ) == m){
			printf("Maximum clients connected. Client %d Rejected \n",cli_count+1);
			send(iptr,&maxCli_warn,strlen(maxCli_warn),0);
			close(iptr);
		}

		Pthread_create(&tid, NULL, &doit, iptr);		
	}
}
//(doit function)--------------------------------------------------------------------------------------------	
static void *
doit(void *arg){

	int	connfd,connfdc;
	ssize_t	n;
	connfd = (int *) arg;	
	char line[MAXLINE],buffer[256];
	char arg1[20],arg2[20],arg3[20], conn[10];
	bool flag = false;

	pthread_mutex_lock(&counter_mutex);

	cli_count++;//new client

	sprintf(conn, "%d", (int *)arg);
	printf("Client : new connfd -");	
	strcpy(connfd_c[cli_count-1], conn);//add new client connfd to array
	printf("%s\n",connfd_c[cli_count-1]);//print new client connfd

	pthread_mutex_unlock(&counter_mutex);

	//Pthread_detach(pthread_self());
		
	while(1) {

		time_t	ticks;
		int yes = 1;
		
		if ( (n = Readline(connfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */
			
		pthread_mutex_lock(&counter_mutex);	
		sscanf(line,"%s %s %s",&arg1,&arg2,&arg3);//divide bufeer strig value from space
		pthread_mutex_unlock(&counter_mutex);
		
		//join new client to server(working some functions)
		if (strcasecmp(arg1, "JOIN") == 0 && flag == false){ 	
			pthread_mutex_lock(&counter_mutex);//mutex lock for data scrambaling 

			//check simmilar realname
			for(int z=0; z<m; z++){
				if(strcasecmp(arg3, realname[z]) == 0){					
					bzero(buffer,sizeof(buffer));//empty buffer for new values
					sprintf(buffer, "Server : realname is already taken, try again!\n");
					Writen(connfd, buffer, sizeof(buffer));
					break;
				}
			}
			
			strcpy(nickname[cli_count-1], arg2);//add nickname to array
			strcpy(realname[cli_count-1], arg3);//add realname to array
			flag = true;//stop join create multiple nicknames in same client

			//welcome message
			bzero(buffer,sizeof(buffer));
			sprintf(buffer, "Server : %s, welcome to SNC!\n", nickname[cli_count-1]);//welcome message to client
			write(connfd, buffer, sizeof(buffer));	

			//send wilcome message to other clients
			//send_message_all(nickname[cli_count-1], cli_count, conn);

			pthread_mutex_unlock(&counter_mutex);
		}
		//check clent real name using nickname(working)
		else if (strncasecmp(arg1, "WHOIS",5) == 0){
			pthread_mutex_lock(&counter_mutex);

			int z=0;
			while(1){				
				if(z < m){
					//check nick name is in chatroom
					if(strcasecmp(arg2, nickname[z]) != 0){					
						bzero(buffer,sizeof(buffer));//empty buffer for new values
						sprintf(buffer, "Server : nickname is not in Simple Network Chat (SNC)!\n");
						Writen(connfd, buffer, sizeof(buffer));
						break;
					}else if(strcasecmp(nickname[z], arg2) == 0){
						bzero(buffer,sizeof(buffer));
						sprintf(buffer, "Server : %s %s\n", nickname[z],realname[z]);
						write(connfd, buffer, sizeof(buffer));
						break;
					}
				}
				z++;			
			}

			pthread_mutex_unlock(&counter_mutex);	
		}
		//check time and date(working)
		else if (strncasecmp(arg1, "TIME", 4) == 0){
			pthread_mutex_lock(&counter_mutex);			

			time(&ticks);
			bzero(buffer,sizeof(buffer));
			snprintf(buffer, sizeof(buffer), "Server : %.24s\r\n", ctime(&ticks));
			write(connfd, buffer, sizeof(buffer));

			pthread_mutex_unlock(&counter_mutex);

		} 
		//quit from client(woking)		
		else if (strncasecmp(arg1, "QUIT",4) == 0){
			printf("test quit 1");
			pthread_mutex_lock(&counter_mutex);
			printf("test quit");

			bzero(buffer,sizeof(buffer));
			printf("Client is quiting...\n");
			sprintf(buffer, "Server : Bye %s\n", nickname[cli_count-1]);
			write(connfd, buffer, sizeof(buffer));
			
			pthread_mutex_unlock(&counter_mutex);
		}
		//quit from server and client(working)		
		else if (strncasecmp(arg1, "QUITSER",7) == 0){
			pthread_mutex_lock(&counter_mutex);

			bzero(buffer,sizeof(buffer));
			printf("server and client are quiting...\n");
			sprintf(buffer, "Server : Server is terminated\n");
			write(connfd, buffer, sizeof(buffer));
			sleep(1);
			close(connfd);
			exit(1);
			pthread_mutex_unlock(&counter_mutex);

		} 
		//reset idle time value in client
		else if(strncasecmp(arg1, "ALIVE",5) == 0){
			pthread_mutex_lock(&counter_mutex);
			pthread_mutex_unlock(&counter_mutex);
		}
		//send message to another user using nickname
		else if(strncasecmp(arg1, "MSG",3) == 0){
			pthread_mutex_lock(&counter_mutex);

			int w=0;
			while(1){				
				if(w < m){
					//check nick name is in chatroom
					if(strcasecmp(arg2, nickname[w]) != 0){					
						bzero(buffer,sizeof(buffer));//empty buffer for new values
						sprintf(buffer, "Server : nickname is not in Simple Network Chat (SNC)!\n");
						Writen(connfd, buffer, sizeof(buffer));
						break;
					}else if(strcasecmp(nickname[w], arg2) == 0){
						send_message_client(nickname[w],cli_count,arg3);//call function to send message
						//bzero(buffer,sizeof(buffer));
						//sprintf(buffer, "%s : %s\n", nickname[w], arg3);//output nickname : message
						break;
					}
				}
				w++;			
			}			

			pthread_mutex_unlock(&counter_mutex);
		}
		//SNC command error
		else{
			if(flag == true){
				bzero(buffer,sizeof(buffer));
				sprintf(buffer, "Server : cannot create multiple nickname in one client program!\n");
				write(connfd, buffer, sizeof(buffer));
			}else{
				bzero(buffer,sizeof(buffer));
				sprintf(buffer, "Server : SNC command is wrong, try again!\n");
				write(connfd, buffer, sizeof(buffer));
			}
		}

		Writen(connfd, line, n);
		
		/* Reduce CPU usage */
		sleep(1);
	}
	
	Close(connfd);			/* we are done with connected socket */
	return(NULL);
}
