#include <FLIGHT/Core/ConfigData.hpp>
#include <FLIGHT/Core/Dialog.hpp>
#include <FLIGHT/Core/Game.hpp>
#include <iostream>

#include <nanosvg/nanosvg.h>
#include <nanosvg/nanosvgrast.h>

void SVGRastTest() {
    NSVGrasterizer * rast = nsvgCreateRasterizer();
    auto path = ResourcePath() + "textures/icons/pulse.svg";
    auto image = nsvgParseFromFile(path.c_str(), "px", 96.f);
    if (not image) {
        throw std::runtime_error("Failed to load image");
    }
    const float scale = 70.f;
    int w = static_cast<int>(image->width * scale);
    int h = static_cast<int>(image->height * scale);
    sf::Image img;
    img.create(w, h);
    nsvgRasterize(rast, image, 0, 0, scale, (unsigned char *)img.getPixelsPtr(),
                  w, h, w * 4);
    nsvgDelete(image);
    img.saveToFile("/Users/evan_bowman/Desktop/test.png");
    nsvgDeleteRasterizer(rast);
}

int main() {
    try {
        Singleton<FLIGHT::Game>::Instance().Configure(FLIGHT::LoadConfig());
        Singleton<FLIGHT::Game>::Instance().Run();
    } catch (const std::exception & ex) {
#ifdef NDEBUG
        CreateDialogBox(ex.what());
#else
        std::cerr << ex.what() << std::endl;
#endif
    }
}

#if defined(_WIN32) or defined(_WIN64)
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return main(); }
#endif
