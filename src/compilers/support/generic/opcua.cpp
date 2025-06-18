#include "opcua.h"
#include <iostream>

OPCUAClient::OPCUAClient()
    : IOClient("opcua"), endpointUrl("opc.tcp://localhost:4840") {
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
}

OPCUAClient::~OPCUAClient() {
    UA_Client_disconnect(client);
    UA_Client_delete(client);
}

void OPCUAClient::connect() {
    if (!connected) {
        if (UA_Client_connect(client, moduleID.c_str()) == UA_STATUSCODE_GOOD) {
            connected = true;
        } else {
            connected = false;
        }
    }
}

template<typename T>
bool OPCUAClient::readValue(const std::string& nodeIdStr, T& value, const UA_DataType* type) {
    UA_Variant val;
    UA_Variant_init(&val);

    UA_NodeId nodeId = UA_NODEID_STRING_ALLOC(1, nodeIdStr.c_str());
    UA_StatusCode status = UA_Client_readValueAttribute(client, nodeId, &val);
    UA_NodeId_clear(&nodeId);

    if (status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&val, type)) {
        value = *(T*)val.data;
        return true;
    }
    return false;
}

template<typename T>
bool OPCUAClient::writeValue(const std::string& nodeIdStr, T value, const UA_DataType* type) {
    UA_Variant val;
    UA_Variant_setScalar(&val, &value, type);

    UA_NodeId nodeId = UA_NODEID_STRING_ALLOC(1, nodeIdStr.c_str());
    UA_StatusCode status = UA_Client_writeValueAttribute(client, nodeId, &val);
    UA_NodeId_clear(&nodeId);

    return status == UA_STATUSCODE_GOOD;
}

// Implement required IOClient methods

bool OPCUAClient::readBit(const std::string& remote, int& result) {
    bool val;
    bool success = readValue(remote, val, &UA_TYPES[UA_TYPES_BOOLEAN]);
    result = val;
    return success;
}

bool OPCUAClient::writeBit(const std::string& remote, int value) {
    bool val = value != 0;
    return writeValue(remote, val, &UA_TYPES[UA_TYPES_BOOLEAN]);
}

bool OPCUAClient::readByte(const std::string& remote, uint8_t& result) {
    return readValue(remote, result, &UA_TYPES[UA_TYPES_BYTE]);
}

bool OPCUAClient::writeByte(const std::string& remote, uint8_t value) {
    return writeValue(remote, value, &UA_TYPES[UA_TYPES_BYTE]);
}

bool OPCUAClient::readWord(const std::string& remote, uint16_t& result) {
    return readValue(remote, result, &UA_TYPES[UA_TYPES_UINT16]);
}

bool OPCUAClient::writeWord(const std::string& remote, uint16_t value) {
    return writeValue(remote, value, &UA_TYPES[UA_TYPES_UINT16]);
}

bool OPCUAClient::readDWord(const std::string& remote, uint32_t& result) {
    return readValue(remote, result, &UA_TYPES[UA_TYPES_UINT32]);
}

bool OPCUAClient::writeDWord(const std::string& remote, uint32_t value) {
    return writeValue(remote, value, &UA_TYPES[UA_TYPES_UINT32]);
}
