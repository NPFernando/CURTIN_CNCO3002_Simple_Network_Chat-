#include	"acc.h"

void	*copyto(void *);

static int	sockfd;		/* global for both threads to access */
static FILE	*fp;

int
main(int argc, char **argv){

	int			sockfd,res,arg;
	struct sockaddr_in	servaddr;
	fd_set myset;
	struct timeval tv;
	int valopt;
	socklen_t lon;

	char			*ptr, **pptr;
	char			str[INET_ADDRSTRLEN];
	struct hostent	*hptr;

	//check argument are correctly entered by user
	if (argc != 3)
		err_quit("usage: tcpcli <IPaddress> <port number>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	/*
	//set non-blocking
	if((arg = fcntl(sockfd, F_GETFL, NULL)) < 0){
		fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
	}
	arg |= SOCK_NONBLOCK;
	if(fcntl(sockfd, F_SETFL, arg) < 0){
		fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
		exit(0);
	}
	*/

	if ( (hptr = gethostbyname2(*++argv, AF_INET)) == NULL) {
			err_msg("gethostbyname error for host: %s: %s",*++argv, hstrerror(h_errno));			
	}

	printf(" _______________________________________________________________________________\n");
	printf("|                \n");
	printf("|official hostname: %s\n", hptr->h_name);
	printf("|address type: %d\n", hptr->h_addrtype);
	printf("|address length: %d\n", hptr->h_length);
	printf("|_______________________________________________________________________________\n");
	
	res = Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));//connecting with server

//(not working - idle_time handling another methode but running errors)----------------------------------------------------------------------------------------	
/*
	if(res < 0){
		if(errno == EINPROGRESS){
			fprintf(stderr, "EINPROGRESS in connect() - selecting\n");
			do{
				tv.tv_sec = max_idle_time;
				tv.tv_usec = 0;
				FD_ZERO(&myset);
				FD_SET(sockfd,&myset);
				res = select(sockfd+1, NULL, &myset, NULL, &tv);
				if(res < 0 && errno != EINTR){
					fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
					exit(0);
				}else if(res > 0){
					//socket selected for write
					lon = sizeof(int);
					if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0){
						fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno));
						exit(0);
					}
					//check the value returned
					if(valopt){
						fprintf(stderr, "Error in delayed connection() %d -%s\n",valopt,strerror(valopt));
						exit(0);
					}
					break;
				}else {
					fprintf(stderr, "Timeout in select() - Cancelling!\n");
					exit(0);
				}
			}while(1);
		}else{
			fprintf(stderr, "Error connecting %d - %s\n", errno,strerror(errno));
			exit(0);
		}
	}
	//set to blocking mode again
	if((arg = fcntl(sockfd, F_GETFL,NULL)) < 0){
		fprintf(stderr,"Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
		exit(0);
	}
	arg &= (~SOCK_NONBLOCK);
	if(fcntl(sockfd, F_SETFL,arg) < 0){
		fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
		exit(0);
	}
*/
//(welcome message and str_cli() calling)--------------------------------------------------------------------------------------------

	printf(" _______________________________________________________________________________\n");
	printf("|                       \n");
	printf("|                               Welcome to SNC!  \n");
	printf("|                                   Client  \n");
	printf("|_______________________________________________________________________________\n\n");

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
