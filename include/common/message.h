#pragma once
#include <iostream>
#include <array>

struct ClientProtocol{ // 24 bytes
    uint32_t userId;
    bool side, padding;
    uint64_t price;
    int16_t quantity;
    uint64_t timestamp;

    ClientProtocol(uint32_t userId,
        bool side, bool padding,
        uint64_t price,
        int16_t quantity,
        uint64_t timestamp) : 
        
        userId(userId),
        side(side),
        padding(padding),
        price(price),
        quantity(quantity),
        timestamp(timestamp) {}

    ClientProtocol() :
        userId(0),
        side(true),
        padding(true),
        price(0),
        quantity(0),
        timestamp(0) {}

    std::array<u_int8_t, 24> as_bytes() const{
        std::array<u_int8_t, 24> bytes {};

        std::memcpy(bytes.data(), &userId, sizeof(userId));

        bytes[4] = side ? 1 : 0;
        bytes[5] = padding ? 1 : 0;

        std::memcpy(bytes.data() + 6, &price, sizeof(price));
        std::memcpy(bytes.data() + 14, &quantity, sizeof(quantity));
        std::memcpy(bytes.data() + 16, &timestamp, sizeof(timestamp));
        
        return bytes;
    }

    static ClientProtocol from_bytes(const std::array<u_int8_t, 24>& bytes){
        ClientProtocol protocol;
        std::memcpy(&protocol.userId, bytes.data(), sizeof(protocol.userId));
        std::memcpy(&protocol.side, bytes.data() + 4, sizeof(protocol.side));
        std::memcpy(&protocol.padding, bytes.data() + 5, sizeof(protocol.padding));
        std::memcpy(&protocol.price, bytes.data() + 6, sizeof(protocol.price));
        std::memcpy(&protocol.quantity, bytes.data() + 14, sizeof(protocol.quantity));
        std::memcpy(&protocol.timestamp, bytes.data() + 16, sizeof(protocol.timestamp));
        
        return protocol;
    }
};

struct SystemProtocol{ // 44 bytes
    uint32_t userId;
    bool side, padding;
    uint64_t price;
    int16_t quantity;
    uint64_t timestamp;
    uint64_t seq_number;
    uint64_t transaction_id;
    uint32_t checksum;

    SystemProtocol(uint32_t userId,
    bool side, bool padding,
    uint64_t price,
    int16_t quantity,
    int64_t timestamp,
    uint64_t seq_number,
    uint64_t transaction_id,
    uint32_t checksum) : 
        userId(userId),
        side(side),
        padding(padding),
        price(price),
        quantity(quantity),
        timestamp(timestamp),
        seq_number(seq_number),
        transaction_id(transaction_id),
        checksum(checksum) {}


    std::array<u_int8_t, 44> as_bytes() const{
        std::array<u_int8_t, 44> bytes {};

        std::memcpy(bytes.data(), &userId, sizeof(userId));

        bytes[4] = side ? 1 : 0;
        bytes[5] = padding ? 1 : 0;

        std::memcpy(bytes.data() + 6, &price, sizeof(price));
        std::memcpy(bytes.data() + 14, &quantity, sizeof(quantity));
        std::memcpy(bytes.data() + 16, &timestamp, sizeof(timestamp));
        std::memcpy(bytes.data() + 24, &seq_number, sizeof(seq_number));
        std::memcpy(bytes.data() + 32, &transaction_id, sizeof(transaction_id));
        std::memcpy(bytes.data() + 40, &checksum, sizeof(checksum));
        return bytes;
    }
    SystemProtocol() :
        userId(0),
        side(false),
        padding(false),
        price(0),
        quantity(0),
        timestamp(0),
        seq_number(0),
        transaction_id(0),
        checksum(0) {}

    static SystemProtocol from_bytes(const std::array<u_int8_t, 44>& bytes) {
        SystemProtocol protocol;
        std::memcpy(&protocol.userId, bytes.data(), sizeof(protocol.userId));
        std::memcpy(&protocol.side, bytes.data() + 4, sizeof(protocol.side));
        std::memcpy(&protocol.padding, bytes.data() + 5, sizeof(protocol.padding));
        std::memcpy(&protocol.price, bytes.data() + 6, sizeof(protocol.price));
        std::memcpy(&protocol.quantity, bytes.data() + 14, sizeof(protocol.quantity));
        std::memcpy(&protocol.timestamp, bytes.data() + 16, sizeof(protocol.timestamp));
        std::memcpy(&protocol.seq_number, bytes.data() + 24, sizeof(protocol.seq_number));
        std::memcpy(&protocol.transaction_id, bytes.data() + 32, sizeof(protocol.transaction_id));
        std::memcpy(&protocol.checksum, bytes.data() + 40, sizeof(protocol.checksum));
        
        return protocol;
    }
};
