#pragma once

#include "Timer.h" 

class WindowsTime : public Timer
{
protected:
    virtual void StartImpl() override;
    virtual uint32_t getApplicationTimeImpl() override;
    virtual double getTimeImpl() override;
private:
    uint32_t m_StartTime;
};