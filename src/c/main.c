/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecorte42 <jdecorte42@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:38:28 by mbucci            #+#    #+#             */
/*   Updated: 2023/11/29 14:27:23 by jdecorte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_generic.h"

#ifdef __linux__

#include "woody_elf.h"
static int check_arg(const char *filename, const char *key)
{
	t_woody context = {filename, (key != NULL), NULL, NULL, 0};

	if (key && !(context.key = ft_strdup(key)))
		return write_error(KEYGEN_ERR, NULL);

	int err = 1;

	if (!(context.base = read_file(filename, &context.size)))
		return err;

	// check architecture
	Elf64_Ehdr *elf = (Elf64_Ehdr *)context.base;
	const unsigned char *f_ident = elf->e_ident;
	if (f_ident[EI_MAG0] != ELFMAG0
			|| f_ident[EI_MAG1] != ELFMAG1
			|| f_ident[EI_MAG2] != ELFMAG2
			|| f_ident[EI_MAG3] != ELFMAG3
	   )
		write_error(filename, ELF_ERR);

	// check file class
	else if (f_ident[EI_CLASS] != ELFCLASS64 && f_ident[EI_CLASS] != ELFCLASS32)
		write_error(filename, FORMAT_ERR);

	// check file type
	else if (elf->e_type != ET_EXEC && elf->e_type != ET_DYN)
		write_error(filename, ELFEXEC_ERR);

	else // do the thing
	{
		if (f_ident[EI_CLASS] == ELFCLASS64)
			err = woody_elf64(&context);
		else if (f_ident[EI_CLASS] == ELFCLASS32)
			err = woody_elf32(&context);
	}

	free(context.base);

	return err;
}

#elif __APPLE__

#include <sys/stat.h>
#include <copyfile.h>
#include "woody_macho.h"
static int check_arg(const char *filename, const char *key)
{
	if (key)
	{
		printf("Macho don't handle custom key\n");
		return 1;
	}

	t_woody context = {0, NULL};
	int err = 1;

	struct stat s;
	if (stat(filename, &s) == -1)
	{
		perror("stat");
		goto cleanup;
	}
	if(!(s.st_mode & S_IXUSR))
	{
		write_error(NULL, "No an executable");
		goto cleanup;
	}
	context.size = s.st_size;

	if (copyfile(filename, PATCH, NULL, COPYFILE_DATA | COPYFILE_UNLINK) == -1)
	{
		perror("copyfile");
		goto cleanup;
	}

	context.f = fopen(PATCH, "r+");
	if (!woody_macho(&context))
		err = 0;

cleanup:
	if (context.f)
		fclose(context.f);
	return err;
}
#endif

int main(int ac, char **av)
{
	if (ac != 2 && ac != 3)
		return write_error(NULL, USAGE_ERR);

	return check_arg(av[1], ( (ac == 3 && ft_strlen(av[2])) ? av[2] : NULL) );
}
