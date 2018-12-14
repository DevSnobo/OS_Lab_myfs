//
//  myfs-structs.h
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#ifndef myfs_structs_h
#define myfs_structs_h

#include <cstdint>

#define NAME_LENGTH 255
#define BLOCK_SIZE 512
#define NUM_DIR_ENTRIES 64
#define NUM_OPEN_FILES 64



// TODO: Add structures of your file system here

struct Superblock {

    static const uint8_t BLOCKSIZE = BLOCK_SIZE;
    static const uint8_t USABLE_BLOCKS = 61760;//30 MiB = 31457280 Byte / 512 Byte + Rest
    static const uint8_t SUPER_BLOCK_POS = 0;
    static const uint8_t SUPER_BLOCK_SIZE = 1;
    static const uint8_t DMAP_POS = 1;
    static const uint8_t DMAP_SIZE = 15;
    static const uint8_t FAT_POS = 16;
    static const uint8_t FAT_SIZE = 240;
    static const uint8_t ROOT_POS = 256;
    static const uint8_t ROOT_SIZE = 64;
    static const uint8_t DATA_POS = 320;
    static const uint8_t DATA_SIZE = 61440;//30 MiB = 31457280 Byte / 512 Byte
    uint8_t current_entries;
    uint8_t open_files;

};

struct DMap {
    bool free_blocks[61440]; //fuer jeden Block einen Eintrag, 0 fuer frei, 1 fuer belegt
};

struct FAT {
    uint16_t fat_entries[61440]; //fuer jeden Block einen Eintrag
};

struct File { //stat struct nehmen?
    char file_name[NAME_LENGTH];
    uint8_t file_size;
    uint8_t uid; // user id
    uint8_t gid; //group id
    uint8_t file_perm;

    struct times {
        uint64_t atime;
        uint64_t mtime;
        uint64_t ctime;
    };

    uint16_t file_start;
};

struct Root {
    File file[NUM_DIR_ENTRIES];
};

#endif /* myfs_structs_h */
