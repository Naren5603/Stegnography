#include <stdio.h>
#include "encode.h"
#include "common.h"
#include "types.h"
#include<string.h>
#include<stdlib.h>
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_file_size(FILE *fptr)
{
    uint count;
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    
    

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    
    
    // No failure return e_success
    return e_success;

}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{

    //checking if it,s .bmp
    if(strstr(argv[2],".bmp")==NULL)
    {
        printf("%s doesn't have .bmp extension\n",argv[2]);
        return e_failure;
    }
    encInfo->src_image_fname=argv[2];

    //checking if it's .txt
    if(strstr(argv[3],".txt")==NULL)
    {
        printf("%s doesn't have .txt or .c or .py or .sh extension\n",argv[3]);
        return e_failure;
    }
    encInfo->secret_fname=argv[3];
    strcpy(encInfo->extn_secret_file,".txt");
    encInfo->extn_secret_file_size=strlen(encInfo->extn_secret_file);

    //checking if optional argument is passed
    if(argv[4]!=NULL)
    {
        if(strstr(argv[4],".bmp")!=NULL)
        {
            encInfo->stego_image_fname=argv[4];
        }
        else
        {
            printf("%s doesn't have .bmp extension\n",argv[4]);
            return e_failure;
        }
    }
    //if didn't passed create a default name
    else
    {
        printf("Default output file name is \"stegno.bmp\"\n");
        encInfo->stego_image_fname="stegno.bmp";
    }
    
    
    
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("Files opened successfully\n");
    }
    

    //to check if the image has the number of bytes to encode the data
    if(check_capacity(encInfo)==e_failure)
    {
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("BMP header copied successfully\n");
    }

    if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("Magic string encoded successfully\n");
    }
    
    if(encode_secret_extern_size(encInfo->extn_secret_file_size,encInfo)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("Secret file extension size encoded successfully\n");
    }

    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("Secret file extension encoded successfully\n");
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("Secret file data size encoded successfully\n");
    }

    if(encode_secret_file_data(encInfo)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("Secret file extension encoded successfully\n");
    }
    
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("Remaining pixes from the images copied sucessfully\n");
    }
}

Status check_capacity(EncodeInfo *encInfo)
{
    //find the size of the image
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);

    //find the file size
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

    //check the capacity if we can store the data into the image or not
    if(encInfo->image_capacity > ( 54 + ( 8 * strlen(MAGIC_STRING) ) + 32 + ( 8 * strlen(encInfo->extn_secret_file) ) + 32 + ( 8 * sizeof(encInfo->size_secret_file) ) ) )
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //declare an array to store header
    char buffer[54];

    rewind(fptr_src_image);

    fread(buffer,54,1,fptr_src_image);

    fwrite(buffer,54,1,fptr_dest_image);

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_string,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
}

Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{

    //create a buffer of size 8(character)
    char image_buffer[8];

    //run a loop for size times(string length)
    for(int i=0;i<size;i++)
    {

        //read 8 bytes form src_image and store it in image_buffer
        fread(image_buffer,8,1,fptr_src_image);

        //call the function to encode character by character to the image 
        if(encode_byte_to_lsb(data[i], image_buffer)==e_failure)
        {
            return e_failure;
        }

        //write 8 bytes form image buffer to stego_image
        fwrite(image_buffer,8,1,fptr_stego_image);

    }
    return e_success;
}
 
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    
    //run a loop for 8 times(character)
    for(int i=0;i<8;i++)
    {
        if(data & (1<<7-i))
        {
            image_buffer[i]= (image_buffer[i] & ~(1) | 1);
        }
        else
        {
            image_buffer[i]=(image_buffer[i] & ~(1) | 0);
        }
       //image_buffer[i]=((image_buffer[i]&~1) |(data&(1<<(7-i))>>(7-i)));
    }
    return e_success;
}


Status encode_secret_extern_size(long size,EncodeInfo *encInfo)
{
    if(encode_size_to_image((int)size,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    return e_success;
}


Status encode_secret_file_extn(const char*file_extn,EncodeInfo *encinfo)
{
    if(encode_data_to_image(file_extn,strlen(file_extn),encinfo->fptr_src_image,encinfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    return e_success;

}

Status encode_size_to_image(long size,FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[32];
    
        fread(image_buffer,32,1,fptr_src_image);

        encode_size_to_lsb(size,image_buffer);

        fwrite(image_buffer,32,1,fptr_stego_image);

        return e_success;
}

Status encode_size_to_lsb(int file_size, char *image_buffer)
{
    //run a loop for 32 times(integer)
    for(int i=0;i<32;i++)
    {
        if(file_size & (1<<31-i))
        {
            image_buffer[i]= (image_buffer[i] & ~(1) | 1);
        }
        else
        {
            image_buffer[i]=(image_buffer[i] & ~(1) | 0);
        }
    }
    return e_success;
}

Status encode_secret_file_size(long size,EncodeInfo *encInfo)
{
    if(encode_size_to_image((int)size,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    return e_success;
}

Status encode_secret_file_data(EncodeInfo*encInfo)
{
    fseek(encInfo->fptr_secret,0,SEEK_SET);

    char data[encInfo->size_secret_file+1];

    fread(data,encInfo->size_secret_file,sizeof(char),encInfo->fptr_secret);

    data[encInfo->size_secret_file] = '\0';


    if(encode_data_to_image(data,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[1]; // Buffer to hold chunks of pixel data

    // Copy the remaining pixel data
    while (fread(buffer, 1, 1, fptr_src_image) > 0)
    {
        fwrite(buffer, 1, 1, fptr_dest_image);
    }

    return e_success;
    
}

