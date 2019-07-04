#include "ITimeKeeper.h"
#include "ILog.h"

double		ITimeKeeper::s_secondsPerCount = 0;
TIMECAPS	ITimeKeeper::s_timecaps = { 0 };
bool		ITimeKeeper::s_setTime = false;
UInt64		ITimeKeeper::s_lastQPC = 0;
UInt64		ITimeKeeper::s_qpcWrapMargin = 0;
bool		ITimeKeeper::s_hasLastQPC = false;
UInt32		ITimeKeeper::s_qpcWrapCount = 0;
UInt32		ITimeKeeper::s_qpcInaccurateCount = 0;

ITimeKeeper::ITimeKeeper() :m_qpcBase(0), m_tickBase(0) {
    Init();
}

ITimeKeeper::~ITimeKeeper() {
    //
}

void ITimeKeeper::Init(void) {
    if (!s_secondsPerCount) {
        // init qpc
        UInt64 countsPerSecond;
        BOOL res = QueryPerformanceFrequency((LARGE_INTEGER *)&countsPerSecond);

        ASSERT_STR(res, "ITimer: no high-resolution timer support");

        s_secondsPerCount = 1.0 / countsPerSecond;

        s_qpcWrapMargin = (UInt64)(-((SInt64)(countsPerSecond * 60))); // detect if we've wrapped around by a delta greater than this - also limits max time
        //_MESSAGE("s_qpcWrapMargin: %016I64X", s_qpcWrapMargin);
        //_MESSAGE("wrap time: %fs", ((double)0xFFFFFFFFFFFFFFFF) * s_secondsPerCount);

        // init multimedia timer
        timeGetDevCaps(&s_timecaps, sizeof(s_timecaps));

        //_MESSAGE("min timer period = %d", s_timecaps.wPeriodMin);

        s_setTime = (timeBeginPeriod(s_timecaps.wPeriodMin) == TIMERR_NOERROR);
		if (!s_setTime)
			MessageBox(NULL, "Error!\n!s_setTime", "ITimeKeeper", MB_OK | MB_ICONEXCLAMATION);
            // g_Log.LogWarning("Couldn't change ITimeKeeper period!");
    }
}

void ITimeKeeper::DeInit(void) {
    if (s_secondsPerCount) {
        if (s_setTime) {
            timeEndPeriod(s_timecaps.wPeriodMin);
            s_setTime = false;
        }

        s_secondsPerCount = 0;
    }
}

void ITimeKeeper::Start(void) {
    m_qpcBase = GetQPC();
    m_tickBase = timeGetTime();
}

double ITimeKeeper::GetElapsedTime(void) {
    UInt64 qpcNow = GetQPC();
    UInt32 tickNow = timeGetTime();

    UInt64 qpcDelta = qpcNow - m_qpcBase;
    UInt64 tickDelta = tickNow - m_tickBase;

    double qpcSeconds = ((double)qpcDelta) * s_secondsPerCount;
    double tickSeconds = ((double)tickDelta) * 0.001; // ticks are in milliseconds
    double qpcTickDelta = qpcSeconds - tickSeconds;

    if (qpcTickDelta < 0) qpcTickDelta = -qpcTickDelta;

    // if they differ by more than one second, something's wrong, return
    if (qpcTickDelta > 1) {
        s_qpcInaccurateCount++;
        return tickSeconds;
    }
    else {
        return qpcSeconds;
    }
}

UInt64 ITimeKeeper::GetQPC(void) {
    UInt64 now;
    QueryPerformanceCounter((LARGE_INTEGER *)&now);

    if (s_hasLastQPC) {
        UInt64 delta = now - s_lastQPC;

        if (delta > s_qpcWrapMargin) {
            // we've gone back in time, return a kludged value

            s_lastQPC = now;
            now = s_lastQPC + 1;

            s_qpcWrapCount++;
        }
        else {
            s_lastQPC = now;
        }
    }
    else {
        s_hasLastQPC = true;
        s_lastQPC = now;
    }

    return now;
}

double ITimeKeeper::Format(int Duration) {
    switch (Duration) {
    case kDuration::Nano:
        return (GetElapsedTime() * 1000000000.0);

    case kDuration::Micro:
        return (GetElapsedTime() * 1000000.0);

    case kDuration::Milli:
        return (GetElapsedTime() * 1000.0);

    case kDuration::Minute:
        return (GetElapsedTime() / 60.0);

    case kDuration::Hour:
        return (GetElapsedTime() / 3600.0);

    default:
        return GetElapsedTime();
    }
}