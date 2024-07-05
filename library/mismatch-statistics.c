#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

size_t num_tilings = 1;
char *mismatch_filename;

size_t compute_mean_mismatches(uint8_t *mismatches) {
    size_t total = 0;
    for (size_t i = 0; i < num_tilings; i++) {
        total += mismatches[i];
    }
    return (double) total / num_tilings;
}

size_t compute_min_incorrect_mismatches(uint8_t *mismatches) {
    size_t min = (size_t) __DBL_MAX__;
    for (size_t i = 0; i < num_tilings; i++) {
        if (mismatches[i] < min && mismatches[i] > 0) {
            min = mismatches[i];
        }
    }
    return min;
}

size_t compute_exp_mean_mismatches(uint8_t *mismatches) {
    size_t total = 0;
    for (size_t i = 0; i < num_tilings; i++) {
        total += exp(mismatches[i]);
    }
    return (double) total / num_tilings;
}

size_t compute_num_valid_solutions(uint8_t *mismatches) {
    size_t num_valid_sols = 0;
    for (size_t i = 0; i < num_tilings; i++) {
        if (mismatches[i] == 0) {
            num_valid_sols++;
        }
    }
    return num_valid_sols;
}

double compute_percentage_valid(size_t num_valid) {
    return (double) num_valid / num_tilings;
}

size_t *compute_valid_solutions(uint8_t *mismatches, size_t num_valid) {
    size_t *valid_arrangements = malloc(sizeof(size_t) * num_valid);
    size_t j = 0;
    for (size_t i = 0; i < num_tilings && j < num_valid; i++) {
        if (mismatches[i] == 0) {
            valid_arrangements[j] = i;
            j++;
        }
    }
    return valid_arrangements;
}

int write_statistics(uint8_t *mismatch_enumerations) {

    // Calculate required buffer size (excluding null-terminator)
    char *raw_mismatch_filename = malloc(sizeof(char) * strlen(mismatch_filename));
    strncpy(raw_mismatch_filename, mismatch_filename, strlen(mismatch_filename) - 4);
    raw_mismatch_filename[strlen(mismatch_filename) - 4] = '\0';
    size_t string_size = snprintf(NULL, 0, "%s-statistics.txt", raw_mismatch_filename);
    // Allocate buffer including null-terminator
    char *statistics_filename = (char *)malloc(string_size + 1);
    // Format the string and store it in buffer
    snprintf(statistics_filename, string_size + 1, "%s-statistics.txt", raw_mismatch_filename);
    FILE* statistics_file = fopen(statistics_filename, "w");
    assert(statistics_filename);
    // write in all the statistics here
    fprintf(statistics_file, "Average number of mismatches: %zu\n", compute_mean_mismatches(mismatch_enumerations));
    fprintf(statistics_file, "Min NO instance mismatches: %zu\n", compute_min_incorrect_mismatches(mismatch_enumerations));
    size_t num_valid = compute_num_valid_solutions(mismatch_enumerations);
    fprintf(statistics_file, "Number of Valid Tilings: %zu\n", num_valid);
    fprintf(statistics_file, "Percentage of Valid Tilings: %f\n", compute_percentage_valid(num_valid));
    fprintf(statistics_file, "Valid Tiling Arrangements (refer to tiledef to determine identities):\n");
    if (num_valid == 0) {
        fprintf(statistics_file, "All Invalid\n");
    } else {
        size_t *valid_arrangements = compute_valid_solutions(mismatch_enumerations, num_valid);
        for (size_t j = 0; j < num_valid; j++) {
            fprintf(statistics_file, "%zu\n", valid_arrangements[j]);
        }
        free(valid_arrangements);
    }

    fclose(statistics_file);
    free(raw_mismatch_filename);

    printf("Output stored in %s\n", statistics_filename);
    return 1;
}

int main(int argc, char *argv[]) {
    printf("The first argument (after the C file name) should be the name of the binary file containining all mismatch data for a specific tile def. Last value in the file name should indicate the number of tiles!\n");
    for (int i = 0; i < argc; i++) {
        printf("%d: %s\n", i, argv[i]);
    } 
    assert(argc == 2);
    mismatch_filename = argv[1];
    size_t num_tiles = pow((size_t) (mismatch_filename[strlen(mismatch_filename) - 5]-'0'), 2);
    num_tilings = pow(2, num_tiles);
    // printf("num: %zu\n", num_tilings);
    FILE *mismatch_file = fopen(mismatch_filename, "r");
    assert(mismatch_file != NULL);
    uint8_t *mismatch_enumerations = malloc(num_tilings * sizeof(uint8_t));
    fread(mismatch_enumerations, sizeof(uint8_t), num_tilings, mismatch_file);
    // printf("last enum mismatches: %d\n", mismatch_enumerations[num_tilings-1]);

    write_statistics(mismatch_enumerations);
    fclose(mismatch_file);
    free(mismatch_enumerations);
    
    return 0;
}