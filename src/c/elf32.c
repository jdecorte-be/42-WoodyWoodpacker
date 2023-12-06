/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf32.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecorte42 <jdecorte42@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:38:12 by mbucci            #+#    #+#             */
/*   Updated: 2023/11/20 16:30:04 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include "woody_generic.h"
#include "woody_elf.h"

/* Import ASM Function */
extern void _rc4_elf64(void *bytes, uint64_t length, const char *key, uint32_t keysize);

/* Global Variables */
static Elf32_Addr g_codeseg_addr = 0;
static Elf32_Addr g_handler_addr = 0;
static Elf32_Addr g_parasite_addr = 0;
static Elf32_Addr g_decryptor_addr = 0;

static Elf32_Off g_codeseg_off = 0;
static Elf32_Off g_handler_off = 0;
static Elf32_Off g_parasite_off = 0;
static Elf32_Off g_decryptor_off = 0;

static uint64_t g_codeseg_size = 0;
static uint64_t g_handler_size = 0;
static uint64_t g_parasite_size = 0;
static uint64_t g_decryptor_size = 0;
static uint64_t g_total_payload_size = 0;

/* CODE */
static int check_boundaries(const t_woody *context)
{
	Elf32_Ehdr *elf = (Elf32_Ehdr *)context->base;

	if ((elf->e_shoff + (sizeof(Elf32_Shdr) * elf->e_shnum) > context->size)
		|| (elf->e_phoff + (sizeof(Elf32_Phdr) * elf->e_phnum) > context->size)
		|| ((elf->e_phentsize * elf->e_phnum) > (sizeof(Elf32_Phdr) * elf->e_phnum))
		|| ((elf->e_shentsize * elf->e_shnum) > (sizeof(Elf32_Shdr) * elf->e_shnum))
		|| (elf->e_ehsize > sizeof(Elf32_Ehdr))
	)
		return 1;

	return 0;
}

static int check_exec(const Elf32_Ehdr *elf)
{
	Elf32_Phdr *ph_tab = (Elf32_Phdr *)((void *)elf + elf->e_phoff);
	for (uint16_t i = 0; i < elf->e_phnum; ++i)
		if (ph_tab[i].p_type == PT_INTERP)
			return 0;
	return 1;
}

static void set_payload_values(Elf32_Off beginoff, Elf32_Addr beginaddr)
{
	g_parasite_off = beginoff;
	g_parasite_addr = beginaddr;

	g_decryptor_off = g_parasite_off + g_parasite_size;
	g_decryptor_addr = g_parasite_addr + g_parasite_size;

	g_handler_off = g_decryptor_off + g_decryptor_size;
	g_handler_addr = g_decryptor_addr + g_decryptor_size;
}

static void patch_section_offsets(const Elf32_Ehdr *elf)
{
	Elf32_Shdr *sh_tab = (Elf32_Shdr *)((void *)elf + elf->e_shoff);

	for (uint16_t i = 0; i < elf->e_shnum; ++i)
	{
		if (sh_tab[i].sh_offset + sh_tab[i].sh_size == g_parasite_off)
		{
			sh_tab[i].sh_size += g_total_payload_size;
			return;
		}
	}
}

static uint8_t not_injectable(Elf32_Ehdr *elf)
{
	Elf32_Phdr *ph_tab = (Elf32_Phdr *)((void *)elf + elf->e_phoff);
	uint32_t padding = 0;
	int8_t found_code = 0;

	for (uint16_t i = 0; i < elf->e_phnum && !found_code; ++i)
	{
		if (ph_tab[i].p_type == PT_LOAD && ph_tab[i].p_flags == (PF_R | PF_X))
		{
			found_code = 1;

			g_codeseg_addr = ph_tab[i].p_vaddr;
			g_codeseg_off = ph_tab[i].p_offset;
			g_codeseg_size = ph_tab[i].p_filesz;

			if (i < elf->e_phnum)
				padding = ph_tab[i + 1].p_offset - (g_codeseg_off + g_codeseg_size);

			if (padding < g_total_payload_size)
				return write_error(NULL, PADD_ERR);
			else
			{
				ph_tab[i].p_filesz += g_total_payload_size;
				ph_tab[i].p_memsz += g_total_payload_size;
				ph_tab[i].p_flags |= PF_W;
			}
			break;
		}
	}

	if (!found_code)
		return write_error(NULL, LOAD_ERR);

	set_payload_values(g_codeseg_off + g_codeseg_size, g_codeseg_addr + g_codeseg_size);
	patch_section_offsets(elf);

	return 0;
}

