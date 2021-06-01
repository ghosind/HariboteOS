#ifndef _FS_H_
#define _FS_H_

struct FileInfo {
  unsigned char name[8], ext[3], type;
  char reserve[10];
  unsigned short time, date, clustno;
  unsigned int size;
};

struct FileHandle {
  char *buf;
  int size;
  int pos;
};

void file_read_fat(int *fat, unsigned char *img);
void file_load_file(int clustno, int size, char *buf, int *fat, char *img);
struct FileInfo *file_search(char *name, struct FileInfo *finfo, int max);

#endif // _FS_H_
