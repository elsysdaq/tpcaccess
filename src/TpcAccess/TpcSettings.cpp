//---------------------------------------------------------------------------
/*
 *
 * ELSYS EXPRESSLY DISCLAIMS ALL WARRANITIES OR REPRESENTATIONS WITH RESPECT TO
 * THIS SOFTWARE AND DOCUMENTATION AND ANY SUPPORT OR MAINTENANCE SERVICES THAT
 * ELSYS MAY CHOOSE TO PROVIDE IN RELATION THERETO (INCLUDING, WITHOUT
 * LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE OR THAT THE SOFTWARE: WILL BE
 * ERROR-FREE, WILL OPERATE WITHOUT INTERUPTION, WILL NOT INFRINGE THE RIGHTS OF A
 * THIRD PARTY, OR WILL PRODUCE RESULTS IN CONNECTION WITH ITS USE). FURTHER,
 * ELSYS EXPRESSLY DISCLAIMS ALL EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * (C) Copyright 2005 - 2019 Elsys AG. All rights reserved.
*/

//---------------------------------------------------------------------------
// $Id: TpcSettings.cpp 29 2019-09-26 10:22:37Z roman $

#include "TpcSettings.h"

#include "xerces_include_start.h"
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include "xerces_include_end.h"

#include <exception>

#ifdef WIN32
# include "TpcAccessInstance.h"
# include "ResourceFile.h"
#else
# include "tpcsettings.inc"
#endif

#include "XercesUtils.h"

#include "TpcSettingsFromXml.h"
#include "TpcSettingsToXml.h"


//-----------------------------------------------------------------------------

// table of all known parameters

