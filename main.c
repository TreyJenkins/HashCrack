#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#define STRINGIFY(x) #x
#define MACRO(x)     STRINGIFY(x)

int strcicmp(char const *a, char const *b) {
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a) {
            return d;
        }
    }
}

int usage(char const* argv[]) {
    printf("%s <HASH> <HASHTYPE> [OPTIONS]\nUSE --help FOR HELP AND OPTIONS\n", argv[0]);
    return 0;
}

int help(char const* argv[]) {
    usage(argv);
    printf("\nOptions:\n");
    printf("\t-l: Length of plaintext string (DEFAULT: 4)\n");
    printf("\t-m: String generator mode (DEFAULT: 0)\n");

    printf("\n");

    printf("\nHash Types:\n");
    printf("\t100: MD5\n");
    printf("\n");

    printf("\nString Generator Modes:\n");
    printf("\t0: Random string generation, lowercase\n");
    printf("\t1: Random string generation, uppercase\n");
    printf("\t2: Random string generation, numerical\n");
    printf("\t3: Random string generation, special\n");
    printf("\t4: Random string generation, modes 0, 1, 2, and 3 combined\n");

    printf("\n");

    return 0;
}


char* MD5SUM(char* string) {
    unsigned char result[MD5_DIGEST_LENGTH];

    MD5(string, strlen(string), result);

    const int MAX_BUF = 33;
    char* buffer = malloc(MAX_BUF);

    int length = 0;
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        length += snprintf(buffer+length, MAX_BUF-length, "%02X", result[i]);
    }

    return buffer;
}

char* CRACK(char* string, int hashtype) {
    switch (hashtype) {
        case 100:
            //printf("HASH TYPE: MD5\n");
            return MD5SUM(string);
        default:
            printf("UNKNOWN HASH TYPE: %i\n", hashtype);
            exit(2);
    }
}

char* randstring(int length, char* charset, int charsetlen) {
    char *randomString;

    randomString = malloc(sizeof(char) * (length +1));

    if (!randomString) {
        return (char*)0;
    }

    unsigned int key = 0;

    for (int n = 0;n < length;n++) {
        key = rand() % charsetlen;
        randomString[n] = charset[key];
    }

    randomString[length] = '\0';

    return randomString;
}

char* GENSTR(int mode, int length) {
    switch (mode) {
        case 0: // Random string generation, lowercase
            return randstring(length, "abcdefghijklmnopqrstuvwxyz", 26);
        case 1: // Random string generation, uppercase
            return randstring(length, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26);
        case 2: // Random string generation, numeric
            return randstring(length, "1234567890", 10);
        case 3: // Random string generation, special
            return randstring(length, "!@#$%^&*()-=_+`~[]\\{}|;':\",./<>? ", 33);
        case 4: // Random string generation, modes 0, 1, 2, and 3 combined
            return randstring(length, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+`~[]\\{}|;':\",./<>? ", 95);
        default:
            printf("UNKNOWN STRGEN MODE: %i\n", mode);
            exit(3);
    }
}

int main(int argc, char const *argv[]) {
    #ifdef DEBUG
    printf("+--------------------+\n| Build ID: %s |\n+--------------------+\n\n", MACRO(BUILDID));
    #endif

    int genmod = 0;
    int length = -1;

    for (size_t i = 0; i < argc; i++) {
        if (strcicmp(argv[i], "--help") == 0) {
            help(argv);
            exit(1);
        }
        if (strcicmp(argv[i], "-m") == 0) {
            genmod = atoi(argv[i + 1]);
        }
        if (strcicmp(argv[i], "-l") == 0) {
            length = atoi(argv[i + 1]);
        }
    }

    if (argc < 3) {
        printf("NOT ENOUGH ARGUMENTS\n\n");
        usage(argv);
        exit(1);
    }

    if (length < 0) {
        length = 4;
    }

    int ht = atoi(argv[2]);

    const char* inh = argv[1];

    srand(time(NULL));

    double begin = time(NULL);

    while (1) {
        char* string = GENSTR(genmod, length);
        char* buffer = CRACK(string, ht);

        printf("ATTEMPT    : %s\n", string);
        printf("CALCULATED : %s\n", buffer);
        printf("HASH       : %s\n", inh);

        if (strcicmp(inh, buffer) == 0) {
            double end = time(NULL);
            double time_spent = (end - begin);
            printf("\n** CRACKED IN %.0f SECONDS **\n", time_spent);
            break;
        }

        printf("\n");
    }

    return 0;
}
