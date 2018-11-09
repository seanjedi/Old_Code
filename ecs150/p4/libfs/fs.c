#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

#define FAT_EOC 0xFFFF

#define MIN(a,b) (((a)<(b))?(a):(b))

struct disk *disk = NULL;

/*
 * The superblock struct represents the 1st block of the file system.
 *
 * signature = name of file system
 * b_total = Total amount of blocks of virtual disk
 * rdb_index = Root directory block index
 * dbs_index = Data block start index
 * total_blocks = Amount of data blocks
 * total_fat = Number of blocks for FAT
 * padding = Unused/Padding
 */
struct superblock {
    uint64_t signature;
    uint16_t b_total;
    uint16_t rdb_index;
    uint16_t dbs_index;
    uint16_t total_blocks;
    uint8_t total_fat;
    char padding[4079];
} __attribute__((packed));

/*
 * The fat_entry struct represents an entry of the FAT array.
 *
 * entry = if not zero, it is link to the next block in the chainmap else it is
 * a free data block
 */
struct fat_entry {
    uint16_t entry;
};

/*
 * The file_entry struct represents an entry of a root_directory.
 *
 * filename = Filename (including NULL character)
 * file_size = Size of the file (in bytes)
 * fdb_index = Index of the first data block
 * padding = Unused/Padding
 */
struct file_entry {
    char filename[16];

    uint32_t file_size;
    uint16_t fdb_index;
    char padding[10];
} __attribute__((packed));

/*
 * The file_descriptor: represents the file descriptors associated with each
 * file.
 *
 * @file_entry: holds information about the file such as filename, size, etc.
 * @offset: The current offset for the file
 * fd = file descriptor number
 */
struct file_descriptor {
    struct file_entry *file_entry;
    size_t offset;
    size_t fd;
} __attribute__((packed));

/*
 * The disk struct represents a disk ready to be used, it contains all the meta-
 * information about the file system.
 *
 * @superblock = the first block of the disk
 * @fat = an array of FAT entries that contain information about file parts
 * @root_directory = an array of file entries that contain information about
 * files
 * rd_free_files = the number of free root directory files (used in printing)
 * fat_free_blks = the number of free fat blocks (used in printing)
 * files_count = number of opened files at the moment (max = FS_OPEN_MAX_COUNT)
 * @file_descriptors = array of file descriptor objects  (which also have the
 * file_entries) which is os size of file_descriptor struct * 32
 */
struct disk {
    struct superblock *superblock;
    struct fat_entry *fat;
    struct file_entry *root_directory;
    int rd_free_files;
    int fat_free_blks;
    int files_count;
    struct file_descriptor *file_descriptors;
};

int fs_cleanup(void)
{
    if (block_disk_close() == -1) {
        return -1;
    }

    if (disk == NULL) {
        return -1;
    }

    free(disk->file_descriptors);
    free(disk->root_directory);
    free(disk->superblock);

    if (disk->fat) {
        free(disk->fat);
    }

    free(disk);
    disk = NULL;

    return 0;
}

/*
 * available_fd: Checks if there is an available space to create a new file
 * descriptor in the disk->file_descriptors array.
 *
 * @filename: pointer to a string to be examined
 * Return: index of available space if there is one, else -1
 */
int available_fd(const char *filename)
{
    if (disk->files_count >= FS_FILE_MAX_COUNT) {
        return -1;
    }

    for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if (disk->file_descriptors[i].file_entry == NULL)
            return i;
    }
}

/*
 * has_filename: Checks if the root_directory array has a file with the filename
 * passed as an argument.
 *
 * @filename: pointer to a string to be examined
 * Return: if filename was found in the array, return the index indicating its
 * location, else return -1;
 */
int has_filename(const char *filename)
{
    for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if (strcmp(filename, disk->root_directory[i].filename) == 0) {
            return i;
        }
    }

    return -1;
}


