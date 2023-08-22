#ifndef DLT_OFFLINE__
#define DLT_OFFLINE__

#include "dlt_common.h"

typedef struct DltOfflineFileList
{
    char name[DLT_MOUNT_PATH_MAX];
    int handle;
    long long size;
    struct DltOfflineFileList *next;
}DltOfflineFileList;

int dlt_offline_scan_file(DltOfflineFileList **head, const char* path, const char* prefix);
int dlt_offline_sort_file(DltOfflineFileList **head);

DltOfflineFileList* dlt_offline_open_file(const char* filename);
void dlt_offline_close_file(DltOfflineFileList *file);
int dlt_offline_write_file(DltOfflineFileList *file, unsigned char *data, int size);
DltOfflineFileList* dlt_offline_pop_file(DltOfflineFileList **head, int maxCountFile);
void dlt_offline_delete_file(DltOfflineFileList *file);

#endif // DLT_OFFLINE__

