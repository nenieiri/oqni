#include <Windows.h>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>

# define BLACK	"\33[1;30m"
# define RED	"\33[1;31m"
# define GREEN	"\33[1;32m"
# define YELLOW	"\33[1;33m"
# define BLUE	"\33[1;34m"
# define PINK	"\33[1;35m"
# define CYAN	"\33[1;36m"
# define WHITE	"\33[1;37m"
# define RESET	"\33[0;m"

bool	isConnected = false;
HANDLE	connectedPort;

void	print_message(std::string msg)
{
	std::cout << msg << std::endl;
}


std::string	random_color(void)
{
	struct timeval	t;
	int				x;

	gettimeofday(&t, NULL);
	srand(t.tv_usec);

	x = rand() % 8;
	switch (x)
	{
		case 0 :
			return (BLACK);
		case 1 :
			return (RED);
		case 2 :
			return (GREEN);
		case 3 :
			return (YELLOW);
		case 4 :
			return (BLUE);
		case 5 :
			return (PINK);
		case 6 :
			return (CYAN);
		case 7 :
			return (WHITE);
	}
	return (NULL);
}

void usleep(__int64 usec) 
{ 
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}

int	SerialBegin(int BaudRate)
{
	DCB SerialParams;
	COMMTIMEOUTS SerialTimeouts;

	CloseHandle(connectedPort);

	connectedPort = CreateFileA(
		"\\\\.\\COM5",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (connectedPort == INVALID_HANDLE_VALUE)
		return -4; //No Port

	SerialParams.DCBlength = sizeof(SerialParams);
	if (!GetCommState(connectedPort, &SerialParams))
		return -3; //GetState error

	SerialParams.BaudRate = BaudRate;
	SerialParams.ByteSize = 8;
	SerialParams.StopBits = ONESTOPBIT;
	SerialParams.Parity = NOPARITY;

	SerialTimeouts.ReadIntervalTimeout = 1;
	SerialTimeouts.ReadTotalTimeoutConstant = 1;
	SerialTimeouts.ReadTotalTimeoutMultiplier = 1;
	SerialTimeouts.WriteTotalTimeoutConstant = 1;
	SerialTimeouts.WriteTotalTimeoutMultiplier = 1;

	if (!SetCommTimeouts(connectedPort, &SerialTimeouts))
		return (-1);
	
	return (0);
}

void	ConnectRequest(void)
{
	int	BaudRate = 9600;

	if (isConnected)
	{
		CloseHandle(connectedPort);
		print_message("Disconnected");
		isConnected = false;
		return;
	}

	switch (SerialBegin(BaudRate))
	{
	case -4:
		print_message("Device Not connected!");
		break;
	case -3:
		print_message("GetState error!");
		break;
	case -2:
		print_message("SetState error!");
		break;
	case -1:
		print_message("SetTimeouts error!");
		break;
	case 0:
		print_message("Connected to: COM5");
		isConnected = true;
		return;
	}
	CloseHandle(connectedPort);
}

void	SerialRead(void)
{
	char					Buffer[256];
	std::string				str_buffer;
	std::string 			color;
	std::string::size_type	ret;

	if (!isConnected)
		return;
	if (!SetCommMask(connectedPort, EV_RXCHAR))
	{
		ConnectRequest();
		return;
	}
	DWORD	BytesIterated;

	while (ReadFile(connectedPort, Buffer, 255, &BytesIterated, NULL))
	{
		str_buffer += Buffer;
		color = random_color();
		ret = str_buffer.find_first_of('\n');
		if (ret == std::string::npos)
			continue ;
		std::cout << color << str_buffer.substr(0, ret) << std::endl;
		str_buffer = str_buffer.substr(ret + 1);
		usleep(500000);
	}
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdsho)
{
	ConnectRequest();
	SerialRead();
	return 0;
}