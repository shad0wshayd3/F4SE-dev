#include "ITimeKeeper.h"

#include "ILog.h"

// ------------------------------------------------------------------------------------------------
// ITimeKeeper
// ------------------------------------------------------------------------------------------------

ITimeKeeper::ITimeKeeper(bool startTimer): m_qpcBase(0), m_tickBase(0) {
    if (!s_secondsPerCount) {
        UInt64 countsPerSecond;
        BOOL res = QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);

        s_secondsPerCount = 1.0 / countsPerSecond;
        s_qpcWrapMargin = (UInt64)(-((SInt64)(countsPerSecond * 60)));    // detect if we've wrapped around by a delta greater than this - also limits max time

        // init multimedia timer
        timeGetDevCaps(&s_timecaps, sizeof(s_timecaps));

        s_setTime = (timeBeginPeriod(s_timecaps.wPeriodMin) == TIMERR_NOERROR);
        if (!s_setTime)
            _LogError("s_setTime is False!");
    }

    if (startTimer)
        Start();
}

void ITimeKeeper::Start() {
    m_qpcBase = GetQPC();
    m_tickBase = timeGetTime();
}

void ITimeKeeper::Stop() {
    m_qpcBase = 0;
    m_tickBase = 0;
}

double ITimeKeeper::Format(int Duration) {
    switch (Duration) {
    case kDuration_Nano:
        return (GetElapsedTime() * 1000000000.0);

    case kDuration_Micro:
        return (GetElapsedTime() * 1000000.0);

    case kDuration_Milli:
        return (GetElapsedTime() * 1000.0);

    case kDuration_Minute:
        return (GetElapsedTime() / 60.0);

    case kDuration_Hour:
        return (GetElapsedTime() / 3600.0);

    default:
        return GetElapsedTime();
    }
}

// ------------------------------------------------------------------------------------------------
// Internal Functions
// ------------------------------------------------------------------------------------------------

double ITimeKeeper::GetElapsedTime() {
    UInt64 qpcNow = GetQPC();
    UInt32 tickNow = timeGetTime();

    UInt64 qpcDelta = qpcNow - m_qpcBase;
    UInt64 tickDelta = tickNow - m_tickBase;

    double qpcSeconds = ((double)qpcDelta) * s_secondsPerCount;
    double tickSeconds = ((double)tickDelta) * 0.001;    // ticks are in milliseconds
    double qpcTickDelta = qpcSeconds - tickSeconds;

    if (qpcTickDelta < 0)
        qpcTickDelta = -qpcTickDelta;

    // if they differ by more than one second, something's wrong, return
    if (qpcTickDelta > 1) {
        s_qpcInaccurateCount++;
        return tickSeconds;
    } else {
        return qpcSeconds;
    }
}

UInt64 ITimeKeeper::GetQPC() {
    UInt64 now;
    QueryPerformanceCounter((LARGE_INTEGER*)&now);

    if (s_hasLastQPC) {
        UInt64 delta = now - s_lastQPC;

        if (delta > s_qpcWrapMargin) {
            // we've gone back in time, return a kludged value

            s_lastQPC = now;
            now = s_lastQPC + 1;

            s_qpcWrapCount++;
        } else {
            s_lastQPC = now;
        }
    } else {
        s_hasLastQPC = true;
        s_lastQPC = now;
    }

    return now;
}

// ------------------------------------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------------------------------------

double ITimeKeeper::s_secondsPerCount = 0;
TIMECAPS ITimeKeeper::s_timecaps = {0};
bool ITimeKeeper::s_setTime = false;
UInt64 ITimeKeeper::s_lastQPC = 0;
UInt64 ITimeKeeper::s_qpcWrapMargin = 0;
bool ITimeKeeper::s_hasLastQPC = false;
UInt32 ITimeKeeper::s_qpcWrapCount = 0;
UInt32 ITimeKeeper::s_qpcInaccurateCount = 0;