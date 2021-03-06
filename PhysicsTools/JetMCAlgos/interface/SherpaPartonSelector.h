#ifndef PhysicsTools_JetMCAlgos_SherpaPartonSelector_H
#define PhysicsTools_JetMCAlgos_SherpaPartonSelector_H

/**\class SherpaPartonSelector SherpaPartonSelector.h PhysicsTools/JetMCAlgos/interface/SherpaPartonSelector.h
 * \brief Sherpa parton selector derived from the base parton selector
 */

#include "PhysicsTools/JetMCAlgos/interface/BasePartonSelector.h"


class SherpaPartonSelector : public BasePartonSelector
{
  public:
    SherpaPartonSelector();
    ~SherpaPartonSelector() override;

    void run(const edm::Handle<reco::GenParticleCollection> & particles,
             std::unique_ptr<reco::GenParticleRefVector> & partons) override;
};

#endif
