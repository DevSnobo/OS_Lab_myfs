//
//  myfs.h
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#ifndef myfs_h
#define myfs_h

#include <fuse.h>
#include <cmath>
#include <cstring>

#include "blockdevice.h"
#include "myfs-structs.h"

class MyFS {
private:
    static MyFS *_instance;
    FILE *logFile;
    
public:
    static MyFS *Instance();

    // TODO: Add attributes of your file system here
    BlockDevice block_dev;
    Superblock superblock;
    DMap  dmap;
    FAT fat;
    Root  root;
    uint8_t open_files_count;
    
    MyFS();
    ~MyFS();
    
    // --- Methods called by FUSE ---
    // For Documentation see https://libfuse.github.io/doxygen/structfuse__operations.html
    int fuseGetattr(const char *path, struct stat *statbuf);
    int fuseReadlink(const char *path, char *link, size_t size);
    int fuseMknod(const char *path, mode_t mode, dev_t dev);
    int fuseMkdir(const char *path, mode_t mode);
    int fuseUnlink(const char *path);
    int fuseRmdir(const char *path);
    int fuseSymlink(const char *path, const char *link);
    int fuseRename(const char *path, const char *newpath);
    int fuseLink(const char *path, const char *newpath);
    int fuseChmod(const char *path, mode_t mode);
    int fuseChown(const char *path, uid_t uid, gid_t gid);
    int fuseTruncate(const char *path, off_t newSize);
    int fuseUtime(const char *path, struct utimbuf *ubuf);
    int fuseOpen(const char *path, struct fuse_file_info *fileInfo);
    int fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    int fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    int fuseStatfs(const char *path, struct statvfs *statInfo);
    int fuseFlush(const char *path, struct fuse_file_info *fileInfo);
    int fuseRelease(const char *path, struct fuse_file_info *fileInfo);
    int fuseFsync(const char *path, int datasync, struct fuse_file_info *fi);
#ifdef __APPLE__
    int fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x);
    int fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x);
#else
    int fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags);
    int fuseGetxattr(const char *path, const char *name, char *value, size_t size);
#endif
    void* fuseInit(struct fuse_conn_info *conn);
    int fuseListxattr(const char *path, char *list, size_t size);
    int fuseRemovexattr(const char *path, const char *name);
    int fuseOpendir(const char *path, struct fuse_file_info *fileInfo);
    int fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
    int fuseReleasedir(const char *path, struct fuse_file_info *fileInfo);
    int fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo);
    int fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo);
    int fuseCreate(const char *, mode_t, struct fuse_file_info *);
    void fuseDestroy();
    
    // TODO: Add methods of your file system here
    /**
    * Writes a generic type of data into the blockdevice. Writes to the position block sets.
    * @tparam T The generic type of the data that should be written.
    * @param block The number of the block where the data should be written to.
    * @param data The data that should be written.
    */
    template<class T>
    void writeDevice(size_t block, const T &data) {
        //static_assert(std::is_trivially_copyable<T>::value, "Can't operate on complex types!");

        const char *rawData = reinterpret_cast<const char *>(&data);

        static char buffer[BLOCK_SIZE];
        size_t blockCount = sizeof(T) / BLOCK_SIZE;
        size_t currentBlock = block;
        for (; currentBlock < block + blockCount; ++currentBlock) {
            blockDevice.write(currentBlock, rawData + ((currentBlock - block) * BLOCK_SIZE));
        }
        memcpy(buffer, rawData + ((currentBlock - block) * BLOCK_SIZE), sizeof(T) % BLOCK_SIZE);
        blockDevice.write(currentBlock, buffer);
    }

    /**
     * Reads a generic type of data from the blockdevice. The position which should be read from is block.
     * @tparam T The generic type of the data that should be read.
     * @param block The number of the block where the data should be read from.
     * @param data Return parameter,the data that was to be read.
     */
    template<class T>
    void readDevice(size_t block, T &data) {
        //static_assert(std::is_trivially_constructible<T>::value, "Can't operate on complex types!");

        char *rawData = reinterpret_cast<char *>(&data);
        static char buffer[BLOCK_SIZE];
        std::size_t blockCount = sizeof(T) / BLOCK_SIZE;
        std::size_t currentBlock = block;
        for (; currentBlock < block + blockCount; ++currentBlock) {
            blockDevice.read(currentBlock, rawData + ((currentBlock - block) * BLOCK_SIZE));
        }
        blockDevice.read(currentBlock, buffer);
        memcpy(rawData + ((currentBlock - block) * BLOCK_SIZE), buffer, sizeof(T) % BLOCK_SIZE);
    }

    /**
     * Writes a generic type of data-array into the blockdevice. Writes to the position block sets.
     * @tparam T The generic type of the data should be written.
     * @tparam N The size of the data array, that should be written.
     * @param block The number of the block where the data should be written to.
     * @param data The data, an array, that should be written.
     */
    template<class T, size_t N>
    void writeDevice(size_t block, const T (&data)[N]) {
        //static_assert(std::is_trivially_copyable<T>::value, "Can't operate on complex types!");

        const char *rawData = reinterpret_cast<const char *>(&data);

        static char buffer[BLOCK_SIZE];
        size_t blockCount = (sizeof(T) * N)/ BLOCK_SIZE;
        size_t currentBlock = block;
        for (; currentBlock < block + blockCount; ++currentBlock) {
            block_dev.write(currentBlock, rawData + ((currentBlock - block) * BLOCK_SIZE));
        }
        memcpy(buffer, rawData + ((currentBlock - block) * BLOCK_SIZE), (sizeof(T) * N) % BLOCK_SIZE);
        block_dev.write(currentBlock, buffer);
    }

    /**
     * Reads a generic type of data-array from the blockdevice. The position which should be read from is block.
     * @tparam T The generic type of the data should be read.
     * @tparam N The size of the data array, that should be read.
     * @param block The number of the block where the data should be read from.
     * @param data Return parameter, the data, an array, that was to be read.
     */
    template<class T, size_t N>
    void readDevice(std::size_t block, T (&data)[N]) {
        //static_assert(std::is_trivially_constructible<T>::value, "Can't operate on complex types!");

        char *rawData = reinterpret_cast<char *>(&data);
        static char buffer[BLOCK_SIZE];
        std::size_t blockCount = (sizeof(T) * N) / BLOCK_SIZE;
        std::size_t currentBlock = block;
        for (; currentBlock < block + blockCount; ++currentBlock) {
            block_dev.read(currentBlock, rawData + ((currentBlock - block) * BLOCK_SIZE));
        }
        block_dev.read(currentBlock, buffer);
        memcpy(rawData + ((currentBlock - block) * BLOCK_SIZE), buffer, (sizeof(T) * N) % BLOCK_SIZE);
    }

    
};

#endif /* myfs_h */
