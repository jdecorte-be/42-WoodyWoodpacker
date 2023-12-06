/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <mbucci@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:38:36 by mbucci            #+#    #+#             */
/*   Updated: 2023/11/20 17:05:18 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

int write_error(const char *filename, const char *custmsg)
{
	fprintf(stderr, "Error: ");
	if (filename)
		fprintf(stderr, "%s: ", filename);
	if (custmsg)
		fprintf(stderr, "%s", custmsg);
	else
	{
		const char *errmsg = strerror(errno);
		fprintf(stderr, "%s", errmsg);
	}
	fprintf(stderr, "\n");

	return 1;
}

#ifdef __linux__

#include <fcntl.h>
#include <unistd.h>

void *read_file(const char *path, uint64_t *fsize)
{
	int fd = -1;
	if ((fd = open(path, O_RDONLY)) == -1)
	{
		write_error(path, NULL);
		return NULL;
	}

	int64_t bytes;
	bytes = lseek(fd, 0, SEEK_END);
	if (bytes == -1)
	{
		write_error(path, NULL);
		close(fd);
		return NULL;
	}

	*fsize = bytes;
	void *file = malloc(sizeof(char) * (*fsize));
	if (file)
	{
		lseek(fd, 0, SEEK_SET);
		bytes = read(fd, file, *fsize);
		if (bytes == -1)
		{
			write_error(path, NULL);
			free(file);
			file = NULL;
		}
	}
	else
		write_error(NULL, NULL);

	close(fd);

	return file;
}

char *generate_key(const char *random, uint32_t keysz)
{
	int fd = open(random, O_RDONLY);
	if (fd == -1)
	{
		write_error(random, NULL);
		return NULL;
	}

	char *ret = (char *)malloc(sizeof(char) * keysz);

	if (ret)
	{
		if (read(fd, ret, keysz) == -1)
		{}
	}
	else
		write_error(NULL, NULL);

	return ret;
}

void print_key(const char *key, uint32_t keysz)
{
	for (uint32_t i = 0; i < keysz; ++i)
		dprintf(STDOUT_FILENO, "%02hhX", (int)key[i]);

	dprintf(STDOUT_FILENO, "\n");
}

void patch_payload_addr32(char *bytes, uint64_t size, int32_t target, int32_t repl)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		int32_t chunk = *(int32_t *)(bytes + i);
		if (!(chunk ^ target))
		{
			*(int32_t *)(bytes + i) = repl;
			return;
		}
	}
}

void patch_payload_addr64(char *bytes, uint64_t size, int64_t target, int64_t repl)
{
	for (uint64_t i = 0; i < size; ++i)
	{
		int64_t chunk = *(int64_t *)(bytes + i);
		if (!(chunk ^ target))
		{
			*(int64_t *)(bytes + i) = repl;
			return;
		}
	}
}

void free_payloads(char *k, void *p1, void *p2, void *p3)
{
	free(k);
	free(p1);
	free(p2);
	free(p3);
}

#elif __APPLE__

size_t fpeek(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream)
{
	off_t pos = ftello(stream);
	size_t result = fread(ptr, size, nitems, stream);
	fseeko(stream, pos, SEEK_SET);
	return result;
}

void *read_load_command(FILE *f, uint32_t cmdsize)
{
	void *lc = malloc(cmdsize);
	if(!lc){
		fclose(f);
		exit(-1);
	}
	fpeek(lc, cmdsize, 1, f);
	return lc;
}
#endif
