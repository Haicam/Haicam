#include "SoundWaveReceiver.h"

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include "liquid/liquid.h"


#define BITSTREAM_LEN       12800
#define MAX_BITSTREAM_NUM   1

void (*soundWaveSuccess) (char* data,unsigned int size) = NULL;

static bool sound_wave_started = false;

static float complex  *din;
static float complex  *dout;

//FILE *ff;

static int sample_rate;
static int HANDSHAKE_START_HZ = 3800;
static int HANDSHAKE_END_HZ = 3900;

static int HANDSHAKE_MAX_HZ = 7100;

static int START_HZ = 4000;
static int STEP_HZ = 200;

static unsigned short* strRecData = NULL;
static unsigned short curPos = 0;
static unsigned char* lastRecData = NULL;
static unsigned short lastlength = 0;
static unsigned char* lastDataCheck = NULL;
static unsigned short lastDataChecklength = 0;
static unsigned char* curData = NULL;

#define Samplingfrequency  2
char recStart = 0;


#define mm 4           /* RS code over GF(2**4) - change to suit */

static  int nn = 15;          /* nn=2**mm -1   length of codeword */
static int tt = 3;           /* number of errors that can be corrected */
static int kk = 9;           /* kk = nn-2*tt  */

static int pp [mm+1] = { 1, 1, 0, 0, 1} ; /* specify irreducible polynomial coeffts */

static int *alpha_to = NULL, *index_of = NULL, *gg = NULL;
static int *recd = NULL, *data = NULL, *bb = NULL;
/*
 int alpha_to [nn+1], index_of [nn+1], gg [nn-kk+1] ;
 int recd [nn], data [kk], bb [nn-kk] ;
 */

//rs（3，1）, rs（4，2）, rs（5，3）, rs（8，4）, rs（9，5）
//rs（10，6 , rs（13，7）, rs（14，8) , rs（15，9)
static void RS_init(int _nn, int _kk)
{
    nn = _nn;
    kk = _kk;
    tt = (nn - kk) / 2;
    if (!alpha_to){
        alpha_to = (int*)malloc((nn+1) * sizeof(int));
        memset((char*)alpha_to, 0, sizeof(alpha_to));
    }
    if (!index_of){
        index_of = (int*)malloc((nn+1) * sizeof(int));
        memset((char*)index_of, 0, sizeof(index_of));
    }
    if (!gg){
        gg = (int*)malloc((nn-kk+1) * sizeof(int));
        memset((char*)gg, 0, sizeof(gg));
    }
    if (!recd){
        recd = (int*)malloc(nn * sizeof(int));
        memset((char*)recd, 0, sizeof(recd));
    }
    if (!data){
        data = (int*)malloc(kk * sizeof(int));
        memset((char*)data, 0, sizeof(data));
    }
    if (!bb){
        bb = (int*)malloc((nn-kk) * sizeof(int));
        memset((char*)bb, 0, sizeof(bb));
    }
}

static void RS_release()
{
    if (alpha_to){
        free(alpha_to);
        alpha_to = NULL;
    }
    if (index_of){
        free(index_of);
        index_of = NULL;
    }
    if (gg){
        free(gg);
        gg = NULL;
    }
    if (recd){
        free(recd);
        recd = NULL;
    }
    if (data){
        free(data);
        data = NULL;
    }
    if (bb){
        free(bb);
        bb = NULL;
    }
}

static void generate_gf()
/* generate GF(2**mm) from the irreducible polynomial p(X) in pp[0]..pp[mm]
 lookup tables:  index->polynomial form   alpha_to[] contains j=alpha**i;
 polynomial form -> index form  index_of[j=alpha**i] = i
 alpha=2 is the primitive element of GF(2**mm)
 */
{
    register int i, mask ;
    
    mask = 1 ;
    alpha_to[mm] = 0 ;
    for (i=0; i<mm; i++)
    { alpha_to[i] = mask ;
        index_of[alpha_to[i]] = i ;
        if (pp[i]!=0)
            alpha_to[mm] ^= mask ;
        mask <<= 1 ;
    }
    index_of[alpha_to[mm]] = mm ;
    mask >>= 1 ;
    for (i=mm+1; i<nn; i++)
    { if (alpha_to[i-1] >= mask)
        alpha_to[i] = alpha_to[mm] ^ ((alpha_to[i-1]^mask)<<1) ;
    else alpha_to[i] = alpha_to[i-1]<<1 ;
        index_of[alpha_to[i]] = i ;
    }
    index_of[0] = -1 ;
}


