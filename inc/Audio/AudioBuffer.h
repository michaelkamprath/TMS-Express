// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIOBUFFER_H
#define TMS_EXPRESS_AUDIOBUFFER_H

#include <string>
#include <vector>

class AudioBuffer {
public:
    explicit AudioBuffer(const std::string &path, int targetSampleRateHz = 8000, float windowWidthMs = 25.0f);
    AudioBuffer(const AudioBuffer &buffer);

    float getWindowWidth() const;
    void setWindowWidth(float windowWidthMs);

    unsigned int getSampleRate() const;
    unsigned int getNSegments() const;
    unsigned int getSamplesPerSegment() const;

    std::vector<float> getSamples();
    void setSamples(const std::vector<float> &newSamples);

    std::vector<float> getSegment(int i);

    void reset();

    // Unused attributes remain implemented, as this class will likely reappear in other projects ;-)
    __attribute__((unused)) std::vector<std::vector<float>> getSegments();
    __attribute__((unused)) void exportAudio(const std::string &path);

private:
    unsigned int sampleRate;
    unsigned int samplesPerSegment;
    unsigned int nSegments;
    std::vector<float> samples;
    std::vector<float> originalSamples;

    void mixToMono(int nChannels);
    void resample(int targetSampleRateHz);
};

#endif //TMS_EXPRESS_AUDIOBUFFER_H
