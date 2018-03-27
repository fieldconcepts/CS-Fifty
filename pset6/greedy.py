


while(True):
    print("O hai! How much change is owed?")
    n = float(input())
    
    if (n > 0):
        break

print("Dollars: {}".format(n))

change = round(n * 100)
print("Cents: {}".format(change))

count = 0

quarter = 25
dime = 10
nickel = 5
cent = 1

while (change >= quarter):
    count += 1
    change -= 25

while (change >= dime):
    count += 1
    change -= 10

while (change >= nickel):
    count += 1
    change -= 5


while (change >= cent):
    count += 1
    change -= 1

print("{}".format(count))