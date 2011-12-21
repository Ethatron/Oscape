#define CBASH
#ifdef	CBASH

#include <CBash.h>
#pragma comment(lib,"CBash")

extern string wedata; extern long weoffs;
extern string weoutn; extern bool calcn;
extern string weouth; extern bool calch;
extern string weoutw; extern bool calcw;
extern string weoutx; extern bool calcx;
extern string weoutm; extern bool calcm;
extern string wename; extern int weid;

DWORD __stdcall ExtractFromCollection(LPVOID lp);

#endif
