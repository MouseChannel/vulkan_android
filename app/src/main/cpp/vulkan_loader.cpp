//
// Created by Admin on 2023/8/24.
//
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_android.h>
#include <optional>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include <memory>
#include <vector>
#include <initializer_list>
#include <string>
#include <android/log.h>

static const char *kTAG = "mocheng";
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))


class VulkanLoader {
public:
    VulkanLoader() = default;

    ~VulkanLoader() = default;

    void Init(android_app *app);

    void Render_Loop();

    bool isready{false};

private:
    android_app *m_app;
    vk::Instance m_instance;
    vk::Device m_device;
    vk::SurfaceKHR m_surface{VK_NULL_HANDLE};
    std::optional<uint32_t> graphic_queue_id, present_queue_id;

    vk::Queue graphic_queue, present_queue;
    vk::PhysicalDevice m_gpu;
    struct VulkanSwapchainInfo {
        vk::SwapchainKHR swapchain_;
        uint32_t swapchainLength_;

        vk::Extent2D displaySize_;
        vk::Format displayFormat_;

        // array of frame buffers and views
        std::vector<vk::Image> displayImages_;
        std::vector<vk::ImageView> displayViews_;
        std::vector<vk::Framebuffer> framebuffers_;
    };
    VulkanSwapchainInfo swapchain;
    vk::SwapchainKHR m_swapchain;
    vk::Format m_swapchain_format;
    vk::Extent2D m_swapchain_extent;
    std::vector<vk::Image> m_swapchain_images;
    std::vector<vk::ImageView> m_swapchain_image_views;
    vk::Image m_swapchain_image;
    vk::ImageView m_swapchain_image_view;

    vk::CommandPool m_command_pool;
    std::vector<vk::CommandBuffer> m_command_buffers;
//    vk::CommandBuffer m_command_buffer;
    vk::RenderPass m_renderpass;
    vk::Pipeline m_pipeline;
    vk::ShaderModule vert_shader, frag_shader;
    vk::Framebuffer m_framebuffer;
    std::vector<vk::Framebuffer> m_framebuffers;
    vk::Buffer vertex_buffer;
    vk::DeviceMemory vertex_memory;
    vk::Buffer indices_buffer;
    std::vector<vk::Semaphore> m_render_semaphores;
    std::vector<vk::Semaphore> m_present_semaphores;
    vk::Semaphore m_semaphore;
    uint32_t current_index = 0;

    void init_instance();

    void init_device();

    void init_surface(ANativeWindow *android_window);

    void init_swapchain();

    void init_command_buffer();

    void init_shader();

    void init_pipeline();

    void init_framebuffer();

    void init_buffer();

    void init_semhore();

    void record_command_buffer();
};

void VulkanLoader::Init(android_app *app) {
    m_app = app;
    init_instance();
    init_surface(app->window);
    init_device();
    init_swapchain();
    init_command_buffer();
    init_shader();
    init_pipeline();
    init_framebuffer();
    init_buffer();
    init_semhore();
    record_command_buffer();
    isready = true;
}

void VulkanLoader::init_instance() {

    auto rr = vk::enumerateInstanceLayerProperties();
    vk::ApplicationInfo app_info;
    app_info.setApiVersion(VK_API_VERSION_1_1);
    std::vector<const char *> vaild_layer = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char *> vaild_extension = {"VK_KHR_surface", "VK_KHR_android_surface"};

    auto instance_ci = vk::InstanceCreateInfo()
            .setPApplicationInfo(&app_info)
            .setPEnabledLayerNames(vaild_layer)
            .setPEnabledExtensionNames(vaild_extension);

    m_instance = vk::createInstance(instance_ci);
    auto ee = m_instance.enumeratePhysicalDevices();
    int eee = 0;

}

