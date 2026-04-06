#include <iostream>
#include "texture.hpp"
#include "image.hpp"

// Constructor: Initializes with a specified OpenGL texture target (e.g., GL_TEXTURE_2D)
Texture::Texture(TextureTarget target)
    : 
    m_textureTarget(target), 
    m_textureObj(0), 
    m_imageWidth(0), 
    m_imageHeight(0), 
    m_imageBPP(0)
{

}

// Copy constructor: Deep copies CPU-side image data, but does NOT copy GPU texture ID
Texture::Texture(const Texture& other)
    : 
    m_fileName(other.m_fileName),
    m_textureTarget(other.m_textureTarget),
    m_textureObj(0), // We generate a new OpenGL texture ID on re-upload
    m_imageWidth(other.m_imageWidth),
    m_imageHeight(other.m_imageHeight),
    m_imageBPP(other.m_imageBPP),
    m_imageData(other.m_imageData)
{
    // The image will need to be uploaded to GPU separately
}

// Loads an image from disk into CPU memory
void Texture::LoadFromFile(const std::string& fileName)
{
    m_fileName = fileName;
    //std::cout << "Trying to load texture: " << fileName << "\n";

    ImageData img;
    if (!LoadImage(fileName, img)) 
    {
        std::cerr << "Failed loading: " << fileName << "\n";
        exit(1); // Consider replacing with a safer error handler
    }

    // Store image properties
    m_imageWidth = img.width;
    m_imageHeight = img.height;
    m_imageBPP = img.bpp;

    // Move image pixel data into member vector
    m_imageData = std::move(img.pixels);
}

// Uploads the image data from CPU to GPU memory and creates an OpenGL texture
void Texture::UploadToGPU()
{
    using namespace Graphics;

    // Generate and bind texture
    GenTexture(m_textureObj);
    BindTexture(m_textureTarget, m_textureObj);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    // Choose appropriate format based on image BPP
    TextureFormat format = TextureFormat::RGB;
    if (m_imageBPP == 1)
    {
        format = TextureFormat::Red;
    }
    if (m_imageBPP == 4)
    {
        format = TextureFormat::RGBA;
    }

    // Upload pixel data to GPU
    TexImage2D(m_textureTarget, m_imageWidth, m_imageHeight, format, m_imageData.data());

    // Generate mipmaps for better texture scaling
    GenerateMipmaps(m_textureTarget);

    // Unbind texture
    BindTexture(m_textureTarget, 0);
}

// Binds this texture to a given texture unit for use in shaders
void Texture::Bind(unsigned int textureUnit)
{
    using namespace Graphics;

    ActiveTextureUnit(textureUnit);               // Set active texture unit (e.g. GL_TEXTURE0)
    BindTexture(m_textureTarget, m_textureObj);   // Bind our texture to it
}

// Deletes the OpenGL texture from GPU memory
void Texture::Delete()
{
    Graphics::DeleteTexture(m_textureObj);
}

// Returns the OpenGL texture object ID
unsigned int Texture::GetID() const
{
    return m_textureObj;
}

int Texture::GetWidth() const 
{ 
    return m_imageWidth; 
}

int Texture::GetHeight() const 
{ 
    return m_imageHeight; 
}