static void gen_poly()
/* Obtain the generator polynomial of the tt-error correcting, length
 nn=(2**mm -1) Reed Solomon code  from the product of (X+alpha**i), i=1..2*tt
 */
{
    register int i,j ;
    
    gg[0] = 2 ;    /* primitive element alpha = 2  for GF(2**mm)  */
    gg[1] = 1 ;    /* g(x) = (X+alpha) initially */
    for (i=2; i<=nn-kk; i++)
    { gg[i] = 1 ;
        for (j=i-1; j>0; j--)
            if (gg[j] != 0)  gg[j] = gg[j-1]^ alpha_to[(index_of[gg[j]]+i)%nn] ;
            else gg[j] = gg[j-1] ;
        gg[0] = alpha_to[(index_of[gg[0]]+i)%nn] ;     /* gg[0] can never be zero */
    }
    /* convert gg[] to index form for quicker encoding */
    for (i=0; i<=nn-kk; i++)  gg[i] = index_of[gg[i]] ;
}


static void encode_rs()
/* take the string of symbols in data[i], i=0..(k-1) and encode systematically
 to produce 2*tt parity symbols in bb[0]..bb[2*tt-1]
 data[] is input and bb[] is output in polynomial form.
 Encoding is done by using a feedback shift register with appropriate
 connections specified by the elements of gg[], which was generated above.
 Codeword is   c(X) = data(X)*X**(nn-kk)+ b(X)          */
{
    register int i,j ;
    int feedback ;
    
    for (i=0; i<nn-kk; i++)   bb[i] = 0 ;
    for (i=kk-1; i>=0; i--)
    {  feedback = index_of[data[i]^bb[nn-kk-1]] ;
        if (feedback != -1)
        { for (j=nn-kk-1; j>0; j--)
            if (gg[j] != -1)
                bb[j] = bb[j-1]^alpha_to[(gg[j]+feedback)%nn] ;
            else
                bb[j] = bb[j-1] ;
            bb[0] = alpha_to[(gg[0]+feedback)%nn] ;
        }
        else
        { for (j=nn-kk-1; j>0; j--)
            bb[j] = bb[j-1] ;
            bb[0] = 0 ;
        } ;
    } ;
} ;



