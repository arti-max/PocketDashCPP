#pragma once
#include "axmol.h"
#include <algorithm>
#include <functional>

using AlertCallback = std::function<void(void)>;

class AlertPopup : public ax::Node
{
public:
    AlertPopup()  = default;
    ~AlertPopup() = default;

    static AlertPopup* create(std::string title, std::string desc, const AlertCallback& callback);
    bool init(std::string title, std::string desc, const AlertCallback& callback);

private:
    AlertCallback callback;
};