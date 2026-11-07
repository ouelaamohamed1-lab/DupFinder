
#define _DEFAULT_SOURCE
#define _CRT_SECURE_NO_WARNINGS     /* stops Visual Studio warnings on Windows */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
 
#ifdef _WIN32
    #include <windows.h>            /* Windows: folder functions */
    #define PATH_LEN MAX_PATH
    #define SEPARATOR "\\"
#else
    #include <dirent.h>             /* Linux and macOS: folder functions */
    #define PATH_LEN PATH_MAX
    #define SEPARATOR "/"
#endif
 
#define CHUNK 4096
 
typedef struct {
    char *path;
    long long size;
    unsigned long long hash;
} FileInfo;
 
static FileInfo *files = NULL;
static int count = 0;
static int capacity = 0;
 
/* ---------- storing files ---------- */
 
static void add_file(const char *path, long long size)
{
    if (count == capacity) {
        capacity = (capacity == 0) ? 64 : capacity * 2;
        FileInfo *tmp = realloc(files, capacity * sizeof(FileInfo));
        if (tmp == NULL) {
            perror("realloc");
            exit(1);
        }
        files = tmp;
    }
    files[count].path = strdup(path);
    files[count].size = size;
    files[count].hash = 0;
    count++;
}
 
/* ---------- scanning folders ---------- */
 
#ifdef _WIN32
 
static void scan_folder(const char *folder)
{
    char pattern[PATH_LEN];
    snprintf(pattern, sizeof(pattern), "%s\\*", folder);
 
    WIN32_FIND_DATAA data;
    HANDLE handle = FindFirstFileA(pattern, &data);
    if (handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Cannot open folder: %s\n", folder);
        return;
    }
 
    do {
        if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
            continue;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            continue;                           /* skip shortcuts and links */
 
        char path[PATH_LEN];
        snprintf(path, sizeof(path), "%s" SEPARATOR "%s", folder, data.cFileName);
 
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            scan_folder(path);                  /* go inside the subfolder */
        } else {
            long long size = ((long long)data.nFileSizeHigh << 32) | data.nFileSizeLow;
            if (size > 0)
                add_file(path, size);           /* skip empty files */
        }
    } while (FindNextFileA(handle, &data));
 
    FindClose(handle);
}
 
#else
 
static void scan_folder(const char *folder)
{
    DIR *dir = opendir(folder);
    if (dir == NULL) {
        fprintf(stderr, "Cannot open folder: %s\n", folder);
        return;
    }
 
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
 
        char path[PATH_LEN];
        snprintf(path, sizeof(path), "%s" SEPARATOR "%s", folder, entry->d_name);
 
        struct stat st;
        if (lstat(path, &st) != 0)
            continue;
 
        if (S_ISDIR(st.st_mode))
            scan_folder(path);                  /* go inside the subfolder */
        else if (S_ISREG(st.st_mode) && st.st_size > 0)
            add_file(path, st.st_size);         /* skip links and empty files */
    }
    closedir(dir);
}
 
#endif
 
/* ---------- hashing and comparing ---------- */
 
/* FNV-1a hash of the whole file content */
static unsigned long long hash_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        return 0;
 
    unsigned long long hash = 1469598103934665603ULL;
    unsigned char buffer[CHUNK];
    size_t n;
 
    while ((n = fread(buffer, 1, CHUNK, f)) > 0) {
        for (size_t i = 0; i < n; i++) {
            hash ^= buffer[i];
            hash *= 1099511628211ULL;
        }
    }
    fclose(f);
    return hash;
}
 
/* returns 1 if both files have exactly the same content */
static int same_content(const char *path1, const char *path2)
{
    FILE *f1 = fopen(path1, "rb");
    FILE *f2 = fopen(path2, "rb");
    if (f1 == NULL || f2 == NULL) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }
 
    unsigned char buf1[CHUNK], buf2[CHUNK];
    int result = 1;
 
    while (result) {
        size_t n1 = fread(buf1, 1, CHUNK, f1);
        size_t n2 = fread(buf2, 1, CHUNK, f2);
        if (n1 != n2 || memcmp(buf1, buf2, n1) != 0)
            result = 0;
        else if (n1 == 0)
            break;                              /* reached the end of both */
    }
 
    fclose(f1);
    fclose(f2);
    return result;
}
 
