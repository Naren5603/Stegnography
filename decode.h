#ifndef DECODE_H
#define DECODE_H  
#include "types.h"
typedef struct _DecodeInfo
{
    //size of magic string
    long size_magic_str;

    //stores the magic string
    char *magic_string;

    //stores the name of the .bmp file
    char* image_name;

    //size of the extension
    int  extn_file_size;

    //stores the extension 
    char *file_extn;

    //stores the file size
    int  file_data_size;

    //stores the output file name
    char op_file_name[20];

    //stores the file data
    char *file_data;

    //file pointer for writing data
    FILE *fptr_write_data;

    //file pointer for output image
    FILE *fptr_ip_image;
} DecodeInfo;

Status open_files_decode(DecodeInfo*decInfo);

Status read_and_validate_decode_args(char*argv[],DecodeInfo*decInfo);

Status do_decoding(DecodeInfo*decInfo);

Status validation_of_magic_string(char*string);

Status decode_magic_string_size(DecodeInfo*decInfo);

Status decode_size_from_image(FILE*fptr_ip_image,int *size);

Status decode_extn_size_from_image(FILE*fptr_ip_image,int *extn_file_size);

Status decode_file_data_size_from_image(FILE*fptr_ip_image,int *file_data_size);

int decode_size_from_byte(char*image_buffer);

char decode_data_from_byte(char*image_buffer);

Status decode_data_from_image(int size,FILE*fptr_ip_image,char*string);

Status decode_magic_string_from_image(int size,FILE*fptr_ip_image,char*string);

Status decode_extn_from_image(int size,FILE*fptr_ip_image,char*string,char *op_file_name);

Status decode_file_data_from_image(int size,FILE*fptr_ip_image,char*string,FILE*fptr_write_data);



#endif
