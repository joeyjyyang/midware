#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char* argv[])
{
    FileParser file_parser;

    const std::string file_name{"../data.csv"};
    std::ifstream file{};
    
    file.open(file_name);

    //assert(file.good() == true);

    if (!file.good())
    {
        std::cout << "Unable to open file: " << file_name << "\n";
        return -1;
    }

    std::string line{};

    // Get each line.
    while (std::getline(file, line))
    {
        std::cout << line << "\n";

        std::istringstream line_stream{line};
        std::string field{};

        // Get each field in line.
        while (std::getline(line_stream, field, ','))
        {
            std::cout << field << "\n";
        }
    }

    return 0;
}