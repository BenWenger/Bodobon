
#include "blsynth.h"

namespace
{
    ////////////////////////////////////////////////////////////////////
    //  There are 32 "sets".
    //      Set 0 is the transition happening exactly on a sample.
    //      Each following set is the transition happening progressively further between two samples, with set 16 being
    //   exactly halfway.
    //
    //      The sum of all transitions in a set is equal to 1.0, resulting in a full transition.
    //
    //      These sets make up the "ripples" that happen near the transitions of audio changes in BL-synth
    //
    const float sincTable[0x20][13] = {
        { -0.01881860f,  0.04358196f, -0.05657481f,  0.06979057f, -0.08697524f,  0.15081083f,  0.79637059f,  0.15081083f, -0.08697524f,  0.06979057f, -0.05657481f,  0.04358196f, -0.01881860f },
        { -0.01945354f,  0.04421701f, -0.05558047f,  0.06619458f, -0.07859476f,  0.12590771f,  0.79545561f,  0.17645174f, -0.09493902f,  0.07291968f, -0.05714877f,  0.04260481f, -0.01803459f },
        { -0.01993629f,  0.04450949f, -0.05417985f,  0.06216846f, -0.06987282f,  0.10182403f,  0.79271466f,  0.20274391f, -0.10241105f,  0.07554779f, -0.05729117f,  0.04128859f, -0.01710575f },
        { -0.02026493f,  0.04446150f, -0.05238979f,  0.05775124f, -0.06088443f,  0.07863619f,  0.78815967f,  0.22959627f, -0.10931694f,  0.07764365f, -0.05699384f,  0.03963890f, -0.01603749f },
        { -0.02043870f,  0.04407763f, -0.05022974f,  0.05298396f, -0.05170392f,  0.05641500f,  0.78181042f,  0.25691367f, -0.11558341f,  0.07917924f, -0.05625177f,  0.03766392f, -0.01483631f },
        { -0.02045799f,  0.04336488f, -0.04772156f,  0.04790931f, -0.04240451f,  0.03522540f,  0.77369452f,  0.28459733f, -0.12113872f,  0.08012994f, -0.05506319f,  0.03537439f, -0.01350981f },
        { -0.02032436f,  0.04233264f, -0.04488933f,  0.04257131f, -0.03305792f,  0.01512620f,  0.76384715f,  0.31254524f, -0.12591316f,  0.08047487f, -0.05342957f,  0.03278354f, -0.01206660f },
        { -0.02004048f,  0.04099251f, -0.04175915f,  0.03701497f, -0.02373397f, -0.00383014f,  0.75231099f,  0.34065273f, -0.12983946f,  0.08019705f, -0.05135578f,  0.02990701f, -0.01051629f },
        { -0.01961011f,  0.03935828f, -0.03835890f,  0.03128599f, -0.01450018f, -0.02159770f,  0.73913589f,  0.36881291f, -0.13285325f,  0.07928364f, -0.04885000f,  0.02676282f, -0.00886939f },
        { -0.01903807f,  0.03744573f, -0.03471806f,  0.02543039f, -0.00542145f, -0.03813724f,  0.72437869f,  0.39691721f, -0.13489347f,  0.07772608f, -0.04592376f,  0.02337123f, -0.00713728f },
        { -0.01833017f,  0.03527250f, -0.03086739f,  0.01949419f,  0.00344031f, -0.05341630f,  0.70810289f,  0.42485595f, -0.13590283f,  0.07552031f, -0.04259195f,  0.01975461f, -0.00533211f },
        { -0.01749319f,  0.03285800f, -0.02683879f,  0.01352309f,  0.01202646f, -0.06740929f,  0.69037830f,  0.45251880f, -0.13582813f,  0.07266685f, -0.03887270f,  0.01593733f, -0.00346673f },
        { -0.01653478f,  0.03022315f, -0.02266498f,  0.00756219f,  0.02028195f, -0.08009752f,  0.67128071f,  0.47979541f, -0.13462073f,  0.06917092f, -0.03478733f,  0.01194559f, -0.00155458f },
        { -0.01546344f,  0.02739030f, -0.01837926f,  0.00165561f,  0.02815554f, -0.09146916f,  0.65089145f,  0.50657594f, -0.13223683f,  0.06504252f, -0.03036028f,  0.00780727f,  0.00039036f },
        { -0.01428841f,  0.02438300f, -0.01401530f, -0.00415373f,  0.03560005f, -0.10151924f,  0.62929702f,  0.53275158f, -0.12863788f,  0.06029644f, -0.02561893f,  0.00355171f,  0.00235370f },
        { -0.01301960f,  0.02122580f, -0.00960685f, -0.00982445f,  0.04257257f, -0.11024953f,  0.60658861f,  0.55821515f, -0.12379082f,  0.05495233f, -0.02059351f, -0.00079045f,  0.00432075f },
        { -0.01166752f,  0.01794411f, -0.00518749f, -0.01531690f,  0.04903463f, -0.11766843f,  0.58286161f,  0.58286161f, -0.11766843f,  0.04903463f, -0.01531690f, -0.00518749f,  0.00627659f },
        { -0.01024320f,  0.01456395f, -0.00079045f, -0.02059351f,  0.05495233f, -0.12379082f,  0.55821515f,  0.60658861f, -0.11024953f,  0.04257257f, -0.00982445f, -0.00960685f,  0.00820620f },
        { -0.00875809f,  0.01111180f,  0.00355171f, -0.02561893f,  0.06029644f, -0.12863788f,  0.53275158f,  0.62929702f, -0.10151924f,  0.03560005f, -0.00415373f, -0.01401530f,  0.01009459f },
        { -0.00722398f,  0.00761434f,  0.00780727f, -0.03036028f,  0.06504252f, -0.13223683f,  0.50657594f,  0.65089145f, -0.09146916f,  0.02815554f,  0.00165561f, -0.01837926f,  0.01192686f },
        { -0.00565291f,  0.00409833f,  0.01194559f, -0.03478733f,  0.06917092f, -0.13462073f,  0.47979541f,  0.67128071f, -0.08009752f,  0.02028195f,  0.00756219f, -0.02266498f,  0.01368837f },
        { -0.00405706f,  0.00059033f,  0.01593733f, -0.03887270f,  0.07266685f, -0.13582813f,  0.45251880f,  0.69037830f, -0.06740929f,  0.01202646f,  0.01352309f, -0.02683879f,  0.01536481f },
        { -0.00244868f, -0.00288344f,  0.01975461f, -0.04259195f,  0.07552031f, -0.13590283f,  0.42485595f,  0.70810289f, -0.05341630f,  0.00344031f,  0.01949419f, -0.03086739f,  0.01694234f },
        { -0.00083999f, -0.00629729f,  0.02337123f, -0.04592376f,  0.07772608f, -0.13489347f,  0.39691721f,  0.72437869f, -0.03813724f, -0.00542145f,  0.02543039f, -0.03471806f,  0.01840766f },
        {  0.00075691f, -0.00962630f,  0.02676282f, -0.04885000f,  0.07928364f, -0.13285325f,  0.36881291f,  0.73913589f, -0.02159770f, -0.01450018f,  0.03128599f, -0.03835890f,  0.01974817f },
        {  0.00233012f, -0.01284641f,  0.02990701f, -0.05135578f,  0.08019705f, -0.12983946f,  0.34065273f,  0.75231099f, -0.00383014f, -0.02373397f,  0.03701497f, -0.04175915f,  0.02095204f },
        {  0.00386806f, -0.01593466f,  0.03278354f, -0.05342957f,  0.08047487f, -0.12591316f,  0.31254524f,  0.76384715f,  0.01512620f, -0.03305792f,  0.04257131f, -0.04488933f,  0.02200828f },
        {  0.00535949f, -0.01886930f,  0.03537439f, -0.05506319f,  0.08012994f, -0.12113872f,  0.28459733f,  0.77369452f,  0.03522540f, -0.04240451f,  0.04790931f, -0.04772156f,  0.02290689f },
        {  0.00679366f, -0.02162997f,  0.03766392f, -0.05625177f,  0.07917924f, -0.11558341f,  0.25691367f,  0.78181042f,  0.05641500f, -0.05170392f,  0.05298396f, -0.05022974f,  0.02363893f },
        {  0.00816034f, -0.02419782f,  0.03963890f, -0.05699384f,  0.07764365f, -0.10931694f,  0.22959627f,  0.78815967f,  0.07863619f, -0.06088443f,  0.05775124f, -0.05238979f,  0.02419657f },
        {  0.00944989f, -0.02655564f,  0.04128859f, -0.05729117f,  0.07554779f, -0.10241105f,  0.20274391f,  0.79271466f,  0.10182403f, -0.06987282f,  0.06216846f, -0.05417985f,  0.02457320f },
        {  0.01065338f, -0.02868796f,  0.04260481f, -0.05714877f,  0.07291968f, -0.09493902f,  0.17645174f,  0.79545561f,  0.12590771f, -0.07859476f,  0.06619458f, -0.05558047f,  0.02476347f } 
    };
}

