#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>
#include "../graphics.hpp"

/**
 * @class Texture
 * @brief Handles image loading, GPU uploading, binding, and cleanup of texture resources.
 *
 * This class provides an abstraction over OpenGL texture operations. It supports 2D textures
 * and wraps functionality for loading from file, uploading to GPU memory, and binding to specific texture units.
 */
class Texture 
{
public:
    /**
     * @brief Constructs a texture with a specified OpenGL texture target.
     * @param target The OpenGL texture target (e.g., TextureTarget::Texture2D).
     */
    Texture(TextureTarget target);

    /**
     * @brief Copy constructor for deep-copying texture state (image data only, not GPU handle).
     * @param other Texture object to copy from.
     */
    Texture(const Texture& other);

    /**
     * @brief Loads image data from a file into CPU memory.
     * @param fileName Path to the image file to load.
     */
    void LoadFromFile(const std::string& fileName);

    /**
     * @brief Uploads the loaded image data to GPU and creates an OpenGL texture.
     */
    void UploadToGPU();

    /**
     * @brief Binds the texture to a specified texture unit for rendering.
     * @param textureUnit The OpenGL texture unit (e.g., GL_TEXTURE0 or COLOR_TEXTURE_UNIT).
     */
    void Bind(unsigned int textureUnit);

    /**
     * @brief Frees GPU texture memory and clears the texture object.
     */
    void Delete();

    /**
     * @brief Returns the OpenGL texture object ID.
     * @return GLuint texture handle.
     */
    unsigned int GetID() const;

    int GetWidth() const;


    int GetHeight() const;

private:
    std::string m_fileName;                 ///< Path to the loaded image file.
    TextureTarget m_textureTarget;         ///< OpenGL texture target type (e.g., GL_TEXTURE_2D).
    unsigned int m_textureObj = 0;         ///< OpenGL texture object ID.

    int m_imageWidth = 0;                  ///< Width of the loaded image.
    int m_imageHeight = 0;                 ///< Height of the loaded image.
    int m_imageBPP = 0;                    ///< Bytes per pixel of the loaded image.
    std::vector<unsigned char> m_imageData;///< Raw pixel data loaded from the image.
};

#endif // TEXTURE_HPP
