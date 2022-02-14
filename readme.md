# Inode based file system Implementation

## Create Disk
Creates an empty disk of size 500Mb and assigns a unique name to it which will be used to mount it.

## Mount Disk and Unmount Disk
- When we mount the disk, the superblock information and inode information is read from the disk.
- The specified disk is then opened for various file operations.
- When we unmount the disk, the updated data blocks and the inodes along with the superblock information is saved back on the disk.

## File Operations - create file
- The user is prompted to enter the name for the file which is allocated the first free data block on the disk.
- The bit corresponding to the block in the inode bitmap and datablock bitmap are set.

## Open file and close file
- The user is prompted to enter the file name to be opened and asked to select the mode in which they want to open the file.
-- 'r' : read mode
-- 'w ' : write mode
-- 'a' : append mode
- A file descriptor is allocated to the file.
- To close the file, the user is asked to enter the file descriptor allocated to it.

## list all files and list all the open files
- list all files displays all the files that have been created on the specific disk.
- The list all open files command, displays all the files that have been opened along with the mode in which they have been opened and the file descriptor allocated to them.

## delete file
- The user is asked to enter the file name to delete.
- The file is then deleted from the disk and inode and data block allocated to it are released.
