#ifndef PARALLEL_QUEUEFAMILYINDICES_H
#define PARALLEL_QUEUEFAMILYINDICES_H

#include <cstdint>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() and presentationFamily.has_value();
    }
};


#endif //PARALLEL_QUEUEFAMILYINDICES_H
