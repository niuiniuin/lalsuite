/**** <lalVerbatim file="TimeFreqFFTCV">
 * $Id$
 **** </lalVerbatim> */

/**** <lalLaTeX>
 * \subsection{Module \texttt{TimeFreqFFT.c}}
 * \label{ss:TimeFreqFFT.c}
 * 
 * Functions for time to frequency Fourier transforms.
 *
 * \subsubsection*{Prototypes}
 * \vspace{0.1in}
 * \input{TimeFreqFFTCP}
 * \idx{LALTimeFreqRealFFT()}
 * \idx{LALFreqTimeRealFFT()}
 * \idx{LALTimeFreqComplexFFT()}
 * \idx{LALFreqTimeComplexFFT()}
 * \idx{LALREAL4AverageSpectrum()}
 *
 * \subsubsection*{Description}
 * 
 * The routines \verb+LALTimeFreqRealFFT()+ and \verb+LALTimeFreqComplexFFT()+
 * transform time series $h_j$, $0\le j<n$, into a frequency series
 * $\tilde{h}_k$.  For \verb+LALTimeFreqRealFFT()+,
 * \[
 *    \tilde{h}_k = \Delta t \times H_k \;
 *    \mbox{for $0\le k\le\lfloor n/2\rfloor$.}
 * \]
 * The packing covers the range from dc (inclusive) to Nyquist (inclusive if
 * $n$ is even).
 * For \verb+LALTimeFreqComplexFFT()+,
 * \[
 *    \tilde{h}_k = \Delta t \left\{
 *    \begin{array}{ll}
 *      H_{k+\lfloor(n+1)/2\rfloor} &
 *        \mbox{for $0\le k<\lfloor n/2\rfloor$}, \\
 *      H_{k-\lfloor n/2\rfloor} &
 *        \mbox{for $\lfloor n/2\rfloor\le k<n$}. \\
 *    \end{array}
 *    \right.
 * \]
 * The packing covers the range from negative Nyquist (inclusive if $n$ is
 * even) up to (but not including) positive Nyquist.
 * Here $H_k$ is the DFT of $h_j$:
 * \[
 *   H_k = \sum_{j=0}^{n-1} h_j e^{-2\pi ijk/n}.
 * \]
 * The units of $\tilde{h}_k$ are equal to the units of $h_j$ times seconds.
 *
 * The routines \verb+LALFreqTimeRealFFT()+ and \verb+LALFreqTimeComplexFFT()+
 * perform the inverse transforms from $\tilde{h}_k$ back to $h_j$.  This is
 * done by shuffling the data, performing the reverse DFT, and multiplying by
 * $\Delta f$.
 * 
 * The routine \verb+LALREAL4AverageSpectrum()+ uses Welch's method to compute
 * the average power spectrum of the time series stored in the input structure
 * \verb+tSeries+ and return it in the output structure \verb+fSeries+.  A
 * Welch PSD estimate is defined by an FFT length, overlap length, choice of
 * window function and averaging method. These are specified in the
 * parameter structure; the FFT length is obtained from the length of the
 * \verb|REAL4Window| in the parameters.
 * 
 * On entry the parameter structure \verb+params+ must contain a valid
 * \verb+REAL4Window+ generated by \verb+LALCreateREAL4Window()+, an integer
 * that determines the overlap as described below and a forward FFT plan for
 * transforming data of the specified window length into the time domain. The
 * method used to compute the average must also be set.
 *
 * If the length of the window is $N$, then the FFT length is defined to be
 * $N/2-1$. The input data of length $M$ is divided into $i$ segments which
 * overlap by $o$, where
 * \begin{equation}
 * i = \frac{M-o}{N-o}.
 * \end{equation}
 * 
 * The PSD of each segment is obtained. The Welch PSD estimate is the average
 * of these $i$ sub-estimates.  The average is computed using the mean or
 * median method, as specified in the parameter structure.
 *
 * Note: the return PSD estimate is a one-sided power spectral density
 * normalized as defined in the conventions document. When the averaging
 * method is choosen to be mean and the window type Hann, the result is the
 * same as returned by the LDAS datacondAPI \texttt{psd()} action for a real
 * sequence without detrending.
 * 
 * \subsubsection*{Operating Instructions}
 *
 * \begin{verbatim}
 * const UINT4 n  = 65536;
 * const REAL4 dt = 1.0 / 16384.0;
 * static LALStatus status; compute average power spectrum
 * static REAL4TimeSeries         x;
 * static COMPLEX8FrequencySeries X;
 * static COMPLEX8TimeSeries      z;
 * static COMPLEX8FrequencySeries Z;
 * RealFFTPlan    *fwdRealPlan    = NULL;
 * RealFFTPlan    *revRealPlan    = NULL;
 * ComplexFFTPlan *fwdComplexPlan = NULL;
 * ComplexFFTPlan *revComplexPlan = NULL;
 *
 * LALSCreateVector( &status, &x.data, n );
 * LALCCreateVector( &status, &X.data, n / 2 + 1 );
 * LALCCreateVector( &status, &z.data, n );
 * LALCCreateVector( &status, &Z.data, n );
 * LALCreateForwardRealFFTPlan( &status, &fwdRealPlan, n, 0 );
 * LALCreateReverseRealFFTPlan( &status, &revRealPlan, n, 0 );
 * LALCreateForwardComplexFFTPlan( &status, &fwdComplexPlan, n, 0 );
 * LALCreateReverseComplexFFTPlan( &status, &revComplexPlan, n, 0 );
 *
 * x.f0 = 0;
 * x.deltaT = dt;
 * x.sampleUnits = lalMeterUnit;
 * strncpy( x.name, "x", sizeof( x.name ) );
 *
 * z.f0 = 0;
 * z.deltaT = dt;
 * z.sampleUnits = lalVoltUnit;
 * strncpy( z.name, "z", sizeof( z.name ) );
 *
 * <assign data>
 *
 * LALTimeFreqRealFFT( &status, &X, &x, fwdRealPlan );
 * LALFreqTimeRealFFT( &status, &x, &X, revRealPlan );
 * LALTimeFreqComplexFFT( &status, &Z, &z, fwdComplexPlan );
 * LALFreqTimeComplexFFT( &status, &z, &Z, revComplexPlan );
 *
 * LALDestroyRealFFTPlan( &status, &fwdRealPlan );
 * LALDestroyRealFFTPlan( &status, &revRealPlan );
 * LALDestroyComplexFFTPlan( &status, &fwdComplexPlan );
 * LALDestroyComplexFFTPlan( &status, &revComplexPlan );
 * LALCDestroyVector( &status, &Z.data );
 * LALCDestroyVector( &status, &z.data );
 * LALCDestroyVector( &status, &X.data );
 * LALSDestroyVector( &status, &x.data );
 * \end{verbatim}
 *
 * \subsubsection*{Notes}
 *
 * \begin{enumerate}
 * \item The routines do not presently work properly with heterodyned data,
 * i.e., the original time series data should have \verb+f0+ equal to zero.
 * \end{enumerate}
 *
 * \vfill{\footnotesize\input{TimeFreqFFTCV}}
 * 
 **** </lalLaTeX> */


