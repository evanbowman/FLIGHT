#include "BlendFunc.hpp"

const BlendFunc AdditiveBlend {
    BlendFactor::One, BlendFactor::One
};

const BlendFunc MultiplyBlend {
    BlendFactor::DstColor, BlendFactor::Zero
};

const BlendFunc AlphaBlend {
    BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha
};
