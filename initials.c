#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    //ask user for input
    string s = get_string();

    //make sure user has input a string
    if (s != NULL)
    {
        //Capitilze and pprint for letter
        printf("%c", toupper(s[0]));
        
        //iterate over the string
        for (int i = 0, n = strlen(s); i < n; i++)
        {
            //Check for spaces and print next letter
            if (s[i] == ' ')
            {
                printf("%c", toupper(s[i+1]));
            }
         }
         printf("\n");
    }
}