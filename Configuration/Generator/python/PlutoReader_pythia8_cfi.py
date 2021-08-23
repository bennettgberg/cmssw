import FWCore.ParameterSet.Config as cms

generator = cms.EDFilter("Pythia8PlutoReaderGun",

    maxEventsToPrint = cms.untracked.int32(1),
    pythiaPylistVerbosity = cms.untracked.int32(1),
    pythiaHepMCVerbosity = cms.untracked.bool(True),

    PGunParameters = cms.PSet(
        Filename = cms.string('GeneratorInterface/Pythia8Interface/test/pluto_EtaTo4Mu_100k_events.csv'),
        ParticleID = cms.vint32(13),
        MakeDisplaced = cms.bool(False),
        MinPhi = cms.double(-3.14159265359),
        MaxPhi = cms.double(3.14159265359),
        MinPt = cms.double(10.0),
        MaxPt = cms.double(100.0),
        MinEta = cms.double(0.0),
        MaxEta = cms.double(2.4),
        MinProdRadius = cms.double(0.0),
        MaxProdRadius = cms.double(10.0)
        ),

    PythiaParameters = cms.PSet(
        py8ZDecaySettings = cms.vstring(  '23:onMode = off', # turn OFF all Z decays
            '23:onIfAny = 15'  # turn ON Z->tautau
            ),
        parameterSets = cms.vstring(  
            #'py8ZDecaySettings' 
            )
        )
    )
