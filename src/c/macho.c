#include "woody_macho.h"
#include "encryption.h"

void fbzero(FILE *f, off_t offset, size_t len)
{
    static unsigned char zeros[BUFSIZE] = {0};
    fseeko(f, offset, SEEK_SET);
    while (len != 0)
    {
        size_t size = MIN(len, sizeof(zeros));
        fwrite(zeros, size, 1, f);
        len -= size;
    }
}

bool check_lc(t_woody *context, struct mach_header *mh)
{
    size_t offset = sizeof(struct mach_header_64);

    fseeko(context->f, offset, SEEK_SET);
    uint32_t ncmds = mh->ncmds;

    off_t link_pos = -1;
    struct segment_command_64 linkedit_64;

    off_t symtab_pos = -1;
    uint32_t symtab_size = 0;

    for (size_t i = 0; i < ncmds; i++)
    {
        struct load_command lc;
        fpeek(&lc, sizeof(lc), 1, context->f);

        uint32_t cmdsize = lc.cmdsize;
        uint32_t cmd = lc.cmd;

        switch (cmd)
        {
        case LC_CODE_SIGNATURE:
            if (i == ncmds - 1)
            {

                struct linkedit_data_command *cmd = read_load_command(context->f, cmdsize);

                fbzero(context->f, ftello(context->f), cmdsize);

                uint32_t dataoff = cmd->dataoff;
                uint32_t datasize = cmd->datasize;

                free(cmd);

                uint64_t linkedit_fileoff = 0;
                uint64_t linkedit_filesize = 0;

                if (link_pos != -1)
                {
                    linkedit_fileoff = linkedit_64.fileoff;
                    linkedit_filesize = linkedit_64.filesize;
                }
                else
                    fprintf(stderr, "Warning: __LINKEDIT segment not found.\n");
                if (link_pos != -1)
                {
                    if ((off_t)(linkedit_fileoff + linkedit_filesize) != context->size)
                        fprintf(stderr, "Warning: __LINKEDIT segment is not at the end of the file, so codesign will not work on the patched binary.\n");
                    else
                    {
                        if (dataoff + datasize != context->size)
                            fprintf(stderr, "Warning: Codesignature is not at the end of __LINKEDIT segment, so codesign will not work on the patched binary.\n");
                        else
                        {
                            context->size -= datasize;
                            if (symtab_pos == -1)
                                fprintf(stderr, "Warning: LC_SYMTAB load command not found. codesign might not work on the patched binary.\n");
                            else
                            {
                                fseeko(context->f, symtab_pos, SEEK_SET);
                                struct symtab_command *symtab = read_load_command(context->f, symtab_size);

                                uint32_t strsize = symtab->strsize;
                                int64_t diff_size = symtab->stroff + strsize - (int64_t)context->size;
                                if (-0x10 <= diff_size && diff_size <= 0)
                                {
                                    symtab->strsize = (uint32_t)(strsize - diff_size);
                                    fwrite(symtab, symtab_size, 1, context->f);
                                }
                                else
                                    fprintf(stderr, "Warning: String table doesn't appear right before code signature. codesign might not work on the patched binary. (0x%llx)\n", diff_size);

                                free(symtab);
                            }

                            linkedit_filesize -= datasize;
                            uint64_t linkedit_vmsize = ROUND_UP(linkedit_filesize, 0x1000);

                            linkedit_64.filesize = linkedit_filesize;
                            linkedit_64.vmsize = linkedit_vmsize;

                            fseeko(context->f, link_pos, SEEK_SET);
                            fwrite(&linkedit_64, sizeof(linkedit_64), 1, context->f);

                            goto fix_header;
                        }
                    }
                }
                fbzero(context->f, dataoff, datasize);

            fix_header:
                mh->ncmds = ncmds - 1;
                mh->sizeofcmds = mh->sizeofcmds - cmdsize;

                return true;
            }
            else
            {
                printf("LC_CODE_SIGNATURE is not the last load command, so couldn't remove.\n");
            }
            break;
        case LC_LOAD_DYLIB:
        case LC_LOAD_WEAK_DYLIB:
        {
            struct dylib_command *dylib_command = read_load_command(context->f, cmdsize);

            free(dylib_command);

            break;
        }
        case LC_SEGMENT:
        case LC_SEGMENT_64:
        {
            struct segment_command_64 *cmd = read_load_command(context->f, cmdsize);
            if (strcmp(cmd->segname, "__LINKEDIT") == 0)
            {
                link_pos = ftello(context->f);
                linkedit_64 = *cmd;
            }
            free(cmd);
        }
        case LC_SYMTAB:
            symtab_pos = ftello(context->f);
            symtab_size = cmdsize;
        }

        fseeko(context->f, lc.cmdsize, SEEK_CUR);
    }
    return true;
}

