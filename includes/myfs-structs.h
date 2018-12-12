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
    static const uint8_t USABLE_BLOCKS = 61440;//30 MiB = 31457280 Byte / 512 Byte
    static const uint8_t SUPER_BLOCK_POS = 0;
    static const uint8_t SUPER_BLOCK_SIZE = 1;//?
    static const uint8_t DMAP_POS = 1;//?
    static const uint8_t DMAP_SIZE = 0;//?
    static const uint8_t FAT_POS = 0; //?
    static const uint8_t FAT_SIZE = 0;//?
    static const uint8_t ROOT_POS = 0;//?
    static const uint8_t ROOT_SIZE = 0;//?
    static const uint8_t DATA_POS = 0;//?
    static const uint8_t DATA_SIZE = 0;//?
    uint8_t current_entries = 0;
    uint8_t open_files = 0;

};

struct DMap {
    //2 Blöcke mit Zahlen von 0-511 bzw 1-512
    short[] dmap;
}

struct FAT {
    short[512][2] fat;
}

struct file { //stat struct nehmen?
    char *file_name;
    short file_size;
    char *uid;
    char *gid;
    short file_perm;


    /*struct times {
        long atime;
        long mtime;
        long ctime;
    }*/

    short* file_start;
}

struct root {
    file* files[];
}

#endif /* myfs_structs_h */
