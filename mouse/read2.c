#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void print_bin(char c)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		printf("%d", (c >> i) & 0x01);
	}
	printf(" ");
}

int main(int argc, char *argv[])
{
	ssize_t r;
	unsigned char c[5];
	int fd, i, x;
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
		for(i = 0; i < 5; i++)
		{
			if(read(fd, &c[i], 1) != 1)
				return 1;
		}

		print_bin(c[0]);
		print_bin(c[1]);
		print_bin(c[2]);
		print_bin(c[3]);
		print_bin(c[4]);
		//printf("%d\t", (char)c);

		x = ((int) c[2] & 0x0C) << 4 | c[0] & 0x3f;
		printf("\tx=%d", (char)x);
		
		printf("\n");
		fflush(stdout);
	}

	return 0;
}
