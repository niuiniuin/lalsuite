BUILT_SOURCES =
MOSTLYCLEANFILES =
include $(top_srcdir)/gnuscripts/lalsuite_header_links.am
include $(top_srcdir)/gnuscripts/lalsuite_vcs_info.am

pkginclude_HEADERS = \
	BinaryPulsarTiming.h \
	CWMakeFakeData.h \
	ComputeFstat.h \
	ComputeSky.h \
	DetectorStates.h \
	DopplerFullScan.h \
	DopplerScan.h \
	FstatisticTools.h \
	ExtrapolatePulsarSpins.h \
	FITSFileIO.h \
	FITSPulsarIO.h \
	GSLHelpers.h \
	GeneratePulsarSignal.h \
	GenerateSpinOrbitCW.h \
	GenerateTaylorCW.h \
	GetEarthTimes.h \
	HeterodynedPulsarModel.h \
	HoughMap.h \
	LALBarycenter.h \
	LALComputeAM.h \
	LALHough.h \
	LALInitBarycenter.h \
	LALPulsarConfig.h \
	LALPulsarVCSInfo.h \
	LALPulsarVCSInfoHeader.h \
	LFTandTSutils.h \
	LISAspecifics.h \
	LUT.h \
	LatticeTiling.h \
	LineRobustStats.h \
	MetricUtils.h \
	NormalizeSFTRngMed.h \
	PHMD.h \
	ProbabilityDensity.h \
	PtoleMetric.h \
	PulsarCrossCorr.h \
	PulsarCrossCorr_v2.h \
	PulsarDataTypes.h \
	PulsarSimulateCoherentGW.h \
	ReadPulsarParFile.h \
	SFTClean.h \
	SFTfileIO.h \
	SFTutils.h \
	SSBtimes.h \
	SimulatePulsarSignal.h \
	SinCosLUT.h \
	Statistics.h \
	SuperskyMetrics.h \
	SynthesizeCWDraws.h \
	TransientCW_utils.h \
	TwoDMesh.h \
	UniversalDopplerMetric.h \
	Velocity.h \
	$(END_OF_LIST)

lib_LTLIBRARIES = liblalpulsar.la

liblalpulsar_la_SOURCES = \
	BinaryPulsarTiming.c \
	CWMakeFakeData.c \
	ComputeFstat.c \
	ComputeFstat_Demod.c \
	ComputeFstat_DemodHL_Generic.c \
	ComputeFstat_DemodHL_OptC.c \
	ComputeFstat_Resamp.c \
	ComputeSky.c \
	ConstructPLUT.c \
	DetectorStates.c \
	DopplerFullScan.c \
	DopplerScan.c \
	DriveHough.c \
	FstatisticTools.c \
	ExtrapolatePulsarSpins.c \
	FITSFileIO.c \
	FITSPulsarIO.c \
	GenerateEllipticSpinOrbitCW.c \
	GenerateHyperbolicSpinOrbitCW.c \
	GenerateParabolicSpinOrbitCW.c \
	GeneratePulsarSignal.c \
	GenerateSpinOrbitCW.c \
	GenerateTaylorCW.c \
	GetEarthTimes.c \
	HeterodynedPulsarModel.c \
	HoughMap.c \
	LALBarycenter.c \
	LALComputeAM.c \
	LALInitBarycenter.c \
	LFTandTSutils.c \
	LISAspecifics.c \
	LatticeTiling.c \
	LineRobustStats.c \
	MetricUtils.c \
	NDParamPLUT.c \
	NormalizeSFTRngMed.c \
	ParamPLUT.c \
	PatchGrid.c \
	Peak2PHMD.c \
	ProbabilityDensity.c \
	PtoleMetric.c \
	PulsarCrossCorr.c \
	PulsarCrossCorr_v2.c \
	PulsarSimulateCoherentGW.c \
	ReadPulsarParFile.c \
	SFTClean.c \
	SFTfileIO.c \
	SFTutils.c \
	SSBtimes.c \
	SimulatePulsarSignal.c \
	SinCosLUT.c \
	Statistics.c \
	Stereographic.c \
	SuperskyMetrics.c \
	SynthesizeCWDraws.c \
	TransientCW_utils.c \
	TwoDMesh.c \
	TwoDMeshInternal.c \
	UniversalDopplerMetric.c \
	Velocity.c \
	$(END_OF_LIST)

nodist_liblalpulsar_la_SOURCES = \
	LALPulsarBuildInfoHeader.h \
	LALPulsarVCSInfo.c \
	$(END_OF_LIST)

noinst_LTLIBRARIES =
liblalpulsar_la_LIBADD =

if HAVE_ALTIVEC
noinst_LTLIBRARIES += libcomputefstat_demodhl_altivec.la
liblalpulsar_la_LIBADD += libcomputefstat_demodhl_altivec.la
libcomputefstat_demodhl_altivec_la_SOURCES = ComputeFstat_DemodHL_Altivec.c
endif

if HAVE_SSE_COMPILER
noinst_LTLIBRARIES += libcomputefstat_demodhl_sse.la
liblalpulsar_la_LIBADD += libcomputefstat_demodhl_sse.la
libcomputefstat_demodhl_sse_la_SOURCES = ComputeFstat_DemodHL_SSE.c
libcomputefstat_demodhl_sse_la_CFLAGS = $(AM_CFLAGS) $(SSE_CFLAGS)
endif

EXTRA_liblalpulsar_la_SOURCES = \
	ComputeFstat_DemodHL_Altivec.i \
	ComputeFstat_DemodHL_Generic.i \
	ComputeFstat_DemodHL_OptC.i \
	ComputeFstat_DemodHL_SSE.i \
	ComputeFstat_Demod_ComputeFaFb.c \
	ComputeFstat_internal.h \
	SinCosLUT.i \
	$(END_OF_LIST)

liblalpulsar_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(LIBVERSION)

DISTCLEANFILES = config.h

vcs_build_info_source = LALPulsarVCSInfo.c
vcs_info_sources = LALPulsarVCSInfoHeader.h
build_info_source = LALPulsarBuildInfoHeader.h
build_info_dependencies = $(liblalpulsar_la_DEPENDENCIES)

LDADD = liblalpulsar.la

pkgdata_DATA = \
	earth00-19-DE200.dat.gz \
	earth00-19-DE405.dat.gz \
	earth00-19-DE414.dat.gz \
	earth00-19-DE421.dat.gz \
	earth00-40-DE200.dat.gz \
	earth00-40-DE405.dat.gz \
	earth00-40-DE421.dat.gz \
	earth00-40-DE430.dat.gz \
	sun00-19-DE200.dat.gz \
	sun00-19-DE405.dat.gz \
	sun00-19-DE414.dat.gz \
	sun00-19-DE421.dat.gz \
	sun00-40-DE200.dat.gz \
	sun00-40-DE405.dat.gz \
	sun00-40-DE421.dat.gz \
	sun00-40-DE430.dat.gz \
	tdb_2000-2019.dat.gz \
	te405_2000-2019.dat.gz \
	tdb_2000-2040.dat.gz \
	te405_2000-2040.dat.gz \
	$(END_OF_LIST)

EXTRA_DIST = \
	$(pkgdata_DATA) \
	Eccentric_and_true_anomaly.png \
	LALPulsarConfig.h.in \
	LUTstereo.png \
	config.h.in \
	inject_binary.png \
	inject_eanomaly.png \
	inject_hanomaly.png \
	inject_phase_diagram.png \
	pulsarHough.dox \
	pulsar_tiling.png \
	$(END_OF_LIST)