static void decode_rs()
/* assume we have received bits grouped into mm-bit symbols in recd[i],
 i=0..(nn-1),  and recd[i] is index form (ie as powers of alpha).
 We first compute the 2*tt syndromes by substituting alpha**i into rec(X) and
 evaluating, storing the syndromes in s[i], i=1..2tt (leave s[0] zero) .
 Then we use the Berlekamp iteration to find the error location polynomial
 elp[i].   If the degree of the elp is >tt, we cannot correct all the errors
 and hence just put out the information symbols uncorrected. If the degree of
 elp is <=tt, we substitute alpha**i , i=1..n into the elp to get the roots,
 hence the inverse roots, the error location numbers. If the number of errors
 located does not equal the degree of the elp, we have more than tt errors
 and cannot correct them.  Otherwise, we then solve for the error value at
 the error location and correct the error.  The procedure is that found in
 Lin and Costello. For the cases where the number of errors is known to be too
 large to correct, the information symbols as received are output (the
 advantage of systematic encoding is that hopefully some of the information
 symbols will be okay and that if we are in luck, the errors are in the
 parity part of the transmitted codeword).  Of course, these insoluble cases
 can be returned as error flags to the calling routine if desired.   */
{
    register int i,j,u,q ;
    int elp[nn-kk+2][nn-kk], d[nn-kk+2], l[nn-kk+2], u_lu[nn-kk+2], s[nn-kk+1] ;
    int count=0, syn_error=0, root[tt], loc[tt], z[tt+1], err[nn], reg[tt+1] ;
//    printf("decode_rs 1\n");
    /* first form the syndromes */
    for (i=1; i<=nn-kk; i++)
    { s[i] = 0 ;
        for (j=0; j<nn; j++)
            if (recd[j]!=-1)
                s[i] ^= alpha_to[(recd[j]+i*j)%nn] ;      /* recd[j] in index form */
        /* convert syndrome from polynomial form to index form  */
        if (s[i]!=0)  syn_error=1 ;        /* set flag if non-zero syndrome => error */
        s[i] = index_of[s[i]] ;
    } ;
//    printf("decode_rs 2\n");
    if (syn_error)       /* if errors, try and correct */
    {
//        printf("decode_rs 21\n");
        /* compute the error location polynomial via the Berlekamp iterative algorithm,
         following the terminology of Lin and Costello :   d[u] is the 'mu'th
         discrepancy, where u='mu'+1 and 'mu' (the Greek letter!) is the step number
         ranging from -1 to 2*tt (see L&C),  l[u] is the
         degree of the elp at that step, and u_l[u] is the difference between the
         step number and the degree of the elp.
         */
        /* initialise table entries */
        d[0] = 0 ;           /* index form */
        d[1] = s[1] ;        /* index form */
        elp[0][0] = 0 ;      /* index form */
        elp[1][0] = 1 ;      /* polynomial form */
        for (i=1; i<nn-kk; i++)
        { elp[0][i] = -1 ;   /* index form */
            elp[1][i] = 0 ;   /* polynomial form */
        }
        l[0] = 0 ;
        l[1] = 0 ;
        u_lu[0] = -1 ;
        u_lu[1] = 0 ;
        u = 0 ;
        
        do
        {
            u++ ;
            if (d[u]==-1)
            { l[u+1] = l[u] ;
                for (i=0; i<=l[u]; i++)
                {  elp[u+1][i] = elp[u][i] ;
                    elp[u][i] = index_of[elp[u][i]] ;
                }
            }
            else
            /* search for words with greatest u_lu[q] for which d[q]!=0 */
            { q = u-1 ;
                while ((d[q]==-1) && (q>0)) q-- ;
                /* have found first non-zero d[q]  */
                if (q>0)
                { j=q ;
                    do
                    { j-- ;
                        if ((d[j]!=-1) && (u_lu[q]<u_lu[j]))
                            q = j ;
                    }while (j>0) ;
                } ;
                
                /* have now found q such that d[u]!=0 and u_lu[q] is maximum */
                /* store degree of new elp polynomial */
                if (l[u]>l[q]+u-q)  l[u+1] = l[u] ;
                else  l[u+1] = l[q]+u-q ;
                
                /* form new elp(x) */
                for (i=0; i<nn-kk; i++)    elp[u+1][i] = 0 ;
                for (i=0; i<=l[q]; i++)
                    if (elp[q][i]!=-1)
                        elp[u+1][i+u-q] = alpha_to[(d[u]+nn-d[q]+elp[q][i])%nn] ;
                for (i=0; i<=l[u]; i++)
                { elp[u+1][i] ^= elp[u][i] ;
                    elp[u][i] = index_of[elp[u][i]] ;  /*convert old elp value to index*/
                }
            }
            u_lu[u+1] = u-l[u+1] ;
            
            /* form (u+1)th discrepancy */
            if (u<nn-kk)    /* no discrepancy computed on last iteration */
            {
                if (s[u+1]!=-1)
                    d[u+1] = alpha_to[s[u+1]] ;
                else
                    d[u+1] = 0 ;
                for (i=1; i<=l[u+1]; i++)
                    if ((s[u+1-i]!=-1) && (elp[u+1][i]!=0))
                        d[u+1] ^= alpha_to[(s[u+1-i]+index_of[elp[u+1][i]])%nn] ;
                d[u+1] = index_of[d[u+1]] ;    /* put d[u+1] into index form */
            }
        } while ((u<nn-kk) && (l[u+1]<=tt)) ;
//        printf("decode_rs 3\n");
        u++ ;
        if (l[u]<=tt)         /* can correct error */
        {
            /* put elp into index form */
            for (i=0; i<=l[u]; i++)   elp[u][i] = index_of[elp[u][i]] ;
            
            /* find roots of the error location polynomial */
            for (i=1; i<=l[u]; i++)
                reg[i] = elp[u][i] ;
            count = 0 ;
            for (i=1; i<=nn; i++)
            {  q = 1 ;
                for (j=1; j<=l[u]; j++)
                    if (reg[j]!=-1)
                    { reg[j] = (reg[j]+j)%nn ;
                        q ^= alpha_to[reg[j]] ;
                    } ;
                if (!q)        /* store root and error location number indices */
                { root[count] = i;
                    loc[count] = nn-i ;
                    count++ ;
                };
            } ;
            if (count==l[u])    /* no. roots = degree of elp hence <= tt errors */
            {
                /* form polynomial z(x) */
                for (i=1; i<=l[u]; i++)        /* Z[0] = 1 always - do not need */
                { if ((s[i]!=-1) && (elp[u][i]!=-1))
                    z[i] = alpha_to[s[i]] ^ alpha_to[elp[u][i]] ;
                else if ((s[i]!=-1) && (elp[u][i]==-1))
                    z[i] = alpha_to[s[i]] ;
                else if ((s[i]==-1) && (elp[u][i]!=-1))
                    z[i] = alpha_to[elp[u][i]] ;
                else
                    z[i] = 0 ;
                    for (j=1; j<i; j++)
                        if ((s[j]!=-1) && (elp[u][i-j]!=-1))
                            z[i] ^= alpha_to[(elp[u][i-j] + s[j])%nn] ;
                    z[i] = index_of[z[i]] ;         /* put into index form */
                } ;
                
                /* evaluate errors at locations given by error location numbers loc[i] */
                for (i=0; i<nn; i++)
                { err[i] = 0 ;
                    if (recd[i]>=0 && recd[i]<=nn) {       /* convert recd[] to polynomial form */
                        recd[i] = alpha_to[recd[i]] ;
                    }else  recd[i] = 0 ;
                }
                for (i=0; i<l[u]; i++)    /* compute numerator of error term first */
                { err[loc[i]] = 1;       /* accounts for z[0] */
                    for (j=1; j<=l[u]; j++)
                        if (z[j]!=-1)
                            err[loc[i]] ^= alpha_to[(z[j]+j*root[i])%nn] ;
                    if (err[loc[i]]!=0)
                    { err[loc[i]] = index_of[err[loc[i]]] ;
                        q = 0 ;     /* form denominator of error term */
                        for (j=0; j<l[u]; j++)
                            if (j!=i)
                                q += index_of[1^alpha_to[(loc[j]+root[i])%nn]] ;
                        q = q % nn ;
                        err[loc[i]] = alpha_to[(err[loc[i]]-q+nn)%nn] ;
                        recd[loc[i]] ^= err[loc[i]] ;  /*recd[i] must be in polynomial form */
                    }
                }
            }
            else  {  /* no. roots != degree of elp => >tt errors and cannot solve */
                for (i=0; i<nn; i++) {       /* could return error flag if desired */
                    if (recd[i]>=0 && recd[i]<=nn) {       /* convert recd[] to polynomial form */
                        recd[i] = alpha_to[recd[i]] ;
                    }else  recd[i] = 0 ;     /* just output received codeword as is */
                }
            }
        }
        else {        /* elp has degree has degree >tt hence cannot solve */
            for (i=0; i<nn; i++) {      /* could return error flag if desired */
                if (recd[i]>=0 && recd[i]<=nn) {       /* convert recd[] to polynomial form */
                    recd[i] = alpha_to[recd[i]] ;
                }else  recd[i] = 0 ;     /* just output received codeword as is */
            }
        }
    }
    else {      /* no non-zero syndromes => no errors: output received codeword */
        for (i=0; i<nn; i++) {
            if (recd[i]>=0 && recd[i]<=nn) {       /* convert recd[] to polynomial form */
                recd[i] = alpha_to[recd[i]] ;
            }else  recd[i] = 0 ;
        }
    }
}



