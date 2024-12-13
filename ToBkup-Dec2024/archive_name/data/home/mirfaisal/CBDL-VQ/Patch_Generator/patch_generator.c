
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

char commit_id[100];
char command_string[100];

int cnt = 0;

int main()
{
	int fd_text;
	
	fd_text = fopen(mir.txt, O_RDWR);

	while(cnt < 1070)
	{
        	fscanf(fd_text, "%s", commit_id);

		sprintf(command_string, "git diff %s > file_%d.patch", commit_id, cnt);
		printf("%s\n", command_string);

		system(command_string);
		cnt++;

	}

	fclose(fd_text);

	return 0;
}

