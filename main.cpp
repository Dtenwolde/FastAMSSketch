#include <iostream>
#include <iomanip>
#include <map>
#include <__random/random_device.h>

#include "include/FastAMS.hpp" // Include the header file for FastAMS

int main(int argc, char** argv)
{
    if (argc != 6)
    {
        std::cerr
            << std::endl
            << "Usage: FastAMS "
            << "stream_length domain_size skew counters hashes.\n"
            << "Parameters:\n"
            << "  stream_length: Total number of insertions.\n"
            << "  domain_size:   Total number of distinct elements.\n"
            << "  skew:          The skew parameter of the Zipf distributions.\n"
            << "  counters:      The number of counters per hash.\n"
            << "  hashes:       The number of hashes per sketch.\n"
            << std::endl;
        return -1;
    }

    unsigned long N = atol(argv[1]);
    unsigned long domainSize = atol(argv[2]);
    double skew = atof(argv[3]);
    unsigned long counters = atol(argv[4]);
    unsigned long hashes = atol(argv[5]);

    FastAMS ams(counters, hashes); // Create a FastAMS instance
    std::map<unsigned long, unsigned long> exact;
    std::map<unsigned long, unsigned long>::iterator itEx;

    // Initialize random number generator
    std::random_device rd;
    MyRandomGenerator gen(rd());
    std::uniform_int_distribution<unsigned long> dis(1, domainSize);

    std::cerr << std::setprecision(2) << std::fixed;
    std::cout << std::setprecision(2) << std::fixed;

    double totalError = 0.0;
    unsigned long totalAnswers = 0;

    try
    {
        std::cerr << "Testing insertions." << std::endl;

        for (unsigned long i = 1; i <= N; i++)
        {
            unsigned long l = dis(gen); // Generate random element
            long val = 1; // Value to insert into the sketch

            ams.insert(l, val); // Insert into the sketch with identifier and value

            itEx = exact.find(l);
            if (itEx != exact.end())
                (*itEx).second++;
            else
                exact[l] = 1;

            if (i % 100 == 0)
            {
                // find top 20 most frequent.
                std::multimap<unsigned long, unsigned long> frequent;
                for (itEx = exact.begin(); itEx != exact.end(); itEx++)
                    frequent.insert(
                        std::pair<unsigned long, unsigned long>(
                            (*itEx).second,
                            (*itEx).first)
                    );

                std::multimap<unsigned long, unsigned long>::iterator it = frequent.end(); it--;
                unsigned long top = 20;
                while (top > 0)
                {
                    unsigned long f = ams.getFrequency((*it).second);

                    double error =
                        std::abs(static_cast<double>(f) - static_cast<double>((*it).first)) / static_cast<double>((*it).first);

                    totalError += error;
                    totalAnswers++;

                    top--;
                    it--;
                }

                std::cerr << "Insertions: " << i << std::endl;
            }
        }

        std::cerr
            << "Average relative error (top-20 frequency): "
            << totalError / totalAnswers << std::endl;

        // Rest of the test code remains unchanged...

        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cerr << "Error: Unknown exception caught." << std::endl;
        return -1;
    }
}
