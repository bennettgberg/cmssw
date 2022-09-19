// ----------------------------------------------------------------------
// PCCNTupler
// ---------

#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

#include "PCCNTupler.h"

#include "CondFormats/Alignment/interface/Definitions.h"
#include "CondFormats/RunInfo/interface/RunInfo.h"

//#include "DataFormats/SiPixelDetId/interface/PixelBarrelName.h"
#include "DataFormats/TrackerCommon/interface/PixelBarrelName.h"
//#include "DataFormats/SiPixelDetId/interface/PixelEndcapName.h"
#include "DataFormats/TrackerCommon/interface/PixelEndcapName.h"
//#include "DataFormats/TrackerCommon/interface/PixelBarrelName.h"
//#include "DataFormats/TrackerCommon/interface/PixelEndcapName.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"


#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"

#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

//added bc of jingyu's code
#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalGenericDetId.h"
#include "CalibFormats/HcalObjects/interface/HcalCalibrations.h"

#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/CaloRecHitAuxSetter.h"
#include "DataFormats/HcalRecHit/test/HcalRecHitDump.cc"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
DEFINE_FWK_MODULE(PCCNTupler);
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalChannelPropertiesRecord.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>

using namespace std;
using namespace edm;
using namespace reco;

// ----------------------------------------------------------------------
PCCNTupler::PCCNTupler(edm::ParameterSet const& iConfig): 
    fPrimaryVertexCollectionLabel(iConfig.getUntrackedParameter<InputTag>("vertexCollLabel", edm::InputTag("offlinePrimaryVertices"))), 
    fPixelClusterLabel(iConfig.getUntrackedParameter<InputTag>("pixelClusterLabel", edm::InputTag("siPixelClusters"))), 
    fPileUpInfoLabel(edm::InputTag("addPileupInfo")),
    saveType(iConfig.getUntrackedParameter<string>("saveType")),
    sampleType(iConfig.getUntrackedParameter<string>("sampleType")),
    includeVertexInformation(iConfig.getUntrackedParameter<bool>("includeVertexInformation",0)), //bpg changed to 0
    includePixels(iConfig.getUntrackedParameter<bool>("includePixels",0)), //bpg changed to 0
    //includeJets(iConfig.getUntrackedParameter<bool>("includeJets",0)),
    includeJets(iConfig.getUntrackedParameter<bool>("includeJets",0)), //bpg wants to NOT includeJets now!
    includeHF(iConfig.getUntrackedParameter<bool>("includeHF",1)), //bpg added
    splitByBX(iConfig.getUntrackedParameter<bool>("splitByBX",1)),
    pixelPhase2Geometry(iConfig.getUntrackedParameter<bool>("pixelPhase2Geometry",0))
{
    cout << "----------------------------------------------------------------------" << endl;
    cout << "--- PCCNTupler constructor" << endl;

    edm::Service<TFileService> fs;


    tree = fs->make<TTree>("tree","Pixel Cluster Counters");
    tree->Branch("run",&run,"run/I");
    tree->Branch("LS",&LS,"LS/I");
    tree->Branch("LN",&LN,"LN/I");
    tree->Branch("timeStamp_begin",&timeStamp_begin,"timeStamp_begin/i");
    tree->Branch("timeStamp_end",&timeStamp_end,"timeStamp_end/i");
    tree->Branch("eventCounter",&eventCounter,"eventCounter/I");
    tree->Branch("BXNo","map<int,int>",&BXNo);
    if (saveType=="Event"){
        tree->Branch("event",&event,"event/i");
        tree->Branch("orbit",&orbit,"orbit/I");
        tree->Branch("bunchCrossing",&bunchCrossing,"bunchCrossing/I");
    }

    pileup = fs->make<TH1F>("pileup","pileup",100,0,100);
    if(includeVertexInformation){
        tree->Branch("nGoodVtx","map<int,int>",&nGoodVtx); 
        tree->Branch("nValidVtx","map<int,int>",&nValidVtx); 
        recoVtxToken=consumes<reco::VertexCollection>(fPrimaryVertexCollectionLabel);
        if (saveType=="Event"){
            tree->Branch("nVtx",&nVtx,"nVtx/I");
            tree->Branch("vtx_nTrk",&vtx_nTrk,"vtx_nTrk[nVtx]/I");
            tree->Branch("vtx_ndof",&vtx_ndof,"vtx_ndof[nVtx]/I");
            tree->Branch("vtx_x",&vtx_x,"vtx_x[nVtx]/F");
            tree->Branch("vtx_y",&vtx_y,"vtx_y[nVtx]/F");
            tree->Branch("vtx_z",&vtx_z,"vtx_z[nVtx]/F");
            tree->Branch("vtx_xError",&vtx_xError,"vtx_xError[nVtx]/F");
            tree->Branch("vtx_yError",&vtx_yError,"vtx_yError[nVtx]/F");
            tree->Branch("vtx_zError",&vtx_zError,"vtx_zError[nVtx]/F");
            tree->Branch("vtx_chi2",&vtx_chi2,"vtx_chi2[nVtx]/F");
            tree->Branch("vtx_normchi2",&vtx_normchi2,"vtx_normchi2[nVtx]/F");
            tree->Branch("vtx_isValid",&vtx_isValid,"vtx_isValid[nVtx]/O");
            tree->Branch("vtx_isFake",&vtx_isFake,"vtx_isFake[nVtx]/O");
            tree->Branch("vtx_isGood",&vtx_isGood,"vtx_isGood[nVtx]/O");
        }

    }

    if(includePixels){
        tree->Branch("nPixelClusters","map<std::pair<int,int>,int>",&nPixelClusters);
        tree->Branch("nClusters",     "map<std::pair<int,int>,int>",&nClusters);
        //tree->Branch("nPixelClusters","map<int,int>",&nPixelClusters);
        //tree->Branch("nClusters","map<int,int>",&nClusters);
        tree->Branch("layers","map<int,int>",&layers);
        //bpg adding
        tree->Branch("nClusTot", &nClusTot, "nClusTot/I");
        pixelToken=consumes<edmNew::DetSetVector<SiPixelCluster> >(fPixelClusterLabel);
    }

    if(sampleType=="MC"){
        pileUpToken=consumes<std::vector< PileupSummaryInfo> >(fPileUpInfoLabel);
        tree->Branch("nPU",&nPU,"nPU/I");
    }

    if(includeJets){
        hltjetsToken_=consumes<reco::CaloJetCollection >(edm::InputTag("ak4CaloJets"));
        const int kMaxJetCal = 100;
        jhcalpt = new float[kMaxJetCal];
        jhcalphi = new float[kMaxJetCal];
        jhcaleta = new float[kMaxJetCal];
        jhcale = new float[kMaxJetCal];
        jhcalemf = new float[kMaxJetCal]; 
        jhcaln90 = new float[kMaxJetCal]; 
        jhcaln90hits = new float[kMaxJetCal];

        //generate the dictionary thing to avoid the stupid root error
        //gInterpreter->GenerateDictionary("std::map<std::pair<int,int>,float>");
        //ccla HLTJETS
        tree->Branch("NohJetCal",&nhjetcal,"NohJetCal/I");
        tree->Branch("ohJetCalPt",jhcalpt,"ohJetCalPt[NohJetCal]/F");
        tree->Branch("ohJetCalPhi",jhcalphi,"ohJetCalPhi[NohJetCal]/F");
        tree->Branch("ohJetCalEta",jhcaleta,"ohJetCalEta[NohJetCal]/F");
        tree->Branch("ohJetCalE",jhcale,"ohJetCalE[NohJetCal]/F");
        tree->Branch("ohJetCalEMF",jhcalemf,"ohJetCalEMF[NohJetCal]/F");
        tree->Branch("ohJetCalN90",jhcaln90,"ohJetCalN90[NohJetCal]/F");
        tree->Branch("ohJetCalN90hits",jhcaln90hits,"ohJetCalN90hits[NohJetCal]/F");
       // tree->Branch("hcalTotE","map<std::pair<int,int>,float>",&hcalTotE);
    }

    //bpg adding this whole section.
    if(includeHF) {
        //hfToken = consumes< edm::SortedCollection<HFRecHit> >(edm::InputTag("hfInput")); //?????
        //hfToken = consumes< edm::SortedCollection<HFRecHit> >(edm::InputTag("hfrechits")); //?????
        //hfToken = consumes< edm::SortedCollection<HFRecHit> >(edm::InputTag("hfreco")); //?????
        
        //hfToken = consumes< edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit> > >(edm::InputTag("hfreco")); //?????
        //hfToken = consumes< edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit> > >(edm::InputTag("hcalDigis")); //?????
        //hfToken = consumes< edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit> > >(edm::InputTag("reducedHcalRecHits")); //?????
        //hfToken = consumes< edm::SortedCollection<HFPreRecHit,edm::StrictWeakOrdering<HFPreRecHit> > >(edm::InputTag("hfprereco")); //?????
        //reco file needed for this one
        //qie10digisToken_ = consumes<HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("simHcalUnsuppressedDigis")); 
        //????
        //qie10digisToken_ = consumes< QIE10DigiCollection >(edm::InputTag("hcalDigis")); //simHcalUnsuppressedDigis")); 
        qie10digisToken_ = consumes< HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("hcalDigis")); //simHcalUnsuppressedDigis")); //valid!
        //qie10digisToken_ = consumes< HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("simHcalUnsuppressedDigis")); 
        //qie10digisToken_ = consumes<HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("HFQIE10DigiCollection")); 
        //??
        //qie10digisToken_ = consumes< QIE10DigiCollection >(edm::InputTag("simHcalUnsuppressedDigis")); 
       // qie10digisToken_ = consumes< QIE10DigiCollection >(edm::InputTag("HFQIE10DigiCollection")); 
        othertoken = consumes<edm::SortedCollection<HBHEDataFrame,edm::StrictWeakOrdering<HBHEDataFrame> >>(edm::InputTag("simHcalUnsuppressedDigis"));
        hcalDbServiceToken_ = esConsumes<HcalDbService, HcalDbRecord>();
        const int kMaxHFCal = 10000;
        hfcalphi = new float[kMaxHFCal];
        hfcaleta = new float[kMaxHFCal];
        subdet  = new int[kMaxHFCal];
        depth   = new int[kMaxHFCal];
        rawId   = new int[kMaxHFCal];
        linkEr  = new int[kMaxHFCal];
        flags   = new int[kMaxHFCal];
        //hfcale = new int[kMaxHFCal];
        soi = new int*[kMaxHFCal];
        ok  = new int*[kMaxHFCal];
        adc = new int*[kMaxHFCal];
        le_tdc = new int*[kMaxHFCal];
        te_tdc = new int*[kMaxHFCal];
        capid = new int*[kMaxHFCal];
        fC = new double*[kMaxHFCal];
        for(int ctr=0; ctr<kMaxHFCal; ctr++) { 
            soi[ctr] = new int[3];
            ok    [ctr] = new int[3];
            adc   [ctr] = new int[3];
            le_tdc[ctr] = new int[3];
            te_tdc[ctr] = new int[3];
            capid [ctr] = new int[3];
            //trying to get this from adc2fc
            fC  [ctr]   = new double[3];
        }
        
        //tree->Branch("Nohf", &nhf, "Nohf/I");
        tree->Branch("Noqie", &nqieval, "Noqie/I");
        //tree->Branch("hfcaleta", hfcaleta, "hfcaleta[Nohf]/F");
        //tree->Branch("hfPhi", hfcalphi, "hfcalphi[Nohf]/F");
        tree->Branch("hfcaleta", hfcaleta, "hfcaleta[Noqie]/F");
        tree->Branch("hfPhi", hfcalphi,  "hfcalphi[Noqie]/F");
        tree->Branch("subdet", subdet,  "subdet[Noqie]/I");
        tree->Branch("depth", depth,  "depth[Noqie]/I");
        tree->Branch("rawId", rawId,  "rawId[Noqie]/I");
        tree->Branch("linkEr", linkEr,  "linkEr[Noqie]/I");
        tree->Branch("flags", flags,  "flags[Noqie]/I");
        tree->Branch( "soi"   , soi   , "soi[Noqie][3]/I"       );
        tree->Branch( "ok"    , ok    , "ok[Noqie][3]/I"        );
        tree->Branch( "adc"   , adc   , "adc[Noqie][3]/I"       );
        tree->Branch( "le_tdc", le_tdc, "le_tdc[Noqie][3]/I"    );
        tree->Branch( "te_tdc", te_tdc, "te_tdc[Noqie][3]/I"    );
        tree->Branch( "capid" , capid , "capid[Noqie][3]/I"     );
        //will this work??
        tree->Branch( "fC" , fC , "fC[Noqie][3]/D"     );
        tree->Branch( "etsum" , &etsum , "etsum/D"     );
       // tree->Branch("hfcale", hfcale, "hfcale[Nohf]/F");
                      

    }
}

