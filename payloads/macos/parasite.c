#include <stdio.h>
#include <mach-o/dyld.h>
#include "encryption.h"

int yay(){
	return printf("....WOODY....\n");
}

void __attribute__((constructor)) __attribute__((section("__TEXT,__timac"))) decryptTextSection()
{
	struct mach_header_64 *header = (struct mach_header_64 *)_dyld_get_image_header(0);
	encrypt_decrypt_text(header);
	yay();
}