import numpy as np

# Generate random 4-bit binary numbers for 25 rows and 2 columns
random_binary_matrix = np.random.randint(0, 2, (25, 2, 4))

# Convert to binary string format
binary_string_matrix = []
for row in random_binary_matrix:
    binary_row = ["".join(map(str, elem)) for elem in row]
    binary_string_matrix.append(" ".join(binary_row))

# Save to a text file
file_content = "\n".join(binary_string_matrix)

file_path = "./data/rand_binary.txt"
with open(file_path, "w") as file:
    file.write(file_content)

file_path