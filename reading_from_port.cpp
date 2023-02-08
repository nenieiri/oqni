#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <map>
#include <vector>

void	fatal(int sockfd)
{
	std::cerr << "Fatal error" << std::endl;
	close(sockfd);
	exit(1);
}

int	main(int argc, char **argv)
{
    int     sockfd = 0;
    fd_set	current, read_set;
    double	msg;
    std::map<int, std::vector<double> > data;

    int					client;
	struct sockaddr_in	clientaddr;
    socklen_t			len = sizeof(clientaddr);

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
		fatal(sockfd);
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
		fatal(sockfd);
	if (listen(sockfd, 100) == -1)
		fatal(sockfd);

    FD_ZERO(&current);
	FD_SET(sockfd, &current);
	bzero(&msg, sizeof(msg));

    while (1)
	{
		read_set = current;
        int max_fd;
        if (data.empty() == false)
            max_fd = (--(data.end()))->first + 1;
        else
            max_fd = sockfd + 1;
		if (select(max_fd, &read_set, NULL, NULL, NULL) == -1)
			continue ;
		for (int fd = 0; fd < max_fd; ++fd)
		{
			if (FD_ISSET(fd, &read_set))
			{
                if (fd == sockfd)
				{
                    client = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
                    if (client == -1)
		                fatal(sockfd);
                    FD_SET(client, &current);
					data.insert(std::make_pair(client, std::vector<double>()));
					break ;
				}
				recv(fd, &msg, sizeof(msg), 0);
                std::cout << "MSG: " << msg << std::endl;
            }
		}
	}
	return (0);
}