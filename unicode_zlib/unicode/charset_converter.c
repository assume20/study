#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

extern int errno;
/*
//http://www.gnu.org/software/libiconv/documentation/libiconv-1.13/iconv_open.3.html
iconv_t iconv_open (const char* tocode, const char* fromcode);

int iconv_close (iconv_t cd);

size_t iconv (iconv_t cd,
              const char* * inbuf, size_t * inbytesleft,
              char* * outbuf, size_t * outbytesleft);

*/

// http://www.gnu.org/software/libiconv/documentation/libiconv-1.13/iconv_open.3.html
typedef struct charset_converter
{
    int error;  // 错误码
    iconv_t cd; // iconv句柄
} charset_converter_t;

charset_converter_t *charset_converter_init(char *tocode, char *fromcode)
{
    charset_converter_t *converter = (charset_converter_t *)malloc(sizeof(charset_converter_t));
    if (!converter)
    {
        printf("malloc charset_converter_t failed\n");
        return NULL;
    }
    converter->error = 0;
    // 打开错误则返回(iconv_t)(-1), 并设置errno
    converter->cd = iconv_open(tocode, fromcode);
    printf("converter->cd = %p\n", converter->cd);
    if ((iconv_t)(-1) == converter->cd)
    {
        printf("iconv_open failed, errno = %d, %s\n", errno, strerror(errno));
        free(converter);
        return NULL;
    }

    return converter;
}
int charset_convert(charset_converter_t *convert,
                    unsigned char **inbuf, size_t *inbytesleft,
                    unsigned char **outbuf, size_t *outbytesleft, size_t *outlen)
{
    size_t temp_out_len = (*outbytesleft);
    if (!convert)
    {
        printf("convert is null\n");
        return -1;
    }
    if (0 == convert->cd)
    {
        printf("convert->cd is 0\n");
        return -1;
    }
    /* 进行转换
    *@param cd iconv_open()产生的句柄
    *@param inbuf 需要转换的字符串, 指针最后指向当前已经转换的最后位置
    *@param inbytesleft 存放还有多少字符没有转换
    *@param outbuf      存放转换后的字符串
    *@param outbytesleft 存放转换后,temp_outbuf剩余的空间
    *
    * */
    printf("iconv inbytesleft = %ld, outbytesleft = %ld\n", *inbytesleft, *outbytesleft);
    size_t ret = iconv(convert->cd, (char **)inbuf, inbytesleft, (char **)outbuf, outbytesleft);
    (*outlen) = temp_out_len - (*outbytesleft);
    printf("iconv ret = %ld\n", ret);
    if ((size_t)(-1) == ret)
    {
        convert->error = errno;
        printf("iconv failed, errno = %d, %s\n", errno, strerror(errno));
        switch (convert->error)
        {
        // 这种情况是输出缓冲不够大
        case E2BIG:
            printf("E2BIG\n");
            break;
        // 这种情况是遇到无效的字符了
        case EILSEQ:
            printf("EILSEQ\n");
            break;
        case EINVAL:
            printf("EINVAL\n");
            break;
        default:
            printf("unknown\n");
            break;
        }
        // return -1;
    }

    return 0;
}

int charset_converter_close(charset_converter_t **convert)
{
    if (!(*convert))
    {
        printf("*convert is null\n");
        return -1;
    }
    int ret = iconv_close((*convert)->cd);
    *convert = NULL;
    return ret;
}

int main(int argc, char *argv[])
{
    charset_converter_t *converter = charset_converter_init("utf-16", "utf-8");

    if (!converter)
    {
        printf("code_init failed and exit\n");
        exit(1);
    }

    // 需要转换的字符串
    unsigned char inbuf[1024] = "廖庆富𤋮darren老师";
    size_t srclen = strlen(inbuf);
    // 打印需要转换的字符串的长度
    printf("converter = %p, srclen = %ld\n", converter, srclen);

    // 存放转换后的字符串
    size_t outbuf_left = 1024; // 转换时 iconv函数根据输入的buffer长度和可输出的buffer长度决定进行多少字符的转换
    unsigned char outbuf[outbuf_left];
    size_t out_len = 0; // 实际转换出来多少字节的字符长度
    memset(outbuf, 0, outbuf_left);

    // 由于iconv()函数会修改指针，所以要保存源指针
    unsigned char *temp_inbuf = inbuf;
    unsigned char *temp_outbuf = outbuf;

    // 以十六进制打印转换之前的数据
    printf("\nutf8:\n");
    for (int i = 0; i < strlen(inbuf); i++)
    {
        printf("%02x ", inbuf[i]);
    }
    printf("\n\n");

    // srclen = 6;
    outbuf_left = 22; // 测试不同的输出buffer空间，实际转换出来的字符长度
    // 打印转换之前的数据buffer情况
    printf("inbuf = %p, inleft = %ld, outbuf = %p, outleft = %ld\n",
           inbuf, srclen, outbuf, outbuf_left);

    size_t ret = charset_convert(converter, &temp_inbuf, &srclen, &temp_outbuf, &outbuf_left, &out_len);
    if (ret == -1)
    {
        printf("charset_convert failed and exit\n");
        exit(1);
    }
    printf("inbuf = %p, inleft = %ld, outbuf = %p, outleft = %ld,  out_len = %ld\n",
           temp_inbuf, srclen, temp_outbuf, outbuf_left, out_len);
    printf("skip in:%ld, out:%ld\n", temp_inbuf - inbuf, temp_outbuf - outbuf);

    // 以十六进制打印转换之后的数据
    printf("\nutf16:\n");
    for (int i = 0; i < out_len; i++)
    {
        printf("%02x ", outbuf[i]);
    }
    printf("\n\n");
    // 关闭句柄
    charset_converter_close(&converter);
    printf("converter = %p\n", converter);

    return 0;
}