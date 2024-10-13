#include <stdio.h>
#include "decode.h"
#include "common.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>

Status open_files_decode(DecodeInfo*decInfo)
{
    decInfo->fptr_ip_image=fopen(decInfo->image_name,"r");
    if (decInfo->fptr_ip_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->op_file_name);

    	return e_failure;
    }
    else
    {
        printf("### Files opened successfully ###\n");
        return e_success;
    }
}

Status read_and_validate_decode_args(char*argv[],DecodeInfo*decInfo)
{
    if(strstr(argv[2],".bmp")==NULL)
    {
        printf("%s doesn't have \".bmp\" extention",argv[2]);
        return e_failure;
    }
    else
    {
        decInfo->image_name=argv[2];
    }

    if(argv[3]!=NULL)
    {
        strcpy(decInfo->op_file_name,argv[3]);

        printf("Output file is created with the name of %s\n",decInfo->op_file_name);
    }
    else if(argv[3]==NULL)
    {
        strcpy(decInfo->op_file_name,"stegno");
        
        printf("Output file is created with the default name of %s\n",decInfo->op_file_name);
    }
    else
    {
        return e_failure;
    }
    return e_success;
}

Status do_decoding(DecodeInfo*decInfo)
{
    int size=strlen(MAGIC_STRING);
    printf("Decoding started...\n");
    char string[100];
    printf("Enter the magic string:");
    scanf("%s",string);
    if(strlen(string)!=size)
    {
        printf("The magic string didn't match\n");
        return e_success;
    }

    if(validation_of_magic_string(string)==e_failure)
    {
        exit(0);
    }

    if(open_files_decode(decInfo)==e_failure)
    {
        return e_failure;
    }
    if(decode_magic_string_from_image(strlen(string),decInfo->fptr_ip_image,decInfo->magic_string)==e_failure)
    {
        return e_failure;
    }


    if(decode_extn_size_from_image(decInfo->fptr_ip_image,&decInfo->extn_file_size)==e_failure)
    {
        return e_failure;
    }

    

    if(decode_extn_from_image(decInfo->extn_file_size,decInfo->fptr_ip_image,decInfo->file_extn,decInfo->op_file_name)==e_failure)
    {
        return e_failure;
    }


    if(decode_file_data_size_from_image(decInfo->fptr_ip_image,&decInfo->file_data_size)==e_failure)
    {
        return e_failure;
    }

    if(decode_file_data_from_image(decInfo->file_data_size,decInfo->fptr_ip_image,decInfo->op_file_name,decInfo->fptr_write_data)==e_failure)
    {
        return e_failure;
    }

    
    return e_success;
}
Status validation_of_magic_string(char*string)
{
    printf("Validating magic string.....\n");
    if(strcmp(string,MAGIC_STRING)==0)
    {
        return e_success;
    }
    else
    {

        printf("The magic string didn't match\n");
        return e_failure;
    }
    
}

Status decode_extn_size_from_image(FILE*fptr,int *extn_file_size)
{
    printf("Decoding extension size from image.....\n");
    if(decode_size_from_image(fptr,extn_file_size)==e_failure)
    {
        return e_failure;
    }
}

Status decode_file_data_size_from_image(FILE*fptr,int *file_data_size)
{
    printf("Decoding file data size from image.....\n");
    if(decode_size_from_image(fptr,file_data_size)==e_failure)
    {
        return e_failure;
    }
}

Status decode_size_from_image(FILE*fptr_ip_image,int *size)
{
    char image_buffer[32];


    fread(image_buffer,32,1,fptr_ip_image);
   *size=decode_size_from_byte(image_buffer);
    return e_success;
}

Status decode_magic_string_from_image(int size,FILE*fptr_ip_image,char*string)
{
    fseek(fptr_ip_image,54,SEEK_SET);

    char image_buffer[8];
    fseek(fptr_ip_image,54,SEEK_SET);
    for(int i=0;i<size;i++)
    {
        fread(image_buffer,8,1,fptr_ip_image);
        decode_data_from_byte(image_buffer);
    }

    return e_success;
}

Status decode_extn_from_image(int extn_file_size,FILE*fptr_ip_image,char*file_extn,char *op_file_name)
{
    printf("Decoding extention from image.....\n");
    char arr[extn_file_size];
    char image_buffer[8];
    for(int i=0;i<extn_file_size;i++)
    {

        fread(image_buffer,1,8,fptr_ip_image);
        arr[i]=decode_data_from_byte(image_buffer);
    }
    arr[extn_file_size]='\0';
   
    file_extn=arr;
    strcat(op_file_name,file_extn);

    return e_success;
}

Status decode_file_data_from_image(int file_data_size,FILE*fptr_ip_image,char*string,FILE*fptr_write_data)
{
    printf("Decoding file data from image.....\n");

     fptr_write_data=fopen(string,"w");

  
    char arr[file_data_size];


    arr[file_data_size]='\0';
    char image_buffer[8];
    for(int i=0;i<file_data_size;i++)
    {
        fread(image_buffer,1,8,fptr_ip_image);
        arr[i]=decode_data_from_byte(image_buffer);
    }
    fwrite(arr,file_data_size,1,fptr_write_data);
    return e_success;
}


char decode_data_from_byte(char*image_buffer)
{

    char data=0;
    for(int i=0;i<8;i++)
    {
        if(image_buffer[i]&1)
        {
            data=(data|(1<<(7-i)));
        }
        else 
        {
            data=(data|0);
        }
    }
    return data;
}

int decode_size_from_byte(char*image_buffer)
{
    int data=0;
    for(int i=0;i<32;i++)
    {
        if(image_buffer[i]&1)
        {
            data=(data|(1<<(31-i)));
        }
        else 
        {
            data=(data|0);
        }
    }
    return data;

}