static void RS_encode(int* _data, int _data_len, int* _bb, int _bb_len)
{
    int i;
    
    /* generate the Galois Field GF(2**mm) */
    generate_gf() ;
    
    /* compute the generator polynomial for this RS code */
    gen_poly() ;
    
    memcpy(data, _data, kk * sizeof(int));
    
    /* encode data[] to produce parity in bb[].  Data input and parity output
     is in polynomial form
     */
    encode_rs() ;
    
    memcpy(_bb, bb, (nn-kk) * sizeof(int));
}

static void RS_decode(int* _data, int data_len, int* _bb, int bb_len)
{
    
    int i;
    
    memcpy(bb, _bb, (nn-kk) * sizeof(int));
    memcpy(data, _data, kk * sizeof(int));
    
    /* generate the Galois Field GF(2**mm) */
//    printf("generate_gf\n");
    generate_gf() ;
//    printf("gen_poly\n");
    /* compute the generator polynomial for this RS code */
    gen_poly() ;
    
    /* put the transmitted codeword, made up of data plus parity, in recd[] */
    for (i=0; i<nn-kk; i++)  recd[i] = bb[i] ;
    for (i=0; i<kk; i++) recd[i+nn-kk] = data[i] ;
    
    
    for (i=0; i<nn; i++){
//        printf("recd[%d]:%d ---",i,recd[i]);
        recd[i] = index_of[recd[i]] ;          /* put recd[i] into index form */
//        printf("recd[%d]:%d \n",i,recd[i]);
    }
//    printf("decode_rs\n");
    /* decode recv[] */
    decode_rs() ;         /* recd[] is returned in polynomial form */
//    printf("decode_rs end\n");
    memcpy(_bb, recd, (nn-kk) * sizeof(int));
    memcpy(_data, recd + (nn-kk), kk * sizeof(int));
    
}

