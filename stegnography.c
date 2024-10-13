#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char*argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    //check the argv[i] is -e or -d
    int op_type = check_operation_type(argv);
    {
        if(op_type==e_encode)
        {
            printf("Encode Started.....\n");

            if(read_and_validate_encode_args(argv,&encInfo)==e_success)
            {
                printf("### Read and validations are successfully done for encodeing ###\n");
                if(do_encoding(&encInfo)==e_failure)
                {
                    return 0;
                }
                else
                {
                    printf("### Encoding data to image operation is done ###\n");
                    return 0;
                }
            }
            else
            {
                printf("Read and validations are failed\n");
                return 0;
            }
            
        }
        else if(op_type==e_decode)
        {
            printf("Decode Started.....\n");
            if(read_and_validate_decode_args(argv,&decInfo)==e_success)
            {
                printf("### Read and validations are successfully done for decodeing ###\n");
                if(do_decoding(&decInfo)==e_failure)
                {
                    return 0;
                }
                else
                {
                    printf("### Decoding data from image operation is done ###\n");
                    return 0;
                }
            }
            else
            {
                printf("Read and validations are failed\n");
                return 0;
            }
        }
        else if(op_type==e_unsupported)
        {
            printf("unsupported operation");
            return 0;
        }
    }
    return 0;
}

OperationType check_operation_type(char**argv)
{
    if(strcmp(argv[1],"-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        return e_decode;
    }
    else 
    {
        return e_unsupported;
    }
    //check if argv is -e or not
    //if return e_encode
    //else if e_decode
    //else e_unsupported    
}