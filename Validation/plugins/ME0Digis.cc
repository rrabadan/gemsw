#include <memory>
#include <iostream>
#include <map>
#include <string>


// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "CondFormats/GEMObjects/interface/GEMeMap.h"
#include "Validation/MuonGEMHits/interface/GEMValidationUtils.h"

#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartition.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"
#include "Geometry/CommonTopologies/interface/GEMStripTopology.h"

#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
//
// class declaration
//

class ME0Digis : public DQMEDAnalyzer {
public:
  explicit ME0Digis(const edm::ParameterSet&);
  ~ME0Digis() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;

  void analyze(const edm::Event&, const edm::EventSetup&) override;

  // ------------ member data ------------
  edm::EDGetTokenT<GEMDigiCollection> gemDigis_;

  MEMap1Ids digi_occ_;
  MEMap3Ids digi_occ_detail_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ME0Digis::ME0Digis(const edm::ParameterSet& iConfig)
  //  : folder_(iConfig.getParameter<std::string>("folder")) 
{
  // now do what ever initialization is needed
  gemDigis_ = consumes<GEMDigiCollection>(iConfig.getParameter<edm::InputTag>("gemDigiLabel"));
}

ME0Digis::~ME0Digis() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called for each event  ------------
void ME0Digis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  // using namespace edm;
  
  edm::ESHandle<GEMGeometry> hGEMGeom;
  iSetup.get<MuonGeometryRecord>().get(hGEMGeom);
  const GEMGeometry* GEMGeometry_ = &*hGEMGeom;

  edm::Handle<GEMDigiCollection> gemDigis;
  iEvent.getByToken(gemDigis_,gemDigis);

  for (auto etaPart : GEMGeometry_->etaPartitions()) {
    auto etaPartId = etaPart->id();

    int station = etaPartId.station();
    int chamber = etaPartId.chamber();
    int ieta = etaPartId.ieta();

    ME3IdsKey key3{station, chamber, ieta};

    auto digiRange = gemDigis->get(etaPartId); 
    for (auto digi = digiRange.first; digi != digiRange.second; ++digi) {
      auto strip = digi->strip();
      
      digi_occ_[chamber]->Fill(strip, ieta);
      digi_occ_detail_[key3]->Fill(strip);
    }
  }
}

void ME0Digis::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& iSetup) {
  edm::ESHandle<GEMGeometry> hGEMGeom;
  iSetup.get<MuonGeometryRecord>().get(hGEMGeom);
  const GEMGeometry* GEMGeometry_ = &*hGEMGeom;
  
  ibook.cd();
  ibook.setCurrentFolder("ME0/Digis");

  for (auto station : GEMGeometry_->stations()) {
    int st = station->station();
    int nEta = st==2 ? 16 : 8;
    
    for (auto superChamber : station->superChambers()) {
      for (auto chamber : superChamber->chambers()) {
        int ch = chamber->id().chamber();
        
        digi_occ_[ch] = ibook.book2D(Form("digi_occ_GE%d_ch%d", st, ch),
                                     Form("Occupancy GE%d ch%d", st, ch),
                                     384, -0.5, 383.5,
                                     nEta, 0.5, nEta+0.5);


        for (auto etaPart : chamber->etaPartitions()) {
          int ieta = etaPart->id().ieta();
          
          ME3IdsKey key3{st, ch, ieta}; 
          digi_occ_detail_[key3] = ibook.book1D(Form("digi_occ_GE%d_ch%d_ieta%d", st, ch, ieta),
                                                Form("Occupancy GE%d chamber %d iEta%d", st, ch, ieta),
                                                384, -0.5, 383.5);
        }
      }
    }
  }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void ME0Digis::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("gemDigiLabel", edm::InputTag("muonGEMDigis", ""));
  descriptions.add("ME0Digis", desc);
}

// define this as a plug-in
DEFINE_FWK_MODULE(ME0Digis);
