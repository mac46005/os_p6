#include <functional>
#include <string>
#include "../error/error.hpp"
#include "../color/ui.hpp"

struct Options{
    int sec;
    int nano;
};

class ArgumentProcessor{
private:
    int argc_;
    char **argv_;
public:
    explicit ArgumentProcessor(int argc, char **argv);
    void process(std::function<void(Options)> useOptions);
};