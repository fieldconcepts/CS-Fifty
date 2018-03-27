/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */
 
#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    //declare variables: first, last, middle positon markers depending on array size
    int first = 0;
    int last = n - 1;
    int middle = (first + last) / 2;
    
    //iterate thorugh loop as long at first marker is less than last marker
    while (first <= last)
    {
        //if middle value in array is less than target value then update first marker to (middle + 1) & last marker stays the same
        if (values[middle] < value)
        {
            first = middle + 1;
        }
        
        //if middle value equals target value then stop, return true.
        else if (values[middle] == value)
        {
            return true;
            break;
        }
        
        //if middle value in array is more than target value then first marker stays the same & update last marker to (middle -1)
        else
        {
            last = middle - 1;
        }
        
        // Update middle value marker to take into account new first/last makrer values if target value has not been found yet
        middle = (first + last) / 2;
    }
    
    //if first > last then tagret is not found and return false
    return false;
}



/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    //declare variables: length, temperorary placeholder for empty array
    int length = n;
    int holder = values[0];
    
    
    //iterate through array starting at 0. check each pair to see if vlaue on left is greater than value on right.
    for (int i = 0; i < length; i++)
    {
        for (int j = i + 1; j < length; j++)
        {
            //swap positions of the pair of elements if value on lef tis greater than valiue on right.
            if (values[i] > values[j])
            {
                holder = values[i];
                values[i] = values[j];
                values[j] = holder;
            }
        }
    }
    
    //return the newly sorted array
    return;
}