static unsigned char const table_byte[256] = {
    0x0, 0x7, 0xe, 0x9, 0x5, 0x2, 0xb, 0xc, 0xa, 0xd, 0x4, 0x3, 0xf, 0x8, 0x1, 0x6,
    0xd, 0xa, 0x3, 0x4, 0x8, 0xf, 0x6, 0x1, 0x7, 0x0, 0x9, 0xe, 0x2, 0x5, 0xc, 0xb,
    0x3, 0x4, 0xd, 0xa, 0x6, 0x1, 0x8, 0xf, 0x9, 0xe, 0x7, 0x0, 0xc, 0xb, 0x2, 0x5,
    0xe, 0x9, 0x0, 0x7, 0xb, 0xc, 0x5, 0x2, 0x4, 0x3, 0xa, 0xd, 0x1, 0x6, 0xf, 0x8,
    0x6, 0x1, 0x8, 0xf, 0x3, 0x4, 0xd, 0xa, 0xc, 0xb, 0x2, 0x5, 0x9, 0xe, 0x7, 0x0,
    0xb, 0xc, 0x5, 0x2, 0xe, 0x9, 0x0, 0x7, 0x1, 0x6, 0xf, 0x8, 0x4, 0x3, 0xa, 0xd,
    0x5, 0x2, 0xb, 0xc, 0x0, 0x7, 0xe, 0x9, 0xf, 0x8, 0x1, 0x6, 0xa, 0xd, 0x4, 0x3,
    0x8, 0xf, 0x6, 0x1, 0xd, 0xa, 0x3, 0x4, 0x2, 0x5, 0xc, 0xb, 0x7, 0x0, 0x9, 0xe,
    0xc, 0xb, 0x2, 0x5, 0x9, 0xe, 0x7, 0x0, 0x6, 0x1, 0x8, 0xf, 0x3, 0x4, 0xd, 0xa,
    0x1, 0x6, 0xf, 0x8, 0x4, 0x3, 0xa, 0xd, 0xb, 0xc, 0x5, 0x2, 0xe, 0x9, 0x0, 0x7,
    0xf, 0x8, 0x1, 0x6, 0xa, 0xd, 0x4, 0x3, 0x5, 0x2, 0xb, 0xc, 0x0, 0x7, 0xe, 0x9,
    0x2, 0x5, 0xc, 0xb, 0x7, 0x0, 0x9, 0xe, 0x8, 0xf, 0x6, 0x1, 0xd, 0xa, 0x3, 0x4,
    0xa, 0xd, 0x4, 0x3, 0xf, 0x8, 0x1, 0x6, 0x0, 0x7, 0xe, 0x9, 0x5, 0x2, 0xb, 0xc,
    0x7, 0x0, 0x9, 0xe, 0x2, 0x5, 0xc, 0xb, 0xd, 0xa, 0x3, 0x4, 0x8, 0xf, 0x6, 0x1,
    0x9, 0xe, 0x7, 0x0, 0xc, 0xb, 0x2, 0x5, 0x3, 0x4, 0xd, 0xa, 0x6, 0x1, 0x8, 0xf,
    0x4, 0x3, 0xa, 0xd, 0x1, 0x6, 0xf, 0x8, 0xe, 0x9, 0x0, 0x7, 0xb, 0xc, 0x5, 0x2
};

static unsigned char crc4itu(unsigned char crc, unsigned char *data, unsigned char len) {
    if (data == NULL)
        return 0;
    crc &= 0xf;
    while (len--)
        crc = table_byte[crc ^ *data++];
    return crc;
}

