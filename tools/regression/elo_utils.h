#pragma once

#include <cmath>

constexpr double inverseError(double x)
{
    const double pi = M_PI;
    const double a = 8.0 * (pi - 3.0) / (3.0 * pi * (4.0 - pi));
    const double y = std::log(1.0 - x * x);
    const double z = 2.0 / (pi * a) + 0.5 * y;
    const double ret = std::sqrt(std::sqrt(z * z - y / a) - z);
    return x < 0.0 ? -ret : ret;
}

constexpr double phiInverse(double p)
{
    return M_SQRT2 * inverseError(2 * p - 1);
}

// computes ELO difference based on win percentage
constexpr double computeEloDifference(double percentage)
{
    return -400.0 * std::log10((1.0 / percentage) - 1.0);
}

// computes ELO difference based on number of wins, draws and losses
constexpr double computeEloDifferenceFromScores(double wins, double draws, double losses)
{
    const double score = wins + 0.5 * draws;
    const double total = wins + draws + losses;
    return computeEloDifference(score / total);
}

// computes ELO error margin
constexpr double computeErrorMargin(double wins, double draws, double losses)
{
    const double total = wins + draws + losses;

    const double perc = (wins + 0.5 * draws) / total;
    const double winsDev = (wins / total) * (1.0 - perc) * (1.0 - perc);
    const double drawsDev = (draws / total) * (0.5 - perc) * (0.5 - perc);
    const double lossesDev = (losses / total) * perc * perc;
    const double stdDev = std::sqrt((winsDev + drawsDev + lossesDev) / total);

    const double confidence = 0.95;
    const double minConfidence = 0.5 * (1.0 - confidence);
    const double maxConfidence = 1.0 - minConfidence;

    const double minDev = perc + phiInverse(minConfidence) * stdDev;
    const double maxDev = perc + phiInverse(maxConfidence) * stdDev;

    return 0.5 * (computeEloDifference(maxDev) - computeEloDifference(minDev));
}
