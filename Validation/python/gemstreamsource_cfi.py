import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

options = VarParsing.VarParsing('analysis')

options.register('runNumber',
                 210605,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Run number.")

options.register('runInputDir',
                 '/data',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Directory with the input files.")

options.register('streamLabel',
                 'gemlocal',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Stream Label.")

options.register('scanOnce',
                 False,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.bool,
                 'Do not repeat file scans.')

options.parseArguments()

if not options.inputFiles:

    source = cms.Source("GEMStreamReader",
                        fedId = cms.untracked.int32(1479),
                        fedId2 = cms.untracked.int32(1480),
                        fileNames=cms.untracked.vstring(options.inputFiles),
                        firstLuminosityBlockForEachRun=cms.untracked.VLuminosityBlockID({}),
                        runNumber = cms.untracked.uint32(options.runNumber),
                        runInputDir =  cms.untracked.string(options.runInputDir),
                        streamLabel =  cms.untracked.string(options.streamLabel),
                        minEventsPerFile = cms.untracked.uint32(15000),
                        scanOnce = cms.untracked.bool(options.scanOnce),
                        delayMillis= cms.untracked.uint32(500),
                        nextEntryTimeoutMillis = cms.untracked.int32(200000))

else:
    source = cms.Source("GEMStreamSource",
                        fedId = cms.untracked.int32(1479),
                        fedId2 = cms.untracked.int32(1480),
                        fileNames=cms.untracked.vstring(options.inputFiles),
                        firstLuminosityBlockForEachRun=cms.untracked.VLuminosityBlockID({}))

print("Source settings", source)
