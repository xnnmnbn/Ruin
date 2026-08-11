#include "ri_renderer.h"
#include <stdio.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

void ri_renderer_create_renderpass_present(RI_Renderer *r) {
    VkAttachmentDescription color_attachment = {0};
    color_attachment.format         = r->swapchain.swapchain_image_format;
    color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {0};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint    = r->pipelines.test_pipeline.bind_point;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &color_attachment_ref;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo ri = {0};
    ri.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    ri.pSubpasses      = &subpass;
    ri.subpassCount    = 1;
    ri.pAttachments    = &color_attachment;
    ri.attachmentCount = 1;
    ri.pDependencies   = &dependency;
    ri.dependencyCount = 1;


    if (vkCreateRenderPass(r->core.device, &ri, NULL, &r->renderpasses.present_pass) != VK_SUCCESS) {
        printf("Failed to create presentation renderpass.\n");
        return;
    }

    printf("Presentation renderpass created.\n");
}



void ri_renderer_create_renderpass_offscreen(RI_Renderer *r) {
    VkAttachmentDescription color_attachment = {0};
    color_attachment.format         = r->swapchain.swapchain_image_format;
    color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference color_attachment_ref = {0};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint    = r->pipelines.bindless_offscreen_2d.bind_point;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &color_attachment_ref;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo ri = {0};
    ri.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    ri.pSubpasses      = &subpass;
    ri.subpassCount    = 1;
    ri.pAttachments    = &color_attachment;
    ri.attachmentCount = 1;
    ri.pDependencies   = &dependency;
    ri.dependencyCount = 1;


    if (vkCreateRenderPass(r->core.device, &ri, NULL, &r->renderpasses.offscreen_pass) != VK_SUCCESS) {
        printf("Failed to create offscreen renderpass.\n");
        return;
    }

    printf("Offscreen renderpass created.\n");
}
