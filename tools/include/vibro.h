#ifndef __VIBRO_H__
#define __VIBRO_H__

#include <SFML/Audio.hpp>
#include <vector>
#include <math.h>
#include <stdint.h>
#include <iostream>
#include <conio.h>
#include <string.h>

class V0 {
private:
    double x, a;
    double da;
    double t, t0;
    double dt;
    double amax;
    double damax;
    double damin;
    double p;//, l;
    bool up;

    void reset(void);
public:
    V0();
    V0(double dt);
    void init(double dt);
    void pick(double p);//, double l);
    double timestep(void);
};

class Vibro {
private:
    V0 v0[9];
    int vcnt;
    void init(double dt);
public:
    Vibro(double dt);
    void reset(void);
    void pick(double p);//, double l);
    double timestep(void);
};

class VibroStream : public sf::SoundStream
{
public:
    VibroStream();
    void pick(double p);//, double l);
    static const unsigned SAMPLE_RATE = 44100;
	void save_start(char * filename);
	void save_stop();
	bool is_saving;
protected:
	virtual bool onGetData(Chunk& data);
private:
	sf::OutputSoundFile outfile;
    Vibro vibro;
    static const unsigned SAMPLES = 1024;
    sf::Int16 raw[SAMPLES];
    int dbgcnt;
    int zerocnt;
    virtual void onSeek(sf::Time timeOffset);
};

#endif//__VIBRO_H__
