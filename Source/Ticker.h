#pragma once
#include <functional>
#include <algorithm>

using TickCallback = std::function<void(float)>;

class Ticker
{
public:
    explicit Ticker(int tps = 60, float maxAccum = 0.2F)
        : _fixedDt(1.0F / static_cast<float>(tps)), _maxAccumulator(maxAccum)
    {}

    void setCallback(const TickCallback& callback) { _callback = callback; }

    void update(float dt)
    {
        if (!_callback) return;

        _accumulator += std::min(dt, _maxAccumulator);

        while (_accumulator >= _fixedDt)
        {
            _callback(_fixedDt);
            _accumulator -= _fixedDt;
        }
    }

    [[nodiscard]] float getAlpha() const { return _accumulator / _fixedDt; }

    void setTPS(int tps) { _fixedDt = 1.0F / static_cast<float>(tps); }
    [[nodiscard]] float getFixedDt() const { return _fixedDt; }

private:
    TickCallback _callback = nullptr;
    float _fixedDt         = 0.0F;
    float _maxAccumulator  = 0.0F;
    float _accumulator     = 0.0F;
};