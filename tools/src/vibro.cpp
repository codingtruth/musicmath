#include "vibro.h"

void V0::reset(void)
    {
        up = true;
        x = 0.0;
        a = amax;
        t = 0;
        da = damax;
    }
V0::V0(){}
V0::V0(double dt)
    {
        init(dt);
    }
void V0::init(double dt)
    {
        p = 1000000.0;
        //l = 0.0;
		t = t0 = 0.0;
        V0::dt = dt;
        amax = 0.90;
        damin = (1.0 - 0.0001/M_PI);
        reset();
    }
void V0::pick(double p)//, double l)
    {
        reset();
        V0::p = p;
        //V0::l = l;
		V0::t0 = t;
		V0::a = amax;
    }
double V0::timestep(void)
    {
        x = a * (
	          1.0/1.0  * sin(1.0/1.0  * t / p) 
			+ 1.0/12.0 * sin(2.0/1.0  * t / p)
			- 7.0/12.0 * sin(1.0/2.0  * t / p)
			+ 1.0/24.0 * sin(8.0/1.0  * t / p)
			- 13.0/24.0 * sin(1.0/4.0  * t / p)
			//+ 1.0/48.0 * sin(16.0/1.0  * t / p)
			- 17.0/48.0 * sin(1.0/16.0  * t / p)
		);
        t = t + dt;
        a = a * damin;
        return x;
    }

void Vibro::init(double dt)
    {
    }

Vibro::Vibro(double dt)
    {
        vcnt = 0;
        v0[0].init(dt);
        v0[1].init(dt);
        v0[2].init(dt);
        v0[3].init(dt);
        v0[4].init(dt);
        v0[5].init(dt);
        v0[6].init(dt);
        v0[7].init(dt);
        v0[8].init(dt);
        v0[9].init(dt);
    }
void Vibro::reset(void)
    {
    }
void Vibro::pick(double p)//, double l)
    {
        v0[vcnt++].pick(p);//, l);

        if (vcnt >= 9)
            vcnt = 0;
    }
double Vibro::timestep(void)
    {
        double a;
        a = (v0[0].timestep()+v0[1].timestep()+v0[2].timestep()+
             v0[3].timestep()+v0[4].timestep()+v0[5].timestep()+
             v0[6].timestep()+v0[7].timestep()+v0[8].timestep()
            ) / 9.0;
        return a;
    }

VibroStream::VibroStream() : sf::SoundStream(), vibro(1.0/(double)SAMPLE_RATE)
    {
		is_saving = false;
        initialize(1, SAMPLE_RATE);
        vibro.reset();
        dbgcnt = 0;
        memset(raw, 0, sizeof(raw));
    }
void VibroStream::pick(double p)//, double l)
    {
        vibro.pick(p);//, l);
    }
bool VibroStream::onGetData(Chunk& data)
    {
        sf::Int16 x, xmax = 0x7ffe;
        double a;
        for (int i=0; i < SAMPLES; i++) {
            a = vibro.timestep();// / 2.0;
            x = (sf::Int16) (((double)xmax) * a);
            raw[i] = x;
        }

        // set the pointer to the next audio samples to be played
        data.samples = raw;
        data.sampleCount = SAMPLES;
		
		if (is_saving)
		{
			outfile.write(raw, SAMPLES);
		}
        return true;
    }

void VibroStream::onSeek(sf::Time timeOffset)
    {
    }

void VibroStream::save_start(char * filename)
{
    outfile.openFromFile(filename, SAMPLE_RATE, 1);
	is_saving = true;
}