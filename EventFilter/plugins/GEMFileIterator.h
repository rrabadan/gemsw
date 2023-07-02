#ifndef gemsw_EventFilter_GEMFileIterator_h
#define gemsw_EventFilter_GEMFileIterator_h

// A copy of
// DQMServices_StreamerIO_DQMFileIterator

#include <chrono>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include <iostream>

namespace edm {
  class ParameterSet;
  class ParameterSetDescription;
} // namespace edm

class GEMFileIterator {
  public:

    struct Entry {
      std::string filename;
      std::string run_path;

      unsigned int entry_number;
      bool sec_file;

      static Entry load_entry(const std::string& run_path,
                              const std::string& filename,
                              const unsigned int entryNumber,
                              bool seFile);

      std::string get_data_path() const;
      std::string state;
    };
    
    struct EorEntry {
      bool loaded = false;
      std::string filename;
      std::string run_path;
      std::size_t n_entry;

      static EorEntry load_eor(const std::string& run_path, const std::string& filename);
    };

    enum State {
      OPEN = 0,
      EOR_CLOSING = 1, // EoR file found, but files are still pending
      EOR = 2
    };

    GEMFileIterator(edm::ParameterSet const& pset);
    ~GEMFileIterator() = default;
    void init(int run, const std::string&, const std::string&);

    State state() const { return state_; }

    /* methods to iterate the actual files */
    
    /* nextNumber_ is the first unprocessed lumi number
     * lumiReady() returns if the next lumi is ready to be loaded
     * open() opens a file and advances the pointer to the next lumi
     * 
     * front() a reference to the description (LumiEntry)
     * pop() advances to the next lumi
    */
    
    bool entryReady();
    Entry open();

    void pop();

    // control
    void reset();
    void update_state();

    void delay();

    void logFileAction(const std::string& msg, const std::string& fileName = "") const;
    void logEntryState(const Entry& entry, const std::string& msg);

    unsigned int runNumber() const { return runNumber_; }
    unsigned int lastEntryFound();
    void advanceEntry(unsigned int entry, std::string reason);

    static void fillDescription(edm::ParameterSetDescription& desc);

  private:
    unsigned int runNumber_;
    std::string runInputDir_;
    std::string streamLabel_;
    unsigned long delayMillis_;
    long nextEntryTimeoutMillis_;
    long forceFileCheckTimeoutMillis_;
    bool flagScanOnce_;
    bool secFile_;

    std::string runPath_;

    EorEntry eor_;
    State state_;

    unsigned int nextEntryNumber_;
    std::map<unsigned int, Entry> entrySeen_;
    std::unordered_set<std::string> filesSeen_;

    unsigned runPathModTime_;
    std::chrono::high_resolution_clock::time_point runPathLastCollect_;

    std::chrono::high_resolution_clock::time_point lastEntryLoad_;

    unsigned mtimeHash() const;
    void collect(bool ignoreTimers);
};

#endif  // gemsw_EventFilter_GEMFileIterator_h
