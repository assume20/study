#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

int main(int argc, char **argv)
{
    /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
   *          IGNORE  ：遇到无法转换字符跳过*/
    //char *encTo = "UNICODE//TRANSLIT";
    // char *encTo = "UNICODE//IGNORE";
    unsigned char *encTo = "UTF-16";
    /* 源编码 */
    unsigned char *encFrom = "UTF-8";

    /* 获得转换句柄
   *@param encTo 目标编码方式
   *@param encFrom 源编码方式
   *
   * */
    iconv_t cd = iconv_open(encTo, encFrom);
    if (cd == (iconv_t)-1)
    {
        perror("iconv_open");
    }

    /* 需要转换的字符串 */
    unsigned char inbuf[1024] = "廖庆富darren老师"; // 21
    size_t srclen = strlen(inbuf);
    /* 打印需要转换的字符串的长度 */
    printf("srclen=%d\n", srclen);

    /* 存放转换后的字符串 */
    size_t outlen = 1024;
    unsigned char outbuf[outlen];
    size_t utf16_len = outlen;
    memset(outbuf, 0, outlen);

    /* 由于iconv()函数会修改指针，所以要保存源指针 */
    unsigned char *srcstart = inbuf;
    unsigned char *tempoutbuf = outbuf;

    printf("utf8:\n");
    for (int i = 0; i < strlen(inbuf); i++)
    {
        printf("%02x ", inbuf[i]);
    }
    printf("\n");

    /* 进行转换
   *@param cd iconv_open()产生的句柄
   *@param srcstart 需要转换的字符串
   *@param srclen 存放还有多少字符没有转换
   *@param tempoutbuf 存放转换后的字符串
   *@param outlen 存放转换后,tempoutbuf剩余的空间
   *
   * */
    outlen = 20;
    printf("1 srcstart=%p, tempoutbuf=%p, srclen=%ld, outlen=%ld\n",
           srcstart, tempoutbuf, srclen, outlen);
    size_t ret = iconv(cd, &srcstart, &srclen, &tempoutbuf, &outlen);
    printf("2 srcstart=%p, tempoutbuf=%p, srclen=%ld, outlen=%ld\n",
           srcstart, tempoutbuf, srclen, outlen);

    if (ret == -1)
    {
        perror("iconv");
    }
    utf16_len = utf16_len - outlen;
    printf("inbuf=%s, srclen=%d, outbuf=%s, outlen=%d,  ret = %d\n",
           inbuf, srclen, outbuf, utf16_len, ret);

    printf("utf16:\n");
    for (int i = 0; i < utf16_len; i++)
    {
        printf("%02x ", outbuf[i]);
    }
    printf("\n");
    /* 关闭句柄 */
    iconv_close(cd);

    return 0;
}