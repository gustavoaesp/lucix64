#include <lucix/init/initramfs.h>
#include <lucix/init.h>
#include <lucix/string.h>
#include <lucix/utils.h>
#include <lucix/printk.h>
#include <lucix/vfs.h>
#include <lucix/slab.h>

struct tar_header
{                              /* byte offset */
    char name[100];               /*   0 */
    char mode[8];                 /* 100 */
    char uid[8];                  /* 108 */
    char gid[8];                  /* 116 */
    char size[12];                /* 124 */
    char mtime[12];               /* 136 */
    char chksum[8];               /* 148 */
    char typeflag;                /* 156 */
    char linkname[100];           /* 157 */
    char magic[6];                /* 257 */
    char version[2];              /* 263 */
    char uname[32];               /* 265 */
    char gname[32];               /* 297 */
    char devmajor[8];             /* 329 */
    char devminor[8];             /* 337 */
    char prefix[155];             /* 345 */
                                /* 500 */
};

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

#define XHDTYPE  'x'            /* Extended header referring to the
                                   next file in the archive */
#define XGLTYPE  'g'            /* Global extended header */

/* Bits used in the mode field, values in octal.  */
#define TSUID    04000          /* set UID on execution */
#define TSGID    02000          /* set GID on execution */
#define TSVTX    01000          /* reserved */
                                /* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

static int tar_valid_header(struct tar_header *header)
{
    if (memcmp(header->magic, TMAGIC, strlen(TMAGIC) - 1)) {
        return -1;
    }
    return 0;
}

static char* sanitize_name(char *name)
{
    while (*name == '.') name++;
    while (name[strlen(name) - 1] == '/')
        name[strlen(name) - 1] = '\0';

    return name;
}

static int process_directory(struct tar_header *header)
{
    int ret = 0;
    char *name = NULL;

    if (!strcmp(header->name, "./") || !strcmp(header->name, "/")) {
        printf("Skipping root\n");
        return 0;
    }

    name = sanitize_name(header->name);

    ret = vfs_mkdir(name, 0);

    printf("initramfs directory: %s\n", name);

    return ret;
}

static int process_file(struct tar_header *header)
{
    int ret = 0;
    char *name = sanitize_name(header->name);

    printf("initramfs file: %s\n", name);

    return ret;
}

int unpack_initramfs()
{
    int ret = 0;
    for (void *ptr = initramfs_info.addr; ptr < initramfs_info.addr + initramfs_info.size; ptr += 512)
    {
        struct tar_header *header = ptr;

        if (tar_valid_header(header)) {
            continue;
        }

        switch(header->typeflag)
        {
        case DIRTYPE:
            ret = process_directory(header);
            break;
        case REGTYPE:
            ret = process_file(header);
            break;
        }

        if (ret) {
            printf("Something broke: %s\n", header->name);
            return ret;
        }
    }

    return 0;
}
