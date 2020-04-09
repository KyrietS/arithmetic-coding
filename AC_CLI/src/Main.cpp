#include <CLI/CLI11.hpp>
#include <iostream>
#include <string>
#include <filesystem>

#include "AdaptiveScalingCoder.hpp"

bool endsWith(std::string const& fullString, std::string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

void printStats(Statistics stats);

int main(int argc, char** argv)
{
    CLI::App app;

    // All flags and options.
    bool encode{ false };
    bool decode{ false };
    std::string path_in;
    std::string path_out;
    bool file_override{ false };
    bool print_stats{ false };

    // encode | decode option group.
    auto action = app.add_option_group("Action type", "What action on a file should be performed.");
    action->add_flag("-e,--encode", encode, "Encode a file");
    action->add_flag("-d,--decode", decode, "Decode a file");
    action->require_option(1);
    action->required(true);

    // input_file option.
    app.add_option("source", path_in, "Path to a file you want to encode or decode.")
        ->required(true)
        ->check(CLI::ExistingFile);

    // output_file option.
    app.add_option("dest", path_out, "Path to a result file.")
        ->required(false)
        ->default_str("<source>.ac");

    // file override option.
    app.add_flag("-o,--override", file_override, "Whether output file should override existing file.");

    // print stats after encoding process
    app.add_flag("-s,--stats", print_stats, "Print stats after encoding process.");

    app.get_formatter()->label("Positionals", "Files");
    app.get_formatter()->label("TEXT", "PATH");
    app.get_formatter()->label("TEXT:FILE", "PATH");


    // Parse input parameters.
    CLI11_PARSE(app, argc, argv);

    // Generate output path if not specified.
    if (path_out.empty())
    {
        if (encode)                                 // add *.ac extension
            path_out = path_in + ".ac";
        if (decode && endsWith(path_in, ".ac"))     // remove *.ac extension if exists
            path_out = path_in.substr(0, path_in.find_last_of("."));
    }

    // 'source' and 'dest' cannot be the same (letter case should be ignored)
    if (path_in == path_out)
    {
        std::cerr << "source and dest paths cannot be the same!" << std::endl;
        return -1;
    }

    // Remove existing file if -o,--override flag is set.
    if (file_override) 
        std::filesystem::remove(path_out);

    // File 'path_out' should not exist at this point.
    if (std::filesystem::exists(path_out))
    {
        std::cerr << "dest: Path already exists: " << path_out << std::endl;
        std::cerr << "Specify different name or consider using -o,--override option." << std::endl;
        std::cerr << "Run with --help for more information." << std::endl;
        return -1;
    }

    // The meat.
    AdaptiveScalingCoder coder(print_stats);

    if (encode) {
        Statistics stats = coder.encode(path_in, path_out);
        if (print_stats)
            printStats(stats);
    }
    else if (decode) {
        coder.decode(path_in, path_out);
    }

    return 0;
}

void printStats(Statistics stats)
{
    std::cout.precision(2);
    std::cout << std::fixed;
    std::cout << "input entropy:       " << stats.entropy() << std::endl;
    std::cout << "average code length: " << stats.averageCodingLength() << "   [bits/byte]" << std::endl;
    std::cout << "compression rate:    " << stats.compressionRatio()*100 << "%" << std::endl;
    std::cout << std::defaultfloat;
}