bool insert_dylib(t_woody *context)
{
    struct mach_header mh;
    fread(&mh, sizeof(struct mach_header), 1, context->f);

    if (mh.magic != MH_MAGIC_64 && mh.magic != MH_CIGAM_64 && mh.magic != MH_MAGIC && mh.magic != MH_CIGAM && mh.magic != FAT_CIGAM)
    {
        printf("Unknown magic: 0x%x\n", mh.magic);
        return false;
    }

    size_t offset = sizeof(struct mach_header_64);

    if (!check_lc(context, &mh))
    {
        printf("Failed to check load commands.\n");
        return true;
    }

    size_t path_padding = 8;

    size_t dylib_path_len = strlen(PARASITE_MACHO_PATH);
    size_t dylib_path_size = (dylib_path_len & ~(path_padding - 1)) + path_padding;
    uint32_t cmdsize = (uint32_t)(sizeof(struct dylib_command) + dylib_path_size);

    struct dylib_command dylib_command = {
        .cmd = LC_LOAD_DYLIB,
        .cmdsize = cmdsize,
        .dylib = {
            .name = {sizeof(struct dylib_command)},
            .timestamp = 0,
            .current_version = 0,
            .compatibility_version = 0}};

    uint32_t sizeofcmds = mh.sizeofcmds;

    fseeko(context->f, offset + sizeofcmds, SEEK_SET);
    char space[cmdsize];

    fread(&space, cmdsize, 1, context->f);

    for (size_t i = 0; i < cmdsize; i++)
    {
        if (space[i] != 0)
        {
            printf("It doesn't seem like there is enough empty space.\n");
            return false;
        }
    }

    fseeko(context->f, -((off_t)cmdsize), SEEK_CUR);

    char *dylib_path_padded = calloc(dylib_path_size, 1);
    if (!dylib_path_padded)
    {
        printf("Couldn't allocate dylib_path_padded\n");
        return false;
    }
    memcpy(dylib_path_padded, PARASITE_MACHO_PATH, dylib_path_len);

    fwrite(&dylib_command, sizeof(dylib_command), 1, context->f);
    fwrite(dylib_path_padded, dylib_path_size, 1, context->f);

    free(dylib_path_padded);

    mh.ncmds = mh.ncmds + 1;
    sizeofcmds += cmdsize;
    mh.sizeofcmds = sizeofcmds;

    fseeko(context->f, 0, SEEK_SET);
    fwrite(&mh, sizeof(mh), 1, context->f);

    return true;
}

int woody_macho(t_woody *context)
{
    FILE *f = fopen(PARASITE_MACHO_PATH, "r+b");
    if (!f)
    {
        printf("Couldn't open %s\n", PARASITE_MACHO_PATH);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);

    char *buffer = calloc(len, 1);
    if (!buffer)
    {
        printf("Couldn't allocate buffer\n");
        return 1;
    }
    fread(buffer, 1, len, f);

    struct mach_header_64 *mh;
    mh = (struct mach_header_64 *)buffer;
    free(buffer);

    if (mh->magic != MH_MAGIC_64 && mh->magic != MH_CIGAM_64)
    {
        printf("Not a 64-bit Mach-O file\n");
        return false;
    }

    encrypt_decrypt_text(mh);
    fwrite(mh, 1, len, f);

    fseeko(context->f, 0, SEEK_END);
    context->size = ftello(context->f);
    rewind(context->f);

    insert_dylib(context);

    ftruncate(fileno(context->f), context->size);
    fclose(f);
    return 0;
}
