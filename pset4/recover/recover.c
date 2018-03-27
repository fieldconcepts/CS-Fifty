#include <stdio.h>
#include <stdint.h>

typedef uint8_t  BYTE;


int main(int argc, char *argv[])
{
    //ensure the correct command line arguments have been used
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover file\n");
        return 1;
    }
    
    //get file name and print to check
    char *infile = argv[1];
    printf("File to open: %s\n", infile);
    
    //create file pointer called rawfile and set to NULL
    FILE* rawfile = NULL;
    
    //open rawfile as a read only file "r"
    rawfile = fopen(infile, "r");
    
    //check file exists and exit program if file == NULL
    if (rawfile == NULL)
    {
        printf("Could not open file\nExit program\n");
        return 2;
    }
    
    printf("File successfully opened\n");
    

    //set variable for prpgram
    //count the number of jpges found so far.
    int jpegcounter = 0;

    
    //create a buffer array of 512bytes size. define the data type at uint8_t = 1 byte = 8 bits.
    BYTE buffer[512];
    printf("Size of Buffer: %lu\n", sizeof(buffer));
    
    
    //create a array of 8 bytes to accomodate the sequential naming convetion.
    char filename[8];

 
    //create a temporary file pointer call jpeg. This is where we will store the data when we wrtie from buffer to file.
    FILE* jpeg = NULL;
    
    
    //TODO
    // 1. OPEN THE MEMORY CARD FILE
    // 2. FIND BEGINNING OF JEPG
    // 3. OPEN NEW JPEG
    // 4. WRITE 512 BYTES UNTIL NEW JPEG IS FOUND
    // 5. DETECT END OF FILE
    
    
    printf("\n\nSTART WHILE LOOP\n\n");
    
    while ((fread(buffer, sizeof(buffer), 1, rawfile)) == 1)
    {
        
        //check frist 4 bytes of buffer. 4th byte uses bitwise AND operator aritmatic to get value.
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0Xe0)
        {
            
            if (jpegcounter == 0)
            {
                printf("First JPEG detected\n");

                //sort out file sequential filenaming by using jpegcounter value
                sprintf(filename, "%03d.jpg", jpegcounter);
                printf("Filename: %s\n", filename);
                
                //open the file pointer and assign it a name and write operator "w"
                jpeg = fopen(filename, "w");
                
                //check if file is NULL, exit program is file == NULL
                if (jpeg == NULL)
                {
                    printf("Could not open JPEG file%s\nExit program\n", filename);
                    return 3;
                }
                            
                printf("Successfully opened JPEG: %s\n", filename);
                
                //write the current 512bytes in that have just been read into the jpeg file that has been opened
                fwrite(buffer, sizeof(buffer), 1, jpeg);
                printf("Sucessfully written first 512 bytes to file: %s\n", filename);
                
                //bump up the counter
                jpegcounter++;
                printf("Number of JPEGS found: %i\n\n", jpegcounter);
                
                
            //end of "FIRST JPEG" IF
            }
            
            else if (jpegcounter > 0)
            {
                //close the current jpeg file that nis being written
                fclose(jpeg);
                printf("Next JPEG detected\nSuccessfully closed current JPEG file\n");
                
                //sort out file sequential filenaming by using jpegcounter value
                sprintf(filename, "%03d.jpg", jpegcounter);
                printf("Filename: %s\n", filename);
                
                //open the file pointer and assign it a name and write operator "w"
                jpeg = fopen(filename, "w");
                
                //check if file is NULL, exit program is file == NULL
                if (jpeg == NULL)
                {
                    printf("Could not open JPEG file%s\nExit program\n", filename);
                    return 4;
                }
                
                printf("Successfully opened JPEG: %s\n", filename);
                
                //write the current 512bytes in that have just been read into the jpeg file that has been opened
                fwrite(buffer, sizeof(buffer), 1, jpeg);
                printf("Sucessfully written first 512 bytes to file: %s\n", filename);
                

                //bump up the counter
                jpegcounter++;
                printf("Number of JPEGS found: %i\n\n", jpegcounter);
                
                
            //end of "NEXT JPEG" IF    
            }

        //end of "BYTE CHECKING" IF     
        }
        
        //While the buffer is full of data and a new JPEG has NOT been detected, continue to write the buffer data to the current open JPEG file.
        else if (jpeg != NULL)
        {
            fwrite(buffer, sizeof(buffer), 1, jpeg);
        }
        
       
    //end of while loop    
    }
    
    //close current jpeg file and original rawfile
    fclose(jpeg);
    printf("EOF detected\nLast JPEG file closed\n");
    
    fclose(rawfile);
    printf("File: %s closed\n", infile);
    
    //exit from program
    return 0; 
}