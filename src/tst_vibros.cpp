#include <SFML/Audio.hpp>
#include <vector>
#include <math.h>
#include <stdint.h>
#include <iostream>
#include <conio.h>

class V0 {
private:
    double x, a;
    double da;
    double t;
    double dt;
    double amax;
    double damax;
    double damin;
    double p, l;
    bool up;

    void reset(void)
    {
        up = true;
        x = 0.0;
        a = amax;
        t = 0.0;
        da = damax;
    }
public:
    V0(){}
    V0(double dt)
    {
        init(dt);
    }
    void init(double dt)
    {
        V0::dt = dt;
        amax = 0.90;
        damax = (1.0 + 0.02/M_PI);
        damin = (1.0 - 0.0001/M_PI);
    }
    void pick(double p, double l)
    {
        reset();
        V0::p = p;
        V0::l = l;
        //V0::damin = (1.0 - 0.01/(l));
    }
    double timestep(void)
    {
        x = a * sin(t / p);
        t = t + dt;
        a = a * damin;
        /*
        if (up)
            if (a < amax)
                a = (a + 1.0) * damax;
            else
                up = false;
        else
            a = a * damin;
        */
        return x;
    }
};

class Vibro {
private:
    V0 v0[4];
    int vcnt;

    void init(double dt)
    {
    }
public:
    Vibro(double dt)
    {
        vcnt = 0;
        v0[0].init(dt);
        v0[1].init(dt);
        v0[2].init(dt);
        v0[3].init(dt);
    }
    void reset(void)
    {
        //v0.reset();
    }
    void pick(double p, double l)
    {
        v0[vcnt++].pick(p, l);

        if (vcnt >= 4)
            vcnt = 0;
    }
    double timestep(void)
    {
        double a;
        a = (v0[0].timestep()+v0[1].timestep()+v0[2].timestep()+v0[3].timestep()) / 4.0;
        return a;
    }
};

// custom audio stream that plays a loaded buffer
class MyStream : public sf::SoundStream
{
public:
    MyStream() : sf::SoundStream(), vibro(1.0/(double)SAMPLE_RATE){
        initialize(1, SAMPLE_RATE);
        vibro.reset();
        dbgcnt = 0;
    }
    void pick(double p, double l)
    {
        //vibro.reset();
        vibro.pick(p, l);
    }

private:
    Vibro vibro;
    static const unsigned SAMPLES = 4410;
    static const unsigned SAMPLE_RATE = 44100;
    sf::Int16 raw[SAMPLES];
    int dbgcnt;
    int zerocnt;

    virtual bool onGetData(Chunk& data)
    {
        sf::Int16 x, xmax = 0x7ffe;
        double a;
        for (int i=0; i < SAMPLES; i++) {
            a = vibro.timestep();
            x = (sf::Int16) (((double)xmax) * a);
            raw[i] = x;
        }

        // set the pointer to the next audio samples to be played
        data.samples = raw;
        data.sampleCount = SAMPLES;
        return true;

    }

    virtual void onSeek(sf::Time timeOffset)
    {
        // compute the corresponding sample index according to the sample rate and channel count
        //m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
    }

//    std::Int16 m_samples[SAMPLES];
//    std::size_t m_currentSample;
};

int main()
{
    // load an audio buffer from a sound file
//    sf::SoundBuffer buffer;
//    buffer.loadFromFile("sound.wav");

    // initialize and play our custom stream
    MyStream stream;
//    stream.load(buffer);
    stream.play();

    char c;
    double p0 = 2 * M_PI / (440.0 * 32.0), l = 1000.0 * M_PI*M_PI;
    double p = p0;
    while (stream.getStatus() == MyStream::Playing)
    {
        std::cout << "p = " << p / M_PI << " l = " << l / (M_PI * M_PI) << "\n";
        stream.pick(p, l);
        c = getch();
        switch (c){
        case 'p':
            break;
        case 'q':
            return 0;
        case '`':
            p = p0;
            break;
        case '1':
            p = p0 * 1.0/2.0;
            break;
        case '2':
            p = p0 * 2.0/3.0;
            break;
        case '3':
            p = p0 * 3.0/4.0;
            break;
        case '4':
            p = p0 * 4.0/5.0;
            break;
        }
    }
        //sf::sleep(sf::seconds(0.1f));

    return 0;
}
