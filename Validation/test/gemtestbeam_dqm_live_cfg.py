import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

options = VarParsing.VarParsing('analysis')

options.register('firstRun',
                 341761,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 'first run.')

options.parseArguments()

process = cms.Process("GEMStreamSource")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    SkipEvent=cms.untracked.vstring('ProductNotFound'),
)

debug = False
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load('Configuration.StandardSequences.EndOfProcess_cff')
#process.MessageLogger.cout.threshold = cms.untracked.string('INFO')
#process.MessageLogger.debugModules = cms.untracked.vstring('*')

if debug:
    #process.MessageLogger.cerr.threshold = "DEBUG"
    process.MessageLogger.cerr.FwkReport.reportEvery = 5000
    #process.MessageLogger.debugModules = ["source", "muonGEMDigis"]
    process.maxEvents.input = cms.untracked.int32(4000)
else:
    process.MessageLogger.cerr.FwkReport.reportEvery = 5000

process.source = cms.Source("GEMStreamSource",
                            fedId = cms.untracked.int32(12),
                            fedId2 = cms.untracked.int32(10),
                            firstRun = cms.untracked.uint32(options.firstRun),
                            fileNames = cms.untracked.vstring(options.inputFiles),
                            firstLuminosityBlockForEachRun = cms.untracked.VLuminosityBlockID({}))

print(options.inputFiles)

# this block ensures that the output collection is named rawDataCollector, not source
process.rawDataCollector = cms.EDAlias(source=cms.VPSet(
    cms.PSet(type=cms.string('FEDRawDataCollection'))))

process.load('EventFilter.GEMRawToDigi.muonGEMDigis_cfi')
process.muonGEMDigis.InputLabel = cms.InputTag("rawDataCollector")
process.muonGEMDigis.fedIdStart = cms.uint32(10)
process.muonGEMDigis.fedIdEnd = cms.uint32(12)
process.muonGEMDigis.skipBadStatus = cms.bool(False)
process.muonGEMDigis.useDBEMap = True

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.load('gemsw.Geometry.GeometryTestBeam2022_cff')
#process.gemGeometry.applyAlignment = cms.bool(True)

process.GlobalTag.toGet = cms.VPSet(cms.PSet(record=cms.string("GEMeMapRcd"),
                                             tag=cms.string("GEMeMapTestBeam"),
                                             connect=cms.string("sqlite_fip:gemsw/EventFilter/data/GEMeMap_TestBeam_2022.db")),
)

#process.load('Configuration.StandardSequences.Reconstruction_cff')

process.load("DQM.Integration.config.environment_cfi")
process.dqmEnv.subSystemFolder = "GEMTestBeam"
process.dqmSaver.tag = "GEMTestBeam"
process.dqmSaver.runNumber = options.firstRun
process.dqmSaverPB.tag = "GEMTestBeam"
process.dqmSaverPB.runNumber = options.firstRun
#process.dqmEnv.eventInfoFolder = "EventInfo"
#process.dqmSaver.path = ""

process.Validation = DQMEDAnalyzer("TestBeamValidation",
                                   gemDigiLabel = cms.InputTag("muonGEMDigis", ""))

process.unpack = cms.Path(process.muonGEMDigis)
#process.reco = cms.Path(process.gemRecHits)
process.dqm = cms.Path(process.Validation)
process.dqmout = cms.EndPath(
    process.dqmEnv
    + process.dqmSaver
    + process.dqmSaverPB
)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(
    process.unpack,
    #process.reco,
    process.dqm,
    process.dqmout,
    process.endjob_step
)
