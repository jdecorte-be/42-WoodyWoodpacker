#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mach-o/loader.h>
#include <mach/mach.h>
#include <stdbool.h>

#include <unistd.h>

#define KEY "\x87\xc0\x0f\xdb\x51\x9f\x5e\x69\xcb\x2d\xce\x3f\xe9\x86\xe9\x9f"
#define KEY_LEN 16

static inline void __attribute__((always_inline))
ft_swap(char *s1, char *s2)
{
	char tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

static inline void __attribute__((always_inline))
rc4(char *bytes, long length, const char *key, int keysz)
{
	unsigned char state[256] = {0};
	for (int i = 0; i < 256; ++i)
		state[i] = i;

	int j = 0;
	for (int i = 0; i < 256; ++i)
	{
		j = (j + state[i] + key[i % keysz]) & 255;
		ft_swap((char *)&state[i], (char *)&state[j]);
	}

	j = 0;
	int k = 0;
	for (int i = 0; i < length; ++i)
	{
		k = (k + 1) & 255;
		j = (j + state[k]) & 255;

		ft_swap((char *)&state[k], (char *)&state[j]);

		int t = (state[k] + state[j]) & 255;

		bytes[i] = bytes[i] ^ state[t];
	}
}

static inline void __attribute__((always_inline))
set_virtual_memory(vm_address_t addr, vm_size_t size)
{
	kern_return_t returnValue = vm_protect(mach_task_self(), addr, size, false, VM_PROT_ALL);
	if (returnValue != KERN_SUCCESS)
	{
		fprintf(stderr, "Error %d: Fail to change virtual memory protections\n", returnValue);
		exit(1);
	}
}

static inline void __attribute__((always_inline))
encrypt_decrypt(void *src, uint64_t len)
{
	int32_t numberOfBytes = 1;

	for (uint64_t i = 0; i < len; i += numberOfBytes)
	{
		if (numberOfBytes == 1)
			if (i + numberOfBytes > len && i + 16 <= len)
				numberOfBytes = 16;
			else
				break;
		else if (i + numberOfBytes > len)
			break;
		set_virtual_memory((vm_address_t)(src + i), numberOfBytes);
		rc4(src + i, numberOfBytes, (char *)KEY, KEY_LEN);
	}
}

static inline void __attribute__((always_inline))
encrypt_decrypt_text(struct mach_header_64 *machHeader)
{
	size_t segmentOffset = sizeof(struct mach_header_64);

	for (uint32_t i = 0; i < machHeader->ncmds; i++)
	{
		struct load_command *loadCommand = (struct load_command *)((uint8_t *)machHeader + segmentOffset);
		if (loadCommand->cmd == LC_SEGMENT_64)
		{
			struct segment_command_64 *segCommand = (struct segment_command_64 *)loadCommand;

			void *sectionPtr = (void *)(segCommand + 1);
			for (uint32_t nsect = 0; nsect < segCommand->nsects; ++nsect)
			{
				struct section_64 *section = (struct section_64 *)sectionPtr;

				if (strncmp(segCommand->segname, SEG_TEXT, 16) == 0){
					if (strncmp(section->sectname, SECT_TEXT, 16) == 0)
						encrypt_decrypt((uint8_t *)machHeader + section->offset, section->size);
				}
				sectionPtr += sizeof(struct section_64);
			}
		}
		segmentOffset += loadCommand->cmdsize;
	}
}
