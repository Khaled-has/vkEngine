#pragma once

#include <stdint.h>

class Timer
{
public:
    inline static void Start() { m_pInstance->StartImpl(); }
    inline static uint32_t getApplicationTime() { return m_pInstance->getApplicationTimeImpl(); }
    inline static double getTime() { return m_pInstance->getTimeImpl(); }
protected:
    virtual void StartImpl() = 0;
    virtual uint32_t getApplicationTimeImpl() = 0;
    virtual double getTimeImpl() = 0;

    static Timer* m_pInstance;
};