/* sort by size first, then by hash */
static int compare_files(const void *a, const void *b)
{
    const FileInfo *x = a;
    const FileInfo *y = b;
 
    if (x->size < y->size) return -1;
    if (x->size > y->size) return 1;
    if (x->hash < y->hash) return -1;
    if (x->hash > y->hash) return 1;
    return 0;
}
 
/* ---------- printing ---------- */
 
static void print_size(long long bytes)
{
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    double size = (double)bytes;
    int i = 0;
 
    while (size >= 1024 && i < 4) {
        size /= 1024;
        i++;
    }
    printf("%.2f %s", size, units[i]);
}
 
/* ---------- finding duplicates ---------- */
 
/*
 * Goes through the sorted list and handles each group of duplicates.
 * If delete_mode is 0, it only prints the groups.
 * If delete_mode is 1, it keeps the first file of each group and
 * deletes the others.
 * Returns the number of duplicate files found.
 */
static int process_duplicates(int delete_mode, long long *wasted)
{
    int duplicates = 0;
    int group_number = 0;
    int i = 0;
    *wasted = 0;
 
    while (i < count) {
        /* find the end of the run with the same size and hash */
        int j = i + 1;
        while (j < count && files[j].size == files[i].size
                         && files[j].hash == files[i].hash)
            j++;
 
        if (j - i > 1) {
            int printed_header = 0;
 
            for (int k = i + 1; k < j; k++) {
                if (!same_content(files[i].path, files[k].path))
                    continue;                   /* same hash but different file */
 
                if (!printed_header) {
                    group_number++;
                    if (!delete_mode) {
                        printf("Group %d  (", group_number);
                        print_size(files[i].size);
                        printf(" each)\n");
                        printf("  keep:   %s\n", files[i].path);
                    }
                    printed_header = 1;
                }
 
                if (delete_mode) {
                    if (remove(files[k].path) == 0)
                        printf("Deleted: %s\n", files[k].path);
                    else
                        printf("Could not delete: %s\n", files[k].path);
                } else {
                    printf("  copy:   %s\n", files[k].path);
                }
 
                duplicates++;
                *wasted += files[i].size;
            }
            if (printed_header && !delete_mode)
                printf("\n");
        }
        i = j;
    }
    return duplicates;
}
 
/* ---------- main ---------- */
 
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <folder> [--delete]\n", argv[0]);
        return 1;
    }
 
    int delete_flag = (argc >= 3 && strcmp(argv[2], "--delete") == 0);
 
    printf("Scanning %s ...\n", argv[1]);
    scan_folder(argv[1]);
    printf("Found %d files.\n\n", count);
 
    if (count == 0)
        return 0;
 
    /* sort by size so files with the same size are next to each other */
    qsort(files, count, sizeof(FileInfo), compare_files);
 
    /* only hash files that have another file with the same size */
    for (int i = 0; i < count; i++) {
        int same_as_prev = (i > 0 && files[i].size == files[i - 1].size);
        int same_as_next = (i < count - 1 && files[i].size == files[i + 1].size);
        if (same_as_prev || same_as_next)
            files[i].hash = hash_file(files[i].path);
    }
 
    /* sort again, now the hash is used too */
    qsort(files, count, sizeof(FileInfo), compare_files);
 
    long long wasted;
    int duplicates = process_duplicates(0, &wasted);
 
    if (duplicates == 0) {
        printf("No duplicates found.\n");
    } else {
        printf("%d duplicate files, wasting ", duplicates);
        print_size(wasted);
        printf(".\n");
 
        if (delete_flag) {
            char answer[16];
            printf("\nDelete the copies and keep one file per group? (y/n): ");
            if (fgets(answer, sizeof(answer), stdin) != NULL
                && (answer[0] == 'y' || answer[0] == 'Y')) {
                printf("\n");
                process_duplicates(1, &wasted);
                printf("\nFreed ");
                print_size(wasted);
                printf(".\n");
            } else {
                printf("Nothing was deleted.\n");
            }
        } else {
            printf("Run again with --delete to remove the copies.\n");
        }
    }
 
    for (int i = 0; i < count; i++)
        free(files[i].path);
    free(files);
    return 0;
}