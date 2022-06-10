#include "Editor.hpp"

char Editor::ctrlKey(char key)
{
    return key &= 0x1f;
}

