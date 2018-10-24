#include <stdio.h>

#define BUFFER_LENGTH (512)

int main()
{

    ssize_t read_ret;
    int fd;
    char bufferToSend[BUFFER_LENGTH];
    printf("Starting device test code example...\n");

    fd = open("/dev/test_dev", O_RDWR | O_APPEND);
    if (fd < 0) {
	perror("Failed to open the device...");
	return errno;
    }
    read(fd, bufferToSend, 5);

}