const Param paramTable[numKnownParameters] = 
{
    { tpc_parOperationMode,					_TEXT("operationMode"),					true },
    { tpc_parMultiplexerMode,				_TEXT("multiplexerMode"),				true },
    { tpc_parTimebaseSource,				_TEXT("timebaseSource"),				true },
    { tpc_parSamplingFrequency,				_TEXT("samplingFrequency"),				true },
    { tpc_parExternalClockDivisor,			_TEXT("externalClockDivisor"),			true },
    { tpc_parExternalTriggerMode,			_TEXT("externalTriggerMode"),			true },
    { tpc_parBlkNumberOfBlocks,				_TEXT("blkNumberOfBlocks"),				true },
    { tpc_parBlkBlockLength,				_TEXT("blkBlockLength"),				true },
    { tpc_parBlkTriggerDelay,				_TEXT("blkTriggerDelay"),				true },
    { tpc_parContMaximumDataLength,			_TEXT("contMaximumDataLength"),			true },
    { tpc_parContStopTrailer,				_TEXT("contStopTrailer"),				true },
	{ tpc_parEcrDualMode,					_TEXT("ecrDualMode"),					true},							//MU
    //{ tpc_parEcrMinimumBlockLength, _TEXT("ecrMinimumBlockLength"), true },
    //{ tpc_parEcrTriggerDelay, _TEXT("ecrTriggerDelay"), true },
    { tpc_parEcrPreTrigger,					_TEXT("ecrPreTrigger"),					true },
    { tpc_parEcrPostTrigger,				_TEXT("ecrPostTrigger"),				true },
    { tpc_parEcrMaximumNumberOfBlocks,		_TEXT("ecrMaximumNumberOfBlocks"),		true },
    { tpc_parEcrHoldOffTime,				_TEXT("ecrHoldOffTime"),				true },
    { tpc_parEcrMaximumBlockLength,			_TEXT("ecrMaximumBlockLength"),			true },
    { tpc_parEcrRetriggerTime,				_TEXT("ecrRetriggerTime"),				true },
    { tpc_parEcrTrailer,					_TEXT("ecrTrailer"),					true },
    { tpc_parEcrClockDivisor,				_TEXT("ecrClockDivisor"),				true },
    { tpc_parInputMode,						_TEXT("inputMode"),						false },
	{ tpc_parIEPECurrent,					_TEXT("IEPECurrent"),					false },
    { tpc_parTriggerOnly,					_TEXT("triggerOnly"),					false },
    { tpc_parInputCoupling,					_TEXT("inputCoupling"),					false },
    { tpc_parRange,							_TEXT("range"),							false },
    { tpc_parOffset,						_TEXT("offset"),						false },
    { tpc_parInvert,						_TEXT("invert"),						false },
    { tpc_parFilterMode,					_TEXT("filterMode"),					false },
    { tpc_parFilterFreq,					_TEXT("filterFreq"),					false },
    { tpc_parAveragingMode,					_TEXT("averagingMode"),					false },
    { tpc_parPhysFactor,					_TEXT("physFactor"),					false },
    { tpc_parPhysConstant,					_TEXT("physConstant"),					false },
    { tpc_parTrgMode,						_TEXT("trgMode"),						false },
    { tpc_parTrgComparatorMode,				_TEXT("trgComparatorMode"),				false },
    { tpc_parTrgLevelA,						_TEXT("trgLevelA"),						false },
    { tpc_parTrgLevelB,						_TEXT("trgLevelB"),						false },
    { tpc_parTrgTimerTSamples,				_TEXT("trgTimerTSamples"),				false },
    { tpc_parTrgSlewRateDTSamples,			_TEXT("trgSlewRateDTSamples"),			false },
	{ tpc_parTrgSlewRateComparatorMode,		_TEXT("trgSlewRateComparatorMode"),		false },
    { tpc_parTrgComparator2Mode,			_TEXT("trgComparator2Mode"),			false },
    { tpc_parTrgLevel2A,					_TEXT("trgLevel2A"),					false },
    { tpc_parTrgLevel2B,					_TEXT("trgLevel2B"),					false },
    { tpc_parShowTrgProduct,				_TEXT("trgShowProduct"),				false },
	{ tpc_parANDTriggerMask,				_TEXT("ANDTriggerMask"),				false },
	{ tpc_parDualModeStopTrgComparatorMode, _TEXT("dualModeStopTrgComparatorMode"), false },
    { tpc_parDualModeStopTrgLevelA,			_TEXT("dualModeStopTrgLevelA"),			false },
    { tpc_parDualModeStopTrgLevelB,			_TEXT("dualModeStopTrgLevelB"),			false },
	{ tpc_parANDTriggerMask,				_TEXT("TrgANDMask"),					false },
	{ tpc_parTrgPreTimerTSamples,			_TEXT("TrgTimerT1Samples"),				false },
	{ tpc_parResetOverflow,					_TEXT("ResetOverload"),					false },

    { tpc_parScopeAutoTrigger,				_TEXT("scopeAutoTrigger"),				true },
    { tpc_parScopeSingleShot,				_TEXT("scopeSingleShot"),				true },

    { tpc_parExtClockSamplingFrequency,		_TEXT("extClockSamplingFrequency"),		true },
    { tpc_parContEnableStopTrigger,			_TEXT("contEnableStopTrigger"),			true },
    { tpc_parContEnableRecLengthLimit,		_TEXT("contEnableRecLengthLimit"),		true },
    { tpc_parScopeBlockLength,				_TEXT("scopeBlockLength"),				true },
    { tpc_parScopeTriggerDelay,				_TEXT("scopeTriggerDelay"),				true },
    { tpc_parEcrEnableHoldOff,				_TEXT("ecrEnableHoldOff"),				true },
    { tpc_parEcrDualEnableTrailer,			_TEXT("ecrDualEnableTrailer"),			true },
    { tpc_parEcrDualTrailer,				_TEXT("ecrDualTrailer"),				true },
    { tpc_parEcrDualEnableRecLengthLimit,	_TEXT("ecrDualEnableRecLengthLimit"),	true },
    { tpc_parEcrDualMaxRecLength,			_TEXT("ecrDualMaxRecLength"),			true },
    { tpc_parEcrEnableStopTrigger ,			_TEXT("ecrEnableStopTrigger"),			true },
    { tpc_parEcrEnableRetrigger,			_TEXT("ecrEnableRetrigger"),			true },
	{ tpc_parExternalTimebaseOutFreq,		_TEXT("ExternalTimebaseOutFrequency"),	true },
	{ tpc_parGPSSync,						_TEXT("GPSSync"),						true },
	{ tpc_parLXIMsgConfig,					_TEXT("LXIMsgConfig"),					false },
	{ tpc_parSyncPingMode,				    _TEXT("SyncPingMode"),					true},
	{ tpc_parSyncDelay,						_TEXT("SyncDelay"),						true },
    //{ tpc_parEcrEnableDualMode, _TEXT("ecrEnableDualMode"), true },
    //{ tpc_parEcrEnableMultiChn, _TEXT("ecrEnableMultiChn"), true },
    //{ tpc_parEcrEnableRetrigger, _TEXT("ecrEnableRetrigger"), true },
    //{ tpc_parEcrEnableStopTrigger, _TEXT("ecrEnableStopTrigger"), true },
};


