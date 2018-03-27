/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "dictionary.h"



#define HASHTABLE_SIZE 65536




//nodes for linked lists
typedef struct node
{
    char word[46];
    struct node* next;
}
node;

//global hashtable set to NULL, going to index dictionary words into 26 buckets, A - Z
node* hashtable[HASHTABLE_SIZE] = {NULL};

//global variable for dictionary size
int wordcount = 0;


/**
 * HASH FUCNTION taken from https://www.reddit.com/r/cs50/comments/1x6vc8/pset6_trie_vs_hashtable/cf9nlkn/
 */

int hashfunction(char* str)
{
    unsigned int hash = 0;
    for (int i=0, n=strlen(str); i<n; i++)
        hash = (hash << 2) ^ str[i];
    return hash % HASHTABLE_SIZE;
}


/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    char tofind[LENGTH + 1];            //convert the word to lowercase and save ina  temporary array called tofind
    int len = strlen(word);             //get the string length of the word we are checking
    
    for (int i = 0; i < len; i++)       //convert the string to all lowercase by looping through eac character
    {
        tofind[i] = tolower(word[i]);   //make each chharacter lowercase and save to the temporary tofind[] array
    }
    tofind[len] = '\0';                 //add a \0 to signify end of string.

    int indexletter = hashfunction(tofind);  //convert word to a hashed integer value
    

    
    if (hashtable[indexletter] == NULL)             //if the hashtable[index] return NULL then there are no nodes at the letter index
    {
        return false;
    }
    
    node* cursor = hashtable[indexletter];      //create a new node struct called cursor that will iterate throught each linked list
                                                //cursor begins by pointing at same placee hashtable element is pointing, hopefully the first node in linked list
    while (cursor != NULL)                      
    {
        if (strcmp(tofind, cursor->word) == 0)  //compare word we are checking to the var word found in that node
        {
            return true;                        //if the words match then we have found the check word in the dictionary.fucntion returns true, end of loop.
        }
        
        cursor = cursor->next;                  //if we did not get a match in that node then we mmove onto the next node
    }                                           // cursor is now set to the next cursor address.
                                                //loop continues until word is found
    
    return false;                               //if word cannot be found in the linked list then it will iterate to end of linked list until cursor->next == NULL
                                                //fucntion return false, WORD NOT FOUND in dictionary
    
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    
    //open file and read into temporary rawfile
    FILE* rawfile = fopen(dictionary, "r");
    
    //check file exists and exit program if file == NULL
    if (rawfile == NULL)
    {
        return false;
    }

    
    //variables
    char dictword[LENGTH + 1];          //maximum charcters in a word 45 + '\0'

    //26 values for A-Z buckets in hashtable. each elemnt in array is a pointer that 
    //will point to the start of a linked list


    while (fscanf(rawfile, "%s", dictword) != EOF)     //scan dictionary word by word. take dictionary file, look for a string and then put string into var called word.
    {
        node* new_node = malloc(sizeof(node));             //create a new node, allocate memory
        
        strcpy(new_node->word, dictword);                   //set the word in new_node struct to the word red in by fscanf

    
        int index = hashfunction(dictword);                 //convert word to hash index value

        //if array is empty then start a new linked list
        if (hashtable[index] == NULL)
        {
            hashtable[index] = new_node;        //point array pointer to the newly created first node
            new_node->next = NULL;              //make sure to declare the newly created node->next as a NULL
        }
        
        //if array has a list already attached then start prepending by point new_node to head of list and then redirect head of list pointer to new array.
        else
        {
            new_node->next = hashtable[index];          //point new_node->next at the same loaction as the hashtable elemnet is pointing to
            hashtable[index] = new_node;                //no re direct the hashtable elemnt pointer to point at the newly created node
        }

        wordcount++;                                        //bumper up counter                 
    }
    
    //close dictonary file after loading in
    fclose(rawfile);
    
    return true;

}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    //if global variable wordcount is greater than 0 then return wordcount
    if (wordcount > 0)
    {
        return wordcount;
    }
    
    //if wordcount is 0 then return 0
    else
    {
        return 0;
    }
    
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    int i = 0;
    
    
    //iterate through the hastable array (26 elements for A-Z)
    while (i < HASHTABLE_SIZE)
    {
        //if hashtable element is empty at index i, go to te next elemnt in array
        if (hashtable[i] == NULL)           
        {
            i++;
        }
        
        else
        {
            //while the hashtable pointer doesn't equal NULL, movee the cursor throught the linked list, freeing up nodes.
            while(hashtable[i] != NULL)
            {
                node* cursor = hashtable[i];
                hashtable[i] = cursor->next;
                free(cursor);
                
                
            }
            
            //once the hastable is empty and equals NULL, go to the next elemnt in hastbale array
            i++;
        }
        
    }
    
    //return true when all nodes are free
    return true;
}
