#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

void to_upper_naive(char *const p_str, const unsigned str_len)
{
    static const unsigned DIFF = 'a' - 'A';

    for (
        char *p_char = p_str, *p_end = p_str + str_len;
        p_char < p_end;
        ++p_char
    ) {
        if (*p_char >= 'a' && *p_char <= 'z')
            *p_char -= DIFF;
    }
}

void to_upper_branchless(char *const p_str, const unsigned str_len)
{
    static const unsigned DIFF = 'a' - 'A';

    for (
        char *p_char = p_str, *p_end = p_str + str_len;
        p_char < p_end;
        ++p_char
    ) {
        *p_char -= (DIFF * (*p_char >= 'a' && *p_char <= 'z'));
    }
}

void to_upper_c_std(char *const p_str, const unsigned str_len)
{
    for (
        char *p_char = p_str, *p_end = p_str + str_len;
        p_char < p_end;
        ++p_char
    ) {
        *p_char = toupper((unsigned char)*p_char);
    }
}

// `str_len` is the number of ASCII characters requested.
// No null terminator is appended.
// `p_str` must point to an already allocated memory block of size `str_len` bytes.
void generate_random_string(char *const p_str, const unsigned str_len)
{
    static const char set[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const unsigned set_size = sizeof(set) - 1;

    if (!str_len) return;

    for (
        char *p_char = p_str, *p_end = p_str + str_len;
        p_char < p_end;
        ++p_char
    ) {
        unsigned idx_selected = rand() % set_size;
        *p_char = set[idx_selected];
    }
}

void benchmark(
    void const (*func)(char *const, const unsigned),
    const char const *func_label,
    const unsigned rounds,
    const size_t string_len
) {
    printf("function: %s", func_label);
    fflush(stdout);

    char *const p_str = malloc(string_len);
    char *const p_correct_output = malloc(string_len);
    char *const p_output = malloc(string_len);

    clock_t total_time_clocks = 0;
    for (unsigned i = 0; i < rounds; ++i) {
        generate_random_string(p_str, string_len);

        strncpy(p_output, p_str, string_len);
        clock_t t = clock();
        func(p_output, string_len);
        total_time_clocks += clock() - t;

        strncpy(p_correct_output, p_str, string_len);
        to_upper_c_std(p_correct_output, string_len);
        if (strncmp(p_output, p_correct_output, string_len)) {
            printf(" | wrong output\n");
            free(p_str);
            free(p_correct_output);
            free(p_output);
            return;
        }
    }

    double total_time_us = 1000 * 1000 * ((double)total_time_clocks) / CLOCKS_PER_SEC;
    double per_call_time_us = total_time_us / rounds;
    printf(" | time: %.3f microseconds\n", per_call_time_us);

    free(p_str);
    free(p_correct_output);
    free(p_output);
}

void print_usage(const char const *argv0)
{
    printf("\nUsage: %s N L\n", argv0);
    printf(
        "    N = number of test rounds\n"
        "    L = length of test string\n"
    );
    printf("\nExample: %s 500 1000000\n", argv0);
}

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Invalid invocation.\n");
        print_usage(argv[0]);
        return 1;
    }
    const unsigned test_rounds = strtoul(argv[1], NULL, 10);
    if (!test_rounds) {
        fprintf(stderr, "Error: N must be a non-zero positive integer.\n");
        print_usage(argv[0]);
        return 1;
    }
    const unsigned test_string_len = strtoul(argv[2], NULL, 10);
    if (!test_string_len) {
        fprintf(stderr, "Error: L must be a non-zero positive integer.\n");
        print_usage(argv[0]);
        return 1;
    }

    srand((unsigned) time(NULL));

    benchmark(to_upper_naive, "naive", test_rounds, test_string_len);
    benchmark(to_upper_branchless, "branchless", test_rounds, test_string_len);
    benchmark(to_upper_c_std, "c std", test_rounds, test_string_len);

    return 0;
}
