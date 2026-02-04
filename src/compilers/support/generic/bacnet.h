// Copyright [2025] Nathan Skipper
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @description Nodalis PLC BACnet/IP client wrapper
 * @author Nathan Skipper, MTI
 * @version 1.0.2
 * @copyright Apache 2.0
 */
#pragma once

#include "nodalis.h"
#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

extern "C"
{
#include "bacnet/config.h"
#include "bacnet/apdu.h"
#include "bacnet/bacapp.h"
#include "bacnet/bacdef.h"
#include "bacnet/bacenum.h"
#include "bacnet/bacdcode.h"
#include "bacnet/npdu.h"
#include "bacnet/rp.h"
#include "bacnet/wp.h"
#include "bacnet/datalink/bip.h"
#include "bacnet/datalink/datalink.h"
#include "bacnet/bacaddr.h"
#include "bacnet/basic/binding/address.h"
#include "bacnet/basic/tsm/tsm.h"
}

inline double uint64_to_double(uint64_t value)
{
    uint32_t fractional = static_cast<uint32_t>(value & 0xFFFFFFFFULL);
    int32_t integer = static_cast<int32_t>(value >> 32);

    double result = static_cast<double>(integer);
    result += static_cast<double>(fractional) / 4294967296.0;

    return result;
}

inline uint64_t double_to_uint64(double x)
{

    if (!std::isfinite(x))
    {
        return 0;
    }

    // Clamp to the representable range of int32 + fraction in [0,1).
    // Max representable is INT32_MAX + (1 - 2^-32)
    const double minVal = static_cast<double>(INT32_MIN);
    const double maxVal = static_cast<double>(INT32_MAX) +
                          (1.0 - 1.0 / 4294967296.0);

    if (x < minVal)
        x = minVal;
    if (x > maxVal)
        x = maxVal;

    // Use floor so fractional is always in [0,1), even for negative numbers.
    // Example: x = -1.25 -> integer = -2, frac = 0.75 (this will round-trip correctly)
    double intPartD = std::floor(x);
    double fracD = x - intPartD; // in [0,1)

    int32_t integer = static_cast<int32_t>(intPartD);

    // Scale fractional to 32-bit.
    // Round to nearest to reduce error; clamp to avoid 2^32 on edge cases.
    double scaled = fracD * 4294967296.0; // 2^32
    uint64_t frac = static_cast<uint64_t>(std::llround(scaled));

    if (frac >= 4294967296ULL)
    {
        // Carry into integer if rounding pushed us over (rare, but possible).
        frac = 0;
        if (integer < INT32_MAX)
        {
            ++integer;
        }
        else
        {
            // Already at max; saturate
            frac = 0xFFFFFFFFULL;
        }
    }

    uint64_t uInt = static_cast<uint32_t>(integer); // preserve bit pattern for negative int32_t
    return (uInt << 32) | (frac & 0xFFFFFFFFULL);
}

struct BACnetRemotePoint
{
    BACNET_OBJECT_TYPE objectType = OBJECT_ANALOG_INPUT;
    uint32_t objectInstance = 0;
    BACNET_PROPERTY_ID propertyId = PROP_PRESENT_VALUE;
    BACNET_ARRAY_INDEX arrayIndex = BACNET_ARRAY_ALL;
    uint8_t valueType = BACNET_APPLICATION_TAG_ENUMERATED;
    uint8_t direction = 0;
};

class BACNETClient : public IOClient {
public:
    BACNETClient(const std::string& ip = "", uint16_t port = 0xBAC0);
    ~BACNETClient() override;

protected:
    bool readBit(const std::string& remote, int& result) override;
    bool writeBit(const std::string& remote, int value) override;
    bool readByte(const std::string& remote, uint8_t& result) override;
    bool writeByte(const std::string& remote, uint8_t value) override;
    bool readWord(const std::string& remote, uint16_t& result) override;
    bool writeWord(const std::string& remote, uint16_t value) override;
    bool readDWord(const std::string& remote, uint32_t& result) override;
    bool writeDWord(const std::string& remote, uint32_t value) override;
    bool readLWord(const std::string &remote, uint64_t &result) override;
    bool writeLWord(const std::string &remote, uint64_t value) override;
    void connect() override;
    void onMappingAdded(const IOMap& map) override;

private:
    static constexpr std::chrono::milliseconds REQUEST_TIMEOUT{1000};

    uint8_t nextInvokeId();
    bool ensureDatalink();
    BACNET_ADDRESS buildAddress() const;
    bool resolveRemote(const std::string& remote, BACnetRemotePoint& point);
    bool parseRemoteDefinition(const IOMap& map, BACnetRemotePoint& point);
    bool parseJsonRemote(const json& config, BACnetRemotePoint& point);
    bool parseStringRemote(const std::string& definition, BACnetRemotePoint& point) const;
    BACNET_OBJECT_TYPE parseObjectType(const std::string& raw) const;
    BACNET_PROPERTY_ID parsePropertyId(const std::string& raw) const;
    uint8_t parseValueType(const std::string &raw) const;

    bool performRead(const BACnetRemotePoint &point, BACNET_APPLICATION_DATA_VALUE &value);
    bool performWrite(const BACnetRemotePoint& point, const BACNET_APPLICATION_DATA_VALUE& value);

    template<typename T>
    bool decodeNumeric(const BACNET_APPLICATION_DATA_VALUE& value, T& result);
    bool encodeValue(uint64_t raw, BACnetRemotePoint point, BACNET_APPLICATION_DATA_VALUE &value);

    std::unordered_map<std::string, BACnetRemotePoint> remoteCache;
    std::string remoteIp;
    uint16_t remotePort;
    uint8_t invokeId = 1;
    bool datalinkReady = false;
};

template<typename T>
bool BACNETClient::decodeNumeric(const BACNET_APPLICATION_DATA_VALUE& value, T& result) {
    double rf;
    uint64_t uf;
    switch (value.tag)
    {
    case BACNET_APPLICATION_TAG_BOOLEAN:
        result = static_cast<T>(value.type.Boolean ? 1 : 0);
        return true;
    case BACNET_APPLICATION_TAG_UNSIGNED_INT:
        result = static_cast<T>(value.type.Unsigned_Int);
        return true;
    case BACNET_APPLICATION_TAG_SIGNED_INT:
        result = static_cast<T>(value.type.Signed_Int);
        return true;
    case BACNET_APPLICATION_TAG_ENUMERATED:
        result = static_cast<T>(value.type.Enumerated);
        return true;
    case BACNET_APPLICATION_TAG_REAL:
        rf = (double)value.type.Real;
        uf = double_to_uint64(rf);
        result = static_cast<T>(uf);
        return true;
    case BACNET_APPLICATION_TAG_DOUBLE:
        rf = value.type.Double;
        uf = double_to_uint64(rf);
        result = static_cast<T>(uf);
        return true;
    default:
        return false;
    }
}