// ----------------------------------------------------------------------
PCCNTupler::~PCCNTupler() { 
}  

// ----------------------------------------------------------------------
void PCCNTupler::endJob() { 
    cout << "==>PCCNTupler> Succesfully gracefully ended job" << endl;
}

// ----------------------------------------------------------------------
void PCCNTupler::beginJob() {

}


void PCCNTupler::beginLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& isetup){
    firstEvent = true;
    Reset();
}

void PCCNTupler::endLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& isetup){
    tree->Fill();
}


// ----------------------------------------------------------------------
void PCCNTupler::analyze(const edm::Event& iEvent, 
            const edm::EventSetup& iSetup)  {

    using namespace edm;
    using reco::VertexCollection;

    eventCounter++;

    saveAndReset=false;
    sameEvent = (event==(int)iEvent.id().event());
    sameLumiNib = true; // FIXME where is this info?
    sameLumiSect = (LS==(int)iEvent.getLuminosityBlock().luminosityBlock());
    
    // When arriving at the new LS, LN or event the tree 
    // must be filled and branches must be reset.
    // The final entry is saved in the deconstructor.
    saveAndReset = (saveType=="LumiSect" && !sameLumiSect)
                || (saveType=="LumiNib" && !sameLumiNib)
                || (saveType=="Event" && !sameEvent);


    if(   !saveAndReset && !sameLumiSect
       && !sameLumiNib  && !sameEvent) {
        std::cout<<"Diff LS, LN and Event, but not saving/resetting..."<<std::endl;
    }

    if(saveAndReset){
        SaveAndReset();
    }

    if(sampleType=="MC"){
        edm::Handle<std::vector< PileupSummaryInfo> > pileUpInfo;
        iEvent.getByToken(pileUpToken, pileUpInfo);
        std::vector<PileupSummaryInfo>::const_iterator PVI;
        for(PVI = pileUpInfo->begin(); PVI != pileUpInfo->end(); ++PVI) {
            int pu_bunchcrossing = PVI->getBunchCrossing();
            //std::cout<<"pu_bunchcrossing getPU_NumInteractions getTrueNumInteractions "<<pu_bunchcrossing<<" "<<PVI->getPU_NumInteractions()<<" "<<PVI->getTrueNumInteractions()<<std::endl;
            if(pu_bunchcrossing == 0) {
                nPU=PVI->getPU_NumInteractions();
                pileup->Fill(nPU);
            }
        }
    }

    // Get the Run, Lumi Section, and Event numbers, etc.
    run   = iEvent.id().run();
    LS    = iEvent.getLuminosityBlock().luminosityBlock();
    //LN    = -99; // FIXME need the luminibble
    event = iEvent.id().event();
    bunchCrossing   = iEvent.bunchCrossing();
    if(!splitByBX){ //if no splitting by BX then we can remove info.
        bunchCrossing=-10;
    }
    timeStamp_local = iEvent.time().unixTime();
    if(timeStamp_end  <timeStamp_local) timeStamp_end   =timeStamp_local;
    if(timeStamp_begin>timeStamp_local) timeStamp_begin =timeStamp_local;
    orbit = iEvent.orbitNumber();
    //LN    = ((int) (orbit/pow(2,12)) % 64);
    //int LN2    = iEvent.nibble;
    LN    = ((int) (orbit >> 12) % 64); // FIXME need the luminibble
   
    bxModKey.first=bunchCrossing;
    bxModKey.second=-1;
   
    if((BXNo.count(bunchCrossing)==0||nGoodVtx.count(bunchCrossing)==0) && !(BXNo.count(bunchCrossing)==0&&nGoodVtx.count(bunchCrossing)==0)){
        std::cout<<"BXNo and nGoodVtx should have the same keys but DO NOT!!!"<<std::endl;
    }
    
    if(BXNo.count(bunchCrossing)==0){
        BXNo[bunchCrossing]=0;
    }

    if(nGoodVtx.count(bunchCrossing)==0){
        nGoodVtx[bunchCrossing]=0;
        nValidVtx[bunchCrossing]=0;
    }

    BXNo[bunchCrossing]=BXNo[bunchCrossing]+1;
    // add the vertex information

    if(includeVertexInformation){
        edm::Handle<reco::VertexCollection> recVtxs;
        iEvent.getByToken(recoVtxToken,recVtxs);
        
   
        if(recVtxs.isValid()){
            //nVtx=recVtxs->size();
            int ivtx=0;
            for(reco::VertexCollection::const_iterator v=recVtxs->begin(); v!=recVtxs->end(); ++v){
                if(v->isFake()) continue;
                vtx_isGood[ivtx] = false;
                vtx_nTrk[ivtx] = v->tracksSize();
                vtx_ndof[ivtx] = (int)v->ndof();
                vtx_x[ivtx] = v->x();
                vtx_y[ivtx] = v->y();
                vtx_z[ivtx] = v->z();
                vtx_xError[ivtx] = v->xError();
                vtx_yError[ivtx] = v->yError();
                vtx_zError[ivtx] = v->zError();
                vtx_chi2[ivtx] = v->chi2();
                vtx_normchi2[ivtx] = v->normalizedChi2();
                vtx_isValid[ivtx] = v->isValid();
                vtx_isFake[ivtx] = v->isFake();
                if(vtx_isValid[ivtx] && (vtx_isFake[ivtx] == 0)){
                    nValidVtx[bunchCrossing]=nValidVtx[bunchCrossing]+1;
                }
                if(vtx_ndof[ivtx] > 4 && vtx_isValid[ivtx] && (vtx_isFake[ivtx] == 0)){
                    if(vtx_nTrk[ivtx] > 0){
                        nGoodVtx[bunchCrossing]=nGoodVtx[bunchCrossing]+1;
                        vtx_isGood[ivtx] = true;
                    }
                }
                ivtx++;
            }
            nVtx=ivtx;
        }
    }

    if(includeJets){
        edm::Handle< reco::CaloJetCollection > hltjets;
        iEvent.getByToken(hltjetsToken_, hltjets);
        bool valid = hltjets.isValid();
        if (not valid) {
            std::cout << "hltjets not valid "<<std::endl;
            nhjetcal = -1;
        } else {
            reco::CaloJetCollection mycalojets;
            mycalojets=*hltjets;
            //std::sort(mycalojets.begin(),mycalojets.end(),PtGreater());
            typedef reco::CaloJetCollection::const_iterator cjiter;
            int jhcal=0;
            for ( cjiter i=mycalojets.begin(); i!=mycalojets.end(); i++) {
                if (i->pt()>5 && i->energy()>0.){
                    jhcalpt[jhcal] = i->pt();
                    jhcalphi[jhcal] = i->phi();
                    jhcaleta[jhcal] = i->eta();
                    jhcale[jhcal] = i->energy();
                    jhcalemf[jhcal] = i->emEnergyFraction();
                    jhcaln90[jhcal] = i->n90();

                    //first get the values for ieta and iphi
                //    int ieta = (int) (i->eta() / 2.4 * 85); //ieta goes from -85 to 85 (no crystal at 0).
                 //   int iphi = (int) (i->phi() / 3.1415927 * 180) + 180; //iphi goes from 1 to 360
                    //make pair to use in the map
                  //  std::pair<int,int> etaphi;
                  //  etaphi.first = ieta;
                  //  etaphi.second = iphi;
                    //if this pair isn't in the map yet, create it.
                 //   if(hcalTotE.count(etaphi)==0){
                 //     hcalTotE[etaphi]=0;
                 //   }
                 //   //now add the energy to the previous total.
                 //   hcalTotE[etaphi] += i->energy();
                    //jetID->calculate( iEvent, *i );
                    //jhcaln90hits[jhcal] = jetID->n90Hits();
                    jhcal++;
                }

            }
            nhjetcal = jhcal;
        }
    }

    int NumPixelBarrelLayers=3;
    if(pixelPhase2Geometry){
      NumPixelBarrelLayers=4;
    }
    // -- Pixel cluster
    if(includePixels){
      edm::Handle< edmNew::DetSetVector<SiPixelCluster> > hClusterColl;
      iEvent.getByToken(pixelToken,hClusterColl);
      if (!hClusterColl.failedToGet()) {        
	
        
        const edmNew::DetSetVector<SiPixelCluster>& clustColl = *hClusterColl;
        // ----------------------------------------------------------------------
        // -- Clusters without tracks
          
        //bpg added
        int nClusCtr = 0;
	    for (edmNew::DetSetVector<SiPixelCluster>::const_iterator isearch = clustColl.begin();  isearch != clustColl.end(); ++isearch){
	      // these are sorted by modules so we pick the current one
	      edmNew::DetSet<SiPixelCluster>  mod = *isearch;
	      if(mod.empty()) { continue; }// skip empty modules
	      DetId detId = mod.id();
	      
	      bxModKey.second=detId();
	      for (edmNew::DetSet<SiPixelCluster>::const_iterator di = mod.begin(); di != mod.end(); ++di){
	        if(nPixelClusters.count(bxModKey)==0){
	          nPixelClusters[bxModKey]=0;
	        }
	        nPixelClusters[bxModKey] = nPixelClusters[bxModKey]+1;
            //bpg added
            nClusCtr += nPixelClusters[bxModKey];
	        
	        
	        int nCluster = isearch->size();
	        if(nClusters.count(bxModKey)==0){
	          nClusters[bxModKey]=0;
	        }
	        nClusters[bxModKey] += nCluster;
	        
	        if (detId.subdetId() == PixelSubdetector::PixelBarrel) {
	          PixelBarrelName detName = PixelBarrelName(detId);
	          int layer = detName.layerName();
	          if(layers.count(detId())==0){
                layers[detId()]=layer;
	          }
	        } 
            else {
	          assert(detId.subdetId() == PixelSubdetector::PixelEndcap);
	          PixelEndcapName detName = PixelEndcapName(detId);
	          int disk = detName.diskName();
	          if(layers.count(detId())==0){
                layers[detId()]=disk+NumPixelBarrelLayers; 
	          } //layers.count 0 
	        } //NOT	subdet == PixelBarel    
	      } //for di loop
	    } //for isearch loop
    
        //bpg added
        nClusTot = nClusCtr;

      } //if not failed to get hClusterColl
    } //if includePixels
    
    //bpg added this section
    if(includeHF) {
       // std::cout << "Including HF" << std::endl;

        nqieval = 0;
        etsum = 0.0;
        //nhf = 0;
        //edm::Handle<edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit> >> hfRecHits;
        //edm::Handle< edm::SortedCollection<HFPreRecHit,edm::StrictWeakOrdering<HFPreRecHit> > > hfRecHits;
        //????
        //edm::Handle< HcalDataFrameContainer<QIE10DataFrame> > qiehandle;
        //??
        //edm::Handle< QIE10DigiCollection > qiehandle;
        //????????
        edm::Handle< HcalDataFrameContainer<QIE10DataFrame> > qiehandle;
        edm::Handle< edm::SortedCollection<HBHEDataFrame,edm::StrictWeakOrdering<HBHEDataFrame> > > otherhandle;
        //std::cout << "hfRecHits declared." << std::endl;
        //edm::Handle<edm::SortedCollection<HFRecHit>> hfRecHits;
        //iEvent.getByToken(hfToken, hfRecHits);
        iEvent.getByToken(qie10digisToken_, qiehandle);
        iEvent.getByToken(othertoken, otherhandle);
        //std::cout << "hfRecHit got!" << std::endl;
        //bool valid = hfRecHits.isValid();
        bool valid = qiehandle.isValid();
        bool othervalid = otherhandle.isValid();
        if (not valid) {
         //   std::cout << "hfRecHits not valid. fffffff" <<std::endl;
            std::cout << "Not valid!!!!" << std::endl;
            std::cout << "othervalid: " << othervalid << std::endl;
            //nhf = -1;
            //if(hfRecHits.failedToGet()) {
            if(qiehandle.failedToGet()) {
                //in this case, an attempt to get the data was made.
                std::cout << "data not available!! " <<std::endl;
                //nhf = -2; 
                nqieval = -2; 
            } //end failed to get qie
            else {
                std::cout << "no attmept to get data was made!" << std::endl;
                //nhf = -5;
                nqieval = -5;
            } //end no attempt made block
        } //end not valid block
        else {
            //edm::SortedCollection<HFRecHit> myhfrechits;
            //std::cout << "Valid!!!!!!!!!!!!!!!" << std::endl;
           // edm::SortedCollection<HFRecHit, edm::StrictWeakOrdering<HFRecHit> > myhfrechits;
           // edm::SortedCollection<HFPreRecHit,edm::StrictWeakOrdering<HFPreRecHit> > myhfrechits;
            //????
            //HcalDataFrameContainer<QIE10DataFrame> myqie;
            //myhfrechits = *hfRecHits;
            //myqie = *qiehandle;
            //std::cout << "myhfrechits assigned!!!!" << std::endl;
            //std::sort(mycalojets.begin(),mycalojets.end(),PtGreater());
            //typedef edm::SortedCollection<HFRecHit>::const_iterator hfiter;
            //typedef edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit>>::const_iterator hfiter;
            //typedef edm::SortedCollection<HFPreRecHit,edm::StrictWeakOrdering<HFPreRecHit> >::const_iterator hfiter;
            //typedef HcalDataFrameContainer<QIE10DataFrame>::const_iterator qieiter;
            //std::cout << "qieiter declared." << std::endl;
            //int hfcal=0;
            /////////added///////
            //edm::ESGetToken<HcalDbService, HcalDbRecord> tok_dbService_;
            ////////////////////
//            edm::ESHandle<HcalDbService> conditions;
//            iSetup.get<HcalDbRecord>().get(conditions);

            conditions = &iSetup.getData(hcalDbServiceToken_);


            uint32_t othersize = otherhandle->size();
            uint32_t qiesize = qiehandle->size();
            std::cout << "qie size: " << qiesize << ", other size: " << othersize << std::endl;
            int nqie = 0;
            //for ( hfiter i=myhfrechits.begin(); i!=myhfrechits.end(); i++) {
            //for ( qieiter i=myqie.begin(); i!=myqie.end(); i++) {
            double hetsum = 0.0;
            for (uint32_t i=0; i<qiesize; i++){
                // From: https://github.com/awhitbeck/HFcommissioningAnalysis/blob/b3456c9fe66ef9bcc6c54773d60f768c269a5c74/src/HFanalyzer.cc#L429
                QIE10DataFrame qie10df = static_cast<QIE10DataFrame>((*qiehandle)[i]);
                //if (i->pt()>5 && i->energy()>0.){
                //if(hfcal %10 == 0) std::cout << "inside hfiter loop! hfcal = " << hfcal << std::endl;
                //if(nqie %10 == 0) std::cout << "inside loop! nqie = " << nqie << std::endl;
                //if (i->energy()>0.){
                //get detid
                DetId detid = qie10df.detid();
                HcalDetId hcdi = HcalDetId(detid);
                //std::cout << "got the detid." << std::endl;

                //trying to convert adc to fC
                const HcalQIECoder* channelCoder = conditions -> getHcalCoder(hcdi);
                const HcalQIEShape* shape = conditions -> getHcalShape(channelCoder);
                HcalCoderDb coder(*channelCoder,*shape);
                CaloSamples cs; coder.adc2fC(qie10df,cs);

                hfcalphi[nqie] = hcdi.iphi(); //i->iphi();
                hfcaleta[nqie] = hcdi.ieta(); //i->ieta();
                //subdetector is HF
                subdet[nqie] = 5;
                depth[nqie] = hcdi.depth();
                rawId[nqie] = hcdi.rawId();
                linkEr[nqie] = qie10df.linkError();
                flags[nqie] = qie10df.flags();

                int nTS = qie10df.samples();
              //std::cout << "nTS: " << nTS << std::endl;
              //get the samples
              //std::cout << "about to start the its loop. i=" << i << ", nTS=" << nTS << ",nqie=" << nqie << std::endl;
                for(int its=0; its<nTS; ++its)
                { 
                    auto sam = qie10df[its];
                    //get digital energy as a function of ieta and iphi.
                    //the below are all ints stored by the QIE10DataFrame
                    //std::cout << "will now store soi, ok, adc, etc. its=" << its << std::endl;
                    soi    [nqie][its] = sam.soi() ;
                    ok     [nqie][its] = sam.ok()  ;
                    adc    [nqie][its] = sam.adc()  ;
                    le_tdc [nqie][its] = sam.le_tdc() ;
                    te_tdc [nqie][its] = sam.te_tdc() ;
                    capid  [nqie][its] = sam.capid()  ;

                    //will this work??
                    fC     [nqie][its] = cs[its];
                    //only include long fibers in the etsum
                    if(depth[nqie] == 1) {
                        hetsum += fC[nqie][its]/nTS; 
                    }
                    //hfcal++;
                    //std::cout << "done with its loop iteration." << std::endl;
                } //its loop
                nqie++;
            } //end for i (nqie) loop
            //nhf = hfcal;
            //std::cout << "done with i loop over nqie." << std::endl;
            nqieval = nqie;
            etsum = hetsum;
        } //end valid True block
    } //includeHF
} //analyze function

void PCCNTupler::Reset() {
    nVtx = 0;
    nClusTot = 0;
    nPixelClusters.clear();
    nClusters.clear();
    layers.clear();
    BXNo.clear();
    nValidVtx.clear();
    nGoodVtx.clear();
    eventCounter=1;
    timeStamp_end=0;
    timeStamp_begin=-1;
}

void PCCNTupler::SaveAndReset() {
    if(!firstEvent) tree->Fill();
    Reset();
    firstEvent=false;
}
