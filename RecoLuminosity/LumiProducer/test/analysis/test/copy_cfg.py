import FWCore.ParameterSet.Config as cms

#copy some events from a a file on eos to a local file!

# Give the process a name
process = cms.Process("PickEvent")

# Tell the process which files to use as the source
process.source = cms.Source ("PoolSource",
          #fileNames = cms.untracked.vstring ("/store/mc/PhaseIIFall16DR82/SingleNeutrino/GEN-SIM-RECO/FlatPU0to75RECO_90X_upgrade2023_realistic_v1-v1/70000/00B7D98A-7D64-E711-BD8E-0CC47A745282.root")
          #fileNames = cms.untracked.vstring ("/store/mc/RunIISummer16DR80/SingleNeutrino/GEN-SIM-RAW/FlatPU0to75TuneCP0_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/20000/0031D115-0909-E811-8E5B-0025905D1E0A.root")
          #fileNames = cms.untracked.vstring ("/store/mc/RunIISummer16DR80/SingleNeutrinoGun/GEN-SIM-DIGI-RAW/PUPoissonAve32_80X_mcRun2_asymptotic_2016_TrancheIV_v8-v1/70000/00718993-7656-E911-9D78-0025905B85AE.root")
          fileNames = cms.untracked.vstring ("/store/relval/CMSSW_12_1_0_pre5/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU_121X_mcRun3_2021_realistic_v15-v2/2580000/3de7f4f4-fe54-46a4-a5c2-381431d52c2a.root" )
)

# tell the process to only run over 100 events (-1 would mean run over
#  everything
process.maxEvents = cms.untracked.PSet(
            input = cms.untracked.int32 (100)

)

# Tell the process what filename to use to save the output
process.Out = cms.OutputModule("PoolOutputModule",
         #fileName = cms.untracked.string ("/eos/user/b/bgreenbe/singleNeutrino_RECO.root")
         #fileName = cms.untracked.string ("/eos/user/b/bgreenbe/singleNeutrino_DIGI.root")
         fileName = cms.untracked.string ("/eos/user/b/bgreenbe/RelValTTbar_GSDIGIR.root")
)

# make sure everything is hooked up
process.end = cms.EndPath(process.Out)
