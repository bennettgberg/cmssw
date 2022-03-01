#!/bin/bash

export X509_USER_PROXY=$1
voms-proxy-info -all
echo "exported x509 proxy path $1"
voms-proxy-info -all -file $1

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_2_13/src ] ; then
  echo release CMSSW_10_2_13 already exists
else
  scram p CMSSW CMSSW_10_2_13
fi
cd CMSSW_10_2_13/src
eval `scram runtime -sh`

scram b
cd ../..

cmsRun BPH-RunIIAutumn18DR-Poisson1_cfg.py
echo "Done with cmsRun!!!!!!!!!!!!!"
