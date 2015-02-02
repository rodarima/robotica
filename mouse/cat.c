#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void print_bin(unsigned char c)
{
	int i;
	printf("%02X ", c);
	for(i = 0; i < 7; i++)
	{
		printf("%d", (c >> i) & 0x01);
	}
	printf(" ");
}

int main(int argc, char *argv[])
{
	ssize_t r;
	unsigned char c[5], tmp;
	int fd, i, x = 0, y = 0;
	int count = 0;

	if(argv[1] == NULL) return 1;

	fd = open(argv[1], O_RDWR);

	if(fd < 0)
	{
		perror("open");
		return 1;
	}
	

	while(1)
	{
		for(i = 0; i < 3; i++)
		{
			if(read(fd, &c[i], 1) != 1)
				return 1;
			//print_bin(c[i]);
		}

		x +=   (char) (((c[0] & 0x03) << 6) | (c[1] & 0x7f));
		y += - (char) (((c[0] & 0x0C) << 4) | (c[2] & 0x7f));

		printf("x=%d y=%d\n", x, y);
		fflush(stdout);
	}

	return 0;
}
