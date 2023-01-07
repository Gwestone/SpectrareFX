#pragma once

#include <cstdint>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() and presentationFamily.has_value();
    }
};