void VulkanLoader::init_surface(ANativeWindow *android_window) {


    auto vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(vkGetInstanceProcAddr(
            m_instance, "vkCreateAndroidSurfaceKHR"));
    vk::AndroidSurfaceCreateInfoKHR android_surface_ci = vk::AndroidSurfaceCreateInfoKHR()
            .setWindow(android_window);

    vkCreateAndroidSurfaceKHR(m_instance, (VkAndroidSurfaceCreateInfoKHR *) &android_surface_ci,
                              nullptr, (VkSurfaceKHR *) &m_surface);

//    m_surface = m_instance.createAndroidSurfaceKHR(android_surface_ci);
    int a = 0;
}

void VulkanLoader::init_device() {
    auto gpus = m_instance.enumeratePhysicalDevices();
    m_gpu = gpus[0];
    auto properties = m_gpu.getQueueFamilyProperties();

    {
        for (int i = 0; i < properties.size(); i++) {
            auto property = properties[i];
            if (property.queueFlags | vk::QueueFlagBits::eGraphics) {
                graphic_queue_id = i;
            }
            if (m_gpu.getSurfaceSupportKHR(i, m_surface)) {
                present_queue_id = i;
                if (property.queueFlags | vk::QueueFlagBits::eGraphics) {
                    graphic_queue_id = i;
                }
            }
            if (graphic_queue_id.has_value() && present_queue_id.has_value()) {
                break;
            }
        }
    }
    vk::DeviceQueueCreateInfo queue_create_info;

    float priorities = 1.0f;


    queue_create_info.setPQueuePriorities(&priorities)
            .setQueueCount(1)
            .setQueueFamilyIndex(graphic_queue_id.value());
    const std::vector<const char *> deviceRequiredExtensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    vk::DeviceCreateInfo create_info;
    create_info.setQueueCreateInfos(queue_create_info)
            .setPEnabledExtensionNames(deviceRequiredExtensions);
    m_device = m_gpu.createDevice(create_info);

    graphic_queue = m_device.getQueue(graphic_queue_id.value(), 0);
    present_queue = m_device.getQueue(present_queue_id.value(), 0);


}

void VulkanLoader::init_swapchain() {

    auto surfaceCapabilities = m_gpu.getSurfaceCapabilitiesKHR(m_surface);
    m_swapchain_extent = surfaceCapabilities.currentExtent;
    auto formats = m_gpu.getSurfaceFormatsKHR(m_surface);


//
    uint32_t chosenFormat;
    for (chosenFormat = 0; chosenFormat < formats.size(); chosenFormat++) {
        if (formats[chosenFormat].format == vk::Format::eR8G8B8A8Unorm) break;
    }
    m_swapchain_format
            = formats[chosenFormat].format;
    assert(chosenFormat < formats.size());

    auto swapchain_count = std::clamp(surfaceCapabilities.minImageCount + 1,
                                      surfaceCapabilities.minImageCount,
                                      surfaceCapabilities.maxImageCount);
    vk::SwapchainCreateInfoKHR swapchain_ci;
    swapchain_ci.setClipped(true)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eInherit)
            .setImageExtent(surfaceCapabilities.currentExtent)
            .setImageColorSpace(formats[chosenFormat].colorSpace)
            .setImageFormat(formats[chosenFormat].format)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setMinImageCount(swapchain_count)
            .setImageArrayLayers(1)
            .setPresentMode(vk::PresentModeKHR::eFifo)
            .setPreTransform(surfaceCapabilities.currentTransform)
            .setSurface(m_surface)
            .setQueueFamilyIndices(graphic_queue_id.value())
            .setImageSharingMode(graphic_queue == present_queue ? vk::SharingMode::eConcurrent
                                                                : vk::SharingMode::eExclusive);
    m_swapchain = m_device.createSwapchainKHR(swapchain_ci);

    m_swapchain_images = m_device.getSwapchainImagesKHR(m_swapchain);

    for (int i = 0; i < m_swapchain_images.size(); i++) {

        vk::ImageSubresourceRange range;
        range.setBaseMipLevel(0)
                .setLevelCount(1)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setAspectMask(vk::ImageAspectFlagBits::eColor);
        vk::ImageViewCreateInfo view_create_info;
        view_create_info.setImage(m_swapchain_images[i])
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(m_swapchain_format)
                .setSubresourceRange(range);
        m_swapchain_image_views.emplace_back(m_device.createImageView(view_create_info));
    }


}

