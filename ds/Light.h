#pragma once

#include "Renderable.h"

class Light : public Renderable
{
public:
    Light();
    virtual ~Light();

    void display() const;
};
