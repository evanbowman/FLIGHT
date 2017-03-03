#include "BlendFunc.hpp"

namespace Blend {
    const BlendFunc Additive {
        BlendFactor::One, BlendFactor::One
    };

    const BlendFunc Multiply {
        BlendFactor::DstColor, BlendFactor::Zero
    };

    const BlendFunc Alpha {
        BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha
    };
}