void VulkanLoader::init_command_buffer() {
    vk::CommandPoolCreateInfo command_pool_ci;
    command_pool_ci.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    m_command_pool = m_device.createCommandPool(command_pool_ci);


    vk::CommandBufferAllocateInfo command_buffer_ai;
    command_buffer_ai.setCommandPool(m_command_pool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(m_swapchain_images.size());

    m_command_buffers = m_device.allocateCommandBuffers(command_buffer_ai);

}

void VulkanLoader::init_shader() {
    auto load_shader = [&](char *file_path) {
//        std::string file_path = "s";
        AAsset *file = AAssetManager_open(m_app->activity->assetManager,
                                          file_path, AASSET_MODE_BUFFER);
//        AAsset *file1 = AAssetManager_open(m_app->activity->assetManager,
//                                          "shaders/vert.vert", AASSET_MODE_STREAMING);
//
//        AAssetDir *dirr = AAssetManager_openDir(m_app->activity->assetManager,
//                                          "shaders" );
//        auto rr = AAssetDir_getNextFileName(dirr);

        size_t fileLength = AAsset_getLength(file);

        char *fileContent = new char[fileLength];
        std::vector<uint32_t> shader_code;
        shader_code.resize(fileLength / 4);


        AAsset_read(file, shader_code.data(), fileLength);
        AAsset_close(file);
        vk::ShaderModuleCreateInfo shader_ci;
        shader_ci.setCodeSize(fileLength)
                .setCode(shader_code);
        return m_device.createShaderModule(shader_ci);

    };
    vert_shader = load_shader("shaders/vert.vert.spv");
    frag_shader = load_shader("shaders/frag.frag.spv");
}

void VulkanLoader::init_framebuffer() {
    for (int i = 0; i < m_swapchain_images.size(); i++) {

        vk::FramebufferCreateInfo framebuffer_ci;
        framebuffer_ci.setRenderPass(m_renderpass)
                .setAttachments(m_swapchain_image_views[i])
                .setWidth(m_swapchain_extent.width)
                .setHeight(m_swapchain_extent.height)
                .setLayers(1);
        m_framebuffers.emplace_back(m_device.createFramebuffer(framebuffer_ci));
    }
//    m_framebuffer = m_device.createFramebuffer(framebuffer_ci);
}

void VulkanLoader::init_pipeline() {
    vk::SubpassDescription subpass;
    subpass.setColorAttachments(vk::AttachmentReference()
                                        .setAttachment(0)
                                        .setLayout(vk::ImageLayout::eColorAttachmentOptimal));
    vk::SubpassDependency dependency;
    dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests |
                             vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite |
                              vk::AccessFlagBits::eColorAttachmentWrite);
    vk::RenderPassCreateInfo renderpass_ci;
    renderpass_ci.setSubpasses(subpass)
            .setDependencies(dependency)
            .setAttachments(vk::AttachmentDescription()
                                    .setFormat(m_swapchain_format)
                                    .setSamples(vk::SampleCountFlagBits::e1)
                                    .setLoadOp(vk::AttachmentLoadOp::eClear)
                                    .setStoreOp(vk::AttachmentStoreOp::eStore)
                                    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                                    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                                    .setInitialLayout(vk::ImageLayout::eUndefined)
                                    .setFinalLayout(vk::ImageLayout::ePresentSrcKHR));
    m_renderpass = m_device.createRenderPass(renderpass_ci);
    int r = 0;


    vk::PipelineColorBlendStateCreateInfo ci;
    ci.setAttachments(
            vk::PipelineColorBlendAttachmentState().setBlendEnable(true).setColorWriteMask(
                            vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB |
                            vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR)
                    .setSrcColorBlendFactor(vk::BlendFactor::eOne)
                    .setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
                    .setColorBlendOp(vk::BlendOp::eAdd)
                    .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
                    .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                    .setAlphaBlendOp(vk::BlendOp::eAdd)).setLogicOpEnable(false);
    vk::PipelineInputAssemblyStateCreateInfo input_assemble;
    input_assemble.setPrimitiveRestartEnable(false)
            .setTopology(vk::PrimitiveTopology::eTriangleList);
    vk::PipelineViewportStateCreateInfo viewport_ci;
    viewport_ci.setViewports(vk::Viewport().setHeight(m_swapchain_extent.height)
                                     .setWidth(m_swapchain_extent.width)
                                     .setX(0)
                                     .setY(0)
                                     .setMinDepth(0)
                                     .setMaxDepth(1))
            .setScissors(vk::Rect2D().setExtent(m_swapchain_extent)
                                 .setOffset(vk::Offset2D()
                                                    .setY(0)
                                                    .setX(0)));
    vk::PipelineMultisampleStateCreateInfo mult_ci;
    mult_ci.setSampleShadingEnable(false)
            .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    vk::PipelineRasterizationStateCreateInfo rasterization_ci;
    rasterization_ci.setCullMode(vk::CullModeFlagBits::eBack)
            .setFrontFace(vk::FrontFace::eClockwise)
            .setLineWidth(1)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setRasterizerDiscardEnable(false);
    vk::PipelineDepthStencilStateCreateInfo depth_ci;
    depth_ci.setDepthTestEnable(true)
            .setDepthWriteEnable(true)
            .setDepthCompareOp(vk::CompareOp::eLessOrEqual);


    std::vector<vk::PipelineShaderStageCreateInfo> shaderss{
            vk::PipelineShaderStageCreateInfo().setModule(vert_shader)
                    .setPName("main")
                    .setStage(vk::ShaderStageFlagBits::eVertex),
            vk::PipelineShaderStageCreateInfo().setModule(frag_shader)
                    .setPName("main")
                    .setStage(vk::ShaderStageFlagBits::eFragment)};
    vk::PipelineVertexInputStateCreateInfo input_ci;
    input_ci.setVertexAttributeDescriptions(vk::VertexInputAttributeDescription().setOffset(0)
                                                    .setFormat(vk::Format::eR32G32B32Sfloat)
                                                    .setBinding(0)
                                                    .setLocation(0))
            .setVertexBindingDescriptions(vk::VertexInputBindingDescription().setBinding(0)
                                                  .setInputRate(vk::VertexInputRate::eVertex)
                                                  .setStride(sizeof(float) * 3));
    vk::PipelineLayoutCreateInfo layout_create_ci;
//    layout_create_ci.se
    auto m_pipeline_layout = m_device.createPipelineLayout(layout_create_ci);
    {
        vk::GraphicsPipelineCreateInfo pipeline_ci;
        pipeline_ci
                .setPVertexInputState(&input_ci)
                .setPColorBlendState(&ci)
                .setPInputAssemblyState(&input_assemble)
                .setPViewportState(&viewport_ci)
                .setPMultisampleState(&mult_ci)
                .setPRasterizationState(&rasterization_ci)
                .setPDepthStencilState(&depth_ci)
                .setRenderPass(m_renderpass)
                .setLayout(m_pipeline_layout)
                .setStages(shaderss);
        auto value = m_device.createGraphicsPipeline(nullptr, pipeline_ci);
        if (value.result == vk::Result::eSuccess) { m_pipeline = value.value; }
        int r = 0;

    }


}

