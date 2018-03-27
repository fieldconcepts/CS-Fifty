#include <cs50.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
    float n;
    do {
        printf("O hai! How much change is owed?\n");
        n = get_float();
    } while (n < 0);
    
    
    float cents = roundf(n * 100.00);
    int change = (int)cents;
    
    int count = 0;
    
    int quarter;
    int dime;
    int nickel;
    int cent ;
    quarter = 25;
    dime = 10;
    nickel = 5;
    cent = 1;
    
    while (change >= quarter)
    {
        count = count + 1;
        change = change - 25;
    }
    
    while (change >= dime)
    {
        count = count + 1;
        change = change - 10;
    }
    
    while (change >= nickel)
    {
        count = count + 1;
        change = change - 5;
    }
    
    while (change >= cent)
    {
        count = count + 1;
        change = change - 1;
    }
    
    printf("%i\n", count);
    
}