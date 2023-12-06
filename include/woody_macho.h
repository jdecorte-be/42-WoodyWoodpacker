#pragma once

/* INCLUDES */
#include <stdio.h>
#include <mach-o/loader.h>
#include <mach-o/fat.h>
#include <sys/param.h>

/* t_woody */
typedef struct s_woody
{
	off_t	size;
	FILE *f;
}	t_woody;

/* defines */
#define ROUND_UP(x, y) (((x) + (y) - 1) & -(y))
#define BUFSIZE 512

#define MACHO_ERR "not an macho file"
#define PARASITE_MACHO_PATH "./payloads/parasite.dynl"

/* utils.c */
void fbzero(FILE *f, off_t offset, size_t len);
void fmemmove(FILE *f, off_t dst, off_t src, size_t len);
size_t fpeek(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
void *read_load_command(FILE *f, uint32_t cmdsize);
int woody_macho(t_woody *context);
