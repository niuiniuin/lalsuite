/* <lalVerbatim file="LALDemodTestCV">
Author: Berukoff, S.J., Papa, M.A., $Id$
 </lalVerbatim> */

#if 0
 <lalLaTeX>
   
   \subsection{Program \texttt{LALDemodTest.c}}
   \label{ss:LALDemodTest.c}
   
   Performs required tests of \verb@LALDemod()@.
   
   \subsubsection*{Usage}
   \begin{verbatim}
   LALDemodTest -i <input data file> [-d <gap>] [-n] [-o]
   \end{verbatim}
   
   \subsubsection*{Description}
   
   \noindent This routine performs tests on the routine \verb@LALDemod()@.  
   Options: 
   \begin{itemize}
   \item \verb@-i@ -- the input data file (default is 'in.data'; an example is included, format below)
   \item \verb@-n@ -- add zero-mean Gaussian noise to the signal
   \item \verb@-d <gap>@ -- simulate gaps in the data.  The number \verb@<gaps>@ refers to the integral number of SFT timescales between adjacent timestamps.
   \item \verb@-o@ -- print out result data files
   \end{itemize}
   
   Structure:
   In more detail, let us begin with a discussion of the structure of the test
   code, which is composed of several modules.  
   \begin{itemize}
   \item The first module reads in data from an input parameter data file.  
   The parameters must be listed in the input file in the following order, 
   with the corresponding format:
   \begin{verbatim}
   total observation time -- float
   coherent search time -- float
   factor by which to modify SFT timescale -- float
   Cross amplitude -- float
   Plus amplitude -- float
   DeFT frequency band (centered by default around f0) -- float
   f0, intrinsic frequency of the signal at the beginning of the 
   observation -- float
   maximum order of signal spindown parameters -- int
   signal spindown parameter 1 --  scientific notation
   signal spindown parameter 2 --  scientific notation
   signal spindown parameter 3 --  scientific notation
   signal spindown parameter 4 --  scientific notation
   signal spindown parameter 5 --  scientific notation
   signal source right ascension (alpha) -- float (value in DEGREES)
   signal source declination (delta) -- float (value in DEGREES)
   maximum order of template spindown parameters -- int
   template spindown parameter 1 --  scientific notation (NOT 
   template spindown parameter 2 --  scientific notation (NOT scaled by f0)
   template spindown parameter 3 --  scientific notation (NOT scaled by f0)
   template spindown parameter 4 --  scientific notation (NOT scaled by f0)
   template spindown parameter 5 --  scientific notation (NOT scaled by f0)
   template source right ascension (alpha) -- float (value in DEGREES)
   template source declination (delta) -- float (value in DEGREES)
   \end{verbatim}
   Note: Above, the *signal* spindown parameters are scaled by the intrinsic frequency, while the *template* spindown parameters are not.  This is due to the difference in definitions between the SimulateCoherentGW() package, which generates the signal, and this package.
   
   \item The next module in the test code, which is optionally executed with 
   the '\verb@-n@' switch, creates noise using LALs
   \verb@LALNormalDeviates()@ routine.  By design, the noise is created in 
   single
   precision, and is zero-mean and Gaussian.  This noise is added, 
   datum-by-datum, to the time series created in
   the next module, after the amplitude of the time series has been 
   changed by a
   factor of \verb@SNR@, which is specified in the input data file.
   
   \item The next module to be invoked creates a time series, according to 
   the standard model for pulsars with spindown.  This is done by using the \verb@LALGenerateTaylorCW()@ and \verb@LALSimulateCoherentGW()@ functions.  This time series undergoes 
   an FFT, and this transformed data then constitutes the SFT data to be 
   input to the demodulation code.  The fake signal data is characterized 
   by an intrinsic frequency at the beginning of the observation plus some 
   other source parameters.  The DeFT is produced in a band \verb@f0Band@ 
   (as specified as an input parameter) and centered at this frequency.   
   The width of the band (plus some extra width of $2\cdot 10^{-4}f0 $ Hz) determines the
   sampling frequency of the time series (Nyquist theorem).  In practice this
   would be the inverse FFT of a data set that has been band-passed around
   \verb@f0@ and then appropriately down-sampled (e.g. with a lock-in).  The
   normalization rule for FFT data is the following: if sinusoidal data over a
   time $T$ and with amplitude $A$ is FFT-ed, the sum of the square amplitude of
   the output of the FFT (power) is equal to ${A^2 T}$.  Thus, the power peak at
   the sinusoids frequency should be expected to be $\sim$ $\frac{A^2}{2} T$,
   within a factor of 2.  The same normalization rule applies to the DeFT data.
   Thus by piecing together $N$ SFTs we expect a DeFT power peak $\sim N$ higher
   than that of the SFTs - at least in the case of perfect signal-template match.
   
   
   Let us now spend a few words on the choice of the SFT time baseline.  Given an
   intrinsic search frequency one can compute the longest time baseline which is
   still compatible with the requirement that the instantaneous signal frequency
   during such time baseline does not shift by more than a frequency bin.  This
   is the default choice for the SFT length, having assumed that the modulation
   is due to the spin of the Earth and having taken a simple epicyclic model to
   evaluate the magnitude of this effect.  It is possible to choose a different 
   time baseline by specifying a value for
   the variable \verb@gap@ other than 1.  Note that the SFT time baseline is
   approximated to the nearest value such that the number of SFT samples is a
   power of two.  This is also well documented in the code.  
   
   
   The set of SFTs does not necessarily come from contiguous data sets: a set of
   time stamps is created that defines the time of the first sample of each SFT
   data chunk.  The timestamps which are required in many parts of the code are
   generated in a small subroutine \verb@times()@.  This routine takes as input
   the SFT timescale \verb@tSFT@, the number of SFTs which will be created,
   \verb@mObsSFT@, and a switch which lets the code know whether to make even
   timestamps, or timestamps with gaps (see below for more on this).  The
   subroutine then writes the times to the \verb@LIGOTimeGPS@ vector containing
   the timestamps for the entire test code, and returns this vector.  Note that
   each datum of the  \verb@LIGOTimeGPS@ vector is comprised of two fields; if
   accessing the $i^{th}$ datum, the seconds part of the timestamp vector
   \verb@ts@ is \verb@ts[i].gpsSeconds@ and the nanoseconds part is
   \verb@ts[i].gpsNanoSeconds@.  These are the fields which are written in this
   \verb@times()@.
   
   
   As an important side note, let us discuss the effect that a vector of
   timestamps with gaps has on the resulting transformed data.  Since each of the
   timestamps refers to the first datum of each SFT, the existence of the gaps
   means that instead of transforming a continuous set of data, we are reduced to
   transforming a piecewise continuous set.  Since we can envision these gaps as
   simply replacing real data with zeros, we correspondingly should see a power
   loss in the resulting FFTs signal bins and a broadening of the power spectrum.
   Since real detectors will clearly have gaps in the data, this effect is
   obviously something we seek to minimize or eliminate if possible.  This work
   continues to be under development.
   
   
   The total observation time determines how many SFTs and how many DeFTs are
   created.  The actual time baseline for both the DeFTs and the total
   observation time might differ from the ones defined in the input file, the
   reason being that they are rounded to the nearest multiple of the SFT time
   baseline.
   
   Note that use is made of the \verb@LALBarycenter()@ routine (see section 
   \ref{s:LALBarycenter.h}), which (among other things) provides,  at any given 
   time, the actual instantaneous position and velocity of a  detector at any 
   specified location of the Earth with respect to the SSB.  
   
   \item Following the creation of a short chunk of time series data, an FFT is
   performed with the internal FFTW routines.  This outputs a frequency domain
   chunk which is placed into the \verb@SFTData@ array of structures.  This will
   contain all of the SFT data we need to demodulate, and in the future, will be
   the storage area for the real data.
   
   \item The next module begins the demodulation process.  First, the parameters
   for the demodulation routine are assigned from values previously calculated in
   the test code.  Similarly, parameters for the \verb@ComputeSky()@ routine are
   assigned.  This routine computes the coefficients $A_{s\alpha}$ and
   $B_{s\alpha}$ (see section \ref{s:ComputeSky.h}) of the spindown parameters
   for the phase model weve assumed.  These coefficients are used within the
   \verb@LALDemod()@ routine itself. Since they only depend on the template sky
   position, in a search over many different spin-down parameters they are
   reused, thus one needs compute them only once.  Then, the \verb@LALComputeAM()@ 
   routine is called, to calculate the amplitude modulation filter information.  Finally, at last, the
   demodulation routine itself is called, and, if the command line option
   '\verb@-o@' is used,  output are several data files containing demodulated
   data (these are by default named '\verb@xhat_#@').  These output files have two columns, one for the value of the periodogram and one for the frequency.
   
   \end{itemize}
   
   
   \subsubsection*{Exit codes}
   
   \subsubsection*{Uses}
   \begin{verbatim}
   lalDebugLevel
   LALMalloc()
   LALFopen()
   LALFclose()
   LALSCreateVector()
   LALCreateRandomParams()
   LALNormalDeviates()
   LALDestroyRandomParams()
   LALSDestroyVector()
   LALCCreateVector()
   LALCreateForwardRealFFTPlan()
   LALREAL4VectorFFT()
   LALCDestroyVector()
   LALDestroyRealFFTPlan()
   LALGenerateTaylorCW()
   LALSimulateCoherentGW()
   ComputeSky()
   LALFree()
   LALDemod()
   LALBarycenter()
   LALComputeAM()
   \end{verbatim}
   
   \subsubsection*{Notes}
   The implementation of the code here is intended to give a general outline of 
   what the demodulation code needs to work.  Most of this test function performs 
   steps (e.g., noise, time- and frequency-series generation) that will be already 
   present in the data.  
   
   \vfill{\footnotesize\input{LALDemodTestCV}}
   
   </lalLaTeX>
#endif /* autodoc block */
 
#ifndef LALDEMODTEST_C
#define LALDEMODTEST_C
#endif

/* Usage */
#define USAGE "Usage: %s [-i basicInputsFile] [-n] [-d] [-o]\n"


/* Error macro, taken from ResampleTest.c in LAL's pulsar/test package */
#define ERROR( code, msg, statement )                                \
if ( lalDebugLevel & LALERROR )                                      \
{                                                                    \
  LALPrintError( "Error[0] %d: program %s, file %s, line %d, %s\n"   \
		 "        %s %s\n", (code), *argv, __FILE__,		\
		 __LINE__, LALDEMODTESTC, statement ? statement : "",\
		 (msg) );                                            \
}                                                                    \
else (void)(0)

#include <lal/LALDemod.h>
#include <lal/LALInitBarycenter.h>
#include <lal/FileIO.h>
#include <lal/AVFactories.h>
#include <lal/SeqFactories.h>

static void TimeToFloat(REAL8 *f, LIGOTimeGPS *tgps);

static void FloatToTime(LIGOTimeGPS *tgps, REAL8 *f);

NRCSID(LALDEMODTESTC, "$Id$");

int lalDebugLevel = 3;

int main(int argc, char **argv)
{
  static LALStatus status;
	
  /***** VARIABLE DECLARATION *****/
  char earthEphemeris[] = "earth98.dat";
  char sunEphemeris[] = "sun98.dat";

  ParameterSet *signalParams;
  ParameterSet *templateParams;
  char *basicInputsFile;
  FILE *bif;
  REAL8 tObs, tCoh, tSFT;
  REAL8 oneOverSqrtTSFT;
  REAL8 aCross, aPlus, SNR;
  REAL8 f0;
	
  INT4 mCohSFT, mObsCoh, mObsSFT;
  REAL8 dfSFT, dt;
  INT4 if0Min, if0Max, ifMin, ifMax;
  REAL8 f0Min, f0Max, fMin, f0Band, fWing;
  INT4 nDeltaF;

  LIGOTimeGPS *timeStamps;

  REAL4Vector *temp = NULL;
  REAL4Vector *noise = NULL;
  static RandomParams *params;
  INT4 seed=0;
	
  INT4 a,i,k;
	
  FFT **SFTData;
  RealFFTPlan *pfwd = NULL;
  COMPLEX8Vector *fvec = NULL;
	
  DemodPar *demParams;
  CSParams *csParams;
  INT4 iSkyCoh;
	
  DeFTPeriodogram **xHat;
	
  FILE /* *PeaksFile, *TimeSFile, *SftFile, */ *XhatFile;

  const CHAR *noi=NULL;
  INT4 deletions=1;
  const CHAR *output=NULL;
	
  CHAR filename[13];
  REAL8 factor;
  INT2 arg;

  /* file name if needed for SFT output files */
  CHAR *sftoutname=NULL;

  /* Quantities for use in LALBarycenter package */
  BarycenterInput baryinput;
  EmissionTime emit;
  EarthState earth;
  LALDetector cachedDetector;
	  
  EphemerisData *edat=NULL;

  REAL4TimeSeries *timeSeries = NULL;

  /*
   *  Variables for AM correction
   */
  CoherentGW cgwOutput;
  TaylorCWParamStruc genTayParams;
  DetectorResponse cwDetector;
  AMCoeffsParams *amParams;
  AMCoeffs amc;

#define DEBUG 1
#if (0)
     
  INT2 tmp=0; 		
  REAL8 dtSFT, dfCoh;	
  REAL8 fMax; 		
  INT4 nSFT; 		

#endif
	
  /***** END VARIABLE DECLARATION *****/
  
   
  /***** PARSE COMMAND LINE OPTIONS *****/	
  basicInputsFile=(char *)LALMalloc(50*sizeof(char));
  LALSnprintf(basicInputsFile, 9, "in.data\0");
  arg=1;
  while(arg<argc) {
		
    /* the input file */
    if(!strcmp(argv[arg],"-i"))
      {
	strcpy(basicInputsFile,argv[++arg]);
	arg++;
	if(LALOpenDataFile(basicInputsFile)==NULL)
	  {
	    ERROR(LALDEMODH_ENOFILE, LALDEMODH_MSGENOFILE, 0);
	    LALPrintError(USAGE, *argv);
	    return LALDEMODH_ENOFILE;
	  }
      }
		
    /* turn noise on? if string is not NULL, it will be */
    else if(!strcmp(argv[arg],"-n"))
      {
	noi="n";
	arg++;
      }

    /* make SFT file output (such that the driver code can read in as input data) */
    else if (!strcmp(argv[arg],"-m")) 
      {
      sftoutname=argv[++arg];
      arg++;
      }	
    /* turn timestamp deletions on? if string is not NULL, it will be */
    else if(!strcmp(argv[arg],"-d"))
      {
	deletions=atoi(argv[++arg]);
	arg++;
      }
    
    /* turn on output? if string is not NULL, it will be */
    else if(!strcmp(argv[arg],"-o"))
      {
	output="o";
	arg++;
      }
		
    /* default: no input file specified */
    else if(basicInputsFile==NULL)
      {
	bif=LALFopen("in.data","r");
      }
		
    /* erroneous command line argument */
    else if(basicInputsFile!=NULL && arg<argc)
      {
	ERROR(LALDEMODH_EBADARG, LALDEMODH_MSGEBADARG, 0);		
	LALPrintError(USAGE, *argv);
	arg=argc;
	return LALDEMODH_EBADARG;
      }
  }
	
  /***** END COMMAND LINE PARSING *****/

	
  /***** INITIALIZATION OF SIGNAL AND TEMPLATE *****/

  /* Allocate space for signal parameters */
  signalParams=LALMalloc(sizeof(ParameterSet));
  signalParams->spind=LALMalloc(sizeof(Spindown));
  signalParams->skyP=LALMalloc(2*sizeof(SkyPos));
  signalParams->spind->spParams=LALMalloc(5*sizeof(REAL8));

  /* Allocate space for template parameters */
  templateParams=LALMalloc(sizeof(ParameterSet));
  templateParams->spind=LALMalloc(sizeof(Spindown));
  templateParams->skyP=LALMalloc(2*sizeof(SkyPos));
  templateParams->spind->spParams=LALMalloc(5*sizeof(REAL8));

  /***** END INITIALIZATION *****/
	
	
  /***** GET INPUTS FROM FILES *****/

  bif=LALOpenDataFile(basicInputsFile);
	
  fscanf(bif, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%d\n%le\n%le\n%le\n%le\n%le\n%lf\n"
	 " %lf\n%d\n%le\n%le\n%le\n%le\n%le\n%lf\n%lf\n",
	 &tObs, &tCoh, &factor, &aCross, &aPlus, &SNR, &f0Band, &f0,
	 &signalParams->spind->m, 
	 &signalParams->spind->spParams[0], &signalParams->spind->spParams[1], 	
	 &signalParams->spind->spParams[2], &signalParams->spind->spParams[3],
	 &signalParams->spind->spParams[4], 
	 &signalParams->skyP->alpha, &signalParams->skyP->delta, 
	 &templateParams->spind->m,
	 &templateParams->spind->spParams[0], &templateParams->spind->spParams[1],
	 &templateParams->spind->spParams[2], &templateParams->spind->spParams[3],
	 &templateParams->spind->spParams[4], 
	 &templateParams->skyP->alpha, &templateParams->skyP->delta);
	
  LALFclose(bif);

  /***** END FILE INPUT *****/
	

  /***** CALCULATE USEFUL QUANTITIES *****/

  /*	2^n gives the number of samples of the SFT. This will be a signal in a 	*/	
  /*	band fSample/2 with fSample=2*(2.e-4 f0+f0Band). This makes sure that	*/ 
  /*	that the sampling frequency enables us to produce the DeFT frequency 	*/ 
  /*	band that we want (f0Band) and that there's enought wings of SFT data	*/ 
  /*	(4.e-4*f0) to produce such DeFT band. 					*/
  /* 	The main formula used here is that the gives the maximum allowed	*/ 
  /*	time-baseline (Tmax) such that a signal of frequency f0 Doppler 	*/ 
  /*	modulated due to Earth spin is seen as monochromatic during such	*/ 
  /*	observation time: 							*/
  /* 	  	Tmax < 9.6e4/sqrt(f0). 						*/
  /*	We have thus taken Tmax=9.4e4/sqrt(f0). A different criterion can be	*/ 
  /*	chosen by setting the variable "factor" to a suitable value. We have	*/ 
  /*	rounded the resulting number of samples to the nearest smallest power 	*/ 
  /*	of two. 							       	*/

  /* Convert signal spindown to conform with GenerateTaylorCW() definition */
  for(i=0;i<signalParams->spind->m;i++){signalParams->spind->spParams[i] /= f0;}

  {
    REAL8 tempf0Band;
    /* compute size of SFT wings */
    fWing = 2.0e-4 * f0;
    /* Adjust search band to include wings */
    f0Band += 2.0*fWing;
    tempf0Band = f0Band;
    tSFT=9.6e4/sqrt(f0);
    nDeltaF = (INT4)(ceil(f0Band*tSFT));
    tSFT = (REAL8)nDeltaF/f0Band;
    oneOverSqrtTSFT = 1.0/sqrt(tSFT);
    /* Number of data in time series for 1 SFT */
    dfSFT=1.0/tSFT;
    /* Get rid of the wings */
    f0Band -= 2.0*fWing;
    /* the index of the right side of the band, NO WINGS */
    if0Max=ceil((f0+f0Band/2.0)/dfSFT);
    /* the index of the left side of the band, NO WINGS */
    if0Min=floor((f0-f0Band/2.0)/dfSFT);
    /* frequency of the right side of the band, NO WINGS */
    f0Max=dfSFT*if0Max;
    /* frequency of the left side of the band, NO WINGS */
    f0Min=dfSFT*if0Min;   
    
    f0Band = tempf0Band;
  }
  /* index of the left side of the band, WITH WINGS */
  ifMin=floor(f0/dfSFT)-nDeltaF/2;
  /* indexof the right side of the band, WITH WINGS */
  ifMax=ifMin+nDeltaF;
  /* frequency of the left side of the band, WITH WINGS */
  fMin=dfSFT*ifMin;  
  nDeltaF=(INT4)(ceil(f0Band/dfSFT));
  /* Number of SFTs which make one DeFT */ 
  mCohSFT=ceil(tCoh/tSFT);
  if (mCohSFT==0) mCohSFT=1;
  /* Coherent search time baseline */
  tCoh=tSFT*mCohSFT;
  /* Number of coherent timescales in total obs. time */
  mObsCoh=floor(tObs/tCoh);
  if (mObsCoh ==0) mObsCoh=1;
  /* Total observation time */
  tObs=tCoh*mObsCoh;
  /* Number of SFTs needed during observation time */
  mObsSFT=mCohSFT*mObsCoh;	

  /* convert input angles from degrees to radians */
  signalParams->skyP->alpha=signalParams->skyP->alpha*LAL_TWOPI/360.0;
  signalParams->skyP->delta=signalParams->skyP->delta*LAL_TWOPI/360.0;
  templateParams->skyP->alpha=templateParams->skyP->alpha*LAL_TWOPI/360.0;
  templateParams->skyP->delta=templateParams->skyP->delta*LAL_TWOPI/360.0;

  /***** END USEFUL QUANTITIES *****/	


  /***** CALL ROUTINE TO GENERATE TIMESTAMPS *****/
	
  timeStamps=(LIGOTimeGPS *)LALMalloc(mObsSFT*sizeof(LIGOTimeGPS));
  times(tSFT, mObsSFT, timeStamps, deletions);
	
  /***** END TIMESTAMPS *****/


  /***** CREATE NOISE *****/

  if(noi!=NULL)
    {
      LALCreateVector(&status, &noise, (UINT4)nDeltaF*mObsSFT);
      LALCreateVector(&status, &temp, (UINT4)nDeltaF*mObsSFT);
      LALCreateRandomParams(&status, &params, seed);
	
      /* fill temp vector with normal deviates*/ 
      LALNormalDeviates(&status, temp, params); 
	
      /* rewrite into COMPLEX8 VECTOR *noise */
      i=0;
      while(i<nDeltaF*mObsSFT)
	{
	  noise->data[i]=temp->data[i];
	  i++;
	} 

      /* Destroy structures that were used here, if we can */
      LALDestroyRandomParams(&status, &params);
      LALSDestroyVector(&status, &temp);
    }

  /***** END CREATE NOISE *****/

  /* Quantities computed for barycentering */
  edat=(EphemerisData *)LALMalloc(sizeof(EphemerisData));
  (*edat).ephiles.earthEphemeris = earthEphemeris;
  (*edat).ephiles.sunEphemeris = sunEphemeris;

  /* Read in ephemerides */  
  LALInitBarycenter(&status, edat);
  /*Getting detector coords from DetectorSite module of tools package */   
  
  /* Cached options are:
     LALDetectorIndexLHODIFF, LALDetectorIndexLLODIFF,
     LALDetectorIndexVIRGODIFF, LALDetectorIndexGEO600DIFF,
     LALDetectorIndexTAMA300DIFF,LALDetectorIndexCIT40DIFF 
  */
  cachedDetector = lalCachedDetectors[LALDetectorIndexGEO600DIFF];
  baryinput.site.location[0]=cachedDetector.location[0]/LAL_C_SI;
  baryinput.site.location[1]=cachedDetector.location[1]/LAL_C_SI;
  baryinput.site.location[2]=cachedDetector.location[2]/LAL_C_SI;
  baryinput.alpha=signalParams->skyP->alpha;
  baryinput.delta=signalParams->skyP->delta;
  baryinput.dInv=0.e0;


  /***** CREATE SIGNAL *****/
  
  /* Set up parameter structure */  
  /* Source position */
  genTayParams.position.latitude  = signalParams->skyP->delta;
  genTayParams.position.longitude = signalParams->skyP->alpha;
  genTayParams.position.system = COORDINATESYSTEM_EQUATORIAL;
  /* Source polarization angle */
  /* Note, that the way we compute the statistic, we don't care what this value is. */
  genTayParams.psi = 0.0;
  /* Source initial phase */
  genTayParams.phi0 = 0.0; 
  /* Source polarization amplitudes */
  genTayParams.aPlus = aPlus;
  genTayParams.aCross = aCross;
  /* Source intrinsic frequency */
  genTayParams.f0 = f0;
  /* Source spindown parameters: create vector and assign values */
  genTayParams.f = NULL;
  LALDCreateVector(&status, &(genTayParams.f), signalParams->spind->m); 
  for(i=0;i<signalParams->spind->m;i++){genTayParams.f->data[i] = signalParams->spind->spParams[i];} 
  /* Time resolution for source */
  /* Note, that this needs to be sampled only very coarsely! */
  genTayParams.deltaT = 100.0;
  /* Length should include fudge factor to allow for barycentring */
  genTayParams.length = (tObs+1200.0)/genTayParams.deltaT;
  memset(&cwDetector, 0, sizeof(DetectorResponse));
  /* The ephemerides */
  cwDetector.ephemerides = edat;
  /* Specifying the detector site (set above) */
  cwDetector.site = &cachedDetector;  
  /* The transfer function.  
   * Note, this xfer function has only two points */
  cwDetector.transfer = (COMPLEX8FrequencySeries *)LALMalloc(sizeof(COMPLEX8FrequencySeries));
  memset(cwDetector.transfer, 0, sizeof(COMPLEX8FrequencySeries));
  cwDetector.transfer->epoch = timeStamps[0];
  cwDetector.transfer->f0 = 0.0;
  cwDetector.transfer->deltaF = 16384.0;
  cwDetector.transfer->data = NULL;
  LALCCreateVector(&status, &(cwDetector.transfer->data), 2);
  /* Allocating space for the eventual time series */
  timeSeries = (REAL4TimeSeries *)LALMalloc(sizeof(REAL4TimeSeries));
  timeSeries->data = (REAL4Vector *)LALMalloc(sizeof(REAL4Vector));
  
  /* The length of the TS will be plenty long; below, we have it set so that
   * it is sampled at just better than twice the Nyquist frequency, then 
   * increased that to make it a power of two (to facilitate a quick FFT)  
   */
  {
    INT4 lenPwr;
    lenPwr = ceil(log(tSFT*2.01*f0)/log(2.0));
    timeSeries->data->length = pow(2.0,lenPwr);
  }
  
  timeSeries->data->data = (REAL4 *)LALMalloc(timeSeries->data->length*sizeof(REAL4));
  dt = timeSeries->deltaT = tSFT/timeSeries->data->length;

  /* unit response function */
  cwDetector.transfer->data->data[0].re = 1.0;
  cwDetector.transfer->data->data[1].re = 1.0;
  cwDetector.transfer->data->data[0].im = 0.0;
  cwDetector.transfer->data->data[1].im = 0.0;
  /* again, the note about the barycentring */
  genTayParams.epoch.gpsSeconds = timeStamps[0].gpsSeconds - 600; 
  genTayParams.epoch.gpsNanoSeconds = timeStamps[0].gpsNanoSeconds; 
  memset(&cgwOutput, 0, sizeof(CoherentGW));

  /* 
   *
   * OKAY, GENERATE THE SIGNAL @ THE SOURCE 
   *
   */
  LALGenerateTaylorCW(&status, &cgwOutput, &genTayParams);      

  {
    INT4 len,len2;
    
    len=timeSeries->data->length;
    len2=len/2+1;
    
    /* Create vector to hold frequency series */
    LALCCreateVector(&status, &fvec, (UINT4)len2);
    
    /* Compute measured plan for FFTW */
    LALCreateForwardRealFFTPlan(&status, &pfwd, (UINT4)len, 0);
    
    /* Allocate memory for the SFTData structure */
    /* 
     * Note that the length allocated for the data
     * array is the width of the band we're interested in,
     * plus f0*4E-4 for the 'wings', plus a bit of wiggle room.
     */
    SFTData=(FFT **)LALMalloc(mObsSFT*sizeof(FFT *));
    for(i=0;i<mObsSFT;i++){
      SFTData[i]=(FFT *)LALMalloc(sizeof(FFT));
      SFTData[i]->fft=(COMPLEX8FrequencySeries *)
	LALMalloc(sizeof(COMPLEX8FrequencySeries));
      SFTData[i]->fft->data=(COMPLEX8Vector *)
	LALMalloc(sizeof(COMPLEX8Vector));
      SFTData[i]->fft->data->data=(COMPLEX8 *)
	LALMalloc((nDeltaF+1)*sizeof(COMPLEX8));
    }

    /* Lots of debugging stuff.  If you want to use these, go ahead, but
     * be warned that these files may be HUGE (10s-100s of GB).
     */
#if(0)   
    PeaksFile=LALFopen("/tmp/peaks.data","w");
#endif
#if(0)	
    TimeSFile=LALFopen("/tmp/ts.data","w");
#endif
#if(0)
    SftFile=LALFopen("/tmp/sft.data","w");
#endif

    {
      REAL4Vector *tempTS = NULL; 
      
      LALSCreateVector(&status, &tempTS, (UINT4)len); 
      
      for(a=0;a<mObsSFT;a++)
	{
	  REAL4Vector *ts = timeSeries->data;
	  /*  
	   *  Note that this epoch is different from the epoch of GenTay.
	   *  The difference is seconds, a little bit more than the 
	   *  maximum propagation delay of a signal between the Earth
	   *  and the solar system barycentre.
	   */
	  timeSeries->epoch = timeStamps[a];
	  /* 
	   *
	   *  OKAY, CONVERT SOURCE SIGNAL TO WHAT SOMEBODY SEES AT THE DETECTOR OUTPUT
	   *
	   */
	  LALSimulateCoherentGW(&status, timeSeries, &cgwOutput, &cwDetector);
  	    
#if (0)
	  /* Print out time series if DEBUGging on */
	  for(i=0; i<len; i++)
	    {
	      fprintf(TimeSFile,"%20.10lf\n",ts->data[i]);
	    }
#endif

	  /* Write time series of correct size to temp Array, for FFT */
	  for(i=0;i<len;i++)
	    {
	      tempTS->data[i] = ts->data[i];
	    }
	  
	  /* Perform FFTW-LAL Fast Fourier Transform */
	  LALForwardRealFFT(&status, fvec, tempTS, pfwd);

	    
#if(0)
	  {INT4 g;
	  for(g=0; g<fvec->length; g++)
	    {
	      COMPLEX8 temp= fvec->data[g];
	      fprintf(SftFile,"%20.10lf\n",temp.re*temp.re+temp.im*temp.im);
	    }}
#endif
	  
	  {     
	    INT4 fL = ifMin;
	    INT4 cnt = 0; 
	    while(fL < ifMin+nDeltaF+1)
	      {
		COMPLEX8 *tempSFT=SFTData[a]->fft->data->data;
		COMPLEX8 *fTemp=fvec->data;
		
		/* Also normalise */
		tempSFT[cnt].re = fTemp[fL].re * oneOverSqrtTSFT;
		tempSFT[cnt].im = fTemp[fL].im * oneOverSqrtTSFT;
#if (0)
		fprintf(SftFile,"%20.12lf %20.12lf\n", (REAL8)fL*dfSFT, tempSFT[cnt].re*tempSFT[cnt].re+tempSFT[cnt].im*tempSFT[cnt].im);
#endif
		cnt++; fL++;
		
	      }

#if(0)
	    for(i=0;i<cnt-1;i++)
	      {	
		pw = SFTData[a]->fft->data->data[i].re * 
		  SFTData[a]->fft->data->data[i].re +
		  SFTData[a]->fft->data->data[i].im *
		  SFTData[a]->fft->data->data[i].im;
		
		if(pwMax<pw)
		  {
		    pwMax=pw;
		    ipwMax=i;
		  }
	      }
	    {
	      REAL8 Temp, Temp2, Temp3;
	      TimeToFloat(&Temp, &(timeStamps[a]));
	      TimeToFloat(&Temp2, &(timeStamps[0]));
	      Temp3 = (Temp-Temp2)/86400.0;
	      fprintf(PeaksFile,"%d\t%d\n",ipwMax,a);
	    }
#endif
	  }
	  /* assign particulars to each SFT */
	  SFTData[a]->fft->data->length = nDeltaF+1;  
	  SFTData[a]->fft->epoch = timeStamps[a];
	  SFTData[a]->fft->f0 = f0Min; /* this is the frequency of the first freq in the band */
	  SFTData[a]->fft->deltaF = dfSFT;
	  
	}
      LALSDestroyVector(&status, &tempTS);
    }
    

#if(0)
    /***** file output of SFT power and peaks. *****/
    
    /* If user requested SFT output, generate it here */
    if (sftoutname) {
      CHAR fname[64]={"DefaultFileName"};
      INT4 datasize,errorcode;
      FILE *fp;
      
      struct headertag {
	REAL8 endian;
	INT4  gps_sec;
	INT4  gps_nsec;
	REAL8 tbase;
	INT4  firstfreqindex;
	INT4  nsamples;
      } header;
    
      /* construct file name, and open file */
      sprintf(fname,"%s_SFT.%05d",sftoutname,k);
      fp=fopen(fname,"w");
      if (fp==NULL) {
	LALPrintError("Unable to open file %s for writing!\n",fname);
	exit(1);
      }
    
      /* Write header information */
      header.endian=1.0;
      header.gps_sec=(REAL8)(timeStamps[k].gpsSeconds);
      header.gps_nsec=(REAL8)(timeStamps[k].gpsNanoSeconds);
      header.tbase=tSFT;
      header.firstfreqindex=ifMin;
      datasize=header.nsamples=nDeltaF;
    
      /* open file, write header into it */
      errorcode=fwrite((void*)&header,sizeof(header),1,fp);
      if (errorcode!=1){
	LALPrintError("Error in writing header into file!\n");
	exit(1);
      }
    
      /* datasize is number of complex values to output */
      errorcode=fwrite((void*)SFTData[k]->fft->data->data,2*sizeof(REAL8),datasize,fp);
      if (errorcode!=datasize){
	printf("Error in writing data into file!\n");
	exit(1);
      }
    
      /* close file! */
      fclose(fp);
    }

#endif
    
#if (0)
    LALFclose(PeaksFile);
#endif
#if (0)
    LALFclose(SftFile);
#endif
#if(0)
    LALFclose(TimeSFile);
#endif

    /*
     * Note, we have to destroy the memory that GenTay allocates.
     * This is currently (04.02) not documented! 
     */
    
    if(&(cgwOutput.a) !=NULL) {
      LALSDestroyVectorSequence(&status, &(cgwOutput.a->data));
      LALFree(cgwOutput.a); 
    }
    if(&(cgwOutput.f) !=NULL) {
      LALSDestroyVector(&status, &(cgwOutput.f->data));
      LALFree(cgwOutput.f);
    }
    if(&(cgwOutput.phi) !=NULL) {
      LALDDestroyVector(&status, &(cgwOutput.phi->data));
      LALFree(cgwOutput.phi);
    }

    if(noi!=NULL) {LALDestroyVector(&status, &noise);} 
    LALCDestroyVector(&status, &fvec);
    LALFree(timeSeries->data->data);
    LALFree(timeSeries->data);
    LALFree(timeSeries);
    LALDestroyRealFFTPlan(&status, &pfwd);
  }
  LALDDestroyVector(&status,&(genTayParams.f));
  LALCDestroyVector(&status, &(cwDetector.transfer->data));
  LALFree(cwDetector.transfer);

  /***** END CREATE SIGNAL *****/


 /* BEGIN AMPLITUDE MODULATION */

  /* Allocate space for amParams stucture */
  /* Here, amParams->das is the Detector and Source info */
  amParams = (AMCoeffsParams *)LALMalloc(sizeof(AMCoeffsParams));
  amParams->das = (LALDetAndSource *)LALMalloc(sizeof(LALDetAndSource));
  amParams->das->pSource = (LALSource *)LALMalloc(sizeof(LALSource));
  /* Fill up AMCoeffsParams structure */
  amParams->baryinput = &baryinput;
  amParams->earth = &earth;
  amParams->edat = edat;
  amParams->das->pDetector = &cachedDetector; 
  amParams->das->pSource->equatorialCoords.latitude = templateParams->skyP->delta;
  amParams->das->pSource->equatorialCoords.longitude = templateParams->skyP->alpha;
  amParams->das->pSource->orientation = 0.0;
  amParams->das->pSource->equatorialCoords.system = COORDINATESYSTEM_EQUATORIAL;
  amParams->polAngle = genTayParams.psi;
  amParams->tObs = tObs;
 /* Allocate space for AMCoeffs */
  amc.a = NULL;
  amc.b = NULL;
  LALSCreateVector(&status, &(amc.a), (UINT4)mObsSFT);
  LALSCreateVector(&status, &(amc.b), (UINT4)mObsSFT);
 
 /* 
  * Compute timestamps for middle of each ts chunk 
  * Note, we have decided to use the values of 'a' and 'b'
  * that correspond to the midpoint of the timeSeries for 
  * which the SFT denotes.  In practice, this can be a flaw
  * since 'a' is a sinusoid with T=45ksec and T_b=90ksec; both
  * of these have amplitude about 0.4.  Thus, for 'a', on a timescale
  * of 10ksec, the value changes significantly.  
  */
  {
    LIGOTimeGPS *midTS;  /* MODIFIED BY JOLIEN: DYNAMIC MEMORY ALLOCATION */
    midTS = LALCalloc( mObsSFT, sizeof( *midTS ) );
    if ( ! midTS )
      return fprintf( stderr, "Allocation error near line %d", __LINE__ ), 1;

    for(k=0; k<mObsSFT; k++)
      {
	REAL8 teemp=0.0;
       
	TimeToFloat(&teemp, &(timeStamps[k]));
	teemp += 0.5/dfSFT;
	FloatToTime(&(midTS[k]), &teemp);
      }
    /* Compute the AM coefficients */
    LALComputeAM(&status, &amc, midTS, amParams);
    LALFree( midTS );
  }

  /***** DEMODULATE SIGNAL *****/
 
 /* Allocate space and set quantity values for demodulation parameters */
  demParams=(DemodPar *)LALMalloc(sizeof(DemodPar));
  demParams->skyConst=(REAL8 *)LALMalloc((2*templateParams->spind->m * 
					  (mObsSFT+1)+2*mObsSFT+3)*sizeof(REAL8));
  demParams->spinDwn=(REAL8 *)LALMalloc(templateParams->spind->m*sizeof(REAL8));
  demParams->if0Max=if0Max;
  demParams->if0Min=if0Min;
  demParams->mCohSFT=mCohSFT;
  demParams->mObsCoh=mObsCoh;
  demParams->ifMin=ifMin;
  demParams->spinDwnOrder=templateParams->spind->m;
  demParams->amcoe = &amc;
 
 for(i=0;i<signalParams->spind->m;i++)
   {
     demParams->spinDwn[i]=templateParams->spind->spParams[i];
   }
 
 /* Allocate space and set quantities for call to ComputeSky() */
 csParams=(CSParams *)LALMalloc(sizeof(CSParams));
 csParams->skyPos=(REAL8 *)LALMalloc(2*sizeof(REAL8));
 csParams->skyPos[0]=templateParams->skyP->alpha;
 csParams->skyPos[1]=templateParams->skyP->delta;
 csParams->tGPS=timeStamps;
 csParams->spinDwnOrder=templateParams->spind->m;
 csParams->mObsSFT=mObsSFT;
 csParams->tSFT=tSFT;
 csParams->edat=edat; 
 csParams->emit=&emit;
 csParams->earth=&earth;
 csParams->baryinput=&baryinput;

  iSkyCoh=0;
  
  /* Call COMPUTESKY() */
  ComputeSky(&status, demParams->skyConst, iSkyCoh, csParams);
  
  /* Deallocate space for ComputeSky parameters */
  LALFree(csParams->skyPos);
  LALFree(csParams);
  
  /* Allocate memory for demodulated data */
  xHat=(DeFTPeriodogram **)LALMalloc(mObsCoh*sizeof(DeFTPeriodogram *));
  for(i=0; i<mObsCoh; i++)
    {
      xHat[i]=(DeFTPeriodogram *)LALMalloc(sizeof(DeFTPeriodogram));
      xHat[i]->fft=(REAL8FrequencySeries *) 	
	LALMalloc(sizeof(REAL8FrequencySeries));
      xHat[i]->fA=(COMPLEX16FrequencySeries *)LALMalloc(sizeof(COMPLEX16FrequencySeries));
      xHat[i]->fB=(COMPLEX16FrequencySeries *)LALMalloc(sizeof(COMPLEX16FrequencySeries));      
      xHat[i]->fft->data=(REAL8Vector *)LALMalloc(sizeof(REAL8Vector));
      xHat[i]->fft->data->data=(REAL8 *)LALMalloc((UINT4)((if0Max-if0Min+1)*mCohSFT)*sizeof(REAL8));
      xHat[i]->fft->data->length=(UINT4)((if0Max-if0Min+1)*mCohSFT);
      xHat[i]->fA->data=(COMPLEX16Vector *)LALMalloc(sizeof(COMPLEX16Vector));
      xHat[i]->fA->data->data=(COMPLEX16 *)LALMalloc((UINT4)((if0Max-if0Min+1)*mCohSFT)*sizeof(COMPLEX16));      
      xHat[i]->fA->data->length=(UINT4)((if0Max-if0Min+1)*mCohSFT);
      xHat[i]->fB->data=(COMPLEX16Vector *)LALMalloc(sizeof(COMPLEX16Vector));
      xHat[i]->fB->data->data=(COMPLEX16 *)LALMalloc((UINT4)((if0Max-if0Min+1)*mCohSFT)*sizeof(COMPLEX16));      
      xHat[i]->fB->data->length=(UINT4)((if0Max-if0Min+1)*mCohSFT);
    }
  
  for(k=0; k<mObsCoh; k++)
    {
      demParams->iCoh=k;
      
      /**************************/
      /*       DEMODULATE       */
      /**************************/
      
      LALDemod(&status, *(xHat+k), SFTData, demParams);
      if(output!=NULL){
	sprintf(filename,"/scratch/steveb/xhat_%d.data",k);
	XhatFile=LALFopen(filename,"w");
	printf("Dumping demodulated data to disk: xhat_%d.data  \n",k);
	for(i=0;i<(if0Max-if0Min)*mCohSFT+1;i++) {
	  fprintf(XhatFile,"%24.16f\t%24.16f\n",f0Min+(REAL8)i/tCoh, 	
		  xHat[k]->fft->data->data[i]);
	}
	LALFclose(XhatFile);
      }

    }		
  /***** END DEMODULATION *****/
			
		
  /***** DEALLOCATION *****/

  /* Deallocate AM  */
  LALSDestroyVector(&status, &(amc.a));
  LALSDestroyVector(&status, &(amc.b));
  LALFree(amParams->das->pSource);
  LALFree(amParams->das);
  LALFree(amParams);
  
  /* Deallocate SFTData structure, since we don't need it anymore */
  for(i=0;i<mObsSFT;i++)
    {
      LALFree(SFTData[i]->fft->data->data);
      LALFree(SFTData[i]->fft->data);
      LALFree(SFTData[i]->fft);
      LALFree(SFTData[i]);
    }
  LALFree(SFTData);
  
  /* Deallocate memory used by demodulated data */
  for(i=0; i<mObsCoh; i++)
    {
      LALFree(xHat[i]->fft->data->data);
      LALFree(xHat[i]->fft->data);
      LALFree(xHat[i]->fft);
      LALFree(xHat[i]->fA->data->data);
      LALFree(xHat[i]->fA->data);
      LALFree(xHat[i]->fA);
      LALFree(xHat[i]->fB->data->data);
      LALFree(xHat[i]->fB->data);
      LALFree(xHat[i]->fB);
      LALFree(xHat[i]);
    }
  LALFree(xHat);
	
  /* Deallocate template and signal params */
  LALFree(templateParams->spind->spParams);
  LALFree(templateParams->spind);
  LALFree(templateParams->skyP);
  LALFree(templateParams);

  LALFree(signalParams->spind->spParams);
  LALFree(signalParams->spind);
  LALFree(signalParams->skyP);
  LALFree(signalParams);

  /* Deallocate demodulation params */
  LALFree(demParams->skyConst);
  LALFree(demParams->spinDwn);
  LALFree(demParams);
	
  LALFree(timeStamps);
  LALFree(basicInputsFile);
  /* Anything else */

  LALFree(edat->ephemS);
  LALFree(edat->ephemE);
  LALFree(edat);
  LALCheckMemoryLeaks(); 
  return 0;
}


/***** This is the routine which computes the timestamps *****/
void times(REAL8 tSFT, INT4 howMany, LIGOTimeGPS *ts, INT4 sw)
{
  int i=0, j=0;
  int temp1=0, temp2=0;

  while(i<sw*howMany)
    {
      temp1=floor(tSFT*i);
      temp2=(int)((tSFT*(double)i-temp1)*1E9);
	
      ts[j].gpsSeconds=temp1+567648000+86400*30;
      ts[j].gpsNanoSeconds=temp2;
		
      i=i+sw;
      j++;
    }
}

static void TimeToFloat(REAL8 *f, LIGOTimeGPS *tgps)
{
  INT4 x, y;

  x=tgps->gpsSeconds;
  y=tgps->gpsNanoSeconds;
  *f=(REAL8)x+(REAL8)y*1.e-9;
}


static void FloatToTime(LIGOTimeGPS *tgps, REAL8 *f)
{
  REAL8 temp0, temp2, temp3;
  REAL8 temp1, temp4;
  
  temp0 = floor(*f);     /* this is tgps.S */
  temp1 = (*f) * 1.e10;
  temp2 = fmod(temp1, 1.e10);
  temp3 = fmod(temp1, 1.e2); 
  temp4 = (temp2-temp3) * 0.1;

  tgps->gpsSeconds = (INT4)temp0;
  tgps->gpsNanoSeconds = (INT4)temp4;
}





