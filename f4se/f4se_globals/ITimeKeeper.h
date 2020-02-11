#pragma once

/**
*	A less verbose clone of ITimer with built-in formatter.
*/
class ITimeKeeper {
public:
                    ITimeKeeper(bool startTimer = true);

    void            Start();
    void            Stop();

    double          Format(int Duration);

    enum Duration {
        kDuration_Nano    = 0x00000001,
        kDuration_Micro   = 0x00000010,
        kDuration_Milli   = 0x00000100,
        kDuration_Second  = 0x00001000,
        kDuration_Minute  = 0x00010000,
        kDuration_Hour    = 0x00100000
    };

private:
    double  GetElapsedTime();

    UInt64  m_qpcBase;
    UInt32  m_tickBase;

    static double   s_secondsPerCount;
    static TIMECAPS s_timecaps;
    static bool     s_setTime;

    // safe QPC stuff
    static UInt64   GetQPC();

    static UInt64   s_lastQPC;
    static UInt64   s_qpcWrapMargin;
    static bool     s_hasLastQPC;

    static UInt32   s_qpcWrapCount;
    static UInt32   s_qpcInaccurateCount;
};
