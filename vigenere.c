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
        string key = argv[1];
        
        //checking the key input and its alphanumeric value for both upper and lower case, convert any lower case to upper.
        for (int k = 0, l = strlen(key); k < l; k++)
        {
            //check is charachter is alpha
            if (isalpha(key[k]))
            { 
                //convert any lower case yo upper case
                if (islower(key[k]))
                {
                    key[k] = key[k] - 32;
                }
            }
            
            //exit program is any letter are not alpha
            else
            {
                printf("Invalid Command\n");
                return 1;
            }
        }

        //prompt user for string
        printf("plaintext: ");
        string s = get_string();
 
        printf("ciphertext: ");
        
        //iterate through string
        
        int j = 0;
        
        for (int i = 0, n = strlen(s); i < n; i++)
        {
            //check is charachter is alpha
            if (isalpha(s[i]))
            {            

                //check if character is Upper
                if (isupper(s[i]))
                {
                    int key_shift = (key[j % strlen(key)] - 65);
                    int c = (((s[i] - 65) + key_shift) % 26) + 65;
                    printf("%c", (char) c);
                    j++;
                }
                
                //check if character is lower
                if (islower(s[i]))
                {
                    int key_shift = (key[j % strlen(key)] - 65);
                    int c = ((s[i] - 97 + key_shift) % 26) + 97;
                    printf("%c", (char) c);
                    j++;
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
