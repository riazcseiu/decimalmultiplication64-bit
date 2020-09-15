//#define DECNUMDIGITS 34
#define  MAXFILE     255           // maximum filename length
#define  DECNUMDIGITS 31           // maximum precision
#include "decNumber.h"             // base number library
#include "decimal64.h"             // decimal64 conversions
#include "decDouble.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
/* ------------------------------------------------------------------ */
/* main entry point                                                   */
/* ------------------------------------------------------------------ */
//##int main(int argc, char *argv[]) {
int main() {

 //# decNumber sumT, sumB, sumD;      // sums
  decDouble d_sumT, d_sumB, d_sumD;      // sums
  decDouble d_n,d_p,d_b,d_d,d_t;   //all value in double format
  decDouble  d_baserate, d_distrate, d_basetax, d_disttax, d_mtwo; // all value in double format

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
 
  char inputNumber[20]; 
  unsigned long  totalTime;//cyclestart,cycleend,cyclestart_ex,cycleend_ex; // declare for clock test
  clock_t timeStart = 0;
  clock_t timeEnd   = 0;

  unsigned int urc, len;           // work
  char **parm;                     // ..

  // set up arithmetic context
  
  decContextDefault(&set, DEC_INIT_DECDOUBLE);   // initialize 64 bit operation
 
// # set.traps=0;                                    // no traps
// # set.digits=31;                                  // working precision

  // set up 64-bit format the call rates, tax rates and other constants
  decDoubleFromString(&d_baserate, "0.0013", &set); // low call rate
  decDoubleFromString(&d_distrate, "0.00894", &set);// high call rate
  decDoubleFromString(&d_basetax,  "0.0675", &set); // base tax rate
  decDoubleFromString(&d_disttax,  "0.0341", &set); // distance tax rate
  decDoubleFromString(&d_mtwo,     "-2", &set);     // scale setting




char callDuration[10][26];

//==================RESULT START===========================
strncpy (callDuration[0],"5.99", strlen("5.99")+1);
strncpy (callDuration[1],"1.23", strlen("1.23")+1);
strncpy (callDuration[2],"100.32", strlen("100.32")+1);
strncpy (callDuration[3],"32.99", strlen("32.99")+1);
strncpy (callDuration[4],"233.27", strlen("133.27")+1);
strncpy (callDuration[5],"338.93", strlen("338.93")+1);
strncpy (callDuration[6],"100.32", strlen("100.32")+1);
strncpy (callDuration[7],"32.99", strlen("32.99")+1);
strncpy (callDuration[8],"233.27", strlen("133.27")+1);
strncpy (callDuration[9],"338.93", strlen("338.93")+1);

int _calltype[10] = {1,1,1,1,0,0,0,0,1,1};






  printf("telco C benchmark; processing '%s'\n", filein);

  /* ---------------------------------------------------------------- */
  /* Benchmark timing starts here                                     */
  /* ---------------------------------------------------------------- */
 ////// ftime(&tbStart);                 // timestamp

  timeStart = clock(); //Start count Time comment for Cycle cout
  decDoubleZero(&d_sumT);            // zero accumulators
  decDoubleZero(&d_sumB);
  decDoubleZero(&d_sumD);


  for (int scount =0; scount<11; scount++){

   if (calc!=0) {
         calltype = _calltype[scount];    
         decDoubleFromString(&d_n,  callDuration[scount], &set); //

      if (calltype==0) {                                // p=r[c]*n
          	decDoubleMultiply(&d_p, &d_baserate, &d_n, &set);
                       } 
      else {
            decDoubleMultiply(&d_p, &d_distrate, &d_n, &set);
           }
        //set.round=DEC_ROUND_HALF_EVEN;                   // round prices
    
      if (tax!=0) {
        // set.round=DEC_ROUND_DOWN;                      // truncate taxes
         decDoubleMultiply(&d_b, &d_p, &d_basetax, &set); //double b=p*0.0675
         decDoubleAdd(&d_sumB, &d_sumB, &d_b, &set);          // sumB=sumB+b
         decDoubleAdd(&d_t, &d_p, &d_b, &set);          

      	if (calltype!=0) {
          decDoubleMultiply(&d_d, &d_p, &d_disttax, &set);   // d=p*0.0341
          decDoubleAdd(&d_sumD, &d_sumD, &d_d, &set);        // sumD=sumD+d
          decDoubleAdd(&d_t, &d_t, &d_d, &set);              // t=t+d

        }
      } else {
       //# t=p;
        d_t = d_p;
      }
     decDoubleAdd(&d_sumT, &d_sumT, &d_t, &set);            // sumT=sumT+t
     decDoubleToString(&d_t, string);
     printf("my [%d] %d: %s\n", calltype, numbers, string);


    } else {
      printf("something worng!!");
      strcpy(string, "0.77");
    }
    // [Adding CRLF is part of I/O, as other languages have WriteLine]
    strcat(string, "\r\n");                            // Add CRLF
    len=strlen(string);
    urc=fwrite(string, 1, len, oup);
    if (urc!=len) {
      printf("Error: file '%s' could not be written", fileou);
  //    break;
    } // for loop central


 //## } // numbers


//  if (oup!=NULL) fclose(oup);
//  if (inp!=NULL) fclose(inp);
  fflush(NULL);
  /* ---------------------------------------------------------------- */
  /* Benchmark timing ends here                                       */
  /* ---------------------------------------------------------------- */

      }

   timeEnd = clock(); //Start count Time comment for Cycle cout


  totalTime = ((double)(timeEnd-timeStart) / CLOCKS_PER_SEC );			


 //// ftime(&tbFinis);

  printf("-- telco C benchmark result --\n");
  printf("   %d numbers read from '%s'\n", numbers, filein);
//  showus("Time per number", tbStart, tbFinis, numbers);
   printf("Took %lu second \n", totalTime);


  printf("- 64 bit format-\n");
  decDoubleToString(&d_sumT, string);
  printf("  double sumT = %s\n", string);
  decDoubleToString(&d_sumB, string);
  printf("  double sumB = %s\n", string);
  decDoubleToString(&d_sumD, string);
  printf("  double sumD = %s\n", string);


 decDoubleToString(&d_n, string);
 printf("   n = %s\n", string);






  return 0;
  } // main

