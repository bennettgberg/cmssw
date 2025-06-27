#include <fstream>

#include "GeneratorInterface/Core/interface/GeneratorFilter.h"
#include "GeneratorInterface/ExternalDecays/interface/ExternalDecayDriver.h"

#include "GeneratorInterface/Pythia8Interface/interface/Py8GunBase.h"

namespace gen {

class Py8PlutoReaderGun : public Py8GunBase {
   
   public:
      
      Py8PlutoReaderGun( edm::ParameterSet const& );
      ~Py8PlutoReaderGun() override {}

      bool generatePartonsAndHadronize() override;
      const char* classname() const override;
	 
   private:
      
      // PtGun particle(s) characteristics
      double  fMinEta;
      double  fMaxEta;
      double  fMinPt ;
      double  fMaxPt ;
      double  fMinProdRadius;
      double  fMaxProdRadius;
      bool    fMakeDisplaced;
      int     fNumDaughters;
      std::string fFilename;
      double fMinDaughterPt = 0.0; //just set the value here instead of figuring out how to do a default value, and all that...

      std::vector<float> all_ee, all_px, all_py, all_pz;
      std::vector<int> used_events;

};

// implementation 
//
Py8PlutoReaderGun::Py8PlutoReaderGun( edm::ParameterSet const& ps )
   : Py8GunBase(ps), used_events({}) {

   //edm::ParameterSet defpset ;
   edm::ParameterSet pgun_params = 
      ps.getParameter<edm::ParameterSet>("PGunParameters"); // , defpset ) ;
   fFilename = pgun_params.getParameter<std::string>("Filename");
   fMinEta     = pgun_params.getParameter<double>("MinEta"); // ,-2.2);
   fMaxEta     = pgun_params.getParameter<double>("MaxEta"); // , 2.2);
   fMinPt      = pgun_params.getParameter<double>("MinPt"); // ,  0.);
   fMaxPt      = pgun_params.getParameter<double>("MaxPt"); // ,  0.);
   fMinProdRadius = pgun_params.getParameter<double>("MinProdRadius"); // , 0.);
   fMaxProdRadius = pgun_params.getParameter<double>("MaxProdRadius"); // , 0.);
   fMakeDisplaced = pgun_params.getParameter<bool>("MakeDisplaced"); //, true);
   fNumDaughters = pgun_params.getParameter<int>("NumDaughters"); // 4
   //fMinDaughterPt = pgun_params.getParameter<double>("MinDaughterPt"); //, 0.);

   std::cout << "[Py8PlutoReaderGun constructor] Begin reading Pluto input file..." << std::endl;
   std::ifstream infile(fFilename);
   float ee, px, py, pz, dummy1, dummy2, dummy3, dummy4;
   while (infile >> ee >> px >> py >> pz >> dummy1 >> dummy2 >> dummy3 >> dummy4) {
      double pt = sqrt(px*px + py*py);
      if(pt < fMinDaughterPt) {
          //std::cout << "erm... what the sigma?? pt=" << pt << ", fMinDaughterPt=" << fMinDaughterPt << std::endl;
          continue;
      }
      all_ee.push_back(ee);
      all_px.push_back(px);
      all_py.push_back(py);
      all_pz.push_back(pz);
   }
   infile.close();
   std::cout << "[Py8PlutoReaderGun constructor] Finished reading Pluto input file!" << std::endl;
}


bool Py8PlutoReaderGun::generatePartonsAndHadronize()
{

   fMasterGen->event.reset();

   // compute common radius (only used if fMakeDisplaced set to True in config)
   double radius = (fMaxProdRadius-fMinProdRadius) * randomEngine().flat() + fMinProdRadius;
   double phi_prod = (fMaxPhi-fMinPhi) * randomEngine().flat() + fMinPhi;
   double vx = radius * cos(phi_prod);
   double vy = radius * sin(phi_prod);
   double vz = (70 - (-70)) * randomEngine().flat() + (-70); // luminous region in Z: (-70, 70) mm

   // ensure event is unique within single node (accept repetition after 100 times though)
   // note: this of course does not apply for batch production, where probability of repetition exists
   // (this is minimized by randomly sampling pluto list of events -- birthday problem)
   int randomNumber, count = 0;
   do {
      randomNumber = (int)(100000 * randomEngine().flat()) * fNumDaughters;
      count++;
   }
   while (std::find(used_events.begin(), used_events.end(), randomNumber) != used_events.end() && count < 100);
   used_events.push_back(randomNumber);

   std::cout << "Retrieving Pluto random event number " << randomNumber/fNumDaughters << "..." << std::endl;

   // Get the 2--4 muons/electrons four-momenta   
   for (int i = 0; i < fNumDaughters; i++) {

      float ee, px, py, pz;
      ee = all_ee.at(randomNumber), px = all_px.at(randomNumber), py = all_py.at(randomNumber), pz = all_pz.at(randomNumber);
      // std::cout << "Just read: " << ee << " " << px << " " << py << " " << pz << std::endl;

      int particleID;
      if (fPartIDs.size() > 1) { // 2mu2e
         particleID = (i > fNumDaughters/2 - 1 ? 11 : 13); 
      }
      else { // 4mu or 2mu
         particleID = 13;
      }
      particleID = (i % 2 == 0 ? particleID : -particleID);
      
      double pt = sqrt(px*px + py*py);
      double pp = sqrt(px*px + py*py + pz*pz);

      double phi = atan2(py, px);
      double the = asin(pt/pp);
      double eta = -log(tan(the/2));

      double mass = (fMasterGen->particleData).m0(particleID);

      if (!((fMasterGen->particleData).isParticle(particleID))) {
         particleID = std::abs(particleID) ;
      }
      if (1 <= std::abs(particleID) && std::abs(particleID) <= 6) // quarks
          (fMasterGen->event).append(particleID, 23, 101, 0, px, py, pz, ee, mass); 
      else if (std::abs(particleID) == 21)                   // gluons
          (fMasterGen->event).append(21, 23, 101, 102, px, py, pz, ee, mass);
      else { // other
        (fMasterGen->event).append(particleID, 1, 0, 0, px, py, pz, ee, mass); 
        // -log(flat) = exponential distribution
        double tauTmp = -(fMasterGen->event).back().tau0() * log(randomEngine().flat());
        (fMasterGen->event).back().tau(tauTmp);
      }
      if (fMakeDisplaced) { // Make production vertex displaced from IP
          (fMasterGen->event).back().xProd(vx);
          (fMasterGen->event).back().yProd(vy);
          (fMasterGen->event).back().zProd(vz);
      }

      randomNumber++;
   }

   //now the eta meson boost (from input params)
   //double m0 = 0.547; //eta meson
   double m0 = 0.9578; //eta prime meson
   double pT = (fMaxPt-fMinPt) * randomEngine().flat() + fMinPt;
   double phi = (fMaxPhi-fMinPhi) * randomEngine().flat() + fMinPhi;
   double eta  = (fMaxEta-fMinEta) * randomEngine().flat() + fMinEta;
   double pp = pT * cosh(eta);
   double ee = sqrt(m0 * m0 + pp * pp);

   //the boost direction (from input params)
   double the = 2. * atan(exp(-eta));
   double betaX = pp / ee * std::sin(the) * std::cos(phi);
   double betaY = pp / ee * std::sin(the) * std::sin(phi);
   double betaZ = pp / ee * std::cos(the);
   //std::cout << "generated pT: " << pT << std::endl;
   // boost all particles
   (fMasterGen->event).bst(betaX, betaY, betaZ);
   
   if ( !fMasterGen->next() ) return false;
   evtGenDecay();
   
   event().reset(new HepMC::GenEvent);
   return toHepMC.fill_next_event( fMasterGen->event, event().get() );
  
}

const char* Py8PlutoReaderGun::classname() const
{
   return "Py8PlutoReaderGun"; 
}

typedef edm::GeneratorFilter<gen::Py8PlutoReaderGun, gen::ExternalDecayDriver> Pythia8PlutoReaderGun;

} // end namespace

using gen::Pythia8PlutoReaderGun;
DEFINE_FWK_MODULE(Pythia8PlutoReaderGun);
