
while (True):
    print("Height: ", end="")
    n = int(input())
    if (n > 0 and n < 24):
        break
    

for i in range(0, n, 1):
        
        print(" " * (n-i-1), end="")
        print("##", end="")
        print("#" * i, end="")
        print("  ##", end="")
        print("#" * i)