#include "src/Server/Core.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
        return 1;
    Core core(av[1]);
    core.run();

    return 0;
}