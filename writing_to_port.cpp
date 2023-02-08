#include <iostream>
#include <random>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
    double				lower_bound = 0;
    double				upper_bound = 10000;
    struct timeval		t;
    int					fd;
	int					sockfd;

    if (argc != 2)
	{
		std::cerr << "Wrong number of arguments" << std::endl;
		exit(1);
	}

    struct sockaddr_in	servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
    {
        std::cout << "Fatal error" << std::endl;
        return (0);
    }
    fd = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (fd == -1)
    {
        std::cout << "Fatal error" << std::endl;
        return (0);
    }

    gettimeofday(&t, NULL);
    srand(t.tv_usec);
    for (int i = 0; i < 10; ++i)
	{
       double a = (((double)rand()) / ((double)RAND_MAX)) * (upper_bound - lower_bound) + lower_bound;
	   send(sockfd, &a, sizeof(a), 0);
	   std::cout << a << std::endl;
	}

	while (2)
		;
    return (0);
}