int SearchParamName(const nstring& name)
{
    for (int i=0; i<numKnownParameters; i++) {
        if (paramTable[i].name == name) return i;
    }
    return -1;
}



//-----------------------------------------------------------------------------

#ifdef WIN32
class ResourceBasedValidatingDOMInOut : public ValidatingDOMInOut {
    public:
    ResourceBasedValidatingDOMInOut(HINSTANCE hinstance_for_schema, const nstring& schema_resource_name)
        : hinstance(hinstance_for_schema), schema_name(schema_resource_name) {}
    protected:
    virtual void LoadSchema(X::XercesDOMParser& parser);
    private:
    HINSTANCE hinstance;
    nstring schema_name;
};

void ResourceBasedValidatingDOMInOut::LoadSchema(X::XercesDOMParser& parser)
{
    ResourceFile schema_file(hinstance, schema_name.c_str(), _TEXT("XMLSchema"));
    X::MemBufInputSource schema(static_cast<const XMLByte *>(schema_file.data()), // srcDocBytes,
                                schema_file.size(), // byteCount
                                schema_name.c_str()); // bufId
    parser.loadGrammar(schema, X::Grammar::SchemaGrammarType, true);
}
#else
class ResourceBasedValidatingDOMInOut : public ValidatingDOMInOut {
    public:
    ResourceBasedValidatingDOMInOut(const nstring& schema_resource_name)
        : schema_name(schema_resource_name) {}
    protected:
    virtual void LoadSchema(X::XercesDOMParser& parser);
    private:
    nstring schema_name;
};

void ResourceBasedValidatingDOMInOut::LoadSchema(X::XercesDOMParser& parser)
{
    X::MemBufInputSource schema(reinterpret_cast<const XMLByte *>(tpcsettingData), // srcDocBytes,
                                tpcsettingSize, // byteCount
                                schema_name.c_str()); // bufId
    parser.loadGrammar(schema, X::Grammar::SchemaGrammarType, true);
}
#endif


//-----------------------------------------------------------------------------

#define SCHEMA_FILENAME _TEXT("tpcsettings-1.x.xsd")

TPC_ErrorCode WriteSettingFile(const std::string& filename, std::string& errormsg)
{
    errormsg.erase();
#ifdef WIN32
    ResourceBasedValidatingDOMInOut tcpsettings(TpcAccessInstance, SCHEMA_FILENAME);
#else
    ResourceBasedValidatingDOMInOut tcpsettings(SCHEMA_FILENAME);
#endif
    try {
        DOMDocumentHandle settings(CreateXmlTpcSettings());
        if (tcpsettings.WriteFile(settings, filename.c_str())) {
            return tpc_noError;
        } else {
            errormsg=tcpsettings.GetLastError();
            return tpc_errWriteSettingFile;
        }
    }
    catch(TPC_ErrorCode e) {
        return e;
    }
}


TPC_ErrorCode LoadSettingFile (const std::string& filename, std::string& errormsg)
{
    errormsg.erase();
#ifdef WIN32
    ResourceBasedValidatingDOMInOut tcpsettings(TpcAccessInstance, SCHEMA_FILENAME);
#else
    ResourceBasedValidatingDOMInOut tcpsettings(SCHEMA_FILENAME);
#endif
    try {
        DOMDocumentHandle doc(tcpsettings.ParseFile(filename.c_str()));
        if (doc) {
            ProcessXmlTpcSettings(doc);
            return tpc_noError;
        } else {
            errormsg=tcpsettings.GetLastError();
            return tpc_errLoadSettingFile;
        }
    }
    catch(const std::exception&) {
        errormsg="error converting string to nummeric value";
        return tpc_errLoadSettingFile;
    }
    catch(TPC_ErrorCode e) {
        return e;
    }
}
