/* nkf32so.dll nfk32dll.c */
/* e-mail:tkaneto@nifty.com */
/* URL: http://www1.ttcn.ne.jp/~kaneto */

/* こちらのバージョンも更新してください。 */
/* DLLが返す */
#define DLL_VERSION   "2.1.5.1 2"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "nkf32so.h"
#include <unistd.h>

#ifndef GUESS
#define GUESS 64
#endif /*GUESS*/

#ifndef GUESS_OPTION_LEN
#define GUESS_OPTION_LEN 30
#endif 

Thread char *guessbuffA = NULL;

Thread char *ubuff;
Thread int ulen;
Thread int uret;

int dllprintf(FILE *fp,const char *fmt,...)
{
    va_list argp;
    int sts;

    if ( uret != FALSE && ulen >= 1 && fmt != NULL && *fmt != 0 ) {
        va_start(argp, fmt);
        sts = snprintf(ubuff,ulen - 1,fmt,argp);
        va_end(argp);
        if ( sts >= 0 ) {
            ubuff += sts;
            ulen -= sts;
        } else {
            uret = FALSE;
        }
        return sts;
    } else return 0;
}


/** Network Kanji Filter. (PDS Version)
************************************************************************
** Copyright (C) 1987, Fujitsu LTD. (Itaru ICHIKAWA)
** 連絡先： （株）富士通研究所　ソフト３研　市川　至 
** （E-Mail Address: ichikawa@flab.fujitsu.co.jp）
** Copyright (C) 1996,1998
** Copyright (C) 2002
** 連絡先： 琉球大学情報工学科 河野 真治  mime/X0208 support
** （E-Mail Address: kono@ie.u-ryukyu.ac.jp）
** 連絡先： COW for DOS & Win16 & Win32 & OS/2
** （E-Mail Address: GHG00637@niftyserve.or.p）
**
**    このソースのいかなる複写，改変，修正も許諾します。ただし、
**    その際には、誰が貢献したを示すこの部分を残すこと。
**    再配布や雑誌の付録などの問い合わせも必要ありません。
**    営利利用も上記に反しない範囲で許可します。
**    バイナリの配布の際にはversion messageを保存することを条件とします。
**    このプログラムについては特に何の保証もしない、悪しからず。
**
**    Everyone is permitted to do anything on this program 
**    including copying, modifying, improving,
**    as long as you don't try to pretend that you wrote it.
**    i.e., the above copyright notice has to appear in all copies.  
**    Binary distribution requires original version messages.
**    You don't have to ask before copying, redistribution or publishing.
**    THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE.
***********************************************************************/

Thread static const unsigned char *cin = NULL;
Thread static int nin = -1;
Thread static int ninmax = -1;
Thread static int std_getc_mode = 1;

int 
std_getc(f)
FILE *f;
{
    if (!nkf_buf_empty_p(nkf_state->std_gc_buf)) {
        return nkf_buf_pop(nkf_state->std_gc_buf);
    } else {
        if ( std_getc_mode == 1 ) {
            return getc(f);
        }
        if ( std_getc_mode == 2 && cin != NULL ) {
            if ( ninmax >= 0 ) {
                if ( nin >= ninmax ) {
                    return EOF;
                } else {
                    nin++;
                    return *cin++;
                }
            } else {
                if ( *cin ) {
                    return *cin++;
                } else {
                    return EOF;
                }
            }
        }
    }
    return EOF;
}

Thread static FILE *fout = NULL;
Thread static unsigned char *cout = NULL;
Thread static int nout = -1;
Thread static int noutmax = -1;
Thread static int std_putc_mode = 1;

void 
std_putc(c)
int c;
{
    if(c!=EOF)
    {
        if ( (std_putc_mode & 1) && fout != NULL ) {
            putc(c,fout);
        }
        if ( (std_putc_mode & 4) && nout != -1 ) {
            if ( noutmax >= 0 && nout >= noutmax ) std_putc_mode &= ~2;
            nout++;
        }
        if ( (std_putc_mode & 2) && cout != NULL ) {
            *cout++ = c;
        }
    }
}

void
print_guessed_code (filename)
    char *filename;
{
	const char *codename = get_guessed_code();
	char guessOption[GUESS_OPTION_LEN + 1];
	guessOption[0] = '\0';

	if (guess_f != 1) {
		sprintf(guessOption, "%s%s%s",
			iconv != w_iconv16 && iconv != w_iconv32 ? "" :
			input_endian == ENDIAN_LITTLE ? " LE" :
			input_endian == ENDIAN_BIG ? " BE" :
			"[BUG]",
			input_bom_f ? " (BOM)" : "",
			input_eol == CR ? " (CR)" :
			input_eol == LF ? " (LF)" :
			input_eol == CRLF ? " (CRLF)" :
			input_eol == EOF ? " (MIXED NL)" : "");
	} 

	if (filename != NULL) {
		guessbuffA = realloc(guessbuffA, (strlen(filename) + GUESS + GUESS_OPTION_LEN + 1) * sizeof(char));
		sprintf(guessbuffA, "%s:%s:%s", filename, codename, guessOption);
	}
	else {
		guessbuffA = realloc(guessbuffA, (GUESS + GUESS_OPTION_LEN + 1) * sizeof(char));
		sprintf(guessbuffA, "%s%s", codename, guessOption);
	}
}

