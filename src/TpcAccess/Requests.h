#ifndef REQUESTS_H
#define REQUESTS_H

#include <utility>

#include "TpcAccess.h"
#include <common/types.h>
#include <common/utils/type_utils.h>

#include "Device.h"
#include "System.h"
#include "SystemList.h"

template <typename... OutParams>
class Request {
    template <typename... InParams>
    static void func() {}
};

inline TpcExpect<Device*> findDevice(int deviceIdx) {
    System* s = SystemList::TheSystemList()->FindSystem(deviceIdx);
    if (s == nullptr) return tl::unexpected(tpc_errInvalidDeviceIx);
    deviceIdx = deviceIdx % SYSTEM_MULTIPLIER;

    Device* d = s->FindDevice(deviceIdx);
    if (d == nullptr) return tl::unexpected(tpc_errInvalidDeviceIx);

    TPC_ErrorCode err = d->CheckStatus();
    if (err != tpc_noError) return tl::unexpected(err);

    return d;
}

// TODO Just use this function instead of CRTP nonsense lmao
inline TpcExpect<SoapDevice*> findDeviceConn(int deviceIdx) {
    if (auto dev = findDevice(deviceIdx)) {
        return (*dev)->getSoapConnection();
    }
    else {
        return tl::unexpected(dev.error());
    }
}

// Template class that uses CRTP
// Provides boilerplate for easily making an API request to some device
template <typename derived>
class SimpleSoapRequest {
   public:
    SimpleSoapRequest() = default;

    template <typename... Ts>
    TPC_ErrorCode execute(int deviceIdx, Ts&&... args) {
        // Find the associated system object
        System* s = SystemList::TheSystemList()->FindSystem(deviceIdx);
        if (s == nullptr) return tpc_errInvalidDeviceIx;
        deviceIdx = deviceIdx % SYSTEM_MULTIPLIER;

        Device* d = s->FindDevice(deviceIdx);
        if (d == nullptr) return tpc_errInvalidDeviceIx;

        TPC_ErrorCode err = d->CheckStatus();
        if (err != tpc_noError) return err;

        return static_cast<derived*>(this)->impl(d->getSoapConnection(), std::forward<Ts>(args)...);
    }

   private:
    // Device& m_device;
    // System& m_system;
};

template <typename derived>
class DeviceStatusRequest {
   public:
    DeviceStatusRequest() = default;

    // TODO Relevant for what we might try to do here
    // https://www.fluentcpp.com/2017/08/11/how-to-do-partial-template-specialization-in-c/
    template <typename... Ts>
    TPC_ErrorCode execute(int deviceIdx, Ts&&... args) {
        // Find the associated system object
        System* s = SystemList::TheSystemList()->FindSystem(deviceIdx);
        if (s == nullptr) return tpc_errInvalidDeviceIx;
        deviceIdx = deviceIdx % SYSTEM_MULTIPLIER;

        Device* d = s->FindDevice(deviceIdx);
        if (d == nullptr) return tpc_errInvalidDeviceIx;

        TPC_ErrorCode err = d->CheckStatus();
        if (err != tpc_noError) return err;

        return static_cast<derived*>(this)->impl(d, std::forward<Ts>(args)...);
    }
};

class GetBitReadoutErrors : public SimpleSoapRequest<GetBitReadoutErrors> {
    friend SimpleSoapRequest;

   private:
    TPC_ErrorCode impl(SoapDevice* conn, int boardAddress, TPC_BitReadoutTestResult* result) {
        ns__BitReadoutTestResult val;
        auto err = conn->invokeSoap(soap_call_ns__GetBitReadoutErrors, boardAddress, val);
        *result  = utils::convert<TPC_BitReadoutTestResult>(val);
        return err;
    }
};

class WritePhaseParameters : public SimpleSoapRequest<WritePhaseParameters> {
    friend SimpleSoapRequest;

   private:
    TPC_ErrorCode impl(SoapDevice* conn, int boardAddress) {
        auto err = conn->invokeSoapDummy(soap_call_ns__WritePhaseParameters, boardAddress);
        return err;
    }
};

#endif  // REQUESTS_H