/*
 * strlen_s: Our version of C++'s strlen_s. It returns the length of the string
 * if it is null-terminated, else it returns 0 to indicate that the string is
 * invalid.
 * A string is invalid if: it is NULL (unitialized filenames), or it has >
 * FS_FILENAME_LEN bytes, or it is not a null-terminated string.
 *
 * @filename: pointer to a string to be examined
 * Return: length of string if it is valid, else 0 to indicate failure
 */
int strlen_s(const char *filename, size_t size)
{
    for (int i = 0; i < size; i++) {
        if (filename[i] == '\0') {
            return i;
        }
    }

    return 0;
}

/*
 * first_fit: finds the first block in the FAT array that is free
 *
 * fd: the file descriptor we want to add to FAT
 * Return: the free index if we find any, else return -1
 */
int first_fit(int fd)
{
    // iterates through the FAT array, and finds the first
    // entry that is free (if it points to 0, its free)
    int fit = 0;
    for (int i = 0; i < disk->superblock->total_blocks; i++) {
        if (disk->fat[i].entry == 0) {
            break;
        }
        fit++;
    }
    if (fit == disk->superblock->total_blocks) {
        return -1;
    }

    return fit;
}

/* find_index: finds the index of the data block corresponding to the file’s
 * offset.
 *
 * fdb_index: first data block index (from file)
 * dbs_index: data block start (from superblock)
 * offset: the offset given by user
 * Return: the index of the data block corresponding to the file’s offset
 */
int find_index(uint16_t fdb_index, uint16_t dbs_index, size_t *offset)
{
    // find how many blocks the offset will take up
    int index = fdb_index;

    while (*offset > BLOCK_SIZE) {
        // if no more fat to iterate through, return error
        if (disk->fat[index].entry == FAT_EOC) {
            return -1;
        }

        // if FAT has next entry, get the index of the next entry
        index = disk->fat[index].entry;
        *offset -= BLOCK_SIZE;
    }

    // index should now be at the file's first datablock
    // then add fdb_index and dbs_index to get the index the offset points at
    index += dbs_index;

    return index;
}

/*~                         FS API Functions                                 ~*/
int fs_mount(const char *diskname)
{
    // make sure disk was opened successfully & that disk isn't empty
    if (block_disk_open(diskname) == -1 || block_disk_count() == -1) {
        return -1;
    }

    disk = (struct disk *)malloc(sizeof(struct disk));
    disk->fat = NULL;
    disk->files_count = 0;
    // Use calloc here to initialize all entries to NULL without needing to
    // iterate
    disk->file_descriptors = (struct file_descriptor *) calloc(
        sizeof(struct file_descriptor), FS_OPEN_MAX_COUNT);
    // read root_directory data
    disk->root_directory = (struct file_entry *)malloc(
        sizeof(struct file_entry) * FS_FILE_MAX_COUNT);

    // read superblock data
    disk->superblock = (struct superblock *)malloc(BLOCK_SIZE);

    if (block_read(0, disk->superblock) == -1) {
        fs_cleanup();
        return -1;
    }

    // now that we know fat size, allocate an array for it and read its data
    int size_fat = disk->superblock->total_fat * BLOCK_SIZE;
    disk->fat = (struct fat_entry *)malloc(size_fat);

    size_t offset = 0;
    for (int i = 0; i < disk->superblock->total_fat; i++) {
        if (block_read(i + 1, disk->fat + offset) == -1) {
            fs_cleanup();
            return -1;
        }
        // read 1 block (4096 bytes) as FAT objects = 2048 bytes
        offset += BLOCK_SIZE / sizeof(struct fat_entry);
    }

    if (block_read(disk->superblock->rdb_index, disk->root_directory) == -1) {
        fs_cleanup();
        return -1;
    }

    // calculate rdir_free_files (count how many files in the disk for fs_info)
    int files = 0;
    for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if (strlen_s(disk->root_directory[i].filename, FS_FILENAME_LEN) != 0) {
            files++;
        }
    }
    disk->rd_free_files = FS_FILE_MAX_COUNT - files;

    // calculate fat_free_blk (count how many data blocks are used for fs_info)
    int blocks = 0;
    for (int i = 0; i < disk->superblock->total_blocks; i++) {
        if (disk->fat[i].entry != 0) {
            blocks++;
        }
    }
    disk->fat_free_blks = disk->superblock->total_blocks - blocks;

    return 0;
}

