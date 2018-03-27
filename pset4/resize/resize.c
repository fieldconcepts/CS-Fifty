/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    
    // remember filenames
   
    char *infile = argv[2];
    char *outfile = argv[3];
    
    // get scaling factor from command line and convert to integer
    int factor = atoi(argv[1]);
    if (factor <= 0 || factor >= 101)
    {
        fprintf(stderr, "Scaling factor must be between 1 and 100\n");
        return 1;
    }
    
    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    

    //save the original widhts/hights/padding values to variables
    int original_padding = (4 - (bi.biWidth* sizeof(RGBTRIPLE)) % 4) % 4;
    int original_width = bi.biWidth;
    int original_height = bi.biHeight;

    //caluclate new bmp width, padding, height image size and file size to take into accout scaling factor
    int width2 = bi.biWidth * factor;
    int height2 = bi.biHeight * factor;
    int new_padding = (4 - (width2 * sizeof(RGBTRIPLE)) % 4) % 4;
    int sizeImage2 = ((sizeof(RGBTRIPLE) * width2) + new_padding) * abs(height2);
    int size2 = sizeImage2 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    
    //update the bf/bi  structs with the new widhts/height, sizes.
    bf.bfSize = size2;
    bi.biWidth = width2;
    bi.biHeight = height2;
    bi.biSizeImage = sizeImage2;
    

    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);


    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    //alocate memory of size RGBTRIPLE(3bytes) * new_width for a temporary buffer to save each horizontal scan line
    RGBTRIPLE *buffer = malloc(sizeof(RGBTRIPLE) * bi.biWidth);
    
    // iterate over infile's scanlines vertically. Make sure to use original height size
    for (int i = 0, height = abs(original_height); i < height; i++)
    {
        
        //counter to track the position inside the buffer array
        int buff_track = 0;
        
        // iterate horizontally over pixels in scanline
        
        for (int j = 0; j < original_width; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            
            //save each pixel(RGBTRIPLE) to buffer array 'n' times (scale factor) and update tracker to keep track of postion inside buffer array
            for (int n = 0; n < factor; n++)
            {
                *(buffer+(buff_track)) = triple;
                buff_track++;
            }
                
        }

        // skip over padding by moving file pointer by the amount of padding caluclated in orginal_padding
        fseek(inptr, original_padding, SEEK_CUR);

        //at the end of each horiontal scanline, write the data saved in the *buffer 'n' times to file and add the correct amount of padding after eachline.
        for (int z = 0; z < factor; z++)
        {
            fwrite((buffer), sizeof(RGBTRIPLE), bi.biWidth, outptr);
            // then add padding to end of line after printing array saved inside buffer
            for (int k = 0; k < new_padding; k++)
            {
                fputc(0x00, outptr);
            }
        }
        
        
        
        
        
        
        
        
    }

    // free memory from buffer
    free(buffer);
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
