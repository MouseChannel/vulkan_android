#pragma once

#include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
#include "Wrapper/RenderPass.hpp"

namespace MoCheng3D {
class Pipeline : public Component<vk::Pipeline, Pipeline> {
public:
    Pipeline();
    ~Pipeline();
    void Make_Layout(vk::DescriptorSetLayout descriptor_layout, uint32_t push_constants_size, vk::ShaderStageFlags push_constants_stage);

    void Make_VertexInput(vk::ArrayProxyNoTemporaries<const vk::VertexInputBindingDescription> const& bind,
        vk::ArrayProxyNoTemporaries<const vk::VertexInputAttributeDescription> const& attr);
    void Make_VertexAssembly();
    void Make_viewPort();
    void Add_Shader_Modules(vk::ShaderModule shader_module, vk::ShaderStageFlagBits stage);
    void Make_Resterization();
    void Make_MultiSample();
    void Make_DepthTest();
    void Make_Blend();
    void Make_attach();

    void Build_Pipeline(std::shared_ptr<RenderPass> render_pass);
    [[nodiscard("Missing Layout")]] auto GetLayout() { return layout; }

private:
    vk::PipelineVertexInputStateCreateInfo input_state;

    vk::PipelineInputAssemblyStateCreateInfo input_assembly;

    vk::PipelineViewportStateCreateInfo viewportInfo;
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stage;
    vk::PipelineRasterizationStateCreateInfo rasterization_info;
    vk::PipelineMultisampleStateCreateInfo multi_sample;
    vk::PipelineDepthStencilStateCreateInfo depth_test;
    vk::PipelineColorBlendStateCreateInfo blend;
    std::vector<vk::PipelineColorBlendAttachmentState> attachs;
    vk::PipelineLayout layout;

    vk::Viewport viewport;
    vk::Rect2D scissor;
};
} // namespace MoCheng3D