/*
 * unused.c --- quick and dirty unused space dumper
 *
 * Copyright (C) 1997 Theodore Ts'o.  This file may be redistributed
 * under the terms of the GNU Public License.
 */

#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include <sys/types.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
extern int optind;
extern char *optarg;
#endif

#include "debugfs.h"
#include <unistd.h>

void do_dump_unused(int argc, char **argv,
                    int sci_idx EXT2FS_ATTR((unused)),
                    void *infop EXT2FS_ATTR((unused)))
{
    blk64_t         blk;
    unsigned char   buf[EXT2_MAX_BLOCK_SIZE];
    unsigned int    i;
    errcode_t       retval;
    char* keyword = NULL;
    char* found;
    int verbose = 0;
    int c;

    reset_getopt();

    while ((c = getopt(argc, argv, "k:vh")) != -1) {
        switch (c) {
        case 'k':
            keyword = optarg;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
            printf("Usage: %s -k <keyword> [-v] [-h]\n", argv[0]);
            return;
        default:
            printf("Invalid option: -%c\n", optopt);
            return;
        }
    }

    if (keyword == NULL) {
        printf("Keyword must be specified with -k option.\n");
        return;
    }

    for (blk=current_fs->super->s_first_data_block;
         blk < ext2fs_blocks_count(current_fs->super); blk++) {
        if (ext2fs_test_block_bitmap2(current_fs->block_map,blk))
            continue;
        retval = io_channel_read_blk64(current_fs->io, blk, 1, buf);
        if (retval) {
            com_err(argv[0], retval, "While reading block\n");
            return;
        }
        buf[current_fs->blocksize - 1] = '\0';
        found = strstr((char*)buf, keyword);
        if (found == NULL)
            continue;
        printf("\nUnused block %llu contains keyword \"%s\":\n",
               (unsigned long long) blk, keyword);
        if (verbose) {
            for (i=0; i < current_fs->blocksize; i++)
                fputc(buf[i], stdout);
        }
    }
}