static void initdominantData(){
    //    unsigned short strRecData[2048];
    //    unsigned short curPos = 0;
    //    unsigned char lastRecData[128];
    //    unsigned short lastlength = 0;
    //    unsigned char lastDataCheck[20];
    if (!din){
        din  = (float complex*) malloc(sizeof(float complex) * 800);
    }
    if (!dout){
        dout = (float complex*) malloc(sizeof(float complex) * 800);
    }
    if (!strRecData){
        strRecData = (unsigned short*)malloc(2048);
    }
    if (!lastRecData){
        lastRecData = (unsigned char*)malloc(128);
    }
    if (!lastDataCheck){
        lastDataCheck = (unsigned char*)malloc(20);
    }
    if (!curData){
        curData = (unsigned char*)malloc(512);
    }
    
//    int n = 15;
//    int k = 9;
//    RS_init(n, k);
}
static void releasedominantData(){
    if(din) {
        free(din);
        din = NULL;
    }
    
    if(dout) {
        free(dout);
        dout = NULL;
    }
    if (strRecData) {
        free(strRecData);
        strRecData = NULL;
    }
    if (lastRecData) {
        free(lastRecData);
        lastRecData = NULL;
    }
    if (lastDataCheck) {
        free(lastDataCheck);
        lastDataCheck = NULL;
    }
    if (curData) {
        free(curData);
        curData = NULL;
    }
//    RS_release();
}

