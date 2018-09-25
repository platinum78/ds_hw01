import random

file_io = open("./hw1_input.txt", "w")
dimension = int(input("Type dimension: "))
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
        numbers.append(str(random.randint(-10, 10)))
    file_io.writelines(" ".join(numbers))
    file_io.writelines("\n")
file_io.writelines("$\n")

# Print second matrix
numbers = []
for row in range(dimension):
    numbers = []
    for col in range(dimension):
        numbers.append(str(random.randint(-10, 10)))
    file_io.writelines(" ".join(numbers))
    file_io.writelines("\n")
file_io.writelines("$\n")

# Print formula
file_io.writelines(formula)
file_io.close()