#pragma once
#include <cstdint>
#include <array>

struct ClientProtocol{ // 24 bytes
    uint32_t userId;
    bool side, padding;
    uint64_t price;
    int16_t quantity;
    uint64_t timestamp;

    ClientProtocol();
    ClientProtocol(uint32_t userId,
    bool side, bool padding,
    uint64_t price,
    int16_t quantity,
    uint64_t timestamp);

    std::array<u_int8_t, 24> as_bytes() const;
    static ClientProtocol from_bytes(const std::array<u_int8_t, 24>& bytes);
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
    
    SystemProtocol();
    SystemProtocol(uint32_t userId,
    bool side, bool padding,
    uint64_t price,
    int16_t quantity,
    int64_t timestamp,
    uint64_t seq_number,
    uint64_t transaction_id,
    uint32_t checksum);

    std::array<u_int8_t, 44> as_bytes() const;
    static SystemProtocol from_bytes(const std::array<u_int8_t, 44>& bytes);
};
