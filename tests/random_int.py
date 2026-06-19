import random


min = 18*3600
max = 21*3600

num_1 = random.randint(min, max)
num_2 = random.randrange(min, max)

print(f"randint:   {num_1}")
print(f"randrange: {num_2}")
