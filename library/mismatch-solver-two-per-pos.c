#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

const size_t TILES_PER_POS = 2;
const char *FILE_DELIM = " ";
const size_t CHARS_PER_LINE = 11;
char *tile_filename = "";
size_t num_tiles;

// count mismatches
// output to a file
// write functions to compute statistics on the file (for hardness purposes)
// filter the mismatches and be able to access the tile enumeration that led to that
// write a function that takes in the enumeration and tells you the tiles that led to that enumeration
// write a program to convert the tile glues to a python tileset (and potentially vice versa)

u_int8_t binary_string_to_uint(char *bitstring) { 
    u_int8_t tile_info = 0;
    for (size_t i = 0; i < strlen(bitstring); i++) {
        tile_info += (bitstring[i] - '0') << (strlen(bitstring) - i - 1);
    }
    return tile_info;
}
int write_mismatches_to_file(uint8_t *mismatch_enumerations, uint8_t n) {
    // Calculate required buffer size (excluding null-terminator)
    char *raw_tile_filename = malloc(sizeof(char) * strlen(tile_filename));
    strncpy(raw_tile_filename, tile_filename, strlen(tile_filename) - 4);
    raw_tile_filename[strlen(tile_filename) - 4] = '\0';
    size_t string_size = snprintf(NULL, 0, "./output/mismatches-%s-%d.bin", raw_tile_filename, n);
    // Allocate buffer including null-terminator
    char *mismatch_filename = (char *)malloc(string_size + 1);
    // Format the string and store it in buffer
    snprintf(mismatch_filename, string_size + 1, "./output/mismatches-%s-%d.bin", raw_tile_filename, n);
    FILE* mismatches_file = fopen(mismatch_filename, "w");
    assert(mismatches_file);
    // write in all the mismatches here
    fwrite(mismatch_enumerations, sizeof(u_int8_t), pow(2, num_tiles), mismatches_file);
    fclose(mismatches_file);

    printf("Output stored in %s\n", mismatch_filename);
    return 1;
}

void free_tile_types(uint8_t **tile_types, size_t num_tiles) {
    for (size_t i = 0; i < pow(num_tiles, 2); i++) {
        free(tile_types[i]);
    }
    free(tile_types);
}

uint8_t compute_mismatches(size_t arrangement, uint8_t n, u_int8_t** tile_types) {
    uint8_t num_mismatches = 0;
    for (u_int8_t tile_pos = 0; tile_pos < num_tiles; tile_pos++) {
        u_int8_t tile_info = tile_types[tile_pos][(arrangement >> tile_pos) & 1];

        if ((tile_pos / n) < n - 1) {
            // compare with tile below
            u_int8_t bottom_neighbor_info = tile_types[tile_pos + n][(arrangement >> (tile_pos + n)) & 1];
            // compare the south glue of the current tile to the north glue of the below tile
            u_int8_t curr_south = (tile_info >> 1) & 1;
            u_int8_t bot_north = (bottom_neighbor_info >> 3) & 1;

            num_mismatches += curr_south ^ bot_north;
        }
        if ((tile_pos % n) < n - 1) {
            // compare with tile to the right
            u_int8_t right_neighbor_info = tile_types[tile_pos + 1][(arrangement >> (tile_pos + 1)) & 1];
            // compare the east glue of the current tile to the west glue of the next tile
            u_int8_t curr_east = (tile_info >> 2) & 1;
            u_int8_t right_west =  right_neighbor_info & 1;
            num_mismatches += curr_east ^ right_west;
        }
    }

    return num_mismatches;
}

uint8_t *enumerate_mismatches(uint8_t n, uint8_t **tileset) {
    size_t min = 0;
    size_t max = pow(TILES_PER_POS, num_tiles);
    uint8_t *mismatch_enumerations = malloc(sizeof(uint8_t) * max);
    for (size_t arrangement = min; arrangement < max; arrangement++) {
        // access the right type of tile type for each
        // compute the mismatches along the entire 2D array
        mismatch_enumerations[arrangement] = compute_mismatches(arrangement, n, tileset);
    }
    return mismatch_enumerations;
}

int main(int argc, char *argv[]) {
    printf("The first argument (after the C file name) should be the name of the text file containining all of the tiles (%zu per position, enumerate by line)!\n", TILES_PER_POS);
    printf("Within this file, the glues should be listed in binary number as NESW\n");
    printf("The second argument after the file name should be the n dimension of the grid, thus you should be providing n^2 lines\n");
    printf("The order of enumeration matters, it will be populated in a L->R, T->B array like assumption\n");
    for (int i = 0; i < argc; i++) {
        printf("%d: %s\n", i, argv[i]);
    }
    assert(argc == 3);
    tile_filename = argv[1];

    uint8_t n = *(argv[2]) - '0';

    FILE* tile_file = fopen(tile_filename, "r");
    assert(tile_file != NULL);

    uint8_t **tile_types = malloc(pow(n, 2) * sizeof(u_int8_t *));
    assert(tile_types);
    char *line_buffer = malloc(sizeof(char) * CHARS_PER_LINE);
    assert(line_buffer);

    // populate our tile definition into binary forms that can be accessed similar to a 2D array form
    num_tiles = pow(n, 2);
    for (uint8_t i = 0; i < num_tiles; i++) {

        tile_types[i] = malloc(TILES_PER_POS * sizeof(u_int16_t));
        assert(tile_types[i]);

        fgets(line_buffer, CHARS_PER_LINE * sizeof(char), tile_file);
        line_buffer[CHARS_PER_LINE - 2] = '\0';
        char *tile;
        
        tile = strtok(line_buffer, FILE_DELIM);
        uint8_t j = 0;
        while(tile != NULL) {
            u_int8_t tile_uint = binary_string_to_uint(tile);
            tile_types[i][j] = tile_uint;
            printf("%d, %d: %d\n", i, j, tile_types[i][j]);
            tile = strtok(NULL, FILE_DELIM);
            j++;
        }
    }

    free(line_buffer);
    fclose(tile_file);

    tile_filename = strrchr(tile_filename, '/') + 1;
    
    printf("Enumerating tile mismatch combinations\n");
    uint8_t *mismatch_enumerations = enumerate_mismatches(n, tile_types);
    write_mismatches_to_file(mismatch_enumerations, n);
    free(mismatch_enumerations);
    free_tile_types(tile_types, n);
    return 0;
}



