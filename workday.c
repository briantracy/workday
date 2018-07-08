/*
    workday.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

enum exit_codes {
    SUCCESS                   = 0,
    FILE_READ_FAILURE         = 1,
    FILE_WRITE_FAILURE        = 2,
    NO_MEMORY_ERROR           = 3,
    ILLEGAL_OPERATION         = 4,
    ILLEGAL_ARGUMENT          = 5,
    TIME_RETRIEVE_ERROR       = 6,
    UNKNOWN_OPERATION_IN_LOG  = 7,
    MALFORMED_LOG_FILE        = 8
};

const char EVENT_BEGIN  = 'B';
const char EVENT_PAUSE  = 'P';
const char EVENT_RESUME = 'R';
const char EVENT_END    = 'E';
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

/// this is a feaux constant because it can be changed at runtime
/// depending on the --logfile argument
char *LOG_FILE = "/Users/briantracy/Desktop/projects/workday/workday/file.txt";

// Public API //
int usage(void);
int version(void);
int begin(void);
int pause(void);
int resume(void);
int end(void);
int statistics(char *);
// End Public API //

// Core IO //
int append_line(char *);
char *last_line(void);
int emit_event(char);
int generic_event(char);
// End Core IO //

// Time Functions //
char *current_time_str(void);
struct tm parse_time_str(char *);
struct tm current_time(void);
// End Time Functions //

char *acceptable_previous_events(char);

void print_malformed_log_message(void);

void p(char *s) {
    char c = *s;
    printf("[ ");
    while (c != '\0') {
        printf("(%d,'%c') ", c, c);
        c = *(++s);
    }
    printf("]\n");
}

int testmain() {
    p(last_line());
    printf("%d", strcmp("", last_line()));
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

struct tm current_time(void)
{
    time_t now;
    time(&now);
    return *localtime(&now);
}

/// 26 bytes including null terminator.
char *current_time_str()
{
    time_t now;
    time(&now);
    return ctime(&now);
}

/// line shall be null terminated
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

/// Emits the given event at the current time.
int emit_event(char event)
{
    /// allocate extra space for '\n' and '\0'
    char buff[LOGFILE_BYTES_PER_LINE + 2];
    snprintf(buff, LOGFILE_BYTES_PER_LINE + 1, "%c %s\n", event, current_time_str());
    
    return append_line(buff);
}

///  Must free the result of this call.
///  Returns NULL iff there is an error opening the file.
///  Return value is null terminated and does not include the
///  newline at the end of the line.
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
    
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        // empty log file, return empty string
        buff[0] = '\0';
        return buff;
    }
    
    // make sure we are looking at a properly formatted line.
    // all lines start with an EVENT_
    switch (buff[0]) {
        case EVENT_BEGIN:
        case EVENT_PAUSE:
        case EVENT_RESUME:
        case EVENT_END: {
            assert(buff[LOGFILE_BYTES_PER_LINE - 2] != '\n');
            return buff;
        }

        default: {
            print_malformed_log_message();
            return NULL;
        }
    }
}

void print_malformed_log_message(void)
{
    fprintf(stderr, "[ERROR] Cannot parse logfile: %s\n", LOG_FILE);
    fprintf(stderr, "[ERROR] The logfile must have an entry on each line.\n");
    fprintf(stderr, "[ERROR] The logfile must be ended with a single newline character.\n");
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
    printf("[INFO] Beginning workday...\n");
    return generic_event(EVENT_BEGIN);
}

int pause() {
    printf("[INFO] Pausing workday...\n");
    return generic_event(EVENT_PAUSE);
}

int resume() {
    printf("[INFO] Resuming workday...\n");
    return generic_event(EVENT_RESUME);
}

int end() {
    printf("[INFO] Ending workday...\n");
    return generic_event(EVENT_END);
}
/// returns a string (modeling a set) of the
/// legitimate events than can preceed the given event.
/// a_p_e('B') -> "E"
char *acceptable_previous_events(char event)
{
    char *buff = malloc(sizeof(char) * 3);
    
    switch (event) {
        case EVENT_BEGIN: {
            snprintf(buff, 2, "%c", EVENT_END);
            break;
        }
        case EVENT_PAUSE: {
            snprintf(buff, 2, "%c", EVENT_BEGIN);
            break;
        }
        case EVENT_RESUME: {
            snprintf(buff, 2, "%c", EVENT_PAUSE);
            break;
        }
        case EVENT_END: {
            snprintf(buff, 3, "%c%c", EVENT_BEGIN, EVENT_RESUME);
            break;
        }
        default: {
            fprintf(stderr, "[ERROR] Cannot determine valid previous events for illegal event: %c\n", event);
            assert(0);
        }
    }
    
    return buff;
}

int generic_event(char event)
{
    char *last_entry = last_line();
    if (last_entry == NULL) {
        fprintf(stderr, "[ERROR] Aborting operation: %c", event);
        return FILE_READ_FAILURE;
    }
    char previous_event = last_entry[0];
    free(last_entry);
    
    if (strcmp(last_entry, "") == 0) {
        // empty file, 
    }
    
    if (strchr(acceptable_previous_events(event), previous_event) == NULL) {
        fprintf(stderr, "[ERROR] Illegal Operation: %c\n", event);
        fprintf(stderr, "[ERROR] You are not in an appropriate state to perform the requested operation\n");
        return ILLEGAL_OPERATION;
    }
    
emit:
    return emit_event(event);
}


