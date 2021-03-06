#define INNUMLEN      8           // input number length in bytes
#define  MAXFILE     255           // maximum filename length
#define  DECNUMDIGITS 31           // maximum precision
#include "decNumber.h"             // base number library
#include "decimal64.h"             // decimal64 conversions

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
// ----- Non-ANSI timer things; may need altering -------------------
//-#ifndef TOPTIMEB
//-  #include <sys\timeb.h>           // timeb.h is in \include\sys
//--#else
//-  #include <timeb.h>               // timeb.h is in \include
//-#endif
// ------------------------------------------------------------------
//#static void showus(char *, struct timeb, struct timeb, int);

/* ------------------------------------------------------------------ */
/* main entry point                                                   */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[]) {

  decNumber sumT, sumB, sumD;      // sums
  decNumber n;                     // number from file
  decNumber p;                     // price
  decNumber b;                     // base tax
  decNumber d;                     // distance tax
  decNumber t;                     // total price
  decNumber baserate, distrate;    // call rates
  decNumber basetax, disttax;      // tax rates
  decNumber mtwo;                  // constant -2

  decContext set;                  // working context

  decimal64 in64;                  // we read into this

  char string[DECNUMDIGITS+14];    // conversion buffer
  int  i, j, numbers;              // counters
  FILE *inp=NULL;                  // input stream structure
  FILE *oup=NULL;                  // output stream structure
  char filein[MAXFILE+1];          // full input filename
  char fileou[MAXFILE+1];          // full output filename
  int  readlen;                    // read data length
  int  frc;                        // ferror return code
  int  calltype;                   // call type (0 or 1)
  int  calc=1;                     // 1 for calculations, 0 to skip
  int  tax=1;                      // 1 for tax calculations, 0 to skip
//-  struct timeb tbStart, tbFinis;   // time buffers
  unsigned int urc, len;           // work
  char **parm;                     // ..

  // set up arithmetic context
  decContextDefault(&set, DEC_INIT_DECIMAL128);   // initialize
  set.traps=0;                                    // no traps
  set.digits=31;                                  // working precision

  // set up the call rates, tax rates and other constants
  decNumberFromString(&baserate, "0.0013", &set); // low call rate
  decNumberFromString(&distrate, "0.00894", &set);// high call rate
  decNumberFromString(&basetax,  "0.0675", &set); // base tax rate
  decNumberFromString(&disttax,  "0.0341", &set); // distance tax rate
  decNumberFromString(&mtwo,     "-2", &set);     // scale setting

  // Get any parameters
  strcpy(filein, "telco.testr");             // default filenames
  strcpy(fileou, "telco.outc");              // ..

  j=0;                                       // parameter number
  for (i=0, parm=argv; i<argc ; parm++, i++) {
    if (strlen(*parm)>MAXFILE) {             // no buffer overruns, please
      printf("Parameter word too long (>%d characters): %s\n", MAXFILE, *parm);
      exit(1); // abandon
      }
    if (i==0) ;                              // 0. is program name
    else if (*parm[0]=='-') {                // flag expected
      if (strcmp(*parm, "-nocalc")==0) calc=0;
      else if (strcmp(*parm, "-notax")==0) tax=0;
      else printf("Flag '%s' ignored\n", *parm);
    } else {
      j++;                                   // have a parameter
      if (j==1) strcpy(filein, *parm);       // is input file name
      else if (j==2) strcpy(fileou, *parm);  // is output file name
      else printf("Extra parameter '%s' ignored\n", *parm);
    }
  } /* getting arguments */

  printf("telco C benchmark; processing '%s'\n", filein);

  remove(fileou);                  // delete output file if it exists

  /* ---------------------------------------------------------------- */
  /* Benchmark timing starts here                                     */
  /* ---------------------------------------------------------------- */
 //- ftime(&tbStart);                 // timestamp

  decNumberZero(&sumT);            // zero accumulators
  decNumberZero(&sumB);
  decNumberZero(&sumD);

  inp=fopen(filein,"rb");          // open file for reading, binary
  if (inp==NULL) {
    printf("Error: file '%s' not found\n", filein);
    exit(0);
  }
  oup=fopen(fileou,"wb");          // open file for writing, binary
  if (oup==NULL) {
    fclose(inp);
    printf("Error: file '%s' could not be opened\n", fileou);
    exit(0);
    }
  // From now on, files must be closed explicitly

  /* Start of the by-number loop */
  for (numbers=0; ; numbers++) {
    // get next 8-byte decimal64 number
    readlen=fread(in64.bytes, 1, DECIMAL64_Bytes, inp);
    if (readlen<DECIMAL64_Bytes) {      // error or EOF
      frc=ferror(inp);                  // check for error
      // [note: ferror is single thread, but only this thread knows INP]
      if (frc!=0) printf("Error: cannot read file '%s'", filein);
      break;                            // [no message if EOF]
    }
    if (calc!=0) {
      calltype=((unsigned)in64.bytes[DECIMAL64_Bytes-1]) & 0x01; // last bit

      // have packed decimal number; convert to working format
      decimal64ToNumber(&in64, &n);

      if (calltype==0)                                 // p=r[c]*n
        decNumberMultiply(&p, &baserate, &n, &set);
      else
        decNumberMultiply(&p, &distrate, &n, &set);
      set.round=DEC_ROUND_HALF_EVEN;                   // round prices
      decNumberRescale(&p, &p, &mtwo, &set);           // to x.xx

      if (tax!=0) {
        set.round=DEC_ROUND_DOWN;                      // truncate taxes
        decNumberMultiply(&b, &p, &basetax, &set);     // b=p*0.0675
        decNumberRescale(&b, &b, &mtwo, &set);         // to x.xx
        decNumberAdd(&sumB, &sumB, &b, &set);          // sumB=sumB+b
        decNumberAdd(&t, &p, &b, &set);                // t=p+b

        if (calltype!=0) {
          decNumberMultiply(&d, &p, &disttax, &set);   // d=p*0.0341
          decNumberRescale(&d, &d, &mtwo, &set);       // to x.xx
          decNumberAdd(&sumD, &sumD, &d, &set);        // sumD=sumD+d
          decNumberAdd(&t, &t, &d, &set);              // t=t+d
        }
      } else {
        t=p;
      }
      decNumberAdd(&sumT, &sumT, &t, &set);            // sumT=sumT+t
      decNumberToString(&t, string);
      // printf("[%d] %d: %s\n", calltype, numbers, string);
    } else {
      strcpy(string, "0.77");
    }
    // [Adding CRLF is part of I/O, as other languages have WriteLine]
    strcat(string, "\r\n");                            // Add CRLF
    len=strlen(string);
    urc=fwrite(string, 1, len, oup);
    if (urc!=len) {
      printf("Error: file '%s' could not be written", fileou);
      break;
    }


  } // numbers


  if (oup!=NULL) fclose(oup);
  if (inp!=NULL) fclose(inp);
  fflush(NULL);
  /* ---------------------------------------------------------------- */
  /* Benchmark timing ends here                                       */
  /* ---------------------------------------------------------------- */
 //- ftime(&tbFinis);

  printf("-- telco C benchmark result --\n");
  printf("   %d numbers read from '%s'\n", numbers, filein);
  //showus("Time per number", tbStart, tbFinis, numbers);
  printf("--\n");
  decNumberToString(&sumT, string);
  printf("   sumT = %s\n", string);
  decNumberToString(&sumB, string);
  printf("   sumB = %s\n", string);
  decNumberToString(&sumD, string);
  printf("   sumD = %s\n", string);

  return 0;
  } // main

/* ------------------------------------------------------------------ */
/* Calculate and display time per iteration in microseconds           */
/*                                                                    */
/*   label is the benchmark identifier (up to 15 characters)          */
/*   time1 is the start time                                          */
/*   time2 is the finish time                                         */
/*   count is the iteration count for the benchmark                   */
/* ------------------------------------------------------------------ */
/*
static void showus(char *label, struct timeb time1, struct timeb time2,
                   int count) {
  int totalms;                     // milliseconds total
  double dtotus;                   // microseconds total (floating)
  double dcount;                   // count (floating)

  totalms=(time2.time-time1.time)*1000
         +time2.millitm-time1.millitm;
  dtotus=((double)totalms)*1000;
  dcount=(double)count;
  printf("   %-15s %7.3fus [%d]\n", label, dtotus/dcount, count);
  } // showus
*/