int fs_umount(void)
{
    if (disk == NULL) {
        return -1;
    }

    if (disk->files_count > 0) {
        return -1;
    }

    // write the superblock back to the virtual disk
    if (block_write(0, disk->superblock) == -1) {
        return -1;
    }

    // write the FAT entries back to the virtual disk
    size_t offset = 0;
    for (int i = 0; i < disk->superblock->total_fat; i++) {
        if (block_write(i + 1, disk->fat + offset) == -1) {
            return -1;
        }

        // write 1 block (4096 bytes) as FAT objects = 2048 bytes
        offset += BLOCK_SIZE / sizeof(struct fat_entry);
    }

    // write the root_directory entries back to the virtual disk
    if (block_write(disk->superblock->rdb_index, disk->root_directory) == -1) {
        return -1;
    }

    return fs_cleanup();
}

int fs_info(void)
{
    // return if there is no virtual disk mounted
    if (disk == NULL) {
        return -1;
    }

    printf("FS Info:\n");
    printf("total_blk_count=%d\n", disk->superblock->b_total);
    printf("fat_blk_count=%d\n", disk->superblock->total_fat);
    printf("rdir_blk=%d\n", disk->superblock->rdb_index);
    printf("data_blk=%d\n", disk->superblock->dbs_index);
    printf("data_blk_count=%d\n", disk->superblock->total_blocks);
    printf("fat_free_ratio=%d/%d\n", disk->fat_free_blks,
           disk->superblock->total_blocks);
    printf("rdir_free_ratio=%d/%d\n", disk->rd_free_files, FS_FILE_MAX_COUNT);

    return 0;
}

int fs_create(const char *filename)
{
    if (disk == NULL) {
        return -1;
    }

    // return if filename is invalid
    if (strlen_s(filename, FS_FILENAME_LEN) == 0) {
        return -1;
    }

    // return if filename already exists in the root directory array
    if (has_filename(filename) >= 0) {
        return -1;
    }

    // find an empty entry in the root directory
    int i;
    int j = -1;
    for (i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if (strlen_s(disk->root_directory[i].filename, FS_FILENAME_LEN) == 0) {
            j++;
            break;
        }
    }
    //if there is no open slots found, return error
    if(j == -1){
        return -1;
    }

    // create new empty file
    struct file_entry *newFile = &disk->root_directory[i];

    strcpy(newFile->filename, filename);
    newFile->file_size = 0;
    newFile->fdb_index = FAT_EOC;

    return 0;
}

int fs_delete(const char *filename)
{
    if (disk == NULL) {
        return -1;
    }

    int to_delete;

    // return if filename is invalid
    if (strlen_s(filename, FS_FILENAME_LEN) == 0) {
        return -1;
    }

    // return if filename doesn't exist in the root_directory array
    // else to_delete will have the index of filename in the array
    to_delete = has_filename(filename);
    if (to_delete == -1) {
        return -1;
    }

    // delete filename's data from FAT
    int fat_index = disk->root_directory[to_delete].fdb_index;
    int temp;
    while (fat_index != FAT_EOC && fat_index != 0) {
        temp = disk->fat[fat_index].entry; // save next file part's index
        disk->fat[fat_index].entry = 0;    // clean fat entry
        fat_index = temp; // fat_index now holds the next file part
    }

    disk->root_directory[to_delete].fdb_index = FAT_EOC;
    disk->root_directory[to_delete].file_size = 0;
    disk->root_directory[to_delete].filename[0] = '\0';

    return 0;
}

int fs_ls(void)
{
    if (disk == NULL) {
        return -1;
    }

    printf("FS Ls:\n");
    // print files saved in the disk (prints root_directory entry fields)
    int size, data_blk;
    char filename[16];
    for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
        if (strlen_s(disk->root_directory[i].filename, FS_FILENAME_LEN) != 0) {
            size = disk->root_directory[i].file_size;
            data_blk = disk->root_directory[i].fdb_index;
            strcpy(filename, disk->root_directory[i].filename);
            printf("file: %s, size: %d, data_blk: %d\n", filename, size,
                   data_blk);
        }
    }

    return 0;
}

