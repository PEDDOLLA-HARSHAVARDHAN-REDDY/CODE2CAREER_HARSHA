#include "server.h"


// Utility function to calculate the adjusted timeout based on RTT
double adjust_timeout(double base_timeout, double rtt)
{
    return base_timeout + rtt * 0.5; // Simple example Adjust base timeout by half the RTT
}