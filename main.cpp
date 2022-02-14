#include <iostream>
#include <bits/stdc++.h>
using namespace std;

/*
#define BLOCK_SIZE 4096
#define NO_OF_BLOCKS 64
#define INODE_POINTER_SIZE 256
//we assign 5 blocks for inode
#define NO_OF_INODE_POINTERS 16
#define NO_OF_INODES 80
#define NO_OF_DATA_BLOCKS 56
#define NO_OF_INODE_BLOCKS 5
//1 block has-- 16 inode entries
//5 blocks will have -- 80 inode entries
*/

#define BLOCK_SIZE 524288
#define NO_OF_BLOCKS 1000
#define INODE_POINTER_SIZE 512
//we assign 5 blocks for inode
#define NO_OF_INODE_POINTERS 1024
#define NO_OF_INODES 5120
#define NO_OF_DATA_BLOCKS 56
#define NO_OF_INODE_BLOCKS 5
//1 block has-- 16 inode entries
//5 blocks will have -- 80 inode entries

vector<string> disks;

struct inodes
{
    string fileName;
    int data_block_pointer;
    int fileSize;
};

struct dataBlock
{
    char data[BLOCK_SIZE];
    int next_block_num;
};

struct superBlock
{
    int start_address_of_inode = 3 * BLOCK_SIZE;

    // struct dataBlock data_blocks_arr[NO_OF_DATA_BLOCKS];
    struct inodes inode_block_arr[NO_OF_INODES];

    vector<string> all_files;
    map<string, int> file_inode_num;
    int inode_bitmap[NO_OF_INODES];
    int data_bitmap[NO_OF_DATA_BLOCKS];
};

map<string, char> open_file_mode;
vector<string> open_files;
map<string, int> file_to_descriptor;
int free_descriptor = -1;
map<int, string> descriptor_to_file;

struct superBlock sb[20];
map<string, int> disk_superblock_num;
int disk_num = -1;
    int start_address_of_data = 8 * BLOCK_SIZE;

void create_disk(string disk_name)
{
    disk_num++;
    int sb_num = disk_num;
    disk_superblock_num[disk_name] = sb_num;

    char *d = &disk_name[0];
    FILE *disk_ptr = fopen(d, "w");
    char buf[BLOCK_SIZE];
    fseek(disk_ptr, 0, SEEK_SET);

    for (int i = 0; i < BLOCK_SIZE; i++)
        buf[i] = 0;

    for (int i = 0; i < NO_OF_BLOCKS; i++)
    {
        fwrite(buf, sizeof(char), BLOCK_SIZE, disk_ptr);
    }
    for (int i = 0; i < NO_OF_INODES; i++)
    {
        sb[sb_num].inode_bitmap[i] = 0;
    }

    for (int i = 0; i < NO_OF_INODES; i++)
    {
        sb[sb_num].inode_block_arr[i].fileName = "";
        sb[sb_num].inode_block_arr[i].data_block_pointer = -1;
        sb[sb_num].inode_block_arr[i].fileSize = 0;
    }

    for (int i = 0; i < NO_OF_DATA_BLOCKS; i++)
    {
        sb[sb_num].data_bitmap[i] = 0;
        // sb.data_blocks_arr[i].next_block_num = -1;
    }

    fclose(disk_ptr);
}

int find_free_inode(string disk_name)
{
    int sb_num = disk_superblock_num[disk_name];
    for (int i = 0; i < NO_OF_INODES; i++)
    {
        if (sb[sb_num].inode_bitmap[i] == 0)
            return i;
    }
    return -1;
}

int find_free_data_block(string disk_name)
{
    int sb_num = disk_superblock_num[disk_name];
    for (int i = 0; i < NO_OF_DATA_BLOCKS; i++)
    {
        if (sb[sb_num].data_bitmap[i] == 0)
            return i;
    }
    return -1;
}

void create_file(string file_name, string disk_name)
{
    int ino = find_free_inode(disk_name);
    int data = find_free_data_block(disk_name);
    if (ino == -1 || data == -1)
    {
        cout << "No Space is available on the disk." << endl;
    }
    else
    {
        free_descriptor++;
        file_to_descriptor[file_name]=free_descriptor;
        descriptor_to_file[free_descriptor]=file_name;

        int sb_num = disk_superblock_num[disk_name];
        sb[sb_num].all_files.push_back(file_name);
        sb[sb_num].inode_block_arr[ino].fileName = file_name;
        sb[sb_num].inode_block_arr[ino].data_block_pointer = data;
        sb[sb_num].file_inode_num[file_name] = ino;
        sb[sb_num].inode_bitmap[ino] = 1;
        sb[sb_num].data_bitmap[data] = 1;
    }
}

int check_open(string file_name)
{
    for (int i = 0; i < open_files.size(); i++)
    {
        if (open_files[i] == file_name)
            return 1;
    }
    return 0;
}