int fs_open(const char *filename)
{
    // check if a disk is open, if not then can't open a file
    if (disk == NULL) {
        return -1;
    }

    // return if filename is invalid
    if (strlen_s(filename, FS_FILENAME_LEN) == 0) {
        return -1;
    }

    int file;
    // return if filename exists in the root_directory array
    file = has_filename(filename);
    if (file == -1) {
        return -1;
    }

    // find if there is room for another file, else return
    int fd_index = available_fd(filename);
    if (fd_index == -1) {
        return -1;
    }

    struct file_descriptor *new_fd = &disk->file_descriptors[fd_index];

    new_fd->offset = 0;
    new_fd->fd = fd_index;

    // get the file_entry corresponding to this filename and make new_fd's file_
    // entry point to it
    int entry_index = has_filename(filename);
    new_fd->file_entry = &disk->root_directory[entry_index];

    disk->files_count++;

    return fd_index;
}

int fs_close(int fd)
{
    // check if a disk is open, if not then can't open a file
    if (disk == NULL || fd > FS_OPEN_MAX_COUNT || fd < 0) {
        return -1;
    }

    // check if file descriptor has anything in it
    if (disk->file_descriptors[fd].file_entry == NULL) {
        return -1;
    }
    disk->file_descriptors[fd].file_entry = NULL;
    disk->file_descriptors[fd].offset = 0;
    disk->files_count--;

    return 0;
}

int fs_stat(int fd)
{
    // check if a disk is open, if not then can't open a file
    if (disk == NULL || fd > FS_OPEN_MAX_COUNT || fd < 0) {
        return -1;
    }

    // check if file descriptor has anything in it
    if (disk->file_descriptors[fd].file_entry == NULL) {
        return -1;
    }

    // return file size if file descriptor is valid
    return disk->file_descriptors[fd].file_entry->file_size;
}

int fs_lseek(int fd, size_t offset)
{
    // check if a disk is open, if not then can't open a file
    if (disk == NULL || fd > FS_OPEN_MAX_COUNT || fd < 0) {
        return -1;
    }

    // check if file descriptor has anything in it
    if (disk->file_descriptors[fd].file_entry == NULL) {
        return -1;
    }

    // check if offset is out of bounds
    if (offset > disk->file_descriptors[fd].file_entry->file_size) {
        return -1;
    }

    // update offset
    disk->file_descriptors[fd].offset = offset;

    return 0;
}

int fs_write_once(int index, int count, int offset, void *buffer)
{
    char bounce_buffer[BLOCK_SIZE];
    if (block_read(index, bounce_buffer) == -1) {
        return -1;
    }

    int to_write = MIN(count, BLOCK_SIZE - offset);

    if (memcpy(bounce_buffer + offset, buffer, to_write) == NULL) {
        return -1;
    }

    if (block_write(index, bounce_buffer) == -1) {
        return -1;
    }

    return to_write;
}

