#pragma once
#include <iostream>
#include <array>
#include <atomic>
#include <cstring>
#include <vector>

//! @brief Defines protocol structures and serialization for client and system messages in the exchange.

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

struct BatchSystemProtocol{ // 1000 bytes
    // first byte contains the size of the batch
    std::array<uint8_t, 1000> batch;
    static inline std::atomic<uint64_t> transactionId;

    BatchSystemProtocol() : 
        batch() {}

    bool add_message(const SystemProtocol& message) {
        int sz = static_cast<int>(batch[0]);
        if (static_cast<size_t>((sz + 1) * 44) >= batch.size()) return false;
        int ind = sz * 44 + 1;
        batch[0] = static_cast<uint8_t>(++sz);
        std::memcpy(batch.data() + ind, &message.userId, sizeof(message.userId));
        std::memcpy(batch.data() + ind + 4, &message.side, sizeof(message.side));
        std::memcpy(batch.data() + ind + 5, &message.padding, sizeof(message.padding));
        std::memcpy(batch.data() + ind + 6, &message.price, sizeof(message.price));
        std::memcpy(batch.data() + ind + 14, &message.quantity, sizeof(message.quantity));
        std::memcpy(batch.data() + ind + 16, &message.timestamp, sizeof(message.timestamp));
        std::memcpy(batch.data() + ind + 24, &message.seq_number, sizeof(message.seq_number));
        std::memcpy(batch.data() + ind + 32, &message.transaction_id, sizeof(message.transaction_id));
        std::memcpy(batch.data() + ind + 40, &message.checksum, sizeof(message.checksum));
        
        return true;
    }

    SystemProtocol get_message(int index) const {
        if (index < 0 || index >= batch[0]) return SystemProtocol();
        
        int offset = index * 44 + 1;
        
        SystemProtocol message;
        std::memcpy(&message.userId, batch.data() + offset, sizeof(message.userId));
        std::memcpy(&message.side, batch.data() + offset + 4, sizeof(message.side));
        std::memcpy(&message.padding, batch.data() + offset + 5, sizeof(message.padding));
        std::memcpy(&message.price, batch.data() + offset + 6, sizeof(message.price));
        std::memcpy(&message.quantity, batch.data() + offset + 14, sizeof(message.quantity));
        std::memcpy(&message.timestamp, batch.data() + offset + 16, sizeof(message.timestamp));
        std::memcpy(&message.seq_number, batch.data() + offset + 24, sizeof(message.seq_number));
        std::memcpy(&message.transaction_id, batch.data() + offset + 32, sizeof(message.transaction_id));
        std::memcpy(&message.checksum, batch.data() + offset + 40, sizeof(message.checksum));
        return message;
    }

    std::vector<SystemProtocol> get_all_messages() const {
        std::vector<SystemProtocol> messages = {};
        for (int i = 0;i < batch[0]; i ++){
            messages.push_back(get_message(i));
        }
        return messages;
    }
};

inline uint32_t calculateChecksum(const SystemProtocol& message) {
    uint32_t sum = 0;
    
    sum += message.userId;
    sum += message.side ? 1 : 0;
    sum += message.padding ? 1 : 0;
    sum += static_cast<uint32_t>(message.price & 0xFFFFFFFF);
    sum += static_cast<uint32_t>((message.price >> 32) & 0xFFFFFFFF);
    sum += static_cast<uint32_t>(message.quantity);
    sum += static_cast<uint32_t>(message.timestamp & 0xFFFFFFFF);
    sum += static_cast<uint32_t>((message.timestamp >> 32) & 0xFFFFFFFF);
    sum += static_cast<uint32_t>(message.seq_number & 0xFFFFFFFF);
    sum += static_cast<uint32_t>((message.seq_number >> 32) & 0xFFFFFFFF);
    sum += static_cast<uint32_t>(message.transaction_id & 0xFFFFFFFF);
    sum += static_cast<uint32_t>((message.transaction_id >> 32) & 0xFFFFFFFF);
    
    return sum;
}

inline bool verifyChecksum(const SystemProtocol& message) {
    uint32_t expectedChecksum = calculateChecksum(message);
    return message.checksum == expectedChecksum;
}

inline SystemProtocol translate(ClientProtocol message, int seq_number) {
    uint64_t currentTransactionId = BatchSystemProtocol::transactionId.fetch_add(1);
    SystemProtocol dummy = SystemProtocol (
        message.userId,
        message.side,
        message.padding,
        message.price,
        message.quantity,
        message.timestamp,
        seq_number,
        currentTransactionId,
        1
    );
    uint32_t checksum = calculateChecksum(dummy);
    return SystemProtocol (
        message.userId,
        message.side,
        message.padding,
        message.price,
        message.quantity,
        message.timestamp,
        seq_number,
        currentTransactionId,
        checksum
    );
}