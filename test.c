#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h> 
#define BUFFER_LENGTH (512)
#define DEVICE_NAME "GPIO_DEVICE"

void handle_sigterm(int sig) 
{ 
	printf("Caught signal %d\n", sig);
	return -1; 
} 


void handle_sigkill(int sig) 
{ 
	printf("Caught signal %d\n", sig);
	exit(EXIT_FAILURE); 
} 

void handle_sigint(int sig) 
{ 
	printf("Caught signal %d\n", sig);
	exit(EXIT_FAILURE); 
} 

int main()
{

	ssize_t read_ret;
	int fd;
	char bufferToSend[BUFFER_LENGTH];
	signal(SIGTERM, handle_sigterm); 
	signal(SIGKILL, handle_sigkill); 
	signal(SIGINT, handle_sigint); 

	printf("Starting device test code example...\n");

	fd = open("/dev/"DEVICE_NAME, O_RDWR | O_APPEND);
	if (fd < 0) {
		perror("Failed to open the device...");
		return errno;
	}

	read_ret = read(fd, bufferToSend, 5);
	printf("Read returned: %d\n",read_ret);

	return close(fd);
}
