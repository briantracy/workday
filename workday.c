/*
    workday.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

enum return_codes {
    SUCCESS                   = 0,
    FILE_READ_FAILURE         = 1,
    FILE_WRITE_FAILURE        = 2,
    NO_MEMORY_ERROR           = 3,
    ILLEGAL_BEGIN_OPERATION   = 4,
    ILLEGAL_PAUSE_OPERATION   = 5,
    ILLEGAL_RESUME_OPERATION  = 6,
    ILLEGAL_END_OPERATION     = 7,
    ILLEGAL_ARGUMENT          = 8,
    TIME_RETRIEVE_ERROR       = 9
};
/*
    see man 3 ctime
    ctime returns a 26 byte string, including the newline and null terminator.
    Thu Nov 24 18:22:48 1986\n\0
*/
const int BYTES_PER_LINE = 25; // see man 3 ctime

char *DATA_FILE = "/Users/briantracy/Desktop/projects/workday/workday/file.txt";

int usage(void);
int version(void);

int begin(void);
int pause(void);
int resume(void);
int end(void);

int append_line(char *);
char *last_line(void);

char *current_time_str(void);

int testmain() {
    char *line = last_line();
    printf("[%s]", line);
    
    free(line);
    return 0;
}

int main(int argc, char *argv[])
{
    return testmain();
    
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    printf("%s", current_time_str());
    
    if (argc < 2) {
        printf("not enough args\n");
        return usage();
    }
    const char *routine = argv[1];
    
    if (strcmp(routine, "-h") == 0 || strcmp(routine, "--help") == 0) {
        return usage();
    }
    if (strcmp(routine, "-v") == 0 || strcmp(routine, "--version") == 0) {
        return usage();
    }
    if (strcmp(routine, "begin") == 0) {
        return begin();
    }
    if (strcmp(routine, "pause") == 0) {
        return pause();
    }
    if (strcmp(routine, "resume") == 0) {
        return resume();
    }
    if (strcmp(routine, "end") == 0) {
        return end();
    }
    
    fprintf(stderr, "Illegal argument: %s\n", routine);
    return ILLEGAL_ARGUMENT;
}

struct tm parse_time_str(char *buff)
{
    struct tm _tm;
    char *ret = strptime(buff, "%c", &_tm);
    
    if (*ret == '\0') {
        return _tm;
    }
    else {
        fprintf(stderr, "[ERROR] Cannot parse the following time string: %s", buff);
        exit(TIME_RETRIEVE_ERROR);
    }
}

struct tm *current_time(void)
{
    time_t now;
    time(&now);
    return localtime(&now);
}

char *current_time_str()
{
    time_t now;
    time(&now);
    return ctime(&now);
}

/*
    line is null terminated
*/
int append_line(char *line)
{
    FILE *file = fopen(DATA_FILE, "a");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] could not open file: %s\n", DATA_FILE);
        return FILE_WRITE_FAILURE;
    }
    
    if (fputs(line, file) < 0) {
        fprintf(stderr, "[ERROR] could not append to file: %s\n", DATA_FILE);
        return FILE_WRITE_FAILURE;
    }
    return SUCCESS;
}


/*
    Must free the result of this call.
*/
char *last_line(void)
{
    FILE *file = fopen(DATA_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] could not read from file: %s", DATA_FILE);
    }
    fseek(file, -BYTES_PER_LINE, SEEK_END);
    char *buff = malloc(sizeof(char) * BYTES_PER_LINE);
    fread(buff, BYTES_PER_LINE, 1, file);
    
    return buff;
}

int usage()
{
    printf("Helpful Commands:\n");
    printf("workday [begin|pause|resume|end]\n");
    printf("workday [-h|--help]\n");
    printf("workday [-v|--version]\n");
    return SUCCESS;
}

int version() {
    printf("workday version 0.9\n");
    return SUCCESS;
}

int begin() {
    printf("begin\n");
    return 0;
}

int pause() {
    printf("pause\n");
    return 0;
}

int resume() {
    printf("resume\n");
    return 0;
}

int end() {
    printf("end\n");
    return 0;
}
