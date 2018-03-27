#include <cs50.h>
#include <stdio.h>

int main(void)
{
    int n;
    do {
        printf("Height: ");
        n = get_int();
    } while (n < 0 || n > 23);
    
    for (int i = 0; i < n; i++)
    {
        for (int k = n - 1; k > i; --k)
        {
            printf(" ");
        }
        
        for (int j = 0; j < i; j++)
        {
            printf("#");
        }
        
        
        printf("#  ");
        
        for (int j = 0; j < i; j++)
        {
            printf("#");
        }
        
        printf("#\n");
        
    }
    
}