void VulkanLoader::init_buffer() {
    std::vector<float> vertexdata{0};

    vk::BufferCreateInfo buffer_ci;
    buffer_ci.setSize(vertexdata.size() * sizeof(vertexdata[0]))
            .setUsage(
                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer)
            .setSharingMode(vk::SharingMode::eExclusive);
    vertex_buffer = m_device.createBuffer(buffer_ci);
    struct MemoryInfo {
        size_t size;
        uint32_t index;
    } info;
//    MemoryInfo info;
    {
        auto property = vk::MemoryPropertyFlagBits::eHostVisible;
        auto requirements = m_device
                .getBufferMemoryRequirements(vertex_buffer);
        info.size = requirements.size;
        auto properties = m_gpu
                .getMemoryProperties();

        for (int i = 0; i < properties.memoryTypeCount; i++) {
            // satisfy both buffer memory_property AND asked memory_property
            if ((1 << i) & requirements.memoryTypeBits &&
                properties.memoryTypes[i].propertyFlags & property) {
                info.index = i;
                break;
            }
        }
    }
    vk::MemoryAllocateInfo allocate_info;
    allocate_info.setMemoryTypeIndex(info.index)
            .setAllocationSize(info.size);
    vertex_memory = m_device.allocateMemory(
            allocate_info);
    m_device.bindBufferMemory(vertex_buffer, vertex_memory, 0);


}

