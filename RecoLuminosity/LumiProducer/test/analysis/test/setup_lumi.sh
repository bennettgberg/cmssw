#!/bin/bash
#usage: bash setup_lumi.sh N #where N is the number of pileup interactions

#pileup
pu=$1

#name for output file
testnum=0

#will need 100k collisions total eventually
ncoll=420000
#nevents is ncollisions / pileup
nevents=$(bc <<< "$ncoll / $pu")
#nevents=$((ncoll / pu))

echo "Should ask for $nevents events."
        
#get the python cfg file ready
#cmsDriver.py  --python_filename BPH-RunIIAutumn18DR-Poisson${pu}_cfg.py --eventcontent FEVTDEBUGHLT --pileup "AVE_25_BX_25ns,{'N': ${pu}}" --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI-RAW --fileout file:BPH-RunIIAutumn18DR-Poisson${pu}_${testnum}.root --pileup_input "dbs:/MinBias_TuneCP5_13TeV-pythia8/RunIIFall18GS-102X_upgrade2018_realistic_v9-v1/GEN-SIM" --conditions 102X_upgrade2018_realistic_v15 --step DIGI,L1,DIGI2RAW,HLT:@relval2018 --geometry DB:Extended --filein "dbs:/MinBias_TuneCP5_13TeV-pythia8/RunIIFall18GS-102X_upgrade2018_realistic_v11-v2/GEN-SIM" --era Run2_2018 --no_exec --mc -n $nevents
cmsDriver.py  --python_filename BPH-RunIIAutumn18DR-Poisson${pu}_cfg.py --eventcontent FEVTDEBUGHLT --pileup "AVE_25_BX_25ns,{'N': ${pu}}" --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI-RAW --fileout file:BPH-RunIIAutumn18DR-Poisson${pu}.root --pileup_input "dbs:/MinBias_TuneCP5_13TeV-pythia8/RunIIFall18GS-102X_upgrade2018_realistic_v9-v1/GEN-SIM" --conditions 102X_upgrade2018_realistic_v15 --step DIGI,L1,DIGI2RAW,HLT:@relval2018 --geometry DB:Extended --filein "dbs:/MinBias_TuneCP5_13TeV-pythia8/RunIIFall18GS-102X_upgrade2018_realistic_v11-v2/GEN-SIM" --era Run2_2018 --no_exec --mc #-n $nevents

#now make the executable file for the condor job
sed "s/BPH-RunIIAutumn18DR-Poisson1_cfg.py/BPH-RunIIAutumn18DR-Poisson${pu}_cfg.py/" drive_N1_0.sh > drive_N${pu}_0.sh
#
##now make the submit file for the condor job
subfile=sub_drive_N${pu}.sub
sed "s/= 0/= ${pu}/" sub_drive.sub > $subfile
#
echo "pileup is $pu."
echo "$subfile is ready to be submitted."
