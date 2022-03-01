from CRABClient.UserUtilities import config
config = config()
#poisson avg of the pileup
pu = 80
#number of jobs is nevts / 1000 fsr
#  rounded up to the thousands tho
#nevts = (100000/1000)*1.0 / pu
njobs = 14 # nevts / 1000
#change this name for each crab job!
config.General.requestName = 'Poisson%d_420kcol_0'%pu

#subdirectory to send log files and junk to.
config.General.workArea = 'crab'

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'Run_PixVertex_Event.py'
config.JobType.allowUndistributedCMSSW = True

#config.Data.inputDataset = '/SingleNeutrino/PhaseIIFall16DR82-FlatPU0to75RECO_90X_upgrade2023_realistic_v1-v1/GEN-SIM-RECO'
#config.Data.inputDataset = '/SingleNeutrino/RunIISummer16DR80-FlatPU0to75TuneCP0_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW'
#config.Data.inputDataset = '/SingleNeutrinoGun/RunIISummer16DR80-PUPoissonAve32_80X_mcRun2_asymptotic_2016_TrancheIV_v8-v1/GEN-SIM-DIGI-RAW'
#config.Data.inputDataset = '/SingleNeutrino/PhaseIISpring17D-PU140_90X_upgrade2023_realistic_v9-v1/GEN-SIM-DIGI-RAW'
#config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL17DIGI-FEVTDEBUG_106X_mc2017_realistic_v6-v2/GEN-SIM-DIGI-RAW'
#config.Data.inputDataset = '/MinBias_TuneCP5_13TeV-pythia8/RunIIFall18GS-102X_upgrade2018_realistic_v11-v2/GEN-SIM'
#config.Data.userInputFiles = ['/eos/user/b/bgreenbe/lumi/BPH-RunIIAutumn18DR-Poisson1_0.root']
#config.Data.userInputFiles = ['root://eosuser.cern.ch//eos/user/b/bgreenbe/lumi/BPH-RunIIAutumn18DR-Poisson%d_0.root'%pu]
config.Data.userInputFiles = ['root://eosuser.cern.ch//eos/user/b/bgreenbe/lumi/BPH-RunIIAutumn18DR-Poisson%d_%d.root'%(pu,i) for i in range(1,njobs+1)]
#config.Data.lumiMask = ''
#config.Data.runRange = '254833'
#config.Data.ignoreLocality = True
config.Data.inputDBS = 'global'

config.Data.splitting = 'FileBased'
#config.Data.splitting = 'Automatic'

#config.Data.splitting = 'EventAwareLumiBased'
config.Data.publication = False
config.Data.unitsPerJob = 10
#config.Data.totalUnits = -1
#config.Data.publishDbsUrl = 'test'
#config.Data.publishDataName = 'PCC_AlCaLumiPixels_Run2015C_PIXONLY_LS_254833'
# config.Data.outLFNDirBase = '/store/group/comm_luminosity/PCC/ForLumiComputation'
config.Data.outLFNDirBase = '/store/user/bgreenbe/lumi/' #'/store/user/asterenb/ForBennett'

config.Site.storageSite = 'T3_US_FNALLPC'
#config.Site.whitelist=['T2_FR_CCIN2P3','T2_IT_Pisa','T2_UK_London_IC','T2_HU_Budapest']
