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

class TestBeamValidation : public DQMEDAnalyzer {
public:
  explicit TestBeamValidation(const edm::ParameterSet&);
  ~TestBeamValidation() override;

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
TestBeamValidation::TestBeamValidation(const edm::ParameterSet& iConfig)
  //  : folder_(iConfig.getParameter<std::string>("folder")) 
{
  // now do what ever initialization is needed
  gemDigis_ = consumes<GEMDigiCollection>(iConfig.getParameter<edm::InputTag>("gemDigiLabel"));
}

TestBeamValidation::~TestBeamValidation() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called for each event  ------------
void TestBeamValidation::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
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
      
      digi_occ_[station]->Fill(strip, ieta);
      digi_occ_detail_[key3]->Fill(strip);
    }
  }

  // example_->Fill(5);
  // example2D_->Fill(eventCount_ / 10, eventCount_ / 10);
  // example3D_->Fill(eventCount_ / 10, eventCount_ / 10, eventCount_ / 10.f);
  // exampleTProfile_->Fill(eventCount_ / 10, eventCount_ / 10.f);
  // exampleTProfile2D_->Fill(eventCount_ / 10, eventCount_ / 10, eventCount_ / 10.f);
}

void TestBeamValidation::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& iSetup) {
  edm::ESHandle<GEMGeometry> hGEMGeom;
  iSetup.get<MuonGeometryRecord>().get(hGEMGeom);
  const GEMGeometry* GEMGeometry_ = &*hGEMGeom;
  
  ibook.cd();
  ibook.setCurrentFolder("GEM/Digis");

  for (auto station : GEMGeometry_->stations()) {
    int st = station->station();
    int nEta = st==2 ? 16 : 8;
    
    digi_occ_[st] = ibook.book2D(Form("digi_occ_GE%d", st),
                                 Form("Occupancy GE%d", 2),
                                 384, -0.5, 383.5,
                                 nEta, 0.5, nEta+0.5);


    for (auto superChamber : station->superChambers()) {
      for (auto chamber : superChamber->chambers()) {
        int ch = chamber->id().chamber();
        for (auto etaPart : chamber->etaPartitions()) {
          int ieta = etaPart->id().ieta();
          
          ME3IdsKey key3{st, ch, ieta}; 
          digi_occ_detail_[key3] = ibook.book1D(Form("digi_occ_GE%d1_ch%d_ieta%d", st, ch, ieta),
                                                Form("Occupancy GE%d1 chamber %d iEta%d", st, ch, ieta),
                                                384, -0.5, 383.5);
        }
      }
    }
  }
  // example_ = ibook.book1D("EXAMPLE", "Example 1D", 20, 0., 10.);
  // example2D_ = ibook.book2D("EXAMPLE_2D", "Example 2D", 20, 0, 20, 15, 0, 15);
  // example3D_ = ibook.book3D("EXAMPLE_3D", "Example 3D", 20, 0, 20, 15, 0, 15, 25, 0, 25);
  // exampleTProfile_ = ibook.bookProfile("EXAMPLE_TPROFILE", "Example TProfile", 20, 0, 20, 15, 0, 15);
  // exampleTProfile2D_ = ibook.bookProfile2D("EXAMPLE_TPROFILE2D", "Example TProfile 2D", 20, 0, 20, 15, 0, 15, 0, 100);
}



// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void TestBeamValidation::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // The following says we do not know what parameters are allowed so do no
  // validation
  // Please change this to state exactly what you do use, even if it is no
  // parameters
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("gemDigiLabel", edm::InputTag("muonGEMDigis", ""));
  descriptions.add("TestBeamValidation", desc);
}

// define this as a plug-in
DEFINE_FWK_MODULE(TestBeamValidation);
