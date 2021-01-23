/**********************************************************************
*   版权     : http://0voice.ke.qq.com  
*   版本     : 0.1
*   作者     : darren 
*   联系方式 ：QQ 326873713
*   创建时间 : 2019-10-12
*   功能描述 : 字符转码工具
***********************************************************************/
#ifndef __CHARSET_CONVERTER_H__
#define __CHARSET_CONVERTER_H__

#include <iconv.h>
#include <string>

/*
功能：编码转类
参数：
	strFromCharset：源编码
	strToCharset：目标编码
	strSrc：源字符串
	strResult：目标字符串
	bErrContinue：是否忽略转换错误
	Convert返回值：-1表示转换出错
用法：
	基类可以转换转换用户指定编码的字串
	派生类转换特定编码的字串
	
支持的编码：
	
European languages
	ASCII, ISO−8859−{1,2,3,4,5,7,9,10,13,14,15,16}, KOI8−R, KOI8−U, KOI8−RU, CP{1250,1251,1252,1253,1254,1257}, CP{850,866,1131}, Mac{Roman,CentralEurope,Iceland,Croatian,Romania}, Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh

Semitic languages
	ISO−8859−{6,8}, CP{1255,1256}, CP862, Mac{Hebrew,Arabic}

Japanese
	EUC−JP, SHIFT_JIS, CP932, ISO−2022−JP, ISO−2022−JP−2, ISO−2022−JP−1

Chinese
	EUC−CN, HZ, GBK, CP936, GB18030, EUC−TW, BIG5, CP950, BIG5−HKSCS, BIG5−HKSCS:2001, BIG5−HKSCS:1999, ISO−2022−CN, ISO−2022−CN−EXT

Korean
	EUC−KR, CP949, ISO−2022−KR, JOHAB

Armenian
	ARMSCII−8

Georgian
	Georgian−Academy, Georgian−PS

Tajik
	KOI8−T

Kazakh
	PT154, RK1048

Thai
	TIS−620, CP874, MacThai

Laotian
	MuleLao−1, CP1133

Vietnamese
	VISCII, TCVN, CP1258

Platform specifics
	HP−ROMAN8, NEXTSTEP

Full Unicode
	UTF−8
	UCS−2, UCS−2BE, UCS−2LE
	UCS−4, UCS−4BE, UCS−4LE
	UTF−16, UTF−16BE, UTF−16LE
	UTF−32, UTF−32BE, UTF−32LE
	UTF−7
	C99, JAVA

Full Unicode, in terms of uint16_t or uint32_t
(with machine dependent endianness and alignment)
	UCS−2−INTERNAL, UCS−4−INTERNAL

Locale dependent, in terms of char or wchar_t
	(with machine dependent endianness and alignment, and with semantics depending on the OS and the current LC_CTYPE locale facet)

	char, wchar_t

When configured with the option −−enable−extra−encodings, it also provides support for a few extra encodings:
European languages
	CP{437,737,775,852,853,855,857,858,860,861,863,865,869,1125}
Semitic languages

CP864
Japanese
	EUC−JISX0213, Shift_JISX0213, ISO−2022−JP−3

Chinese
	BIG5−2003 (experimental)

Turkmen
	TDS565

Platform specifics
ATARIST, RISCOS−LATIN1	
*/

class CharsetConverter
{
public:
	CharsetConverter() {}
	CharsetConverter(const std::string &strFromCharset, const std::string &strToCharset);
	virtual ~CharsetConverter();
	int Convert(const std::string &strSrc, std::string &strResult,
				bool bErrContinue = true);

	static int Convert(iconv_t iCd,
					   const std::string &strSrc, std::string &strResult,
					   bool bErrContinue = true);

	static int Convert(const std::string &strFromCharset, const std::string &strToCharset,
					   const std::string &strSrc, std::string &strResult,
					   bool bErrContinue = true);

protected:
	iconv_t cd;
};

// 通过集成的方式扩展字符集转换类
class CUnicodeToGbk : public CharsetConverter
{
public:
	CUnicodeToGbk();
};

class CUtf8ToGbk : public CharsetConverter
{
public:
	CUtf8ToGbk();
};

class CUtf8ToUtf16 : public CharsetConverter
{
public:
	CUtf8ToUtf16();
};

class CUtf16ToGbk : public CharsetConverter
{
public:
	CUtf16ToGbk();
};
class CUtf16ToUtf8 : public CharsetConverter
{
public:
	CUtf16ToUtf8();
};

class CUtf16BEToGbk : public CharsetConverter
{
public:
	CUtf16BEToGbk();
};

class CBig5ToGbk : public CharsetConverter
{
public:
	CBig5ToGbk();
};

class CISO8859ToGbk : public CharsetConverter
{
public:
	CISO8859ToGbk();
};

class CHzToGbk : public CharsetConverter
{
public:
	CHzToGbk();
};

class CGb2312ToGbk : public CharsetConverter
{
public:
	CGb2312ToGbk();
};

class CGbkToUtf8 : public CharsetConverter
{
public:
	CGbkToUtf8();
};

class CUtf8ToGb2312 : public CharsetConverter
{
public:
	CUtf8ToGb2312();
};

class CGb2312ToUtf8 : public CharsetConverter
{
public:
	CGb2312ToUtf8();
};
#endif
