#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

size_t num_tilings = 1;
double gse = 5.43;
char *mismatch_filename;

// Would like to compute the minimum, the boltzmann weighted average, the normal average, and the number of valid tilings

double *compute_statistics(uint8_t *mismatches) {
    double *statistics = malloc(sizeof(double) * 9);
    double total = 0;
    double boltzmann_weighted_avg = 0;
    double partition_func = 0;
    double num_valid_sols = 0;
    double one_mismatch_away = 0; // arbitrarily chosen as <= 2 mismatches
    double two_mismatches_away = 0;
    double min = __DBL_MAX__;
    double prob = 0;

    for (size_t i = 0; i < num_tilings; i++) {
        // Check if the tiling was valid
        if (mismatches[i] == 0) {
            num_valid_sols++;
        }

        // Check if the tiling is a new minimum incorrect
        if (mismatches[i] < min && mismatches[i] > 0) {
            min = mismatches[i];
        }

        // Add to the total to compute the straightforward average 
        total += mismatches[i];
        // Compute the probability of seeing this particular state based on gse
        prob = exp(-gse * (double) mismatches[i]);

        // Add to the partition function as well as the boltzmann expectation
        boltzmann_weighted_avg += prob * mismatches[i];
        partition_func += prob;

        if (mismatches[i] == 1) {
            one_mismatch_away++;
        } else if (mismatches[i] == 2) {
            two_mismatches_away++;
        }

    }

    // boltzmann_weighted_avg accumulates the mismatch values weighted by their probabilities
    // partition_func accumulates the total weight (i.e., the sum of all probabilities).
    // need to normalized, since prob is 1/Z * exp (energy)
    // treating partition function as a constant and dividing out last!
    if (partition_func != 0) {
        boltzmann_weighted_avg /= partition_func;
    }

    statistics[0] = num_valid_sols;
    statistics[1] = min;
    statistics[2] = total / num_tilings; // average mismatches
    statistics[3] = boltzmann_weighted_avg; // boltzmann weighted avg mismatches
    statistics[4] = num_valid_sols / num_tilings; // fraction of valid tilings;
    statistics[5] = num_valid_sols / partition_func; // Boltzmann weighted fraction correct
    statistics[6] = partition_func;
    statistics[7] = one_mismatch_away;
    statistics[8] = two_mismatches_away;
    

    return statistics;
}

size_t *compute_valid_solutions(uint8_t *mismatches, double num_valid) {
    size_t *valid_arrangements = malloc(sizeof(size_t) * (size_t) num_valid);
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
    double *statistics = compute_statistics(mismatch_enumerations);

    fprintf(statistics_file, "Number of Valid Tilings: %zu\n", (size_t) statistics[0]);
    fprintf(statistics_file, "Min NO instance mismatches: %zu\n", (size_t) statistics[1]);
    fprintf(statistics_file, "Average number of mismatches: %f\n", statistics[2]);
    fprintf(statistics_file, "Boltzmann Weighted average number of mismatches: %f\n", statistics[3]);
    fprintf(statistics_file, "Percentage of Valid Tilings: %f\n", statistics[4]);
    fprintf(statistics_file, "Boltzmann Weighted Fraction Correct: %f\n", statistics[5]);
    fprintf(statistics_file, "Partition Function Value: %f\n", statistics[6]);
    fprintf(statistics_file, "Number of Tilings one mismatch away: %f\n", statistics[7]);
    fprintf(statistics_file, "Number of Tilings two mismatches away: %f\n", statistics[8]);
    

    fprintf(statistics_file, "Valid Tiling Arrangements (refer to tiledef to determine identities):\n");
    if (statistics[0] == 0) {
        fprintf(statistics_file, "All Invalid\n");
    } else {
        size_t *valid_arrangements = compute_valid_solutions(mismatch_enumerations, statistics[0]);
        for (size_t j = 0; j < statistics[0]; j++) {
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
    assert(argc == 3);
    mismatch_filename = argv[1];
    gse = atof(argv[2]);
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