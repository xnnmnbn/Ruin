#include "ri_renderer.h"
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include "ri_assets.h"
#include "ruin.h"


VkShaderModule create_shader_module(RI_Renderer *r, unsigned char *code, unsigned int len) {
    VkShaderModule m = {0};
    VkShaderModuleCreateInfo mi = {0};

    mi.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    mi.pCode    = (uint32_t*)code;
    mi.codeSize = len;

    if (vkCreateShaderModule(r->core.device, &mi, NULL, &m) != VK_SUCCESS) {
        printf("Failed to create shader module.\n");
        return m;
    }

    return m;
}

void ri_renderer_create_pipeline_test(RI_Renderer *r) {

    VkShaderModule vert_module = create_shader_module(r, test_vert_spv, test_vert_spv_len);
    VkShaderModule frag_module = create_shader_module(r, test_frag_spv, test_frag_spv_len);

    VkPipelineShaderStageCreateInfo v_shader = {0};
    VkPipelineShaderStageCreateInfo f_shader = {0};

    v_shader.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    v_shader.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    v_shader.module = vert_module;
    v_shader.pName  = "main";

    f_shader.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    f_shader.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    f_shader.module = frag_module;
    f_shader.pName  = "main";
    
    VkPipelineShaderStageCreateInfo shader_stages[2] = { v_shader, f_shader };

    VkDynamicState dynamic_states[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = {0};
    dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.pDynamicStates    = dynamic_states;
    dynamic_state.dynamicStateCount = 2;

    VkPipelineVertexInputStateCreateInfo input_state = {0};
    input_state.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    input_state.pVertexBindingDescriptions      = NULL;
    input_state.vertexBindingDescriptionCount   = 0;
    input_state.pVertexAttributeDescriptions    = NULL;
    input_state.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {0};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.width    = r->swapchain.swapchain_extent.width;
    viewport.height   = r->swapchain.swapchain_extent.height;

    VkRect2D scissor = {0};
    scissor.extent   = r->swapchain.swapchain_extent;
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pViewports    = &viewport;
    viewport_state.viewportCount = 1;
    viewport_state.pScissors     = &scissor;
    viewport_state.scissorCount  = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;              // Enable it for shadow map.
    rasterizer.rasterizerDiscardEnable = VK_FALSE;              // Never enable it.
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;  // Completely paint the objects.
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT; // Disable rendering back faces.
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp          = 0.0f;
    rasterizer.depthBiasSlopeFactor    = 0.0f;
    rasterizer.lineWidth               = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable   = VK_FALSE;
    multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading      = 1.0f;
    multisampling.pSampleMask           = NULL;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable      = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT |
                                                 VK_COLOR_COMPONENT_G_BIT |
                                                 VK_COLOR_COMPONENT_B_BIT |
                                                 VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable         = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable   = VK_FALSE;
    color_blending.logicOp         = VK_LOGIC_OP_COPY;
    color_blending.pAttachments    = &color_blend_attachment;
    color_blending.attachmentCount = 1;

    VkPipelineLayoutCreateInfo layout = {0};
    layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout.setLayoutCount = 0;
    layout.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(r->core.device, &layout, NULL, &r->pipelines.test_pipeline.layout) != VK_SUCCESS) {
        printf("Failed to create test pipeline layout.\n");
        return;
    }


    VkGraphicsPipelineCreateInfo pi = {0};
    pi.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pi.layout = r->pipelines.test_pipeline.layout;
    pi.renderPass = r->renderpasses.present_pass;
    pi.subpass = 0;
    pi.stageCount = 2;
    pi.pStages = shader_stages;
    pi.pVertexInputState = &input_state;
    pi.pInputAssemblyState = &input_assembly;
    pi.pViewportState = &viewport_state;
    pi.pRasterizationState = &rasterizer;
    pi.pMultisampleState = &multisampling;
    pi.pDepthStencilState = NULL;
    pi.pColorBlendState = &color_blending;
    pi.pDynamicState = &dynamic_state;
    pi.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(r->core.device, VK_NULL_HANDLE, 1, &pi, NULL, &r->pipelines.test_pipeline.pipeline) != VK_SUCCESS) {
        printf("Failed to create test graphics pipeline.\n");
        return;
    }

    r->pipelines.test_pipeline.bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    printf("Test graphics pipeline created.\n");

    vkDestroyShaderModule(r->core.device, vert_module, NULL);
    vkDestroyShaderModule(r->core.device, frag_module, NULL);
}