void VulkanLoader::init_semhore() {
    for (int i = 0; i < m_swapchain_images.size(); i++) {

//        m_semaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo());
        m_render_semaphores.emplace_back(m_device.createSemaphore(vk::SemaphoreCreateInfo()));
        m_present_semaphores.emplace_back(m_device.createSemaphore(vk::SemaphoreCreateInfo()));
    }
}

void VulkanLoader::record_command_buffer() {
    for (int i = 0; i < m_command_buffers.size(); i++) {
        auto m_command_buffer = m_command_buffers[i];

        m_command_buffer.reset();
        m_command_buffer.begin(vk::CommandBufferBeginInfo()
                                       .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
        m_command_buffer.beginRenderPass(vk::RenderPassBeginInfo()
                                                 .setRenderPass(m_renderpass)
                                                 .setRenderArea(vk::Rect2D()
                                                                        .setOffset({0, 0})
                                                                        .setExtent(
                                                                                m_swapchain_extent))
                                                 .setFramebuffer(m_framebuffers[i])
                                                 .setClearValues(vk::ClearValue()
                                                                         .setColor({0.1f,
                                                                                    0.1f,
                                                                                    0.1f,
                                                                                    1.f})),
                                         vk::SubpassContents::eInline);
        m_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        m_command_buffer.bindVertexBuffers(0, vertex_buffer, vk::DeviceSize());
        m_command_buffer.draw(3, 1, 0, 0);

        m_command_buffer.endRenderPass();
        m_command_buffer.end();

    }

}

void VulkanLoader::Render_Loop() {

    auto result = m_device.acquireNextImageKHR(m_swapchain, std::numeric_limits<uint64_t>::max(),
                                               m_render_semaphores[current_index]);
    uint32_t image_index;
    if (result.result == vk::Result::eSuccess) {
        LOGE("acquireNextImage success");
        image_index = result.value;
    }
    {//submit
        vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        graphic_queue.submit(vk::SubmitInfo()
                                     .setCommandBuffers(m_command_buffers[current_index])
                                     .setWaitSemaphores(m_render_semaphores[current_index])
                                     .setWaitDstStageMask(wait_stage)
                                     .setSignalSemaphores(m_present_semaphores[current_index]));


    }
    vk::PresentInfoKHR present_info;
    present_info.setImageIndices(image_index)
            .setSwapchains(m_swapchain)
            .setWaitSemaphores(m_present_semaphores[current_index]);

    m_device.waitIdle();

    auto present_result = present_queue.presentKHR(present_info);
    if (present_result != vk::Result::eSuccess) {
        LOGE("render fail");
    }
    current_index++;
    current_index %= m_swapchain_images.size();


}
