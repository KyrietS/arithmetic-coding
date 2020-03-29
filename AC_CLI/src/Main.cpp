#include <CLI/CLI11.hpp>
#include <iostream>


using namespace std;

int main(int argc, char** argv)
{
    CLI::App app;



    CLI11_PARSE(app, argc, argv);

    return 0;
}