#include <math.h>
#include <lal/LALStdlib.h>
#include <lal/Units.h>
#include <lal/AVFactories.h>
#include "TimeFreqFFT.h"
#include <lal/LALConstants.h>

NRCSID( TIMEFREQFFTC, "$Id$" );


/******** <lalVerbatim file="CreateRealDFTParamsCP"> ********/
void
LALCreateRealDFTParams ( 
                     LALStatus                         *status, 
                     RealDFTParams                  **dftParams, 
                     LALWindowParams                   *params,
                     INT2                           sign
		     )
/******** </lalVerbatim> ********/
{
  INITSTATUS (status, "LALCreateRealDFTParams", TIMEFREQFFTC);
  ATTATCHSTATUSPTR (status);

  /* 
   * Check return structure: dftParams should point to a valid pointer
   * which should not yet point to anything.
   *
   */
  ASSERT (dftParams, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL); 
  ASSERT (*dftParams == NULL, status, TIMEFREQFFTH_EALLOC, 
          TIMEFREQFFTH_MSGEALLOC);


  ASSERT (params, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL);  

  ASSERT (params->length > 0, status, TIMEFREQFFTH_EPOSARG, 
          TIMEFREQFFTH_MSGEPOSARG);

  ASSERT( (sign==1) || (sign==-1), status, TIMEFREQFFTH_EINCOMP,
          TIMEFREQFFTH_MSGEINCOMP);

  /*  Assign memory for *dftParams and check allocation */
  if ( !( *dftParams = (RealDFTParams *) LALMalloc(sizeof(RealDFTParams)) ) ){
    ABORT (status, TIMEFREQFFTH_EMALLOC, TIMEFREQFFTH_MSGEMALLOC);
  }
  
  /* fill in some values */
  (*dftParams)->window = NULL;
  (*dftParams)->plan = NULL;

  if(sign==1)
    {
      /* Estimate the FFT plan */
      LALCreateForwardRealFFTPlan (status->statusPtr, &((*dftParams)->plan), 
                              params->length, 0);
    }
  else
    {
      /* Estimate the FFT plan */
      LALCreateReverseRealFFTPlan (status->statusPtr, &((*dftParams)->plan), 
                              params->length, 0);
    }
  CHECKSTATUSPTR (status);

  LALSCreateVector (status->statusPtr, &((*dftParams)->window), params->length);
  CHECKSTATUSPTR (status);

  LALWindow (status->statusPtr, ((*dftParams)->window), params);
  CHECKSTATUSPTR (status);

  (*dftParams)->sumofsquares = params->sumofsquares;
  (*dftParams)->windowType = params->type;
  
  /* Normal exit */
  DETATCHSTATUSPTR (status);
  RETURN (status);
}

