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
#if defined(_WIN32)
extern "C" {
#include "bacnet-stack/windows-x64/include/bacnet/config.h"
#include "bacnet-stack/windows-x64/include/bacnet/apdu.h"
#include "bacnet-stack/windows-x64/include/bacnet/bacapp.h"
#include "bacnet-stack/windows-x64/include/bacnet/bacdef.h"
#include "bacnet-stack/windows-x64/include/bacnet/bacenum.h"
#include "bacnet-stack/windows-x64/include/bacnet/bacdcode.h"
#include "bacnet-stack/windows-x64/include/bacnet/npdu.h"
#include "bacnet-stack/windows-x64/include/bacnet/rp.h"
#include "bacnet-stack/windows-x64/include/bacnet/wp.h"
#include "bacnet-stack/windows-x64/include/bacnet/datalink/datalink.h"
#include "bacnet-stack/windows-x64/include/bacnet/datalink/dlenv.h"
#include "bacnet-stack/windows-x64/include/bacnet/datalink/bip.h"
#include "bacnet-stack/windows-x64/include/bacnet/address.h"
#include "bacnet-stack/windows-x64/include/bacnet/basic/tsm/tsm.h"
}
#else
extern "C"
{
#include "bacnet-stack/linux-x64/include/bacnet/config.h"
#include "bacnet-stack/linux-x64/include/bacnet/apdu.h"
#include "bacnet-stack/linux-x64/include/bacnet/bacapp.h"
#include "bacnet-stack/linux-x64/include/bacnet/bacdef.h"
#include "bacnet-stack/linux-x64/include/bacnet/bacenum.h"
#include "bacnet-stack/linux-x64/include/bacnet/bacdcode.h"
#include "bacnet-stack/linux-x64/include/bacnet/npdu.h"
#include "bacnet-stack/linux-x64/include/bacnet/rp.h"
#include "bacnet-stack/linux-x64/include/bacnet/wp.h"
#include "bacnet-stack/linux-x64/include/bacnet/datalink/datalink.h"
#include "bacnet-stack/linux-x64/include/bacnet/datalink/dlenv.h"
#include "bacnet-stack/linux-x64/include/bacnet/datalink/bip.h"
#include "bacnet-stack/linux-x64/include/bacnet/address.h"
#include "bacnet-stack/linux-x64/include/bacnet/basic/tsm/tsm.h"
}
#endif

struct BACnetRemotePoint {
    BACNET_OBJECT_TYPE objectType = OBJECT_ANALOG_INPUT;
    uint32_t objectInstance = 0;
    BACNET_PROPERTY_ID propertyId = PROP_PRESENT_VALUE;
    BACNET_ARRAY_INDEX arrayIndex = BACNET_ARRAY_ALL;
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

    bool performRead(const BACnetRemotePoint& point, BACNET_APPLICATION_DATA_VALUE& value);
    bool performWrite(const BACnetRemotePoint& point, const BACNET_APPLICATION_DATA_VALUE& value);

    template<typename T>
    bool decodeNumeric(const BACNET_APPLICATION_DATA_VALUE& value, T& result);
    bool encodeValueFromWidth(int width, uint64_t raw, BACNET_APPLICATION_DATA_VALUE& value);
    bool encodeBoolean(bool state, BACNET_APPLICATION_DATA_VALUE& value);

    std::unordered_map<std::string, BACnetRemotePoint> remoteCache;
    std::string remoteIp;
    uint16_t remotePort;
    uint8_t invokeId = 1;
    bool datalinkReady = false;
};

template<typename T>
bool BACNETClient::decodeNumeric(const BACNET_APPLICATION_DATA_VALUE& value, T& result) {
    switch (value.tag) {
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
            result = static_cast<T>(value.type.Real);
            return true;
        case BACNET_APPLICATION_TAG_DOUBLE:
            result = static_cast<T>(value.type.Double);
            return true;
        default:
            return false;
    }
}
