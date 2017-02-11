#pragma once

struct Updatable {
    virtual void Update(const long long dt) = 0;
};

struct Displayable {
    virtual void Display() = 0;
};

struct State {
    
};