VkVertexInputBindingDescription vertex2d_binding_desc() {
    return (VkVertexInputBindingDescription){
        .binding   = 0,
        .stride    = sizeof(Vertex2D),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
}

Vertex2D_Attr vertex2d_attr_desc() {
    return (Vertex2D_Attr){
        .descs = {
            (VkVertexInputAttributeDescription) {
                .location = 0,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32_SFLOAT,
                .offset   = offsetof(Vertex2D, pos)
            },
            (VkVertexInputAttributeDescription) {
                .location = 1,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32_SFLOAT,
                .offset   = offsetof(Vertex2D, uv)
            }
        }
    };
}

void ri_renderer_create_pipeline_bindless_2d(RI_Renderer *r) {

    VkShaderModule vert_module = create_shader_module(r, bindless_2d_plain_vert_spv, bindless_2d_plain_vert_spv_len);
    VkShaderModule frag_module = create_shader_module(r, bindless_2d_plain_frag_spv, bindless_2d_plain_frag_spv_len);

    VkPipelineShaderStageCreateInfo v_shader = {0};
    VkPipelineShaderStageCreateInfo f_shader = {0};

    v_shader.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    v_shader.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    v_shader.module = vert_module;
    v_shader.pName  = "main";

    f_shader.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    f_shader.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    f_shader.module = frag_module;
    f_shader.pName  = "main";
    
    VkPipelineShaderStageCreateInfo shader_stages[2] = { v_shader, f_shader };

    VkDynamicState dynamic_states[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = {0};
    dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.pDynamicStates    = dynamic_states;
    dynamic_state.dynamicStateCount = 2;

    

    VkVertexInputBindingDescription binding_descs[] = {
        vertex2d_binding_desc()
    };

    Vertex2D_Attr attr = vertex2d_attr_desc();
    VkPipelineVertexInputStateCreateInfo input_state = {0};
    input_state.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    input_state.pVertexBindingDescriptions      = binding_descs;
    input_state.vertexBindingDescriptionCount   = 1;
    input_state.pVertexAttributeDescriptions    = attr.descs;
    input_state.vertexAttributeDescriptionCount = 2;

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {0};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.width    = r->swapchain.swapchain_extent.width;
    viewport.height   = r->swapchain.swapchain_extent.height;

    VkRect2D scissor = {0};
    scissor.extent   = r->swapchain.swapchain_extent;
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pViewports    = &viewport;
    viewport_state.viewportCount = 1;
    viewport_state.pScissors     = &scissor;
    viewport_state.scissorCount  = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;              // Enable it for shadow map.
    rasterizer.rasterizerDiscardEnable = VK_FALSE;              // Never enable it.
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;  // Completely paint the objects.
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT; // Disable rendering back faces.
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp          = 0.0f;
    rasterizer.depthBiasSlopeFactor    = 0.0f;
    rasterizer.lineWidth               = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable   = VK_FALSE;
    multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading      = 1.0f;
    multisampling.pSampleMask           = NULL;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable      = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT |
                                                 VK_COLOR_COMPONENT_G_BIT |
                                                 VK_COLOR_COMPONENT_B_BIT |
                                                 VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable         = VK_TRUE;  // For PNGs in 2D
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable   = VK_FALSE;
    color_blending.logicOp         = VK_LOGIC_OP_COPY;
    color_blending.pAttachments    = &color_blend_attachment;
    color_blending.attachmentCount = 1;

    const uint8_t set0_binding_count = 3;
    const uint8_t set1_binding_count = 1;

    VkDescriptorSetLayoutBinding set0_bindings[3] = {0};
    VkDescriptorSetLayoutBinding set1_bindings[1] = {0};

    for (uint8_t i = 0; i < set0_binding_count; i++) {
        set0_bindings[i].binding            = i;
        set0_bindings[i].descriptorCount    = 1;
        set0_bindings[i].pImmutableSamplers = NULL;
        set0_bindings[i].stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
    }

    set0_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    set0_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    set0_bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    set1_bindings->binding            = 0;
    set1_bindings->descriptorCount    = RUIN_MAX_TEXTURES;
    set1_bindings->pImmutableSamplers = NULL;
    set1_bindings->stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    set1_bindings->descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    r->pipelines.bindless_pipeline_2d.d_set_count = 2;
    uint8_t set_count = r->pipelines.bindless_pipeline_2d.d_set_count;



    VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                                     VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

    VkDescriptorSetLayoutBindingFlagsCreateInfo dslbfci = {0};
    dslbfci.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    dslbfci.bindingCount  = set1_binding_count;
    dslbfci.pBindingFlags = &flags;

    VkDescriptorSetLayoutCreateInfo dslis[2] = {0};
    dslis[0].pBindings    = set0_bindings;
    dslis[0].bindingCount = set0_binding_count;
    dslis[1].pBindings    = set1_bindings;
    dslis[1].bindingCount = set1_binding_count;
    dslis[1].pNext        = &dslbfci;
    dslis[1].flags        = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;


    for (uint8_t i = 0; i < set_count; i++) {
        dslis[i].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        if (vkCreateDescriptorSetLayout(r->core.device, &(dslis[i]), NULL, &(r->pipelines.bindless_pipeline_2d.d_set_layouts[i])) != VK_SUCCESS) {
            printf("Failed to create descriptor set layout #%d of bindless pipeline 2D.\n", i);
            return;
        }
    }

    VkPipelineLayoutCreateInfo layout = {0};
    layout.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout.pSetLayouts    = r->pipelines.bindless_pipeline_2d.d_set_layouts;
    layout.setLayoutCount = set_count;

    if (vkCreatePipelineLayout(r->core.device, &layout, NULL, &r->pipelines.bindless_pipeline_2d.layout) != VK_SUCCESS) {
        printf("Failed to create test pipeline layout.\n");
        return;
    }


    VkGraphicsPipelineCreateInfo pi = {0};
    pi.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pi.layout = r->pipelines.bindless_pipeline_2d.layout;
    pi.renderPass = r->renderpasses.present_pass;
    pi.subpass = 0;
    pi.stageCount = 2;
    pi.pStages = shader_stages;
    pi.pVertexInputState = &input_state;
    pi.pInputAssemblyState = &input_assembly;
    pi.pViewportState = &viewport_state;
    pi.pRasterizationState = &rasterizer;
    pi.pMultisampleState = &multisampling;
    pi.pDepthStencilState = NULL;
    pi.pColorBlendState = &color_blending;
    pi.pDynamicState = &dynamic_state;
    pi.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(r->core.device, VK_NULL_HANDLE, 1, &pi, NULL, &r->pipelines.bindless_pipeline_2d.pipeline) != VK_SUCCESS) {
        printf("Failed to create test graphics pipeline.\n");
        return;
    }

    r->pipelines.bindless_pipeline_2d.bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    printf("Bindless graphics pipeline created.\n");

    vkDestroyShaderModule(r->core.device, vert_module, NULL);
    vkDestroyShaderModule(r->core.device, frag_module, NULL);
}







