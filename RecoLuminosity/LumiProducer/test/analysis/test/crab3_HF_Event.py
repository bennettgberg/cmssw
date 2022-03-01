from CRABClient.UserUtilities import config
config = config()
#from WMCore.Configuration import Configuration
#config = Configuration()
#pileup
pu = 100
#nevents needed is (number of collisions needed) / pu
nevt = 4000000 / pu
#change this name for each crab job!
#config.General.requestName = 'N0p5_test5'
config.General.requestName = 'N%d_test430'%pu

#subdirectory to send log files and junk to.
config.General.workArea = 'crab'

#keep stdout and stderr
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
#config.JobType.psetName = 'BPH-RunIIAutumn18DR-Poisson0p5_cfg.py'
config.JobType.psetName = 'BPH-RunIIAutumn18DR-Poisson%d_cfg.py'%pu
config.JobType.allowUndistributedCMSSW = True
#trying to get more memory
config.JobType.maxMemoryMB = 5000
#config.JobType.numCores = 8
#numberOfThreads must equal numCores apparently??
#config.JobType.options.numberOfThreads = 8

#config.Data.inputDataset = '/SingleNeutrino/PhaseIIFall16DR82-FlatPU0to75RECO_90X_upgrade2023_realistic_v1-v1/GEN-SIM-RECO'
#config.Data.inputDataset = '/SingleNeutrino/RunIISummer16DR80-FlatPU0to75TuneCP0_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/GEN-SIM-RAW'
#config.Data.inputDataset = '/SingleNeutrinoGun/RunIISummer16DR80-PUPoissonAve32_80X_mcRun2_asymptotic_2016_TrancheIV_v8-v1/GEN-SIM-DIGI-RAW'
#config.Data.inputDataset = '/SingleNeutrino/PhaseIISpring17D-PU140_90X_upgrade2023_realistic_v9-v1/GEN-SIM-DIGI-RAW'
#config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL17DIGI-FEVTDEBUG_106X_mc2017_realistic_v6-v2/GEN-SIM-DIGI-RAW'
config.Data.inputDataset = '/MinBias_TuneCP5_13TeV-pythia8/RunIIFall18GS-102X_upgrade2018_realistic_v11-v2/GEN-SIM'
#to allow not to run at the site where it's hosted
#config.Data.ignoreLocality = True
#config.Site.whitelist = ['T2_FR_IPHC']
#config.Data.lumiMask = ''
#config.Data.runRange = '254833'
#config.Data.ignoreLocality = True
config.Data.inputDBS = 'global'
#config.Data.splitting = 'Automatic' #'FileBased'
#config.Data.splitting = 'FileBased'
config.Data.splitting = 'EventAwareLumiBased'
config.Data.publication = False
#config.Data.unitsPerJob = 150
#seems like this is totally ignored for EventAwareLumiBased splitting??
config.Data.unitsPerJob = 1000
#config.Data.totalUnits = 10000
config.Data.totalUnits = nevt #4000
#config.Data.publishDbsUrl = 'test'
#config.Data.publishDataName = 'PCC_AlCaLumiPixels_Run2015C_PIXONLY_LS_254833'
# config.Data.outLFNDirBase = '/store/group/comm_luminosity/PCC/ForLumiComputation'
#config.Data.outLFNDirBase = '/store/user/bgreenbe/lumi/' #'/store/user/asterenb/ForBennett'
#config.Data.outLFNDirBase = '/eos/user/b/bgreenbe/lumi'
config.Data.outLFNDirBase = '/store/user/bgreenbe/lumi'
#config.Data.outLFNDirBase = '/eos/home-b/bgreenbe/lumi'

#config.Site.storageSite = 'T3_US_FNALLPC'
config.Site.storageSite = 'T3_CH_CERNBOX' #??
#config.Site.storageSite = 'T2_CH_CERNBOX' #??
#config.Site.whitelist=['T2_FR_CCIN2P3','T2_IT_Pisa','T2_UK_London_IC','T2_HU_Budapest']
#config.Site.blacklist=['T2_FR_GRIF_IRFU', 'T2_FR_GRIF_LLR']

