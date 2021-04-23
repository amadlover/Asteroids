#include "vk_descriptor_sets.hpp"

#include "error.hpp"

vk_descriptor_set::vk_descriptor_set (
    const VkDevice& device,
    const VkDescriptorPool& descriptor_pool,
    const VkDescriptorSetLayout& descriptor_set_layout) : device (device), descriptor_pool (descriptor_pool)
{
    printf ("vk_descriptor_set::vk_descriptor_set\n");

    VkDescriptorSetAllocateInfo allocate_info = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr,
        descriptor_pool,
        1,
        &descriptor_set_layout
    };

    VkResult result = vkAllocateDescriptorSets (
        device, &allocate_info, &descriptor_set
    );

    if (result != VK_SUCCESS)
    {
        throw AGE_RESULT::ERROR_GRAPHICS_ALLOCATE_DESCRIPTOR_SETS;
    }
}

vk_descriptor_set::vk_descriptor_set (vk_descriptor_set&& other) noexcept
{
    printf ("vk_descriptor_set move ctor\n");

    *this = std::move (other);
}

vk_descriptor_set& vk_descriptor_set::operator=(vk_descriptor_set&& other) noexcept
{
    printf ("vk_descriptor_set move assignment\n");

    descriptor_set = other.descriptor_set;
    descriptor_pool = other.descriptor_pool;
    device = other.device;

    other.descriptor_set = VK_NULL_HANDLE;
    other.descriptor_pool = VK_NULL_HANDLE;
    other.device = VK_NULL_HANDLE;

    return *this;
}

vk_descriptor_set::~vk_descriptor_set () noexcept
{
    printf ("vk_descriptor_set::~vk_descriptor_set\n");

    if (descriptor_set != VK_NULL_HANDLE &&
        descriptor_pool != VK_NULL_HANDLE &&
        device != VK_NULL_HANDLE)
    {
        vkFreeDescriptorSets (device, descriptor_pool, 1, &descriptor_set);
    }
}

void vk_descriptor_set::update (const VkWriteDescriptorSet descriptor_set_write) const
{
    vkUpdateDescriptorSets (
        device,
        1,
        &descriptor_set_write,
        0,
        0
    );
}

vk_descriptor_sets::vk_descriptor_sets (
    const VkDevice& device,
    const VkDescriptorPool& descriptor_pool,
    const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) : device (device), descriptor_pool (descriptor_pool)
{
    printf ("vk_descriptor_sets::vk_descriptor_sets\n");

    VkDescriptorSetAllocateInfo allocate_info = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		descriptor_pool,
		static_cast<uint32_t> (descriptor_set_layouts.size ()),
		descriptor_set_layouts.data ()
    };

    descriptor_sets.resize (descriptor_set_layouts.size ());

    VkResult result = vkAllocateDescriptorSets (
        device, &allocate_info, descriptor_sets.data ()
    );

    if (result != VK_SUCCESS)
    {
        throw AGE_RESULT::ERROR_GRAPHICS_ALLOCATE_DESCRIPTOR_SETS;
    }
}

vk_descriptor_sets::vk_descriptor_sets (vk_descriptor_sets&& other) noexcept
{
    printf ("vk_descriptor_sets move ctor\n");

    *this = std::move (other);
}

vk_descriptor_sets& vk_descriptor_sets::operator= (vk_descriptor_sets&& other) noexcept
{
    printf ("vk_descriptor_sets move assignment\n");

    descriptor_sets = other.descriptor_sets;
    descriptor_pool = other.descriptor_pool;
    device = other.device;

    other.descriptor_sets.clear ();
    other.descriptor_pool = VK_NULL_HANDLE;
    other.device = VK_NULL_HANDLE;

    return *this;
}

vk_descriptor_sets::~vk_descriptor_sets () noexcept
{
    printf ("vk_descriptor_sets::~vk_descriptor_sets\n");

    if (descriptor_sets.size () > 0 && 
        descriptor_pool != VK_NULL_HANDLE &&
        device != VK_NULL_HANDLE)
    {
        vkFreeDescriptorSets (
            device, descriptor_pool, 
            static_cast<uint32_t> (descriptor_sets.size ()), descriptor_sets.data ()
        );
    }
}

void vk_descriptor_sets::update (const std::vector<VkDescriptorImageInfo>& descriptor_image_infos) const
{
    VkWriteDescriptorSet descriptor_set_write = {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		descriptor_sets[0],
		0,
		0,
		static_cast<uint32_t> (descriptor_image_infos.size ()),
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		descriptor_image_infos.data (),
		nullptr,
		nullptr
    };

    vkUpdateDescriptorSets (device, 1, &descriptor_set_write,0, nullptr);
}