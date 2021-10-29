/*
  ==============================================================================

    Filters.h
    Created: 28 Oct 2021 6:00:45pm
    Author:  vossj

  ==============================================================================
*/

#pragma once
#include <math.h>
#include <memory>
#include <mutex>
#include <algorithm>
using namespace std;

class FilterBase {
protected:
    mutex filterMutex;
    double sampleRate = 48000;
    double timeAdjustmentFactor = 1;
    virtual float getNextSample(const float& currentSample) = 0;
    virtual double getReferenceSampleRate() = 0;
    /// <summary>
    /// Runs after the sampleRate parameter has been set
    /// </summary>
    virtual void setSampleRateCallback();
    
public:
    void setSampleRate(double sampleRate);
};

class SmootheningFilter : public FilterBase {
    const double referenceSampleRate = 48000;
    double filterRatio = 0.5;
    float lastSample = 0;
public:
    /// <summary>
    /// Sets the filter's divisor (the difference between last sample and new sample, times newRatio, added to last sample)
    /// </summary>
    /// <param name="newRatio"></param>
    void setRatio(const double& newRatio);
    
    // Imported from FilterBase
    float getNextSample(const float& currentSample) override;
    double getReferenceSampleRate() override;
};

class MaxDeltaFilter : public FilterBase {
    const double referenceSampleRate = 48000;
    double timeAdjustmentFactor = 1;
    double maxDelta = 0.2;
    float lastSample = 0;
public:
    void setMaxDelta(const double& newMaxDelta);

    // Imported from FilterBase
    float getNextSample(const float& currentSample) override;
    double getReferenceSampleRate() override;
};

class Max2ndDeltaFilter : public FilterBase {
    const double referenceSampleRate = 48000;
    double timeAdjustmentFactor = 1;
    double _max2ndDelta = 1;
    double _lastDelta = 0;
    float _lastSample = 0;
public:
    void setMax2ndDelta(const double& newMax2ndDelta);

    // Imported from FilterBase
    float getNextSample(const float& currentSample) override;
    double getReferenceSampleRate() override;

};

class Filters {
    mutex filterMutex;
    double sampleRate = 48000.0;
public:
    unique_ptr<SmootheningFilter> smootheningFilter = make_unique<SmootheningFilter>();
    unique_ptr<MaxDeltaFilter> maxDeltaFilter = make_unique<MaxDeltaFilter>();
    unique_ptr<Max2ndDeltaFilter> max2ndDeltaFilter = make_unique<Max2ndDeltaFilter>();

    Filters();

    void setSampleRate(const double& newSampleRate, const bool& force = false);
    
};