#include "../clock/clock.hpp"

class ClockChecker {
private:
    Clock *clock_;
    Time end_time_;
    Logger *logger_;
public:
    explicit ClockChecker(int max_sec, int max_nano, Logger *logger);
    bool isTimeUp();
    void cleanUp();
    std::string toString();
};