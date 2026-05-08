#include "../clock/clock.hpp"

class ClockChecker {
private:
    Clock *clock_;
    Time time_limit_;
    Logger *logger_;
public:
    explicit ClockChecker(int max_sec, int max_nano, Logger *logger);
    bool isTimeReached();
    void cleanUp();
};