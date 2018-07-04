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
    TIME_RETRIEVE_ERROR       = 9,
    UNKNOWN_OPERATION_IN_LOG  = 10
};
/*
    see man 3 ctime
    ctime(3) returns a 26 byte string, including the newline and null terminator.
    workday prefixes this with two bytes, one byte for the event type and one byte for
    a space. See the following example for a 'B'(eginning) event.
 
        B Thu Nov 24 18:22:48 1986\n\0
         2          + 26               = 28
 
    So, each line in the log file will occupy 27 bytes as the null terminator is not written
    to file.
*/
/// excludes null terminator
const size_t LOGFILE_BYTES_PER_LINE = 27; // excludes null terminator!


char *LOG_FILE = "/Users/briantracy/Desktop/projects/workday/workday/file.txt";

int usage(void);
int version(void);

int begin(void);
int pause(void);
int resume(void);
int end(void);

int append_line(char *);
char *last_line(void);
int emit_event(char event);

char *current_time_str(void);


int testmain() {
    
    char *line = last_line();
    printf("[%s]\n", line);
    
    free(line);
    return 0;
}

int main(int argc, char *argv[])
{
    char buff[LOGFILE_BYTES_PER_LINE + 1];
    snprintf(buff, LOGFILE_BYTES_PER_LINE, "%c %s\n", 'I', current_time_str());
    printf("[%d]\n", buff[LOGFILE_BYTES_PER_LINE]);
    printf("[%s]\n", buff);
    
    char buff1[] = { 'a', 'b', 'c', '\0', 'd', '\0' };
    printf("[%s]\n", buff1);
    
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
    struct tm time;
    char *ret = strptime(buff, "%c", &time);
    // first char of output from strptime is \0 on success.
    // read the man page man 3 strptime
    if (*ret == '\0') {
        return time;
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

/// 26 bytes including null terminator.
char *current_time_str()
{
    time_t now;
    time(&now);
    return ctime(&now);
}

/*
    line must be null terminated
*/
int append_line(char *line)
{
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] could not open file: %s\n", LOG_FILE);
        return FILE_WRITE_FAILURE;
    }
    
    if (fputs(line, file) < 0) {
        fprintf(stderr, "[ERROR] could not append to file: %s\n", LOG_FILE);
        return FILE_WRITE_FAILURE;
    }
    return SUCCESS;
}

int emit_event(char event)
{
    char buff[LOGFILE_BYTES_PER_LINE + 1];
    snprintf(buff, LOGFILE_BYTES_PER_LINE, "%c %s\n", event, current_time_str());
    
    return append_line(buff);
}


/*
    Must free the result of this call.
    Returns NULL iff there is an error opening the file.
    Return value is null terminated and does not include the
    newline at the end of the line.
*/
char *last_line(void)
{
    FILE *file = fopen(LOG_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] could not read from file: %s\n", LOG_FILE);
        return NULL;
    }
    fseek(file, -LOGFILE_BYTES_PER_LINE, SEEK_END);
    char *buff = malloc(sizeof(char) * LOGFILE_BYTES_PER_LINE); // includes space for \0
    fread(buff, LOGFILE_BYTES_PER_LINE, 1, file);
    
    buff[LOGFILE_BYTES_PER_LINE - 1] = '\0';
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
    char *last_entry = last_line();
    if (last_entry == NULL) {
        fprintf(stderr, "[ERROR] Aborting workday begin operation\n");
        return FILE_READ_FAILURE;
    }
    
    char op = last_entry[0];
    switch (op) {
        case 'B':
        case 'P':
        case 'R': {
            fprintf(stderr, "[ERROR] Cannot begin workday in this state: %c\n", op);
            return ILLEGAL_BEGIN_OPERATION;
        }
        case 'E': {
            
        }
    }
    
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
