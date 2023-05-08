#ifndef gemsw_EventFilter_GEMStreamReader_h
#define gemsw_EventFilter_GEMStreamReader_h

// A copy of 
// DQMServices_StreamerIO_DQMStreamerReader
// and
// GEMStreamSource

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/InputSourceMacros.h"

#include "FWCore/Sources/interface/ProducerSourceFromFiles.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/Provenance/interface/Timestamp.h"

#include "GEMFileIterator.h"

#include <unistd.h>
#include <string>
#include <vector>
#include <fstream>

class GEMStreamReader : public edm::ProducerSourceFromFiles {
public:
  // construction/destruction
  GEMStreamReader(edm::ParameterSet const& pset, edm::InputSourceDescription const& desc);
  ~GEMStreamReader() override{};
  
  static void fillDescription(edm::ParameterSetDescription& desc);

private:
  // member functions
  bool setRunAndEventInfo(edm::EventID& id,
                          edm::TimeValue_t& theTime,
                          edm::EventAuxiliary::ExperimentType& eType) override;
  void produce(edm::Event& e) override;

  bool openFile(const std::string& fileName, std::ifstream& fin);
  std::unique_ptr<FRDEventMsgView> getEventMsg(std::ifstream& fin);
  std::vector<uint64_t>* makeFEDRAW(FRDEventMsgView* frdEventMsg, uint16_t fedId);

private:

  void init();
  bool prepareNextFile();
  void openNextFile();
  void closeFile();
  std::unique_ptr<FRDEventMsgView> prepareNextEvent();

  // member data
  GEMFileIterator fIterator_;
  bool hasSecFile;
  bool flagEndOfRunKills_;
  std::ifstream fin_;
  std::ifstream fin2_;
  GEMRawToDigi gemR2D;
  std::unique_ptr<FEDRawDataCollection> rawData_;
  std::vector<char> buffer_;
  int fedId_;
  int fedId2_;
  uint16_t detectedFRDversion_ = 0;
  uint16_t flags_ = 0;
  unsigned int procEventsFile_ = 0;
  unsigned int minEventsFile_;
};

#endif  // EventFilter_Utilities_GEMStreamReader_h