int fs_write(int fd, void *buffer, size_t count)
{
    // check if a disk is open, if not then can't open a file
    if (disk == NULL || fd > FS_OPEN_MAX_COUNT || fd < 0) {
        return -1;
    }

    // check if file descriptor has anything in it
    if (disk->file_descriptors[fd].file_entry == NULL) {
        return -1;
    }

    // save struct field variables for convenience
    struct file_entry * fe = disk->file_descriptors[fd].file_entry;
    size_t offset = disk->file_descriptors[fd].offset;
    uint16_t dbs_index = disk->superblock->dbs_index;

    // moving fdb_index for each block being written
    uint16_t fdb_index;

    // if first write, make a new FAT location
    if (fe->fdb_index == FAT_EOC) {
        fdb_index = first_fit(fd);
        if (fdb_index == -1) {
            return -1;
        }
        disk->fat[fdb_index].entry = FAT_EOC;
        fe->fdb_index = fdb_index;
    }

    int total = 0;
    int file_space = offset;
    while (count > 0) {
        int index = find_index(fdb_index, dbs_index, &offset);
        int wrote = fs_write_once(index, count, offset, buffer);
        if (wrote == -1) {
            return total;
        }

        // wrote bytes successfully, remove them from count
        count -= wrote;
        // move buffer forward
        buffer += wrote;
        // account for total bytes written to be returned later
        total += wrote;
        // increase file size when writing something new to end of file
        file_space += wrote;
        if(file_space > fe->file_size){
             fe->file_size += (fe->file_size - file_space);
        }


        // we no longer need offset since by now we already used it and
        // every subsequent block will start at 0
        offset = 0;

        // still have more to write, get next block to use
        if (count > 0) {
            // no next entry in FAT, try to find a new available block to link
            if (disk->fat[fdb_index].entry == FAT_EOC) {
                int next_block = first_fit(fd);
                if (next_block == -1) {
                    return total;
                }

                disk->fat[fdb_index].entry = next_block;
            }
            fdb_index = disk->fat[fdb_index].entry;
        }
    }

    return total;
}

int fs_read(int fd, void *buffer, size_t count)
{
    // check if a disk is open and if fd is valid
    if (disk == NULL || fd > FS_OPEN_MAX_COUNT || fd < 0) {
        return -1;
    }

    // check if file descriptor has anything in it
    if (disk->file_descriptors[fd].file_entry == NULL) {
        return -1;
    }

    // assign used struct fields to variables for convenience
    size_t offset = disk->file_descriptors[fd].offset;
    uint16_t fdb_index = disk->file_descriptors[fd].file_entry->fdb_index;
    uint16_t dbs_index = disk->superblock->dbs_index;

    // find the index of the file's first data block and
    int db_index = find_index(fdb_index, dbs_index, &offset);

    // check if read is going to read a full datablock or a section
    // if it is only reading a section, read it and return
    if ((count + offset) < BLOCK_SIZE) {
        char bounce_bufer[BLOCK_SIZE];
        if (block_read(db_index, bounce_bufer) == -1) {
            return -1;
        }

        void *curr_offset = bounce_bufer + offset;
        if (memcpy(buffer, curr_offset, count) != NULL) {
            return count;
        }

        return 0;
    }

    // if read starts from mid-block read that portion and update db_index
    int position = 0;
    if (offset != 0) {
        char bounce_bufer[BLOCK_SIZE];

        if (block_read(db_index, bounce_bufer) == -1) {
            return -1;
        }

        void *curr_offset = bounce_bufer + offset;
        memcpy(buffer, curr_offset, (BLOCK_SIZE - offset));
        position = BLOCK_SIZE - offset;
        count -= position;

        if (fdb_index == FAT_EOC) {
            return -1;
        }
        offset = 0;

        fdb_index = disk->fat[fdb_index].entry; // get next file entry
        db_index = find_index(fdb_index, dbs_index, &offset);
    }

    // read full blocks while we can
    // position is for copying into a location after what we already have,
    // so that we dont overwrite what we had before, we also update
    // the db_index at the end so that we know which block to go to next
    while (count >= BLOCK_SIZE) {
        char bounce_bufer[BLOCK_SIZE];
        if (block_read(db_index, bounce_bufer) == -1) {
            return -1;
        }
        memcpy((buffer + position), bounce_bufer, BLOCK_SIZE);
        position += BLOCK_SIZE;

        // if there is no next FAT entry, return since there is no more to read
        if (fdb_index == FAT_EOC) {
            return -1;
        }

        offset = 0;
        fdb_index = disk->fat[fdb_index].entry;
        db_index = find_index(fdb_index, dbs_index, &offset);
        count -= BLOCK_SIZE;
    }

    // if we have a count left, then that means that we need to read
    // a portion of a final block
    if (count != 0) {
        char bounce_bufer[BLOCK_SIZE];
        if (block_read(db_index, bounce_bufer) == -1) {
            return -1;
        }

        memcpy((buffer + position), bounce_bufer, count);
        position += count;
    }

    return position;
}
