# ######################################################################
#
# pixelLumi.py
#
# ----------------------------------------------------------------------
import os
import FWCore.ParameterSet.Config as cms
process = cms.Process("Lumi")

# ----------------------------------------------------------------------
#process.load('Configuration.StandardSequences.Services_cff')
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
#process.MessageLogger.categories.append('HLTrigReport')
process.MessageLogger.HLTrigReport=dict()
process.MessageLogger.L1GtTrigReport=dict()
#process.MessageLogger.categories.append('L1GtTrigReport')
#process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

#dump event contents
process.dump=cms.EDAnalyzer('EventContentAnalyzer')

# -- Database configuration
process.load("CondCore.CondDB.CondDB_cfi")

# -- Conditions

process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff") #
process.load("RecoVertex.BeamSpotProducer.BeamSpot_cfi")

#is this needed??
#process.load("RecoLocalCalo.HcalRecAlgos.hcalRecAlgoESProd_cfi")
process.load("RecoLocalCalo.Configuration.hcalLocalReco_cff")

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff') #condDBv2_cff')
#from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
from Configuration.AlCa.GlobalTag import GlobalTag

#process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_v14', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '90X_upgrade2023_realistic_v1', '')
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2022_realistic', '')
#??
#process.GlobalTag = GlobalTag(process.GlobalTag, '90X_upgrade2023_realistic_v9', '')

#is this needed for qie?
#process.GlobalTag.globaltag ='100X_dataRun2_HLT_v3'
#process.GlobalTag.globaltag ='106X_mcRun2_asymptotic_v13'


process.load("Configuration.StandardSequences.Reconstruction_cff") # 
# -- number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
    #input = cms.untracked.int32(10)
    )

# -- skimming
process.zerobiasfilter = cms.EDFilter("HLTHighLevel",
   TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
   #HLTPaths = cms.vstring("HLT_ZeroBias_v*"),
   HLTPaths = cms.vstring("*ZeroBias*"),
   eventSetupPathsKey = cms.string(""),
   andOr = cms.bool(True),
   throw = cms.bool(False)
    )

# QIE10  Unpacker
#------------------------------------------------------------------------------------
# this is needed to get hCalDigis
process.load("EventFilter.HcalRawToDigi.HcalRawToDigi_cfi")

#??
process.load('Configuration.EventContent.EventContent_cff')

process.qie10Digis = process.hcalDigis.clone()
#process.qie11Digis = process.hcalDigis.clone()

#process.tuple_step = cms.Sequence(
#    ## Make HCAL tuples: Event info
#    #process.hcalTupleEvent*
#
#    ## Make HCAL tuples: FED info
#    #process.hcalTupleFEDs*
#
#    ## Make HCAL tuples: digi info
#    #process.hcalTupleHBHEDigis*
#    #process.hcalTupleHODigis*
#    #process.hcalTupleHFDigis*
#    process.hcalTupleQIE10Digis* # for HF
#    #process.hcalTupleQIE11Digis* 
#
#    ## Make HCAL tuples: reco info
#    #process.hcalTupleHBHERecHits*
#    #process.hcalTupleHFRecHits*
#    #process.hcalTupleHORecHits*
#
#    ## Make HCAL tuples: trigger info
#    #process.hcalTupleTrigger*
#    #process.hcalTupleTriggerPrimitives*
#    #process.hcalTupleTriggerObjects*
#
#    # noise filter
##    process.hcalTupleHcalNoiseFilters*
#
#    ## Package everything into a tree
#    process.hcalTupleTree
#)

#process.preparation = cms.Path(
#    ## Unpack digis from RAW
#    #process.RawToDigi*
#    #process.hcalDigis*
#    process.qie10Digis*
#    #process.qie11Digis*
#
#    ## reconstruction
#    #process.L1Reco*
#    #process.reconstruction*
#    #process.hcalLocalRecoSequence*
#
#    ## Do energy reconstruction
#    #process.horeco*
##    process.hfprereco*
##    process.hfreco*
##    process.hbheprereco*
##    process.hbheplan1*
#
#    ## For noise filter
##    process.hcalnoise*
##    process.HBHENoiseFilterResultProducer*
#    #process.ApplyBaselineHBHENoiseFilter*
#
#    ## Make the ntuples
#    process.tuple_step
#)

#this is needed??
#process.esp = cms.ESProducer("HcalPedestalWidths")

# the main Analyzer
process.lumi = cms.EDAnalyzer(
    "PCCNTupler",
    verbose                      = cms.untracked.int32(0),
    #rootFileName                 = cms.untracked.string(rootFileName),
    #type                         = cms.untracked.string(getDataset(process.source.fileNames[0])),
    globalTag                    = process.GlobalTag.globaltag,
    dumpAllEvents                = cms.untracked.int32(0),
    vertexCollLabel              = cms.untracked.InputTag('offlinePrimaryVertices'),
    pixelClusterLabel            = cms.untracked.InputTag('siPixelClusters'), # even in Phase2, for now.
    qietag                       = cms.untracked.InputTag("simHcalUnsuppressedDigis", "HFQIE10DigiCollection"),
    saveType                     = cms.untracked.string('Event'), # LumiSect, LumiNib, Event
    sampleType                   = cms.untracked.string('MC'), # MC, DATA
    includeVertexInformation     = cms.untracked.bool(True), 
    includePixels                = cms.untracked.bool(True), 
    includeHF                    = cms.untracked.bool(True), #bpg added
    splitByBX                    = cms.untracked.bool(True), 
    L1GTReadoutRecordLabel       = cms.untracked.InputTag('gtDigis'), 
    #hltL1GtObjectMap             = cms.untracked.InputTag('hltL1GtObjectMap'), 
    HLTResultsLabel              = cms.untracked.InputTag('TriggerResults::HLT'),
    pixelPhase2Geometry          = cms.untracked.bool(True),
    )