/**
 ** パッチ制作者
 **  void@merope.pleiades.or.jp (Kusakabe Youichi)
 **  NIDE Naoyuki <nide@ics.nara-wu.ac.jp>
 **  ohta@src.ricoh.co.jp (Junn Ohta)
 **  inouet@strl.nhk.or.jp (Tomoyuki Inoue)
 **  kiri@pulser.win.or.jp (Tetsuaki Kiriyama)
 **  Kimihiko Sato <sato@sail.t.u-tokyo.ac.jp>
 **  a_kuroe@kuroe.aoba.yokohama.jp (Akihiko Kuroe)
 **  kono@ie.u-ryukyu.ac.jp (Shinji Kono)
 **  GHG00637@nifty-serve.or.jp (COW)
 **
 **/

void
reinitdll(void)
{
    cin = NULL;
    nin = -1;
    ninmax = -1;
    std_getc_mode = 1;
    fout = stdout;
    cout = NULL;
    nout = -1;
    noutmax = -1;
    std_putc_mode = 1;
    if ( guessbuffA ) {
        free(guessbuffA);
        guessbuffA = NULL;
    }
}

static LPCSTR nkfverA = NKF_VERSION;
static LPCSTR dllverA = DLL_VERSION;

BOOL scp(LPSTR s,LPSTR t,DWORD n)
{
    while ( n ) {
        if ( (*s = *t) == 0 ) return TRUE;
        if ( --n == 0 ) {
            *s = 0;
            break;
        }
        s++;
        t++;
    }
    return FALSE;
}

void CALLBACK GetNkfVersion(LPSTR verStr){
    strcpy(verStr,dllverA);
}

BOOL WINAPI GetNkfVersionSafe(LPSTR verStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/)
{
    *lpTCHARsReturned = strlen(dllverA) + 1;
    if ( verStr == NULL || nBufferLength == 0 ) return FALSE;
    return scp(verStr,dllverA,nBufferLength);
}

Thread static LPSTR optStr0 = NULL;

int CALLBACK SetNkfOption(LPSTR optStr)
{
    LPSTR p;
    int len;

    if ( *optStr == '-' ) {
        len = strlen(optStr) + 1;
        p = realloc(optStr0,len);
        strcpy(p,optStr);
    } else {
        len = strlen(optStr) + 2;
        p = realloc(optStr0,len);
        *p = '-';
        strcpy(p + 1,optStr);
    }
    optStr0 = p;
    return 0;
}

int findspace(const char* str)
{
	int find_n = 0;
	while (*str != 0) {
		if (*str == ' ') {
			find_n++;
		}
		str++;
	}
	return find_n;
}
// オプションが 空白で区切られている場合に options を複数呼び出す
void callOptions()
{
	char *work = strdup(optStr0);
	int len = strlen(optStr0);
	for (int i = 0; i < len; i++) {
		if (work[i] == ' ') {
			work[i] = '\0';
		}
	}
	int i = 0;
	while (i < len)
	{
		// 文字を探す
		if (work[i] != '\0') {
			// options 呼び出し
			options(&work[i]);

			// 文字の終端を探す
			while (work[i] != '\0' && i < len) {
				i++;
			}
		}
		else {
			i++;
		}
	}
	free(work);
}

// プロセス起動時に初期値に設定される値を初期設定する。
// dll を使ったコード変換は、フラグ変数が初期化されずに
// 予想外の動きをする場合がある現象の修正用
void reinit_dll()
{
#ifdef UTF8_INPUT_ENABLE
	input_endian = ENDIAN_BIG;
	input_bom_f = FALSE;
#endif
	input_eol = 0;
}

void options0(void)
{
    reinit();
	reinit_dll();
    if ( optStr0 != NULL ) {
		// option の中に 空白があった場合 options を分割して呼び出す
		if (findspace(optStr0) > 0) {
			callOptions();
		}
		else {
			options(optStr0);
		}
    }
}

void CALLBACK NkfConvert(LPSTR outStr, LPCSTR inStr)
{
    options0();
    std_putc_mode = 2;
    cout = outStr;
    noutmax = -1;
    nout = -1;
    std_getc_mode = 2;
    cin = inStr;
    ninmax = -1;
    nin = -1;
    kanji_convert(NULL);
    *cout = 0;
}