/******** <lalVerbatim file="DestroyRealDFTParamsCP"> ********/
void
LALDestroyRealDFTParams (
		      LALStatus                 *status, 
		      RealDFTParams          **dftParams
		      )
/******** </lalVerbatim> ********/
{
  INITSTATUS (status, "LALDestroyRealDFTParams", TIMEFREQFFTC);
  ATTATCHSTATUSPTR (status);

  /* make sure that arguments are not null */
  ASSERT (dftParams, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL);
  ASSERT (*dftParams, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL);

  /* make sure that data pointed to is non-null */
  ASSERT ((*dftParams)->plan, status, TIMEFREQFFTH_ENULL, 
          TIMEFREQFFTH_MSGENULL); 
  ASSERT ((*dftParams)->window, status, TIMEFREQFFTH_ENULL, 
          TIMEFREQFFTH_MSGENULL); 

  /* Ok, now let's free allocated storage */
  LALSDestroyVector (status->statusPtr, &((*dftParams)->window));
  CHECKSTATUSPTR (status);
  LALDestroyRealFFTPlan (status->statusPtr, &((*dftParams)->plan));
  CHECKSTATUSPTR (status);
  LALFree ( *dftParams );      /* free DFT parameters structure itself */

  *dftParams = NULL;	       /* make sure we don't point to freed struct */

  /* Normal exit */
  DETATCHSTATUSPTR (status);
  RETURN (status);
}



