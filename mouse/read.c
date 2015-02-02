#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void print_bin(char c)
{
	int i;
	for(i = 7; i >= 0; i--)
	{
		printf("%d", (c >> i) & 0x01);
	}
	printf(" ");
}

int main(int argc, char *argv[])
{
	ssize_t r;
	unsigned char c[5], tmp;
	int fd, i, x, time, last_time = 0, time_dif, value;
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
		for(i = 0; i < 4; i++)
		{
			if(read(fd, &c[i], 1) != 1)
				return 1;
		}

		if(read(fd, &tmp, 1) != 1) return 1;
		
		if(tmp != 0xff)
		{
			do
			{
				if(read(fd, &tmp, 1) != 1) return 1;
			}
			while(tmp != 0xff);
			continue;
		}


		//printf("%02x %02x %02x %02x %02x\t", c[0], c[1], c[2], c[3], c[4]);

		time = ((int)c[0]) << 8 | ((int)c[1]);
		value = ((int)c[2]) << 8 | ((int)c[3]);

		time_dif = time - last_time;
		if(time_dif < 0) time_dif += (int) 0x10000;
		last_time = time;

		printf("%d\t+%d\t%d", time, time_dif, value);

		
		printf("\n");
		fflush(stdout);
	}

	return 0;
}