# -- Path
process.p = cms.Path(
    #process.dump*
   # process.qie10Digis*
    process.hcalDigis*
   # process.qie11Digis*
    process.zerobiasfilter*
    process.lumi
    )


outFile = 'cristina_test_out.root' # 'pcc_Data_PixVtx_Event_90X.root'
process.TFileService = cms.Service("TFileService",fileName = cms.string(outFile)) 
readFiles = cms.untracked.vstring() 
secFiles = cms.untracked.vstring() 
#inComms = cms.untracked.vstring("keep *", "drop l1tEMTFHitExtras_simEmtfDigis_CSC_HLT", "drop l1tEMTFHitExtras_simEmtfDigis_RPC_HLT", "drop l1tEMTFTrackExtras_simEmtfDigis__HLT") 
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles) 
#process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles, inputCommands = inComms) 
pu = 80
njobs = 14
readFiles.extend([
#'/store/mc/PhaseIIFall16DR82/SingleNeutrino/GEN-SIM-RECO/FlatPU0to75RECO_90X_upgrade2023_realistic_v1-v1/70000/00B7D98A-7D64-E711-BD8E-0CC47A745282.root',
#'/store/mc/RunIISummer16DR80/SingleNeutrino/GEN-SIM-RAW/FlatPU0to75TuneCP0_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/20000/0031D115-0909-E811-8E5B-0025905D1E0A.root', #crab file
#"/store/relval/CMSSW_12_1_0_pre5/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU_121X_mcRun3_2021_realistic_v15-v2/2580000/3de7f4f4-fe54-46a4-a5c2-381431d52c2a.root", #RelVal file
#'/store/mc/PhaseIISpring17D/SingleNeutrino/GEN-SIM-DIGI-RAW/PU140_90X_upgrade2023_realistic_v9-v1/70000/002BE8C6-8E26-E711-B023-0242AC130009.root',
#good one below!!
#'/store/mc/RunIISummer19UL17DIGI/SingleNeutrino/GEN-SIM-DIGI-RAW/FEVTDEBUG_106X_mc2017_realistic_v6-v2/210000/000B8543-10B5-8740-A83E-CC50681297CE.root',
#'/store/mc/RunIIFall18GS/MinBias_TuneCP5_13TeV-pythia8/GEN-SIM/102X_upgrade2018_realistic_v11-v2/2610000/8315AD7E-F603-FB40-A88F-25DEC9F484CF.root',
#test with only 5 evts below!
#'file:/afs/cern.ch/work/b/bgreenbe/private/lumi/CMSSW_10_2_13/src/RecoLuminosity/LumiProducer/test/analysis/test/BPH-RunIIAutumn18DR-00023_0.root',
#test with only 100 evts below!
#'file:/eos/user/b/bgreenbe/lumi/BPH-RunIIAutumn18DR-Flat50_0.root',
#'file:/eos/user/b/bgreenbe/lumi/BPH-RunIIAutumn18DR-Poisson80_0.root',
######this is the good one!!!!!!!!!
#'file:/eos/user/b/bgreenbe/lumi/BPH-RunIIAutumn18DR-Poisson%d_%d.root'%(pu,i) for i in range(1,njobs+1)
'file:/afs/cern.ch/user/c/cbarrera/public/ForBennett/singleneu_gun_forHFtest_step1.root'
#'file:/eos/user/b/bgreenbe/singleNeutrinoSample.root',
#'file:/eos/user/b/bgreenbe/singleNeutrino_RAW.root', #interactive file
# Min Bias 90X files with 2023D4 geometry and timing. no pu.
# '/store/relval/CMSSW_9_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/90X_upgrade2023_realistic_v1_2023D4Timing-v1/10000/28088B65-66C2-E611-BF89-0CC47A7C347A.root',
# '/store/relval/CMSSW_9_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/90X_upgrade2023_realistic_v1_2023D4Timing-v1/10000/20D68D58-3CC2-E611-B15B-0CC47A4C8F18.root',
# '/store/relval/CMSSW_9_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/90X_upgrade2023_realistic_v1_2023D4Timing-v1/10000/94242929-30C3-E611-B3E0-0025905B85DC.root',
# '/store/relval/CMSSW_9_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/90X_upgrade2023_realistic_v1_2023D4Timing-v1/10000/F46D98E7-EAC2-E611-936E-0CC47A7452D0.root',
# '/store/relval/CMSSW_9_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/90X_upgrade2023_realistic_v1_2023D4Timing-v1/10000/36D3D8CD-3BC2-E611-908A-0025905A6088.root',
#'/store/mc/RunIISummer16DR80/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM-RECO/NoPU_RECO_80X_mcRun2_asymptotic_v14-v1/100000/00150044-D075-E611-AAE8-001E67505A2D.root', # 80X file
#'/store/data/Run2015A/ZeroBias1/RECO/PromptReco-v1/000/250/786/00000/B4CDEBBC-F52A-E511-808D-02163E011CE8.root', 
])
