///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: FramePostprocessor
//
// Description: After LPC analysis and Frame packing, postprocessing may improve the quality and realism of synthesized
//              speech. The FramePostprocessor facilitates such modifications.
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FramePostprocessor.h"
#include "Frame_Encoding/Tms5220CodingTable.h"

#include <vector>

/// Create a new Frame Postprocessor
///
/// \param frames Frames to modify
/// \param maxVoicedGainDB Max audio gain for voiced (vowel) segments (in decibels)
/// \param maxUnvoicedGainDB Max audio gain for unvoiced (consonant) segments (in decibels)
FramePostprocessor::FramePostprocessor(std::vector<Frame> *frames, float maxVoicedGainDB, float maxUnvoicedGainDB) {
    frameData = frames;
    maxUnvoicedGain = maxUnvoicedGainDB;
    maxVoicedGain = maxVoicedGainDB;
}

/// Identify frames which are similar to their neighbor and mark them as repeated
///
/// \note   Because the human vocal tract changes rather slowly, consecutive encoded Frames may not always vary
///         significantly. In this case, the size of the bitstream can be reduced by marking certain Frames a repeats of
///         preceding ones and allowing the LPC synthesizer to reuse parameters
void FramePostprocessor::detectRepeatFrames() {
    for (int i = 1; i < frameData->size(); i++) {
        Frame previousFrame = frameData->at(i - 1);
        Frame &frame = frameData->at(i);

        if (frame.isSilent() || previousFrame.isSilent()) {
            continue;
        }

        // TODO: Implement variety of repeat detection algorithms
        // The first reflector coefficient is useful in characterizing a Frame, and experimentally is a good indicator
        // of similarity between consecutive Frames
        int prevCoeff = previousFrame.quantizedCoeffs()[0];
        auto coeff = frame.quantizedCoeffs()[0];

        if (abs(coeff - prevCoeff) == 1) {
            frame.setRepeat(true);
        }
    }
}

/// Normalize Frame gain
///
/// \note Gain normalization gain help reduce DC offsets and improve perceived volume
void FramePostprocessor::normalizeGain() {
    normalizeGain(true);
    normalizeGain(false);
}

/// Shift gain by an integer offset in the coding table
///
/// \note   Following LPC analysis, changing the gain of audio is as simple as selecting a new index of the energy
///         table. A ceiling is applied to the offset to prevent unstable bitstreams
void FramePostprocessor::shiftGain(int offset) {
    // If zero offset, do nothing
    if (!offset) {
        return;
    }

    for (Frame &frame : *frameData) {
        int quantizedGain = frame.quantizedGain();

        // If the Frame is silent, do nothing
        if (quantizedGain == 0) {
            continue;
        }

        // If the shifted gain would exceed the maximum representable gain of the coding table, let it "hit the
        // ceiling." However, because overuse of the largest gain parameter may destabilize the synthesized signal,
        // the shifted gain is ceiling-ed to the penultimate coding table index
        if (quantizedGain + offset >= Tms5220CodingTable::rms.size()) {
            frame.setGain(int(Tms5220CodingTable::rms.size() - 2));

        } else if (quantizedGain > 0) {
            frame.setGain(quantizedGain + offset);
        }
    }
}

/// Normalize gain of either all voiced or all unvoiced frames
///
/// \param normalizeVoicedFrames Whether to operate on voiced or unvoiced frames
void FramePostprocessor::normalizeGain(bool normalizeVoicedFrames) {
    // Compute the max gain value for a Frame category
    float maxGain = 0.0f;
    for (const Frame &frame : *frameData) {
        bool isVoiced = frame.isVoiced();
        float gain = frame.getGain();

        if (isVoiced == normalizeVoicedFrames && gain > maxGain) {
            maxGain = gain;
        }
    }

    // Apply scaling factor to improve naturalness of percieved volume
    float scale = (normalizeVoicedFrames ? maxVoicedGain : maxUnvoicedGain) / maxGain;
    for (Frame &frame : *frameData) {
        bool isVoiced = frame.isVoiced();
        float gain = frame.getGain();

        if (isVoiced == normalizeVoicedFrames) {
            float scaledGain = gain * scale;
            frame.setGain(scaledGain);
        }
    }
}
