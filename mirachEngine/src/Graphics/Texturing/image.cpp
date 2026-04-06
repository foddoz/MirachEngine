#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "../../stb_image/stb_image.h"
#include <iostream>
#include <filesystem>
#include <fstream>

/*
* 
*Debug version of LoadImage function, will searching all the image under the directory
*And display all the path of the image
* 
*/
bool LoadImage(const std::string& filename, ImageData& outImage)
{
    using std::cerr;
    namespace fs = std::filesystem;

    stbi_set_flip_vertically_on_load(true);

    const fs::path rel = fs::path(filename);
    const fs::path abs = fs::absolute(rel);
    const fs::path cwd = fs::current_path();

    cerr << "[Image] cwd        : " << cwd.string() << "\n";
    cerr << "[Image] input path : " << rel.string() << "\n";
    cerr << "[Image] abs path   : " << abs.string() << "\n";

    if (!fs::exists(rel) && !fs::exists(abs)) 
    {
        cerr << "[Image] File does NOT exist (neither rel nor abs)\n";
    }
    else 
    {
        cerr << "[Image] File exists\n";
    }

    int w = 0;
    int h = 0;
    int bpp = 0;
    unsigned char* data = stbi_load(rel.string().c_str(), &w, &h, &bpp, 0);

    if (!data) 
    {
        cerr << "[Image] stb_image failed: " << stbi_failure_reason() << "\n";       
#ifdef _WIN32
        std::wstring wpath(rel.wstring());
        FILE* fp = _wfopen(wpath.c_str(), L"rb");
        if (!fp) 
        {
            cerr << "[Image] _wfopen also failed for wide path.\n";
            return false;
        }
        data = stbi_load_from_file(fp, &w, &h, &bpp, 0);
        fclose(fp);
        if (!data) 
        {
            cerr << "[Image] stbi_load_from_file(wide) failed: " << stbi_failure_reason() << "\n";
            return false;
        }
#else
        return false;
#endif
    }

    outImage.width = w;
    outImage.height = h;
    outImage.bpp = bpp;
    outImage.pixels.assign(data, data + (w * h * bpp));
    stbi_image_free(data);
    return true;
}