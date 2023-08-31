#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>

namespace MoCheng3D {

class Buffer;
class Texture;
class Model {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 nrm;
        glm::vec3 color;

        glm::vec2 texCoord;
#ifdef __cplusplus

        static vk::VertexInputBindingDescription make_bind()
        {
            return vk::VertexInputBindingDescription()
                .setInputRate(vk::VertexInputRate ::eVertex)
                .setBinding(0)
                .setStride(sizeof(Vertex));
        }
        static std::vector<vk::VertexInputAttributeDescription> make_attr()
        {
            std::vector<vk::VertexInputAttributeDescription> res_attr(4);
            enum {
                e_pos = 0,
                e_nrm = 1,
                e_color = 2,
                // e_texture_index = 3,
                e_texCoord = 3
            };
            res_attr[e_pos]
                = vk::VertexInputAttributeDescription()
                      .setBinding(0)
                      .setLocation(e_pos)
                      .setFormat(vk::Format ::eR32G32B32Sfloat)
                      .setOffset(offsetof(Vertex, pos));
            res_attr[e_nrm] = vk::VertexInputAttributeDescription()
                                  .setBinding(0)
                                  .setLocation(e_nrm)
                                  .setFormat(vk::Format ::eR32G32B32Sfloat)
                                  .setOffset(offsetof(Vertex, nrm));
            res_attr[e_color] = vk::VertexInputAttributeDescription()
                                    .setBinding(0)
                                    .setLocation(e_color)
                                    .setFormat(vk::Format ::eR32G32B32Sfloat)
                                    .setOffset(offsetof(Vertex, color));
            // res_attr[e_texture_index] = vk::VertexInputAttributeDescription()
            //                                 .setBinding(0)
            //                                 .setLocation(e_texture_index)
            //                                 .setFormat(vk::Format ::eR32Sint)
            //                                 .setOffset(offsetof(Vertex, texture_index));
            res_attr[e_texCoord] = vk::VertexInputAttributeDescription()
                                       .setBinding(0)
                                       .setLocation(e_texCoord)
                                       .setFormat(vk::Format ::eR32G32Sfloat)
                                       .setOffset(offsetof(Vertex, texCoord));
            return res_attr;
        }

#endif
    };
    Model(std::string_view model_path, std::string_view texture_path);
    ~Model() = default;
    Model(std::vector<Vertex> vertexs,
        std::vector<uint32_t> indices);
    [[nodiscard("missing attr")]] auto& Get_Attr() { return vertex_attrs; }
    [[nodiscard("missing binding")]] auto& Get_Binding()
    {
        return vertex_bindings;
    }
    [[nodiscard("missing m_matrix")]] auto& Get_m_matrix() { return m_matrix; }
    [[nodiscard("missing index_buffer")]] auto Get_index_buffer()
    {
        return m_index_buffer;
    }
    [[nodiscard("Missing Texture")]] auto get_texture() { return texture; }
    // [[nodiscard("missing uv_buffer")]] auto Get_uv_buffer()
    // {
    //     return m_uv_buffer;
    // }
    // [[nodiscard("missing position_buffer")]] auto Get_position_buffer() { return m_position_buffer; }

    [[nodiscard]] auto Get_vertex_buffer()
    {
        // if (vertex_buffers.empty()) {
        //     vertex_buffers.push_back(m_position_buffer);
        //     vertex_buffers.push_back(m_uv_buffer);
        // }
        // return vertex_buffers;
        return std::vector { m_vertex_buffer };
    }
    [[nodiscard]] auto Get_index() { return index_size; }
    void Update();
    static std::vector<std::shared_ptr<Model>> meshs;

    // private:
    void Load_Model(std::string_view model_path);
    void Load_texture(std::string_view texture_path);
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Buffer> m_position_buffer;

    std::shared_ptr<Buffer> m_uv_buffer;
    std::shared_ptr<Buffer> m_index_buffer;
    std::shared_ptr<Buffer> m_vertex_buffer;
    std::vector<vk::VertexInputAttributeDescription> vertex_attrs;
    std::vector<vk::VertexInputBindingDescription> vertex_bindings;
    uint32_t index_size;

    glm::mat4 m_matrix
        // { glm::mat4(1.0f) };

        { glm::rotate((glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f))),
            glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) };
    std::vector<std::shared_ptr<Buffer>> vertex_buffers;
    float mAngle;
};

}