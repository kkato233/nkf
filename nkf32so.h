#ifndef NKF32SO_H
#ifndef CLASS_DECLSPEC
#define CLASS_DECLSPEC
#endif

// Win32 の記述の代替
#define CALLBACK
#define LPSTR char *
#define LPCSTR const char *
#define LPDWORD int *
#define DWORD int
#define BOOL int
#define WINAPI

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#ifndef Thread
#define Thread
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef __BORLANDC__
#pragma argsused
#endif /*__BORLANDC__*/
/* uminchu nkf32103a.lzh 1.00 */
void CALLBACK CLASS_DECLSPEC GetNkfVersion(LPSTR verStr);
int CALLBACK CLASS_DECLSPEC SetNkfOption(LPSTR optStr);
void CALLBACK CLASS_DECLSPEC NkfConvert(LPSTR outStr, LPCSTR inStr);
/* uminchu nkf32103a.lzh 1.02 */
void CALLBACK CLASS_DECLSPEC ToHankaku(LPSTR inStr);
void CALLBACK CLASS_DECLSPEC ToZenkakuKana(LPSTR outStr ,LPCSTR inStr);
/* uminchu nkf32103a.lzh 1.03 */
void CALLBACK CLASS_DECLSPEC EncodeSubject(LPSTR outStr ,LPCSTR inStr);
/* tkaneto nkf32204.zip 2.0.4.0 */
#ifdef TOMIME
void CALLBACK CLASS_DECLSPEC ToMime(LPSTR outStr ,LPCSTR inStr);
#endif /*TOMIME*/
#ifdef GETKANJICODE
int CALLBACK CLASS_DECLSPEC NkfGetKanjiCode(VOID);
#endif /*GETKANJICODE*/
#ifdef FILECONVERT1
void CALLBACK CLASS_DECLSPEC NkfFileConvert1(LPCSTR fName);
#endif /*FILECONVERT1*/
#ifdef FILECONVERT2
void CALLBACK CLASS_DECLSPEC NkfFileConvert2(LPCSTR fInName,LPCSTR fOutName);
#endif /*FILECONVERT2*/
#ifndef NKF32103A
/* safe */
BOOL WINAPI CLASS_DECLSPEC GetNkfVersionSafe(LPSTR verStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC NkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/, LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC ToZenkakuKanaSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC ToHankakuSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC EncodeSubjectSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC NkfFileConvert1Safe(LPCSTR fName,DWORD nBufferLength /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC NkfFileConvert2Safe(LPCSTR fInName,DWORD fInBufferLength /*in TCHARs*/,LPCSTR fOutName,DWORD fOutBufferLength /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC GetNkfGuess(LPSTR outStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC GetNkfSupportFunctions(void *outStr,DWORD nBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC NkfUsage(LPSTR outStr,DWORD nBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/);

int dllprintf(FILE *fp,const char *fmt,...);

void reinitdll(void);
#endif /*!defined(NKF32103A)*/

#ifdef __cplusplus
}       // Balance extern "C" above
#endif /*__cplusplus*/
#endif
