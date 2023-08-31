#pragma once
//#include "Rendering/GLFW_Window.hpp"
#include "Wrapper/Base.hpp"
#include "Wrapper/Instance.hpp"

#include <memory>
#include <optional>
#include <sys/types.h>
#include <vulkan/vulkan_handles.hpp>
namespace MoCheng3D {
class Device : public Component<vk::Device, Device> {

public:
    Device();
    ~Device();
    const std::vector<const char*> deviceRequiredExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_MAINTENANCE1_EXTENSION_NAME
    };
    [[nodiscard("missing physical device")]] auto Get_Physical_device()
    {
        return physical_device;
    }
    [[nodiscard("Missing graphic_queue")]] auto Get_Graphic_queue()
    {
        return graphic_queue;
    }
    [[nodiscard("Missing present_queue")]] auto Get_present_queue()
    {
        return present_queue;
    }
    [[nodiscard("missing queue_family_index")]] auto get_queuefamily_index() { return queue_family_indices; }
    [[nodiscard]] vk::SampleCountFlagBits Get_sampler_count();
    [[nodiscard]] vk::Format Get_supported_format(std::vector<vk::Format> candidates, vk::ImageTiling tiling, vk::FormatFeatureFlagBits feature);
    struct QueueFamilyIndices final {
        std::optional<uint32_t> graphic_queue;
        std::optional<uint32_t> present_queue;
        bool Complete()
        {
            return graphic_queue.has_value() && present_queue.has_value();
        }
    } queue_family_indices;
    //   QueueFamilyIndices queue_family_indices;
private:
    void QueryQueueFamilyIndices();

    vk::PhysicalDevice physical_device;
    vk::Queue graphic_queue;
    vk::Queue present_queue;
    vk::SampleCountFlagBits sampler_count;
    vk::FormatProperties supported_formats;
};
} // namespace MoCheng3D