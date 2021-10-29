/*
  ==============================================================================

    Filters.cpp
    Created: 28 Oct 2021 6:00:45pm
    Author:  vossj

  ==============================================================================
*/

#include "Filters.h"

void FilterBase::setSampleRateCallback()
{
}

void FilterBase::setSampleRate(double newSampleRate)
{
    lock_guard<mutex> l(filterMutex);
    sampleRate = newSampleRate;
    timeAdjustmentFactor = getReferenceSampleRate() / sampleRate;
    setSampleRateCallback();
}

void SmootheningFilter::setRatio(const double& newRatio)
{
    lock_guard<mutex> l(filterMutex);
    filterRatio = newRatio;
}

float SmootheningFilter::getNextSample(const float& currentSample)
{
    lock_guard<mutex> l(filterMutex);
    float lastDelta = currentSample - lastSample * FilterBase::timeAdjustmentFactor;
    float newDelta = filterRatio * lastDelta;
    float result = lastSample + newDelta;
    result = clamp<float>(result, -1, 1);
    lastSample = result;
    return result;
}

double SmootheningFilter::getReferenceSampleRate()
{
    return referenceSampleRate;
}

void MaxDeltaFilter::setMaxDelta(const double& newMaxDelta)
{
    lock_guard<mutex> l(filterMutex);
    maxDelta = newMaxDelta;
}

float MaxDeltaFilter::getNextSample(const float& currentSample)
{
    lock_guard<mutex> l(filterMutex);
    float lastDelta = currentSample - lastSample * FilterBase::timeAdjustmentFactor;
    float lastDeltaAbs = fabs(lastDelta);
    float newDelta = lastDeltaAbs <= maxDelta
        ? lastDelta
        : maxDelta * (lastDelta/lastDeltaAbs);
    float result = lastSample + newDelta;
    result = clamp<float>(result, -1, 1);
    lastSample = result;
    return result;
}

double MaxDeltaFilter::getReferenceSampleRate()
{
    return referenceSampleRate;
}

void Max2ndDeltaFilter::setMax2ndDelta(const double& newMax2ndDelta)
{
    lock_guard<mutex> l(filterMutex);
    _max2ndDelta = newMax2ndDelta;
}

float Max2ndDeltaFilter::getNextSample(const float& currentSample)
{
    lock_guard<mutex> l(filterMutex);
    double currentSampleD = (double)currentSample;
    double lastDelta = currentSampleD - _lastSample * FilterBase::timeAdjustmentFactor;
    double last2ndDelta = lastDelta - _lastDelta;
    double last2ndDeltaAbs = fabs(last2ndDelta);
    double new2ndDelta = last2ndDeltaAbs <= _max2ndDelta
        ? last2ndDelta
        : _max2ndDelta * (last2ndDelta / last2ndDeltaAbs);
    double newDelta = _lastDelta + new2ndDelta;
    double result = _lastSample + newDelta;
    result = clamp<float>(result, -1, 1);

    _lastDelta = lastDelta;
    _lastSample = result;
    return result;
}

double Max2ndDeltaFilter::getReferenceSampleRate()
{
    return referenceSampleRate;
}

Filters::Filters()
{
    setSampleRate(sampleRate, true);
}

void Filters::setSampleRate(const double& newSampleRate, const bool& force)
{
    lock_guard<mutex> l(filterMutex);
    if (force || sampleRate != newSampleRate) {
        sampleRate = newSampleRate;
        smootheningFilter->setSampleRate(newSampleRate);
        maxDeltaFilter->setSampleRate(newSampleRate);
        max2ndDeltaFilter->setSampleRate(newSampleRate);
        
    }
}