void open_file(string file_name, string disk_name)
{
    int x = check_open(file_name);
    if (x == 1)
    {
        cout << "The File is already Open" << endl;
    }
    else
    {
        if(file_to_descriptor.find(file_name)==file_to_descriptor.end())
        {
        free_descriptor++;
        file_to_descriptor[file_name]=free_descriptor;
        descriptor_to_file[free_descriptor]=file_name;
        }
        open_files.push_back(file_name);
        cout << "In which mode do you want to open the file?" << endl;
        cout << "1.Read - r\n2.Write - w\n3.Append - a" << endl;
        char ch;
        cin >> ch;
        open_file_mode[file_name] = ch;
        cout<<"File opened with descriptor: "<<file_to_descriptor[file_name]<<endl;
    }
}

void close_file(string file_name)
{
    for (auto i = open_files.begin(); i != open_files.end(); i++)
    {
        if (*i == file_name)
        {
            open_files.erase(i);
            break;
        }
    }

    for (auto i = open_file_mode.begin(); i != open_file_mode.end(); i++)
    {
        if (i->first == file_name)
        {
            open_file_mode.erase(file_name);
            break;
        }
    }
}

void write_file(string file_name, string disk_name)
{
    int x = check_open(file_name);
    if (x == 0)
    {
        cout << "the file is not open" << endl;
    }
    else if (open_file_mode[file_name] != 'w')
    {
        cout << "The file is not open in Write mode" << endl;
    }
    else
    {
        int sb_num = disk_superblock_num[disk_name];
        int ino = sb[sb_num].file_inode_num[file_name];
        int data_block_num = sb[sb_num].inode_block_arr[ino].data_block_pointer;
        string line;
        cout << "Enter Text: ";
        getline(cin >> ws, line,'$');
        int len = line.length();
        char *buff = &line[0];
        // cout<<buff;
        sb[sb_num].inode_block_arr[ino].fileSize = len;
        int j = 8 * BLOCK_SIZE;

        char *d = &disk_name[0];
        FILE *file_ptr = fopen(d, "r+");
        fseek(file_ptr, j + data_block_num * BLOCK_SIZE, SEEK_SET);
        fwrite(buff, sizeof(char), len, file_ptr);
        fclose(file_ptr);
    }
}

void read_file(string file_name, string disk_name)
{
    int x = check_open(file_name);
    if (x == 0)
    {
        cout << "the file is not open" << endl;
    }
    else if (open_file_mode[file_name] != 'r')
    {
        cout << "The file is not open in read mode" << endl;
    }
    else
    {
        int sb_num = disk_superblock_num[disk_name];
        int ino = sb[sb_num].file_inode_num[file_name];
        int data_block_num = sb[sb_num].inode_block_arr[ino].data_block_pointer;
        int len = sb[sb_num].inode_block_arr[ino].fileSize;
        int j = 8 * BLOCK_SIZE;
        // cout<<"INO: "<<ino<<" Length: "<<len<<endl;

        char *d = &disk_name[0];
        FILE *file_ptr = fopen(d, "r");
        fseek(file_ptr, j + data_block_num * BLOCK_SIZE, SEEK_SET);
        char buff[len];
        fread(buff, sizeof(char), len, file_ptr);
        for(int i=0;i<len;i++)
            cout << buff[i];
        cout<<endl;
        fclose(file_ptr);
    }
}

void append_file(string file_name, string disk_name)
{
    int x = check_open(file_name);
    if (x == 0)
    {
        cout << "the file is not open" << endl;
    }
    else if (open_file_mode[file_name] != 'a')
    {
        cout << "The file is not open in append mode" << endl;
    }
    else
    {
        int sb_num = disk_superblock_num[disk_name];
        int ino = sb[sb_num].file_inode_num[file_name];
        int data_block_num = sb[sb_num].inode_block_arr[ino].data_block_pointer;
        int sz = sb[sb_num].inode_block_arr[ino].fileSize;
        string line;
        cout << "Enter Text: ";
        getline(cin >> ws, line,'$');
        int len = line.length();
        char *buff = &line[0];
        // cout<<buff;
        sb[sb_num].inode_block_arr[ino].fileSize = sz + len;
        int j = 8 * BLOCK_SIZE;

        char *d = &disk_name[0];
        FILE *file_ptr = fopen(d, "r+");
        fseek(file_ptr, j+ data_block_num * BLOCK_SIZE + sz, SEEK_SET);
        fwrite(buff, sizeof(char), len, file_ptr);
        fclose(file_ptr);
    }
}

void list_all_files(string disk_name)
{
    int sb_num = disk_superblock_num[disk_name];
    for (int i = 0; i < sb[sb_num].all_files.size(); i++)
        cout << sb[sb_num].all_files[i] << endl;

    if (sb[sb_num].all_files.size() == 0)
        cout << "No File has been created" << endl;
}

