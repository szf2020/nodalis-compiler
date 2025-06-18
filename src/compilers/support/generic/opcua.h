#pragma once
#include "imperium.h"
#include "open62541.h"

class OPCUAClient : public IOClient {
public:
    OPCUAClient();
    ~OPCUAClient();

protected:
    void connect() override;

    bool readBit(const std::string& remote, int& result) override;
    bool writeBit(const std::string& remote, int value) override;
    bool readByte(const std::string& remote, uint8_t& result) override;
    bool writeByte(const std::string& remote, uint8_t value) override;
    bool readWord(const std::string& remote, uint16_t& result) override;
    bool writeWord(const std::string& remote, uint16_t value) override;
    bool readDWord(const std::string& remote, uint32_t& result) override;
    bool writeDWord(const std::string& remote, uint32_t value) override;

private:
    UA_Client* client;
    std::string endpointUrl;

    template<typename T>
    bool readValue(const std::string& nodeIdStr, T& value, const UA_DataType* type);

    template<typename T>
    bool writeValue(const std::string& nodeIdStr, T value, const UA_DataType* type);
};
