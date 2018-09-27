import numpy as np
import random

file_io = open("./io/hw1_input.txt", "w")
dimension = int(input("Type dimension of matrix: "))
nonzero_ratio_A = float(input("Ratio of nonzeros of A: "))
nonzero_ratio_B = float(input("Ratio of nonzeros of B: "))
formula = input("Type formula: ")
print(dimension)

# Print dimension to file
file_io.writelines(str(dimension))
file_io.writelines("\n")
file_io.writelines("$\n")

# Print first matrix
numbers = []
for row in range(dimension):
    numbers = []
    for col in range(dimension):
        numbers.append(random.randint(-5, 5))
    numbers = np.array(numbers) * np.less(np.random.rand(dimension), nonzero_ratio_A)
    file_io.writelines(" ".join([str(x) for x in numbers]))
    file_io.writelines("\n")
file_io.writelines("$\n")

# Print second matrix
numbers = []
for row in range(dimension):
    numbers = []
    for col in range(dimension):
        numbers.append(random.randint(-5, 5))
    numbers = np.array(numbers) * np.less(np.random.rand(dimension), nonzero_ratio_B)
    file_io.writelines(" ".join([str(x) for x in numbers]))
    file_io.writelines("\n")
file_io.writelines("$\n")

# Print formula
file_io.writelines(formula)
file_io.close()