namespace bodobeep
{
    void BlSynth::setFormat(int sampleRate, double clockRate, double frameSizeInClockCycles)
    {
        transMult = static_cast<timestamp_t>( (sampleRate / clockRate) * (1 << fractionBits) );

        auto bufferSize = static_cast<std::size_t>( frameSizeInClockCycles * transMult / (1<<fractionBits) );
        bufferSize += 60;       // a bit of padding just to be safe

        if(transitionBuffer.size() < bufferSize)
            transitionBuffer.resize(bufferSize);
    }

    void BlSynth::clear()
    {
        for(auto& i : transitionBuffer)
            i = 0;
        
        transAdd = 0;
        outputSample = 0;
    }

    void BlSynth::addTransition(timestamp_t timestamp, float transition)
    {
        auto pos = ((timestamp * transMult) + transAdd) >> dropBits;

        const float* set = sincTable[pos & setMask];
        pos >>= setBits;

        float total = 0;
        for(int i = 0; i < 13; ++i)
        {
            float x = transition * set[i];
            transitionBuffer[pos+i] += x;
            total += x;
        }

        // error goes into [6]
        transitionBuffer[pos+6] += transition - total;
    }

    int BlSynth::generateSamples(const s16*& buffer, timestamp_t timestamp)
    {
        auto finalPos = (timestamp * transMult) + transAdd;
        auto completeSamples = finalPos >> fractionBits;
        transAdd = finalPos & fractionMask;

        buffer = sampleBuffer.data();
        if(completeSamples <= 0)
            return 0;

        if(sampleBuffer.size() < static_cast<std::size_t>(completeSamples))
            sampleBuffer.resize(completeSamples);

        // fill samples
        for(std::size_t i = 0; i < static_cast<std::size_t>(completeSamples); ++i)
        {
            outputSample += transitionBuffer[i];
            outputSample -= (outputSample * 0.01f);
            if(outputSample > 1.0f)         outputSample = 1.0f;
            else if(outputSample < -1.0f)   outputSample = -1.0f;

            sampleBuffer[i] = static_cast<s16>(outputSample * 0x7FFF);
        }

        // wipe transition buffer
        for(std::size_t i = completeSamples; i < transitionBuffer.size(); ++i)
            transitionBuffer[i - completeSamples] = transitionBuffer[i];
        for(std::size_t i = transitionBuffer.size() - completeSamples; i < transitionBuffer.size(); ++i)
            transitionBuffer[i] = 0;

        buffer = sampleBuffer.data();
        return completeSamples;
    }
}