static void dominant (short* samples, int N /*sample_length*/)
{
    int i;
    fftplan p;
    if((din==NULL)||(dout==NULL))
    {
        printf("Error:insufficient available memory\n");
        return;
    }
    else
    {
        for(i=0; i<N; i++)
        {
            din[i] = (float)samples[i] + _Complex_I * 0;
        }
    }
    p = fft_create_plan(N, din, dout, LIQUID_FFT_FORWARD, 0);
    fft_execute(p); /* repeat as needed */
    fft_destroy_plan(p);
    
    float peak = 0;
    int peak_index = 0;
    for(i=0;i< N/2 - 1;i++)/*OUTPUT*/
    {
        float dist = sqrtf(crealf(dout[i]) * crealf(dout[i]) + cimagf(dout[i]) * cimagf(dout[i]));
        if(dist > peak) {
            peak = dist;
            peak_index = i;
        }
    }
    
    float peak11 = 0;
    int peak11_index = 0;
    for(i=0;i< N/2 - 1;i++)/*OUTPUT*/
    {
        int orgValue = (int)round((i * 1.0 / N) * sample_rate);
        if (orgValue >= (HANDSHAKE_START_HZ-50) && orgValue <= (HANDSHAKE_MAX_HZ+50)){
            float dist = sqrtf(crealf(dout[i]) * crealf(dout[i]) + cimagf(dout[i]) * cimagf(dout[i]));
            if(dist > peak11) {
                peak11 = dist;
                peak11_index = i;
            }
        }
    }
    //https://stackoverflow.com/questions/4364823/how-do-i-obtain-the-frequencies-of-each-value-in-an-fft/4371627#4371627
    //    if(peak > 10000)
    {
//                printf("***** peak index: %d, freq: %f\n", peak_index, (peak_index * 1.0 / N) * sample_rate);
        int orgValue = (int)round((peak_index * 1.0 / N) * sample_rate);
        if ((orgValue < (HANDSHAKE_START_HZ-100) || orgValue > (HANDSHAKE_MAX_HZ+100)) && peak11 > 10000 && recStart == 1){
            orgValue = (int)round((peak11_index * 1.0 / N) * sample_rate);
        }
//        printf("***** peak index: %d, orgValue: %d\n", peak_index, orgValue);
        {
            int value = orgValue;
            if(value % 100 <= 20){
                value = value/100*100;
            }else if(value % 100 >= 80){
                value = value/100*100 + 100;
            }else{
//                                printf("restartRec orgValue : %d\n",orgValue);
                value = 0;
            }
            if (value == HANDSHAKE_START_HZ){
                memset(strRecData, 0, 2048);
                curPos = 0;
                recStart = 1;
//                printf("value == HANDSHAKE_START_HZ\n");
                return;
            }
            if (recStart == 0){
//                if ((orgValue < (HANDSHAKE_START_HZ-100) || orgValue > (HANDSHAKE_MAX_HZ+100)) && peak11 > 10000){
//                    printf("recStart == 0: %i, strong: %f  orgValue:%d\n", value, peak,orgValue);
//                }
            }else {
                printf("freq0: %i, strong: %f  orgValue:%d\n", value, peak,orgValue);
                if (value == HANDSHAKE_END_HZ){
                    
                    memset(curData, 0, 512);
                    unsigned char curLength = curPos/Samplingfrequency;
                    if(curLength%15 != 1 && curLength%15 != 0 && curLength%15 != 14){
//                        printf("data error:%d\n",curLength);
                        curPos = 0;
                        value = 0;
                        recStart = 0;
                        return;
                    }
                    if (curLength%15 == 14){
                        curLength+=1;
                    }
//                    printf("curLength:%d\n",curLength);
                    for (i=0; i <curLength;i++){
                        if (Samplingfrequency == 2){
                            unsigned short sValue1 = strRecData[i*Samplingfrequency];
                            unsigned short sValue2 = strRecData[i*Samplingfrequency+1];
                            if (sValue1%200 == (i%2)*100){
                                if (sValue1 - START_HZ > 0){
                                    curData[i] = (sValue1 - START_HZ) / 200;
                                    continue;
                                }else {
                                    curData[i] = 0;
                                    continue;
                                }
                            }else if (sValue2%200 == (i%2)*100){
                                if (sValue2 - START_HZ > 0){
                                    curData[i] = (sValue2 - START_HZ) / 200;
                                    continue;
                                }else {
                                    curData[i] = 0;
                                    continue;
                                }
                            }else {
                                curData[i] = 0;
                            }
                        }else if (Samplingfrequency == 4){
                            int j;
                            unsigned short sValue1 = strRecData[i*4];
//                            printf("sValue1:%d i:%d\n",sValue1,i);
                            unsigned short sValue2 = 0;
                            for (j=1;j <4;j++){
//                                printf("strRecData[i*4+j]:%d curpos:%d\n",sValue1,i*4+j);
                                if (strRecData[i*4+j] >= START_HZ && strRecData[i*4+j] <= HANDSHAKE_MAX_HZ){
                                    if (strRecData[i*4+j] != sValue1){
                                        if (sValue2 == 0){
                                            if (strRecData[i*4+j]%200 == (i%2)*100){
                                                sValue2 = strRecData[i*4+j];
//                                                printf("sValue2:%d curpos:%d\n",sValue2,i*4+j);
                                                continue;
                                            }
                                        }
                                    }else {
                                        if (sValue1%200 == (i%2)*100){
                                            if (i%2){
                                                curData[i] = (sValue1 - START_HZ) / 200;
//                                                printf("curData[%d] :%c\n",i,curData[i]);
                                                break;
                                            }else {
                                                curData[i] = ((sValue1 - START_HZ) / 200);
//                                                printf("curData[%d] :%d sValue1:%d\n",i,curData[i],sValue1);
                                                break;
                                            }
                                        }
                                    }
                                    if (sValue2 > 0){
                                        if (strRecData[i*4+j] != sValue2){
                                            if (strRecData[i*4+j]%200 == (i%2)*100){
                                                sValue2 = strRecData[i*4+j];
                                                continue;
                                            }
                                        }else {
                                            if (sValue2%200 == (i%2)*100){
                                                if (i%2){
                                                    curData[i] = (sValue2 - START_HZ) / 200;
//                                                    printf("curData[%d] :%c\n",i,curData[i]);
                                                    break;
                                                }else {
                                                    curData[i] = ((sValue2 - START_HZ) / 200);
//                                                    printf("curData[%d] :%d sValue2:%d\n",i,curData[i],sValue2);
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    {
                        unsigned char curDecData2[128];
                        memset(curDecData2, 0, 128);
                        int curDecData2Length = 0;
                        int n = 15;
                        int k = 9;
                        int b = n - k;
//                        printf("RS_init 1\n");
                        RS_init(n, k);
                        int bIsFailed = 0;
                        int dataNums = curLength/15;
                        unsigned char curCheck[20];
                        memset(curCheck, 0, 20);
                        int _data[k], _bb[b];
                        for (i=0; i <dataNums;i++){
                            int j;
                            for(j=0;j<k;j++){
                                _data[j] = curData[i*15+j];
                            }
                            for(j=0;j<b;j++){
                                _bb[j] = curData[i*15+j+k];
                            }
//                            printf("RS_decode 1\n");
                            RS_decode(_data, k, _bb, b);
//                            printf("RS_decode 2\n");
                            unsigned char decdata[4];
                            for (j=0;j<4;j++){
                                decdata[j] = _data[j*2]*16+_data[j*2+1];
//                                printf("decdata[j]:%c\n",decdata[j]);
                            }
                            if (_data[8] == crc4itu(0, decdata, 4)){
                                memcpy(curDecData2+curDecData2Length, decdata, 4);
//                                printf("crc4itu 成功\n");
                                curCheck[i] = 1;
                            }else {
//                                printf("crc4itu 失败\n");
                                bIsFailed = 1;
                                curCheck[i] = 0;
                            }
                            curDecData2Length+=4;
                        }
//                        printf("RS_release 1\n");
                        RS_release();
//                        printf("RS_release 2\n");
                        if (bIsFailed == 0){
                            if (strlen(curDecData2)-1 == curDecData2[0]){
                                soundWaveSuccess(curDecData2+1,curDecData2[0]);
                            }
                        }else {
                            if (lastlength > 0){
                                int curLen = dataNums;
                                if (lastDataChecklength < dataNums){
                                    curLen = lastDataChecklength;
                                }
                                for (i=0;i <curLen;i++){
                                    if (curCheck[i] == 1){
                                        memcpy(lastRecData+i*4, curDecData2+i*4, 4);
                                        lastDataCheck[i] = 1;
                                    }
                                }
                                for (i=0;i <curLen;i++){
                                    if (lastDataCheck[i] == 1){
                                        memcpy(curDecData2+i*4, lastRecData, 4);
                                        curCheck[i] = 1;
                                    }else{
                                        if (curCheck[i] == 0){
                                            return;
                                        }
                                    }
                                }
//                                printf("curDecData2:%s\n",curDecData2);
                                if (strlen(curDecData2)-1 == curDecData2[0]){
//                                    printf("成功 curDecData2:%s\n",curDecData2+1);
                                    soundWaveSuccess(curDecData2+1,curDecData2[0]);
                                }
                            }else {
                                lastlength = curDecData2Length;
                                memcpy(lastRecData, curDecData2, curDecData2Length);
                                lastDataChecklength = dataNums;
                                memcpy(lastDataCheck, curCheck, dataNums);
                            }
                        }
                    }
                    
                    
                    curPos = 0;
                    value = 0;
                    recStart = 0;
                    return;
                }
                if (curPos > 1000){
//                    printf("curPos > 1000\n");
                    curPos = 0;
                    value = 0;
                    recStart = 0;
                }else {
                    strRecData[curPos] = value;
                    curPos++;
                }
            }
        }
    }
}

void soundWaveDataFeed(char* data,  unsigned int length){
    memset((char*)din, 0, sizeof(float complex) * 800);
    memset((char*)dout, 0, sizeof(float complex) * 800);
    dominant((short *)data, length/2);
}


int startSoundWave()
{
    if (sound_wave_started){
        return 0;
    }

#if EZHOMELABS_CAMERA    
	set_echo_bypass(1);
#endif
//    printf("sound record from input vch %d\n", ch);
    sound_wave_started = true;
    printf("startSoundWave 1\n");
    /*for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
        sound_grab_object[i] = gm_new_obj(GM_AUDIO_GRAB_OBJECT);
        sound_encode_object[i] = gm_new_obj(GM_AUDIO_ENCODER_OBJECT);
        sound_grab_attr.vch = ch;
        sample_rate = 16000;
        sound_grab_attr.sample_rate = sample_rate;
        float interval = 0.1;
        int num_frames = (int)round(interval * sample_rate / 2.0);
        
        sound_grab_attr.sample_size = 16;
        sound_grab_attr.channel_type = GM_MONO;
        gm_set_attr(sound_grab_object[i], &sound_grab_attr);
        sound_encode_attr.encode_type = GM_PCM;
        sound_encode_attr.bitrate = 32000;
        sound_encode_attr.frame_samples = num_frames;
        gm_set_attr(sound_encode_object[i], &sound_encode_attr);
        sound_bindfd[i] = gm_bind(sound_wave_started, sound_grab_object[i], sound_encode_object[i]);
    }*/

    initdominantData();

    return 0;
}

void stopSoundWave(){
    if (sound_wave_started){
        releasedominantData();
        sound_wave_started = false;
#if EZHOMELABS_CAMERA
	set_echo_bypass(0);
#endif
    }
}


