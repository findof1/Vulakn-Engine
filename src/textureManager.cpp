#include "textureManager.hpp"
#include "utils.h"
#include <stb_image.h>
#include <string>
#include <stdexcept>
#include "bufferManager.hpp"
#include "renderer.hpp"

void TextureManager::createTextureImageView(VkDevice device)
{
  textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, device);
}

void TextureManager::createTextTextureImageView(VkDevice device)
{
  textureImageView = createImageView(textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, device);
}

void TextureManager::createTextureImage(std::string texturePath, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
  int texWidth, texHeight, texChannels;

  stbi_uc *pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  if (!pixels)
  {
    throw std::runtime_error("failed to load texture image! Filepath: " + texturePath);
  }

  VkDeviceSize imageSize = texWidth * texHeight * 4;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  bufferManager.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device, physicalDevice);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(device, stagingBufferMemory);

  stbi_image_free(pixels);

  createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, device, physicalDevice);

  transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, device, commandPool, graphicsQueue);

  copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), device, commandPool, graphicsQueue);

  transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device, commandPool, graphicsQueue);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void TextureManager::createTextureImage(const FT_Bitmap &bitmap, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
  int texWidth = bitmap.width;
  int texHeight = bitmap.rows;
  VkDeviceSize imageSize = texWidth * texHeight;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  bufferManager.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device, physicalDevice);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, bitmap.buffer, static_cast<size_t>(imageSize));
  vkUnmapMemory(device, stagingBufferMemory);

  createImage(texWidth, texHeight, VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, device, physicalDevice);

  transitionImageLayout(textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, device, commandPool, graphicsQueue);

  copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), device, commandPool, graphicsQueue);

  transitionImageLayout(textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device, commandPool, graphicsQueue);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void TextureManager::createTextureImage(const std::vector<uint8_t> &textureData, int texWidth, int texHeight, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
  VkDeviceSize imageSize = texWidth * texHeight;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  bufferManager.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device, physicalDevice);
  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, textureData.data(), static_cast<size_t>(imageSize));
  vkUnmapMemory(device, stagingBufferMemory);

  createImage(texWidth, texHeight, VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, device, physicalDevice);

  transitionImageLayout(textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, device, commandPool, graphicsQueue);

  copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), device, commandPool, graphicsQueue);

  transitionImageLayout(textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device, commandPool, graphicsQueue);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void TextureManager::updateTexture(std::string newTexturePath, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
  /*
  VkSemaphoreWaitInfo waitInfo{};
  waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
  waitInfo.semaphoreCount = 1;
  waitInfo.pSemaphores = &renderer.textureUpdateSemaphore;
  uint64_t timeout = UINT64_MAX;
  waitInfo.pValues = &timeout;

  std::cout << "0" << std::endl;
  if (vkWaitSemaphores(device, &waitInfo, UINT64_MAX) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to wait on texture update semaphore!");
  }
*/

  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load(newTexturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  if (!pixels)
  {
    throw std::runtime_error("Failed to load new texture image: " + newTexturePath);
  }

  VkDeviceSize imageSize = texWidth * texHeight * 4;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  bufferManager.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer, stagingBufferMemory, device, physicalDevice);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(device, stagingBufferMemory);
  stbi_image_free(pixels);

  transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, device, commandPool, graphicsQueue);

  copyBufferToImage(stagingBuffer, textureImage, texWidth, texHeight, device, commandPool, graphicsQueue);

  transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device, commandPool, graphicsQueue);

  VkSemaphoreSignalInfo signalInfo = {};
  signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
  // signalInfo.semaphore = renderer.textureUpdateSemaphore;
  signalInfo.value = 0;

  VkResult result = vkSignalSemaphore(device, &signalInfo);

  if (result != VK_SUCCESS)
  {
    throw std::runtime_error("failed to signal texture update semaphore!");
  }

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);

  std::cout << "updated texture" << std::endl;
}

void TextureManager::createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice)
{
  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;
  if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

void TextureManager::cleanup(VkDevice device)
{

  if (textureSampler != VK_NULL_HANDLE)
  {
    vkDestroySampler(device, textureSampler, nullptr);
    textureSampler = VK_NULL_HANDLE;
  }

  if (textureImageView != VK_NULL_HANDLE)
  {
    vkDestroyImageView(device, textureImageView, nullptr);
    textureImageView = VK_NULL_HANDLE;
  }

  if (textureImage != VK_NULL_HANDLE)
  {
    vkDestroyImage(device, textureImage, nullptr);
    textureImage = VK_NULL_HANDLE;
  }

  if (textureImageMemory != VK_NULL_HANDLE)
  {
    vkFreeMemory(device, textureImageMemory, nullptr);
    textureImageMemory = VK_NULL_HANDLE;
  }
}