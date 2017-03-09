#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/io.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>


#define FILE_PATH_NAME "sdk-log-infomation.log"
#define FILE_PATH_TMP_NAME "sdk-log-format.txt"
#define LINE_CMPARE "commit "
#define MD5_SAMPLE "1a7d2d5ef427701ef33e7c50d9ede3fbbbc3d1bd"

unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;  
    FILE *fp;  
    fp = fopen(path, "r");  
    if(fp == NULL)  
        return filesize;  
    fseek(fp, 0L, SEEK_END);  
    filesize = ftell(fp);  
    fclose(fp);  
    return filesize;  
} 

int main()
{
	/*
	off_t lseek(int filedes, off_t offset, int whence);
	ssize_t read(int fd,void *buf,size_t count)  
	int create(const char *parhname,mode_t mode);
	int open(const char * pathname,int flags);
	
		其中，flags为用户设置 的标准，其可能值为：O_RDONLY（只读），O_WRONLY(只写），O_RDWR(读写），O_APPEND(追加)
		O_CREAT(不在则创建)等等，这里压迫特别注意的是，当我们选择了O_CREAT时，则要在mode参数中设定新文件的权限。
	*/
		
	/*
		fd=open(name, flag, mode);

		if(fd<0)

		.........

		ptr=mmap(NULL, len , PROT_READ|PROT_WRITE, MAP_SHARED , fd , 0);

		..........

		munmap(ptr,len);

		close(fd);
	*/
	int fd = -1, fdFormat = -1;
	unsigned int i = 0;
	char first = 1;
	char *ptr = NULL;
	char *loop = NULL;
	unsigned long len = get_file_size(FILE_PATH_NAME); 
	unsigned int lenCmp = strlen(LINE_CMPARE);
	unsigned int md5Len = strlen(MD5_SAMPLE);
	char buf1[64] = {0};
	char tmp[64] = {0};
	char *pBuf1 = buf1;
	char *pTmp = tmp;
	char buf[512] = {0};

	fd = open(FILE_PATH_NAME, O_RDWR);
	if(fd < 0)
	{
		printf("faild to open file log!\n");
		return;
	}
	ptr = mmap(NULL, len , PROT_READ|PROT_WRITE, MAP_SHARED , fd , 0);
	
	fdFormat = open(FILE_PATH_TMP_NAME, O_RDWR|O_CREAT, 0777);
	if(fdFormat < 0)
	{
		printf("faild to open file format!\n");
		return;
	}	

	for(loop = ptr; *loop != '|'; loop++)
	{
		
		if(strncmp(LINE_CMPARE, loop, lenCmp) == 0)
		{
			//printf("loop == %#2x \n", *loop);
			memset(pBuf1, 0, sizeof(pBuf1));
			memcpy(pBuf1, (loop + lenCmp), md5Len);
			if(first)
			{
				first = 0;
				memset(pTmp, 0, sizeof(pTmp));
				memcpy(pTmp, pBuf1, md5Len);
				//printf("buf1 == %s \n", pBuf1);
				//printf(" tmp == %s \n", pTmp);
				continue;
			}
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "git diff %-40s..%-40s > /home/chenlong/work/sdk-diff/sdk-diff-%d.txt\n", pBuf1, pTmp, ++i);
			write(fdFormat, buf, strlen(buf));
			memset(pTmp, 0, sizeof(pTmp));
			memcpy(pTmp, pBuf1, md5Len);
			
		}
	}

	munmap(ptr,len);
	ptr == NULL;
	loop == NULL;
	close(fdFormat);
	close(fd);

	return;
}