int woody_elf32(t_woody *context)
{
	if (check_boundaries(context))
		return write_error(context->param_name, CORRUPT_ERR);

	Elf32_Ehdr *elf = (Elf32_Ehdr *)context->base;
	if (check_exec(elf))
		return write_error(context->param_name, ELFEXEC_ERR);

	// generate key if need be
	if (!context->keyisparam && !(context->key = generate_key(URANDOM, DFLT_KEY_LEN)))
		return write_error(KEYGEN_ERR, NULL);

	// get injection handler
	void *handler = read_file(HANDLER_ELF32_PATH, &g_handler_size);
	if (!handler)
	{
		free_payloads(context->key, NULL, NULL, NULL);
		return 1;
	}

	// get parasite
	void *parasite = read_file(PARASITE_ELF32_PATH, &g_parasite_size);
	if (!parasite)
	{
		free_payloads(context->key, handler, NULL, NULL);
		return 1;
	}

	// get decryptor
	void *decryptor = read_file(DECRYPTOR_ELF32_PATH, &g_decryptor_size);
	if (!decryptor)
	{
		free_payloads(context->key, handler, parasite, NULL);
		return 1;
	}

	uint32_t keysz = (context->keyisparam ? ft_strlen(context->key) : DFLT_KEY_LEN);
	g_total_payload_size = g_parasite_size
						+ g_decryptor_size
						+ g_handler_size
						+ (sizeof(char) * keysz);

	// prepare for injection
	if (not_injectable(elf))
	{
		free_payloads(context->key, handler, parasite, decryptor);
		return 1;
	}

	// create woody
	int woody_fd = -1;
	if ((woody_fd = open(PATCH, (O_CREAT | O_WRONLY | O_TRUNC), 0755)) == -1)
	{
		free_payloads(context->key, handler, parasite, decryptor);
		return write_error(PATCH, NULL);
	}

	// time to print the key
	if (!context->keyisparam)
		print_key(context->key, DFLT_KEY_LEN);

	// set entrypoint to start of payload
	Elf32_Addr code_entry = elf->e_entry;
	if (elf->e_type == ET_EXEC)
	{
		elf->e_entry = g_handler_addr;
		patch_payload_addr32(handler, g_handler_size, 0x42424242, g_parasite_addr); // offset to parasite
		patch_payload_addr32(handler, g_handler_size, 0xAAAAAAAA, 0);
	}
	else
	{
		elf->e_entry = g_handler_off;
		patch_payload_addr32(handler, g_handler_size, 0x42424242, g_parasite_off); // offset to parasite
		patch_payload_addr32(handler, g_handler_size, 0xAAAAAAAA, 1);
	}

	patch_payload_addr32(handler, g_handler_size, 0x55555555, keysz); // key size
	patch_payload_addr32(handler, g_handler_size, 0x44444444, g_handler_off + g_handler_size); // offset to key
	patch_payload_addr32(handler, g_handler_size, 0x19421942, g_codeseg_size + g_parasite_size); // text size
	patch_payload_addr32(handler, g_handler_size, 0x22222222, g_codeseg_off); // text offset
	patch_payload_addr32(handler, g_handler_size, 0x19191919, g_decryptor_off); // offset to rc4
	patch_payload_addr32(handler, g_handler_size, 0x33333333, code_entry); // return controlflow

	// inject parasite
	void *inject_addr = context->base + g_parasite_off;
	ft_memmove(inject_addr, parasite, g_parasite_size);

	// inject decryptor
	inject_addr += g_parasite_size;
	ft_memmove(inject_addr, decryptor, g_decryptor_size);

	// inject handler
	inject_addr += g_decryptor_size;
	ft_memmove(inject_addr, handler, g_handler_size);

	// inject key
	inject_addr += g_handler_size;
	ft_memmove(inject_addr, context->key, keysz);

	// encrypt code segment
	_rc4_elf64(context->base + g_codeseg_off, g_codeseg_size + g_parasite_size, context->key, keysz);

	free_payloads(context->key, handler, parasite, decryptor);

	// dump file into woody
	if (write(woody_fd, context->base, context->size) == -1)
	{
		write_error(PATCH, NULL);
		close(woody_fd);
		return 1;
	}

	close(woody_fd);

	return 0;
}