BOOL WINAPI NkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/, LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/){
    if ( inStr == NULL ) return FALSE;
    options0();
    std_putc_mode = 6;
    cout = outStr;
    noutmax = nOutBufferLength;
    nout = 0;
    std_getc_mode = 2;
    cin = inStr;
    ninmax = nInBufferLength;
    nin = 0;
    kanji_convert(NULL);
    *lpBytesReturned = nout;
    if ( nout < noutmax ) *cout = 0;
    return TRUE;
}

void CALLBACK ToHankaku(LPSTR inStr)
{
    unsigned char *p;
    int len;

    len = strlen(inStr) + 1;
    p = malloc(len);
    if ( p == NULL ) return;
    memcpy(p,inStr,len);
    options("-ZSs");
    NkfConvert(inStr,p);
    free(p);
}

BOOL WINAPI ToHankakuSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/)
{
    options("-ZSs");
    return NkfConvertSafe(outStr,nOutBufferLength,lpBytesReturned,inStr,nInBufferLength);
}

void CALLBACK ToZenkakuKana(LPSTR outStr, LPCSTR inStr)
{
    options("-Ss");
    NkfConvert(outStr, inStr);
}

BOOL WINAPI ToZenkakuKanaSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/)
{
    options("-Ss");
    return NkfConvertSafe(outStr,nOutBufferLength,lpBytesReturned,inStr,nInBufferLength);
}

void CALLBACK EncodeSubject(LPSTR outStr ,LPCSTR inStr){
    options("-jM");
    NkfConvert(outStr, inStr);
}

BOOL WINAPI EncodeSubjectSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/)
{
    options("-jM");
    return NkfConvertSafe(outStr,nOutBufferLength,lpBytesReturned,inStr,nInBufferLength);
}

#ifdef TOMIME
void CALLBACK ToMime(LPSTR outStr ,LPCSTR inStr)
{
    EncodeSubject(outStr,inStr);
}
#endif /*TOMIME*/

#ifdef GETKANJICODE
int CALLBACK NkfGetKanjiCode(VOID)
{
    int iCode=0;
    /* if(iconv == s_iconv)iCode=0; */ /* 0:シフトJIS */
    if(iconv == w_iconv)iCode=3; /* UTF-8 */
    else if(iconv == w_iconv16){
        if(input_endian == ENDIAN_BIG)iCode=5; /* 5:UTF-16BE */
        else iCode=4; /* 4:UTF-16LE */
    }else if(iconv == e_iconv){
        if(estab_f == FALSE)iCode=2; /* 2:ISO-2022-JP */
        else iCode=1; /* 1:EUC */
    }
    return iCode;
}
#endif /*GETKANJICODE*/

#ifdef FILECONVERT1
void CALLBACK NkfFileConvert1(LPCSTR fName)
{
    FILE *fin;
    char *tempdname;
    char tempfname[MAX_PATH];
    char d[4];
    DWORD len;
    BOOL sts;

    options0();
    len = GetTempPath(sizeof d,d);
    tempdname = malloc(len + 1);
    if ( tempdname == NULL ) return;
    len = GetTempPath(len + 1,tempdname);
    sts = GetTempFileName(tempdname,"NKF",0,tempfname);
    if ( sts != 0 )  {
        sts = CopyFileA(fName,tempfname,FALSE);
         if ( sts ) {
             if ((fin = fopen(tempfname, "rb")) != NULL) {
                 if ((fout = fopen(fName, "wb")) != NULL) {
                     cin = NULL;
                     nin = -1;
                     ninmax = -1;
                     std_getc_mode = 1;
                     cout = NULL;
                     nout = -1;
                     noutmax = -1;
                     std_putc_mode = 1;
                     kanji_convert(fin);
                     fclose(fin);
                 }
                 fclose(fout);
             }
        DeleteFile(tempfname);
        }
    }
    free(tempdname);
}
#endif /*FILECONVERT1*/


#define BUFSIZE 1024
BOOL my_file_copy(const char * from, const char *to) {
    char buf[BUFSIZE];

    FILE *fp_r = fopen(from, "rb");
    if (!fp_r) return FALSE;

    FILE *fp_w = fopen(to, "wb");
    if (!fp_w) {
        fclose(fp_r);
        return FALSE;
    }

    int read_n = fread(buf, 1, BUFSIZE, fp_r);
    while(read_n > 0) {
        fwrite(buf, 1, read_n, fp_w);
        read_n = fread(buf, 1, BUFSIZE, fp_r);
    }
    fclose(fp_r);
    fclose(fp_w);
    return TRUE;
}

