import numpy as np

# Generate random 4-bit binary numbers for 25 rows and 2 columns
random_binary_matrix = np.random.randint(0, 2, (25, 2, 4))

# Number of binary numbers per position
tpp = 2
glues_p_tile = 4
num_tiles = 25

binary_string_matrix = []
for i in range(num_tiles):
    row = set()
    while len(row) != 2:
        elem = np.random.randint(0, 2, glues_p_tile)
        str_elem = "".join(map(str, elem))
        row.add(str_elem)
    binary_string_matrix.append(" ".join(row))

# Save to a text file
file_content = "\n".join(binary_string_matrix)

file_path = "./data/rand_binary.txt"
with open(file_path, "w") as file:
    file.write(file_content)

file_path