#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, string argv[])
{
    //check that only 2 element shave been entered
    if (argc == 2)
    {
        //convert string to integer k
        int k = atoi(argv[1]);
        
        //prompt user for string
        printf("plaintext: ");
        string s = get_string();
 
        printf("ciphertext: ");
        //iterate through string
        for (int i = 0, n = strlen(s); i < n; i++)
        {
            //check is charachter is alpha
            if (isalpha(s[i]))
            {
                //check if character is Upper
                if (isupper(s[i]))
                {
                    int c = ((s[i] - 65 + k) % 26) + 65;
                    printf("%c", (char) c);  
                }
                //check if character is lower
                if (islower(s[i]))
                {
                    int c = ((s[i] - 97 + k) % 26) + 97;
                    printf("%c", (char) c);  ;      
                }
            }
            
            //print character if not alpha
            else
            {
                printf("%c", s[i]);
            }
        }
        
        printf("\n");
        return 0;
    }
    
    else
    {
        printf("Invalid Command\n");
        return 1;
    }
}
