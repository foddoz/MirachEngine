#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>
#include <vector>

// === Image Data Struct ===
struct ImageData
{
    int width = 0;
    int height = 0;
    int bpp = 0; // Bytes per pixel
    std::vector<unsigned char> pixels;
};

/**
 * @brief Loads an image file using stb_image and populates the given ImageData structure.
 *
 * This function reads an image from disk using stb_image, flips it vertically for OpenGL compatibility,
 * and fills the output ImageData struct with pixel data, width, height, and bytes-per-pixel.
 *
 * @param filename The file path to the image to be loaded.
 * @param outImage Reference to an ImageData struct where loaded image info will be stored.
 * @return true if the image loads successfully; false otherwise.
 */
bool LoadImage(const std::string& filename, ImageData& outImage);

#endif // IMAGE_HPP