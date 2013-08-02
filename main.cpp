#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <locale>
#include <functional>
#include <algorithm>

#include "utils.hpp"
#include "log.hpp"
#include "request.hpp"
#include "server.hpp"

int main(int argc, char *argv[])
{
    wot::server server;
    while(true)
    {
        server.handle();
    }
    return 0;
}