BOOL WINAPI NkfFileConvert1Safe(LPCSTR fName,DWORD nBufferLength /*in TCHARs*/)
{
    FILE *fin;
    char *tempdname;
    char d[4];
    DWORD len;
    BOOL sts;
    BOOL ret;
    LPCSTR p;

    options0();
    ret = FALSE;
    p = fName;
    for ( ;; ) {
        if ( nBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --nBufferLength;
    }
    if ( chmod(fName,S_IREAD | S_IWRITE) == -1 ) return ret;
    char tempfname[] = "/tmp/NKF_XXXXXX";
    sts = my_file_copy(fName,tempfname);
    if ( sts ) {
        if ((fin = fopen(tempfname, "rb")) != NULL) {
            if ((fout = fopen(fName, "wb")) != NULL) {
                cin = NULL;
                nin = -1;
                ninmax = -1;
                std_getc_mode = 1;
                cout = NULL;
                nout = -1;
                noutmax = -1;
                std_putc_mode = 1;
                kanji_convert(fin);
                fclose(fin);
                ret = TRUE;
            }
            fclose(fout);
        }
        unlink(tempfname);
    }
    return ret;
}

#ifdef FILECONVERT2
void CALLBACK NkfFileConvert2(LPCSTR fInName,LPCSTR fOutName)
{
    FILE *fin;

    options0();
    if ((fin = fopen(fInName, "rb")) == NULL) return;
    if((fout=fopen(fOutName, "wb")) == NULL) {
        fclose(fin);
        return;
    }
    cin = NULL;
    nin = -1;
    ninmax = -1;
    std_getc_mode = 1;
    cout = NULL;
    nout = -1;
    noutmax = -1;
    std_putc_mode = 1;
    kanji_convert(fin);
    fclose(fin);
    fclose(fout);
}
#endif /*FILECONVERT2*/

BOOL WINAPI NkfFileConvert2Safe(LPCSTR fInName,DWORD fInBufferLength /*in TCHARs*/,LPCSTR fOutName,DWORD fOutBufferLength /*in TCHARs*/)
{
    FILE *fin;
    BOOL sts;
    BOOL ret;
    LPCSTR p;

    options0();
    ret = FALSE;
    p = fInName;
    for ( ;; ) {
        if ( fInBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --fInBufferLength;
    }
    p = fOutName;
    for ( ;; ) {
        if ( fOutBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --fOutBufferLength;
    }
    if ((fin = fopen(fInName, "rb")) != NULL) {
        if((fout=fopen(fOutName, "wb")) != NULL) {
            cin = NULL;
            nin = -1;
            ninmax = -1;
            std_getc_mode = 1;
            cout = NULL;
            nout = -1;
            noutmax = -1;
            std_putc_mode = 1;
            kanji_convert(fin);
            fclose(fin);
            ret = TRUE;
        }
        fclose(fout);
    }
    return ret;
}

BOOL WINAPI GetNkfGuess(LPSTR outStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/)
{
    if ( outStr == NULL || nBufferLength == 0 ) return FALSE;
    print_guessed_code(NULL);
    *lpTCHARsReturned = strlen(guessbuffA) + 1;
    return scp(outStr,guessbuffA,nBufferLength);
}

static struct {
DWORD size;
LPCSTR copyrightA;
LPCSTR versionA;
LPCSTR dateA;
DWORD functions;
} NkfSupportFunctions = {
sizeof(NkfSupportFunctions),
NULL,
NKF_VERSION,
NKF_RELEASE_DATE,
1 /* nkf32103a.lzh uminchu 1.03 */
/* | 2 */ /* nkf32dll.zip 0.91 */
#if defined(TOMIME) && defined(GETKANJICODE) && defined(FILECONVERT1) && defined(FILECONVERT2) 
| 4 /* nkf32204.zip Kaneto 2.0.4.0 */
#endif
| 8 /* this */
| 0x80000000
,
};

BOOL WINAPI GetNkfSupportFunctions(void *outStr,DWORD nBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/)
{
    *lpBytesReturned = sizeof NkfSupportFunctions;
    if ( outStr == NULL || nBufferLength == 0 ) return FALSE;
    NkfSupportFunctions.copyrightA = COPY_RIGHT;
    memcpy(outStr,&NkfSupportFunctions,sizeof NkfSupportFunctions > nBufferLength ? nBufferLength : sizeof NkfSupportFunctions);
    return TRUE;
}

BOOL WINAPI NkfUsage(LPSTR outStr,DWORD nBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/)
{
    ubuff = outStr;
    ulen = nBufferLength;
    uret = TRUE;
    usage();
    if ( uret == TRUE ) {
        *lpBytesReturned = nBufferLength - ulen;
    }
    return uret;
}


void Init() {
    reinit();
}

