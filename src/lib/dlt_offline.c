#include "dlt_offline.h"

#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#if !defined (__WIN32__)
#include <syslog.h> /* for LOG_... */
#endif


#define LOG_TMP_SIZE 2048
char log_tmp[LOG_TMP_SIZE];

int dlt_offline_scan_file(DltOfflineFileList **head, const char* path, const char* prefix)
{

    struct dirent **dirbuf = {0};
    int cnt = 0;
    char *pch = 0;
    int i = 0;
    DltOfflineFileList *currentFile = 0;
    int count_file = 0;
    if (path == 0 || prefix == 0)
    {
        return -1;
    }

    cnt = scandir(path, &dirbuf, 0, alphasort);
    if (cnt < 0)
    {
        snprintf(log_tmp, LOG_TMP_SIZE, "dlt_logstorage_storage_dir_info: Failed to scan directory: %s %s\n", path, strerror(errno));
        dlt_log(LOG_ERR, log_tmp);
        return -1;
    }

    for (i = 0; i < cnt; ++i)
    {
        pch = strstr((char*)dirbuf[i]->d_name, prefix);
        if (0 != pch)
        {
        //printf("%s =? %s\n", dirbuf[i]->d_name, prefix);
            count_file++;
            DltOfflineFileList *newFile = malloc(sizeof(DltOfflineFileList));
            memset(newFile, 0, sizeof(DltOfflineFileList));
            strcat(newFile->name, path);
            strcat(newFile->name, "/");
            strcat(newFile->name, dirbuf[i]->d_name);
            struct stat st;
            if (stat(newFile->name, &st) >= 0)
            {
                newFile->size = st.st_size;
            }
            else
            {
                snprintf(log_tmp, LOG_TMP_SIZE, "stat error: %s %s\n", newFile->name, strerror(errno));
                dlt_log(LOG_WARNING, log_tmp);
            }
            newFile->handle = -1;
            newFile->next = 0;
            if (currentFile == NULL)
            {
                *head = newFile;
            }
            else
            {
                currentFile->next = newFile;
            }
            currentFile = newFile;
        }
    }

    return count_file;
}

int dlt_offline_sort_file(DltOfflineFileList **head)
{
    int done = 0;
    if (head == 0 || *head == 0 || (*head)->next == 0)
    {
        return -1;
    }

    while (!done)
    {
        DltOfflineFileList **pv = head;
        DltOfflineFileList *nd = *head;
        DltOfflineFileList *nx = (*head)->next;

        done = 1;

        while (nx)
        {
            if (strlen(nd->name) > strlen(nx->name)
             || (strlen(nd->name) == strlen(nx->name) && strcmp(nd->name, nx->name) > 0) )
            {
                nd->next = nx->next;
                nx->next = nd;
                *pv = nx;

                done = 0;
            }
            pv = &nd->next;
            nd = nx;
            nx = nx->next;
        }
    }
    return 1;
}

DltOfflineFileList* dlt_offline_open_file(const char* filename)
{
    struct stat st;
    int handle = -1;
    DltOfflineFileList* offlineFile = 0;
    if (filename == 0)
    {
        return 0;
    }


    handle = open(filename,O_WRONLY|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (handle < 0)
    {
        sprintf(log_tmp, "%s file oepn error: %s\n", filename, strerror(errno));
        dlt_vlog(LOG_ERR, log_tmp);
        return 0;
    }

    offlineFile = malloc(sizeof(DltOfflineFileList));
    memset(offlineFile, 0, sizeof(DltOfflineFileList));
    offlineFile->handle = handle;

    strncpy(offlineFile->name, filename, DLT_MOUNT_PATH_MAX);
    offlineFile->name[DLT_MOUNT_PATH_MAX-1] = 0;
    offlineFile->next = 0;
    if (stat(filename, &st) < 0)
    {
        offlineFile->size = 0;
        sprintf(log_tmp, "stat error: %s %s\n", filename, strerror(errno));
        dlt_log(LOG_WARNING, log_tmp);
    }
    else
    {
        offlineFile->size = st.st_size;
    }
    return offlineFile;
}

void dlt_offline_close_file(DltOfflineFileList *file)
{
    close(file->handle);
}

int dlt_offline_write_file(DltOfflineFileList *file, unsigned char *data, int size)
{
    int len = write(file->handle, data, size);
    if (len != size)
    {
        dlt_vlog(LOG_ERR, "Wrote less data than specified");
    }
    else if (len < 0)
    {
        dlt_vlog(LOG_ERR, "file write error\n");
    }
    if (len > 0)
    {
        file->size += len;
    }

    return len;

}

DltOfflineFileList* dlt_offline_pop_file(DltOfflineFileList **head, int maxCountFile)
{
    DltOfflineFileList *curr = 0;
    if (head == 0 || *head ==0)
    {
        return 0;
    }
    if (maxCountFile <= 1)
    {
        return *head;
    }
    
    curr = *head;
    while (1)
    {
        --maxCountFile;
        if (maxCountFile <  0)
        {
            DltOfflineFileList *tmp = *head;
            *head = (*head)->next;
            dlt_offline_delete_file(tmp);
            free(tmp);
        }
        if (curr->next == 0)
        {
            break;
        }
        curr = curr->next;
    }

    return curr;
}

void dlt_offline_delete_file(DltOfflineFileList *file)
{
    remove(file->name);
}