void list_all_open_files()
{
    for (int i = 0; i < open_files.size(); i++)
        cout << open_files[i] << " \tMode: " << open_file_mode[open_files[i]] << "\tFile Descriptor: "<<file_to_descriptor[open_files[i]]<<endl;

    if (open_files.size() == 0)
        cout << "No files is open" << endl;
}

void delete_file(string file_name, string disk_name)
{
    int sb_num = disk_superblock_num[disk_name];
    int ino = sb[sb_num].file_inode_num[file_name];
    int data_block_num = sb[sb_num].inode_block_arr[ino].data_block_pointer;
    int j = 8 * BLOCK_SIZE;

    char *d = &disk_name[0];
    FILE *file_ptr = fopen(d, "r+");
    fseek(file_ptr, j + data_block_num * BLOCK_SIZE, SEEK_SET);
    char buf[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++)
        buf[i] = 0;
    fwrite(buf, sizeof(char), BLOCK_SIZE, file_ptr);
    fclose(file_ptr);

    sb[sb_num].inode_block_arr[ino].fileSize = 0;
    sb[sb_num].inode_block_arr[ino].fileName = "";
    sb[sb_num].inode_block_arr[ino].data_block_pointer = -1;
    sb[sb_num].inode_bitmap[ino] = 0;
    sb[sb_num].data_bitmap[data_block_num] = 0;
    int x = check_open(file_name);
    if (x == 1)
        close_file(file_name);

    for (auto i = sb[sb_num].all_files.begin(); i != sb[sb_num].all_files.end(); i++)
    {
        if (*i == file_name)
        {
            sb[sb_num].all_files.erase(i);
            break;
        }
    }
}

void unmount_disk(string disk_name)
{
    open_files.clear();
    open_file_mode.clear();
    file_to_descriptor.clear();
    descriptor_to_file.clear();
    free_descriptor = -1;
    
    int sb_num=disk_superblock_num[disk_name];

    char *d=&disk_name[0];
    FILE *disk_ptr = fopen(d, "r+");

    fseek(disk_ptr, 0, SEEK_SET);
    fwrite(&sb[sb_num], sizeof(char), sizeof(sb[sb_num]), disk_ptr);

    fclose(disk_ptr);
}

void file_operations(string disk_name)
{
    string fileName;
    int dp;
    int ch;
    do
    {
        cout << "\n1.Create File\t\t2.Open File\n3.Read File\t\t4.Write File\n5.Append File\t\t6.Close File\n7.Delete File\t\t8.List all Open Files\n9.List all Files\t10.Unmount Disk\n";
        cin >> ch;
        switch (ch)
        {
        case 1:
            cout << "Enter File name: ";
            cin >> fileName;
            create_file(fileName, disk_name);
            break;
        case 2:
            cout << "Enter File name: ";
            cin>>fileName;
            open_file(fileName, disk_name);
            break;
        case 3:
            cout << "Enter File Descriptor: ";
            cin >> dp;
            fileName=descriptor_to_file[dp];
            read_file(fileName, disk_name);
            break;
        case 4:
            cout << "Enter File Descriptor: ";
            cin >> dp;
            fileName=descriptor_to_file[dp];
            write_file(fileName, disk_name);
            break;
        case 5:
            cout << "Enter File Descriptor: ";
            cin >> dp;
            fileName=descriptor_to_file[dp];
            append_file(fileName, disk_name);
            break;
        case 6:
            cout << "Enter File Descriptor: ";
            cin >> dp;
            fileName=descriptor_to_file[dp];
            close_file(fileName);
            break;
        case 7:
            cout << "Enter File name: ";
            cin >> fileName;
            delete_file(fileName, disk_name);
            break;
        case 8:
            list_all_open_files();
            break;
        case 9:
            list_all_files(disk_name);
            break;
        case 10:
            unmount_disk(disk_name);
        }
    } while (ch != 10);
}

void mount_disk(string disk_name)
{
    int sb_num = disk_superblock_num[disk_name];
    // cout << "Disk number:: " << sb_num << endl;

    // char *d=&disk_name[0];
    // FILE *disk_ptr = fopen(d, "r");
    // struct superBlock sb;

    // //write superblock 1 block
    // fseek(disk_ptr, 0, SEEK_SET);
    // fread(&sb, sizeof(char), BLOCK_SIZE, disk_ptr);

    // fclose(disk_ptr);

    file_operations(disk_name);
}

int main()
{
    int ch;
    string disk_name;
    do
    {
        cout << "1.Create Disk\n2.Mount Disk\n3.Exit\n";
        cin >> ch;
        switch (ch)
        {
        case 1:
            cout << "Enter Disk name:  ";
            cin >> disk_name;
            disks.push_back(disk_name);
            create_disk(disk_name);
            break;
        case 2:
            cout << "Enter Disk name:  ";
            cin >> disk_name;
            mount_disk(disk_name);
            break;
        }
    } while (ch != 3);

    return 0;
}