/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

namespace
{
    uint16 generateNoteID (int midiChannel, int midiNoteNumber) noexcept
    {
        jassert (midiChannel > 0 && midiChannel <= 16);
        jassert (midiNoteNumber >= 0 && midiNoteNumber < 128);

        return uint16 ((midiChannel << 7) + midiNoteNumber);
    }
}

//==============================================================================
MPENote::MPENote (int midiChannel_,
                  int initialNote_,
                  MPEValue noteOnVelocity_,
                  MPEValue pitchbend_,
                  MPEValue pressure_,
                  MPEValue timbre_,
                  KeyState keyState_) noexcept
    : noteID (generateNoteID (midiChannel_, initialNote_)),
      midiChannel (uint8 (midiChannel_)),
      initialNote (uint8 (initialNote_)),
      noteOnVelocity (noteOnVelocity_),
      pitchbend (pitchbend_),
      pressure (pressure_),
      timbre (timbre_),
      noteOffVelocity (MPEValue::minValue()),
      keyState (keyState_)
{
    jassert (keyState != MPENote::off);
    jassert (isValid());
}

MPENote::MPENote() noexcept
    : noteID (0),
      midiChannel (0),
      initialNote (0),
      noteOnVelocity (MPEValue::minValue()),
      pitchbend (MPEValue::centreValue()),
      pressure (MPEValue::centreValue()),
      timbre (MPEValue::centreValue()),
      noteOffVelocity (MPEValue::minValue()),
      keyState (MPENote::off)
{
}

//==============================================================================
bool MPENote::isValid() const noexcept
{
    return midiChannel > 0 && midiChannel <= 16 && initialNote < 128;
}

//==============================================================================
double MPENote::getFrequencyInHertz (double frequencyOfA) const noexcept
{
    double pitchInSemitones = double (initialNote) + totalPitchbendInSemitones;
    return frequencyOfA * std::pow (2.0, (pitchInSemitones - 69.0) / 12.0);
}

//==============================================================================
bool MPENote::operator== (const MPENote& other) const noexcept
{
    jassert (isValid() && other.isValid());
    return noteID == other.noteID;
}

bool MPENote::operator!= (const MPENote& other) const noexcept
{
    jassert (isValid() && other.isValid());
    return noteID != other.noteID;
}

//==============================================================================
//==============================================================================
#if JUCE_UNIT_TESTS

class MPENoteTests : public UnitTest
{
public:
    MPENoteTests() : UnitTest ("MPENote class") {}

    //==============================================================================
    void runTest() override
    {
        beginTest ("getFrequencyInHertz");
        {
            MPENote note;
            note.initialNote = 60;
            note.totalPitchbendInSemitones = -0.5;
            expectEqualsWithinOneCent (note.getFrequencyInHertz(), 254.178);
        }
    }

private:
    //==============================================================================
    void expectEqualsWithinOneCent (double frequencyInHertzActual,
                                    double frequencyInHertzExpected)
    {
        double ratio = frequencyInHertzActual / frequencyInHertzExpected;
        double oneCent = 1.0005946;
        expect (ratio < oneCent);
        expect (ratio > 1.0 / oneCent);
    }
};

static MPENoteTests MPENoteUnitTests;

#endif // JUCE_UNIT_TESTS
