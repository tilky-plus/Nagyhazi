#include "input.h"

int input_number() {
    int number = -1; // The default value is -1.
    scanf("%d", &number);

    /* Clear stdin */
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { };

    return number;
}

void input_str(const char *msg, char *buf, int size) {
    if (msg)
        printf("%s", msg);

    if (!fgets(buf, size, stdin)) {
        puts("Beolvasasi hiba (stdin)");
        return;
    }

    while (size && (buf[size - 1] == '\n' || buf[size - 1] == '\r'))
        buf[--size] = '\0';
}

bool simple_yesno(const char *msg) {
    char valasz[8];

    printf("%s (I/N): ", msg);
    input_str(valasz, valasz, sizeof(valasz));

    if (!(valasz[0] == 'I' || valasz[0] == 'i')) {
        puts("Megsem.");
        return false;
    }

    return true;
}

int choose_result(int min, int max) {
    /* Ask the user to choose a result */
    printf("Melyiket valasszam? (%d..%d, 0 = megsem): ", min, max);

    int kiv = input_number();

    if (kiv < min || kiv > max) {
        puts("Megsem.");
        return -1;
    }

    return kiv;
}