/* <lalVerbatim file="TimeFreqFFTCP"> */
void
LALTimeFreqRealFFT(
    LALStatus               *status,
    COMPLEX8FrequencySeries *freq,
    REAL4TimeSeries         *time,
    RealFFTPlan             *plan
    )
{ /* </lalVerbatim> */
  LALUnitPair unitPair;
  UINT4 k;

  INITSTATUS( status, "LALTimeFreqRealFFT", TIMEFREQFFTC );
  ATTATCHSTATUSPTR( status );

  ASSERT( plan, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( freq, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time->data, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time->data->length, status,
      TIMEFREQFFTH_ESIZE, TIMEFREQFFTH_MSGESIZE );
  ASSERT( time->deltaT > 0, status, TIMEFREQFFTH_ERATE, TIMEFREQFFTH_MSGERATE );

  unitPair.unitOne = &(time->sampleUnits);
  unitPair.unitTwo = &(lalSecondUnit);

  /*
   *
   * The field f0 is not consistently defined between time and freq series.
   * Just do something....  Assume data is not heterodyned.
   *
   */
  if ( time->f0 != 0 )
  {
    LALWarning( status, "Frequency series may have incorrect f0." );
  }
  freq->f0     = 0; /* correct value for unheterodyned data */
  freq->epoch  = time->epoch;
  freq->deltaF = 1.0 / ( time->deltaT * time->data->length );
  TRY( LALUnitMultiply( status->statusPtr, &freq->sampleUnits, &unitPair ),
      status );
  TRY( LALForwardRealFFT( status->statusPtr, freq->data, time->data, plan ),
      status );
  for ( k = 0; k < freq->data->length; ++k )
  {
    freq->data->data[k].re *= time->deltaT;
    freq->data->data[k].im *= time->deltaT;
  }

  DETATCHSTATUSPTR( status );
  RETURN( status );
}


/* <lalVerbatim file="TimeFreqFFTCP"> */
void
LALFreqTimeRealFFT(
    LALStatus               *status,
    REAL4TimeSeries         *time,
    COMPLEX8FrequencySeries *freq,
    RealFFTPlan             *plan
    )
{ /* </lalVerbatim> */
  LALUnitPair unitPair;
  UINT4 j;

  INITSTATUS( status, "LALFreqTimeRealFFT", TIMEFREQFFTC );
  ATTATCHSTATUSPTR( status );

  ASSERT( plan, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( freq, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time->data, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time->data->length, status,
      TIMEFREQFFTH_ESIZE, TIMEFREQFFTH_MSGESIZE );
  ASSERT( freq->deltaF > 0, status, TIMEFREQFFTH_ERATE, TIMEFREQFFTH_MSGERATE );

  unitPair.unitOne = &(freq->sampleUnits);
  unitPair.unitTwo = &(lalHertzUnit);

  /*
   *
   * The field f0 is not consistently defined between time and freq series.
   * Just do something....  Assume data is not heterodyned.
   *
   */
  if ( freq->f0 != 0 )
  {
    LALWarning( status, "Time series may have incorrect f0." );
  }
  time->f0     = 0; /* correct value for unheterodyned data */
  time->epoch  = freq->epoch;
  time->deltaT = 1.0 / ( freq->deltaF * time->data->length );
  TRY( LALUnitMultiply( status->statusPtr, &time->sampleUnits, &unitPair ),
      status );
  TRY( LALReverseRealFFT( status->statusPtr, time->data, freq->data, plan ),
      status );
  for ( j = 0; j < time->data->length; ++j )
  {
    time->data->data[j] *= freq->deltaF;
  }

  DETATCHSTATUSPTR( status );
  RETURN( status );
}


/*
 *
 * median spectrum estimator based on mark's version
 *
 */


static REAL4 
MedianSpec(
    REAL4      *p, 
    REAL4      *s,
    UINT4       j, 
    UINT4       flength, 
    UINT4       numSegs
    )
{
  /* p points to array of power spectra data over time slices */
  /* s is a scratch array used to sort the values             */
  /* j is desired frequency offset into power spectra array   */
  /* flength is size of frequency series obtained from DFT    */
  /* numSegs is the number of time slices to be evaluated     */
  /* returns the median value, over time slice at given freq. */

  UINT4  outer  = 0;       /* local loop counter */
  UINT4  middle = 0;       /* local loop counter */
  UINT4  inner  = 0;       /* local loop counter */
  REAL4 returnVal = 0.0;  /* holder for return value */

  /* zero out the sort array */
  memset( s, 0, numSegs * sizeof(REAL4) );

  /* scan time slices for a given frequency */
  for ( outer = 0; outer < numSegs; ++outer )
  {
    /* insert power value into sort array */
    REAL4 tmp = p[outer * flength + j]; /* obtain value to insert */
    for ( middle = 0; middle < numSegs; ++middle )
    {
      if ( tmp > s[middle] )
      {
        /* insert taking place of s[middle] */
        for ( inner = numSegs - 1; inner > middle; --inner )
        {
          s[inner] = s [inner - 1];  /* move old values */
        }
        s[middle] = tmp;   /* insert new value */
        break;  /* terminate loop */
      }
    }
  }  /* done inserting into sort array */

  /* check for odd or even number of segments */
  if ( numSegs % 2 )
  {
    /* if odd number of segments, return median */
    returnVal = s[numSegs / 2];
  }
  else
  {
    /* if even number of segments, return average of two medians */
    returnVal = 0.5 * (s[numSegs/2] + s[(numSegs/2) - 1]);
  }

  return returnVal;
}


/*
 *
 * compute average power spectrum
 *
 */


/* <lalVerbatim file="TimeFreqFFTCP"> */
void
LALREAL4AverageSpectrum (
    LALStatus                   *status,
    REAL4FrequencySeries        *fSeries,
    REAL4TimeSeries             *tSeries,
    AverageSpectrumParams       *params
    )
/* </lalVerbatim> */
{
  UINT4                 i, j, k;          /* seg, ts and freq counters       */
  UINT4                 numSeg;           /* number of segments in average   */
  UINT4                 fLength;          /* length of requested power spec  */
  UINT4                 tLength;          /* length of time series segments  */
  REAL4Vector          *tSegment = NULL;  /* dummy time series segment       */
  COMPLEX8Vector       *fSegment = NULL;  /* dummy freq series segment       */
  REAL4                *tSeriesPtr;       /* pointer to the segment data     */
  REAL4                 psdNorm = 0;      /* factor to multiply windows data */
  REAL4                 fftRe, fftIm;     /* real and imag parts of fft      */
  REAL4                *s;                /* work space for computing mean   */
  REAL4                *psdSeg;           /* storage for individual specta   */
  LALUnit               unit;
  LALUnitPair           pair;
  RAT4                  negRootTwo = { -1, 1 };

  INITSTATUS (status, "LALREAL4AverageSpectrum", TIMEFREQFFTC);
  ATTATCHSTATUSPTR (status);

  /* check the input and output data pointers are non-null */
  ASSERT( fSeries, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( fSeries->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( fSeries->data->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( tSeries, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( tSeries->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( tSeries->data->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );

  /* check the contents of the parameter structure */
  ASSERT( params, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( params->window, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( params->window->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( params->window->data->length > 0, status,
      TIMEFREQFFTH_EZSEG, TIMEFREQFFTH_MSGEZSEG );
  ASSERT( params->plan, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  if ( !  ( params->method == useUnity || params->method == useMean || 
        params->method == useMedian ) )
  {
    ABORT( status, TIMEFREQFFTH_EUAVG, TIMEFREQFFTH_MSGEUAVG );
  }

  /* check that the window length and fft storage lengths agree */
  fLength = fSeries->data->length;
  tLength = params->window->data->length;
  if ( fLength != tLength / 2 + 1 )
  {
    ABORT( status, TIMEFREQFFTH_EMISM, TIMEFREQFFTH_MSGEMISM );
  }

  /* compute the number of segs, check that the length and overlap are valid */
  numSeg = (tSeries->data->length - params->overlap) / (tLength - params->overlap);
  if ( (tSeries->data->length - params->overlap) % (tLength - params->overlap) )
  {
    ABORT( status, TIMEFREQFFTH_EMISM, TIMEFREQFFTH_MSGEMISM );
  }

  /* clear the output spectrum and the workspace frequency series */
  memset( fSeries->data->data, 0, fLength * sizeof(REAL4) );

  /* compute the parameters of the output frequency series data */
  fSeries->epoch = tSeries->epoch;
  fSeries->f0 = tSeries->f0;
  fSeries->deltaF = 1.0 / ( (REAL8) tLength * tSeries->deltaT );
  /* THIS IS WRONG: GIVES SQUARE-ROOT OF CORRECT UNITS */
  /*
  pair.unitOne = &(tSeries->sampleUnits);
  pair.unitTwo = &lalHertzUnit;
  LALUnitRaise( status->statusPtr, &unit, pair.unitTwo, &negRootTwo );
  CHECKSTATUSPTR( status );
  pair.unitTwo = &unit;
  */
  pair.unitOne = &(tSeries->sampleUnits);
  pair.unitTwo = &(tSeries->sampleUnits);
  LALUnitMultiply( status->statusPtr, &unit, &pair );
  CHECKSTATUSPTR( status );
  pair.unitOne = &unit;
  pair.unitTwo = &lalSecondUnit;
  LALUnitMultiply( status->statusPtr, &(fSeries->sampleUnits), &pair );
  CHECKSTATUSPTR( status );

  /* if this is a unit spectrum, just set the conents to unity and return */
  if ( params->method == useUnity )
  {
    for ( k = 0; k < fLength; ++k )
    {
      fSeries->data->data[k] = 1.0;
    }
    DETATCHSTATUSPTR( status );
    RETURN( status );
  }

  /* create temporary storage for the dummy time domain segment */
  LALCreateVector( status->statusPtr, &tSegment, tLength );
  CHECKSTATUSPTR( status );

  /* create temporary storage for the individual ffts */
  LALCCreateVector( status->statusPtr, &fSegment, fLength );
  CHECKSTATUSPTR( status );

  if ( params->method == useMedian )
  {
    /* create enough storage for the indivdiual power spectra */
    psdSeg = (REAL4 *) LALCalloc( numSeg, fLength * sizeof(REAL4) );
  }

  /* compute each of the power spectra used in the average */
  for ( i = 0, tSeriesPtr = tSeries->data->data; i < (UINT4) numSeg; ++i )
  {
    /* copy the time series data to the dummy segment */
    memcpy( tSegment->data, tSeriesPtr, tLength * sizeof(REAL4) );

    /* window the time series segment */
    for ( j = 0; j < tLength; ++j )
    {
      tSegment->data[j] *= params->window->data->data[j];
    }

    /* compute the fft of the data segment */
    LALForwardRealFFT( status->statusPtr, fSegment, tSegment, params->plan );
    CHECKSTATUSPTR (status);

    /* advance the segment data pointer to the start of the next segment */
    tSeriesPtr += tLength - params->overlap;

    /* compute the psd components */
    if ( params->method == useMean )
    {
      /* we can get away with less storage */
      for ( k = 0; k < fLength; ++k )
      {
        fftRe = fSegment->data[k].re;
        fftIm = fSegment->data[k].im;
        fSeries->data->data[k] += fftRe * fftRe + fftIm * fftIm;
      }

      /* halve the DC and Nyquist components to be consistent with T010095 */
      fSeries->data->data[0] /= 2;
      fSeries->data->data[fLength - 1] /= 2;
    }
    else if ( params->method == useMedian )
    {
      /* we must store all the spectra */
      for ( k = 0; k < fLength; ++k )
      {
        fftRe = fSegment->data[k].re;
        fftIm = fSegment->data[k].im;
        psdSeg[i * fLength + k] = fftRe * fftRe + fftIm * fftIm;
      }

      /* halve the DC and Nyquist components to be consistent with T010095 */
      psdSeg[i * fLength] /= 2;
      psdSeg[i * fLength + fLength - 1] /= 2;
    }
  }

  /* destroy the dummy time series segment and the fft scratch space */
  LALDestroyVector( status->statusPtr, &tSegment );
  CHECKSTATUSPTR( status );
  LALCDestroyVector( status->statusPtr, &fSegment );
  CHECKSTATUSPTR( status );

  /* compute the desired average of the spectra */
  if ( params->method == useMean )
  {
    /* normalization constant for the arithmentic mean */
    psdNorm = ( 2.0 * tSeries->deltaT ) / 
      ( (REAL4) numSeg * params->window->sumofsquares );

    /* normalize the psd to it matches the conventions document */
    for ( k = 0; k < fLength; ++k )
    {
      fSeries->data->data[k] *= psdNorm;
    }
  }
  else if ( params->method == useMedian )
  {
    /* normalization constant for the median */
    psdNorm = ( 2.0 * tSeries->deltaT ) / 
      ( LAL_LN2 * params->window->sumofsquares );

    /* allocate memory array for insert sort */
    s = LALMalloc( numSeg * sizeof(REAL4) );
    if ( ! s )
    {
      ABORT( status, TIMEFREQFFTH_EMALLOC, TIMEFREQFFTH_MSGEMALLOC );
    }

    /* compute the median spectra and normalize to the conventions doc */
    for ( k = 0; k < fLength; ++k )
    {
      fSeries->data->data[k] = psdNorm * 
        MedianSpec( psdSeg, s, k, fLength, numSeg );
    }

    /* free memory used for sort array */
    LALFree( s );

    /* destroy the storage for the individual spectra */
    LALFree( psdSeg );
  }

  DETATCHSTATUSPTR( status );
  RETURN( status );
}


void
LALCOMPLEX8AverageSpectrum (
    LALStatus                   *status,
    COMPLEX8FrequencySeries     *fSeries,
    REAL4TimeSeries             *tSeries0,
    REAL4TimeSeries             *tSeries1,
    AverageSpectrumParams       *params
    )
/* </lalVerbatim> */
{
  UINT4                 i, j, k, l;          /* seg, ts and freq counters       */
  UINT4                 numSeg;           /* number of segments in average   */
  UINT4                 fLength;          /* length of requested power spec  */
  UINT4                 tLength;          /* length of time series segments  */
  REAL4Vector          *tSegment[2] = {NULL,NULL};
  COMPLEX8Vector       *fSegment[2] = {NULL,NULL};
  REAL4                *tSeriesPtr0,*tSeriesPtr1 ;
  REAL4                 psdNorm = 0;      /* factor to multiply windows data */
  REAL4                 fftRe0, fftIm0, fftRe1, fftIm1;
  LALUnit               unit;
  LALUnitPair           pair;
  RAT4                  negRootTwo = { -1, 1 };

  INITSTATUS (status, "LALCOMPLEX8AverageSpectrum", TIMEFREQFFTC);
  ATTATCHSTATUSPTR (status);

  /* check the input and output data pointers are non-null */
  ASSERT( fSeries, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( fSeries->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( fSeries->data->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( tSeries0, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( tSeries0->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( tSeries0->data->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
   ASSERT( tSeries1, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( tSeries1->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( tSeries1->data->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );

  /* check the contents of the parameter structure */
  ASSERT( params, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( params->window, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( params->window->data, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( params->window->data->length > 0, status,
      TIMEFREQFFTH_EZSEG, TIMEFREQFFTH_MSGEZSEG );
  ASSERT( params->plan, status,
      TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  if ( !  ( params->method == useUnity || params->method == useMean || 
        params->method == useMedian ) )
  {
    ABORT( status, TIMEFREQFFTH_EUAVG, TIMEFREQFFTH_MSGEUAVG );
  }

  /* check that the window length and fft storage lengths agree */
  fLength = fSeries->data->length;
  tLength = params->window->data->length;
  if ( fLength != tLength / 2 + 1 )
  {
    ABORT( status, TIMEFREQFFTH_EMISM, TIMEFREQFFTH_MSGEMISM );
  }

  /* compute the number of segs, check that the length and overlap are valid */
  numSeg = (tSeries0->data->length - params->overlap) / (tLength - params->overlap);
  if ( (tSeries0->data->length - params->overlap) % (tLength - params->overlap) )
  {
    ABORT( status, TIMEFREQFFTH_EMISM, TIMEFREQFFTH_MSGEMISM );
  }

  /* clear the output spectrum and the workspace frequency series */
  memset( fSeries->data->data, 0, fLength * sizeof(COMPLEX8) );

  /* compute the parameters of the output frequency series data */
  fSeries->epoch = tSeries0->epoch;
  fSeries->f0 = tSeries0->f0;
  fSeries->deltaF = 1.0 / ( (REAL8) tLength * tSeries0->deltaT );
  /* THIS IS WRONG: GIVES SQUARE-ROOT OF CORRECT UNITS */
  /*
  pair.unitOne = &(tSeries->sampleUnits);
  pair.unitTwo = &lalHertzUnit;
  LALUnitRaise( status->statusPtr, &unit, pair.unitTwo, &negRootTwo );
  CHECKSTATUSPTR( status );
  pair.unitTwo = &unit;
  */
  pair.unitOne = &(tSeries0->sampleUnits);
  pair.unitTwo = &(tSeries0->sampleUnits);
  LALUnitMultiply( status->statusPtr, &unit, &pair );
  CHECKSTATUSPTR( status );
  pair.unitOne = &unit;
  pair.unitTwo = &lalSecondUnit;
  LALUnitMultiply( status->statusPtr, &(fSeries->sampleUnits), &pair );
  CHECKSTATUSPTR( status );

  /* create temporary storage for the dummy time domain segment */
  for (l = 0; l < 2; l ++){
  LALCreateVector( status->statusPtr, &tSegment[l], tLength );
  CHECKSTATUSPTR( status );

  /* create temporary storage for the individual ffts */
  LALCCreateVector( status->statusPtr, &fSegment[l], fLength );
  CHECKSTATUSPTR( status );}


  /* compute each of the power spectra used in the average */
  for ( i = 0, tSeriesPtr0 = tSeries0->data->data, tSeriesPtr1 = tSeries1->data->data; i < (UINT4) numSeg; ++i )
  {
    /* copy the time series data to the dummy segment */
    memcpy( tSegment[0]->data, tSeriesPtr0, tLength * sizeof(REAL4) );
    memcpy( tSegment[1]->data, tSeriesPtr1, tLength * sizeof(REAL4) );
    /* window the time series segment */
    for ( j = 0; j < tLength; ++j )
    {
      tSegment[0]->data[j] *= params->window->data->data[j];
      tSegment[1]->data[j] *= params->window->data->data[j];
    }

    /* compute the fft of the data segment */
    LALForwardRealFFT( status->statusPtr, fSegment[0], tSegment[0], params->plan );
    CHECKSTATUSPTR (status);
    LALForwardRealFFT( status->statusPtr, fSegment[1], tSegment[1], params->plan );
    CHECKSTATUSPTR (status);

    /* advance the segment data pointer to the start of the next segment */
    tSeriesPtr0 += tLength - params->overlap;
    tSeriesPtr1 += tLength - params->overlap;

    /* compute the psd components */
    /*use mean method here*/
      /* we can get away with less storage */
      for ( k = 0; k < fLength; ++k )
      {
        fftRe0 = fSegment[0]->data[k].re;
        fftIm0 = fSegment[0]->data[k].im;
        fftRe1 = fSegment[1]->data[k].re;
        fftIm1 = fSegment[1]->data[k].im;
        fSeries->data->data[k].re += fftRe0 * fftRe1 + fftIm0 * fftIm1;
        fSeries->data->data[k].im += - fftIm0 * fftRe1 + fftRe0 * fftIm1;
       
      }

      /* halve the DC and Nyquist components to be consistent with T010095 */
      fSeries->data->data[0].re /= 2;
      fSeries->data->data[fLength - 1].re /= 2;
      fSeries->data->data[0].im /= 2;
      fSeries->data->data[fLength - 1].im /= 2;
    
      }

  /* destroy the dummy time series segment and the fft scratch space */
  for (l = 0; l < 2; l ++){
  LALDestroyVector( status->statusPtr, &tSegment[l] );
  CHECKSTATUSPTR( status );
  LALCDestroyVector( status->statusPtr, &fSegment[l] );
  CHECKSTATUSPTR( status );}

  /* compute the desired average of the spectra */
  
    /* normalization constant for the arithmentic mean */
    psdNorm = ( 2.0 * tSeries1->deltaT ) / 
      ( (REAL4) numSeg * params->window->sumofsquares );

    /* normalize the psd to it matches the conventions document */
    for ( k = 0; k < fLength; ++k )
    {
      fSeries->data->data[k].re *= psdNorm;
      fSeries->data->data[k].im *= psdNorm;
    }
 

  DETATCHSTATUSPTR( status );
  RETURN( status );
}



/*
 *
 * We need to define the ComplexFFTPlan structure so that we can check the FFT
 * sign.  The plan is not really a void*, but it is a pointer so a void* is
 * good enough (we don't need it).
 *
 */
struct tagComplexFFTPlan
{
  INT4  sign;
  UINT4 size;
  void *junk;
};


/* <lalVerbatim file="TimeFreqFFTCP"> */
void
LALTimeFreqComplexFFT(
    LALStatus               *status,
    COMPLEX8FrequencySeries *freq,
    COMPLEX8TimeSeries      *time,
    ComplexFFTPlan          *plan
    )
{ /* </lalVerbatim> */
  COMPLEX8Vector *tmp = NULL;
  LALUnitPair unitPair;
  UINT4 n;
  UINT4 k;

  INITSTATUS( status, "LALTimeFreqComplexFFT", TIMEFREQFFTC );
  ATTATCHSTATUSPTR( status );

  ASSERT( plan, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( freq, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time->data, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  n = time->data->length;
  ASSERT( n, status, TIMEFREQFFTH_ESIZE, TIMEFREQFFTH_MSGESIZE );
  ASSERT( time->deltaT > 0, status, TIMEFREQFFTH_ERATE, TIMEFREQFFTH_MSGERATE );
  ASSERT( plan->sign == -1, status, TIMEFREQFFTH_ESIGN, TIMEFREQFFTH_MSGESIGN );

  unitPair.unitOne = &(time->sampleUnits);
  unitPair.unitTwo = &(lalSecondUnit);
  TRY( LALUnitMultiply( status->statusPtr, &freq->sampleUnits, &unitPair ),
      status );

  freq->epoch  = time->epoch;
  freq->deltaF = 1.0 / ( time->deltaT * n );
  freq->f0     = time->f0 - freq->deltaF * floor( n / 2 );

  TRY( LALUnitMultiply( status->statusPtr, &freq->sampleUnits, &unitPair ),
      status );

  TRY( LALCCreateVector( status->statusPtr, &tmp, n ), status );

  LALCOMPLEX8VectorFFT( status->statusPtr, tmp, time->data, plan );
  BEGINFAIL( status )
  {
    TRY( LALCDestroyVector( status->statusPtr, &tmp ), status );
  }
  ENDFAIL( status );

  /*
   *
   * Unpack the frequency series and multiply by deltaT.
   *
   */
  for ( k = 0; k < n / 2; ++k )
  {
    UINT4 kk = k + ( n + 1 ) / 2;
    freq->data->data[k].re = time->deltaT * tmp->data[kk].re;
    freq->data->data[k].im = time->deltaT * tmp->data[kk].im;
  }
  for ( k = n / 2; k < n; ++k )
  {
    UINT4 kk = k - n / 2;
    freq->data->data[k].re = time->deltaT * tmp->data[kk].re;
    freq->data->data[k].im = time->deltaT * tmp->data[kk].im;
  }

  TRY( LALCDestroyVector( status->statusPtr, &tmp ), status );
  DETATCHSTATUSPTR( status );
  RETURN( status );
}


/* <lalVerbatim file="TimeFreqFFTCP"> */
void
LALFreqTimeComplexFFT(
    LALStatus               *status,
    COMPLEX8TimeSeries      *time,
    COMPLEX8FrequencySeries *freq,
    ComplexFFTPlan          *plan
    )
{ /* </lalVerbatim> */
  COMPLEX8Vector *tmp = NULL;
  LALUnitPair unitPair;
  UINT4 n;
  UINT4 k;

  INITSTATUS( status, "LALFreqTimeComplexFFT", TIMEFREQFFTC );
  ATTATCHSTATUSPTR( status );

  ASSERT( plan, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( time, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( freq, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  ASSERT( freq->data, status, TIMEFREQFFTH_ENULL, TIMEFREQFFTH_MSGENULL );
  n = freq->data->length;
  ASSERT( n, status, TIMEFREQFFTH_ESIZE, TIMEFREQFFTH_MSGESIZE );
  ASSERT( freq->deltaF > 0, status, TIMEFREQFFTH_ERATE, TIMEFREQFFTH_MSGERATE );
  ASSERT( plan->sign == 1, status, TIMEFREQFFTH_ESIGN, TIMEFREQFFTH_MSGESIGN );

  unitPair.unitOne = &(freq->sampleUnits);
  unitPair.unitTwo = &(lalHertzUnit);
  TRY( LALUnitMultiply( status->statusPtr, &time->sampleUnits, &unitPair ),
      status );

  time->f0     = freq->f0 + freq->deltaF * floor( n / 2 );
  time->epoch  = freq->epoch;
  time->deltaT = 1.0 / ( freq->deltaF * n );

  TRY( LALUnitMultiply( status->statusPtr, &freq->sampleUnits, &unitPair ),
      status );

  TRY( LALCCreateVector( status->statusPtr, &tmp, n ), status );

  /*
   *
   * Pack the frequency series and multiply by deltaF.
   *
   */
  for ( k = 0; k < n / 2; ++k )
  {
    UINT4 kk = k + ( n + 1 ) / 2;
    tmp->data[kk].re = freq->deltaF * freq->data->data[k].re;
    tmp->data[kk].im = freq->deltaF * freq->data->data[k].im;
  }
  for ( k = n / 2; k < n; ++k )
  {
    UINT4 kk = k - n / 2;
    tmp->data[kk].re = freq->deltaF * freq->data->data[k].re;
    tmp->data[kk].im = freq->deltaF * freq->data->data[k].im;
  }

  LALCOMPLEX8VectorFFT( status->statusPtr, time->data, tmp, plan );
  BEGINFAIL( status )
  {
    TRY( LALCDestroyVector( status->statusPtr, &tmp ), status );
  }
  ENDFAIL( status );

  TRY( LALCDestroyVector( status->statusPtr, &tmp ), status );
  DETATCHSTATUSPTR( status );
  RETURN( status );
}





