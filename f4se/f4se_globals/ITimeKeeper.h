#pragma once

/**
*	A less verbose clone of ITimer with built-in formatter.
*/
class ITimeKeeper {
public:
                    ITimeKeeper();
                    ~ITimeKeeper();

    static void     Init(void);
    static void     DeInit(void);

    void            Start(void);

    double          Format(int Duration);

    enum kDuration {
        Nano    = 0x00000001,
        Micro   = 0x00000010,
        Milli   = 0x00000100,
        Second  = 0x00001000,
        Minute  = 0x00010000,
        Hour    = 0x00100000
    };

private:
    double  GetElapsedTime(void);

    UInt64  m_qpcBase;  // QPC
    UInt32  m_tickBase; // timeGetTime

    static double   s_secondsPerCount;
    static TIMECAPS s_timecaps;
    static bool     s_setTime;

    // safe QPC stuff
    static UInt64   GetQPC(void);

    static UInt64   s_lastQPC;
    static UInt64   s_qpcWrapMargin;
    static bool     s_hasLastQPC;

    static UInt32   s_qpcWrapCount;
    static UInt32   s_qpcInaccurateCount;
};
