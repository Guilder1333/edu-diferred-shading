#pragma once

#include "Renderable.h"
#include "Font.h"

class Text : public Renderable
{
public:
    Text(const Font *font, const std::string &text);
    void display() const;
private:
    std::string &text;
    Font *font;
};
