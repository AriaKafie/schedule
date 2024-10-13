
#include <iostream>
#include <string>
#include <unistd.h>

constexpr double NO_ALPHA = -1;

int main(int argc, char** argv)
{
    for (int i = 0; i < )
    double alpha = getopt(argc, argv, "a:") == 'a' ? std::stod(optarg) : NO_ALPHA;
    
    std::cout << alpha << std::endl << argv[optind < argc ? optind : 1] << std::endl;
}
