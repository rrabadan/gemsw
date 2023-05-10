import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

process = cms.Process("ME0stackDQM")

process.load("gemsw.Validation.gemstreamsource_cfi")
from gemsw.Validation.gemstreamsource_cfi import options

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    SkipEvent=cms.untracked.vstring('ProductNotFound'),
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.MessageLogger.cerr.FwkReport.reportEvery = 5000

# this block ensures that the output collection is named rawDataCollector, not source
process.rawDataCollector = cms.EDAlias(source=cms.VPSet(
    cms.PSet(type=cms.string('FEDRawDataCollection'))))

process.load('EventFilter.GEMRawToDigi.muonGEMDigis_cfi')
process.muonGEMDigis.InputLabel = cms.InputTag("rawDataCollector")
process.muonGEMDigis.fedIdStart = cms.uint32(1479)
process.muonGEMDigis.fedIdEnd = cms.uint32(1479)
process.muonGEMDigis.skipBadStatus = cms.bool(False)
process.muonGEMDigis.useDBEMap = True

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.load('gemsw.Geometry.GeometryMe0_cff')
#process.gemGeometry.applyAlignment = cms.bool(True)


process.GlobalTag.toGet = cms.VPSet(cms.PSet(record=cms.string("GEMeMapRcd"),
                                             tag=cms.string("GEMeMapME0stack"),
                                             connect=cms.string("sqlite_fip:gemsw/EventFilter/data/GEMeMap_ME0stack.db")),
)

#process.load('Configuration.StandardSequences.Reconstruction_cff')

process.load("DQM.Integration.config.environment_cfi")
process.dqmEnv.subSystemFolder = "ME0"
process.dqmSaver.tag = "ME0"
process.dqmSaver.runNumber = options.runNumber
process.dqmSaverPB.tag = "ME0"
process.dqmSaverPB.runNumber = options.runNumber
#process.dqmEnv.eventInfoFolder = "EventInfo"
process.dqmSaver.path = f"{options.runInputDir}/upload"
process.dqmSaverPB.path = f"{options.runInputDir}/upload/pb"

process.Validation = DQMEDAnalyzer("ME0Digis",
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
