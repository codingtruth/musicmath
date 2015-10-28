#include <SFML/Audio.hpp>
#include <vector>
#include <math.h>
#include <stdint.h>
#include <iostream>
#include <conio.h>
#include <string.h>

struct Note {
    int pos;
    int len;
    int pitch[4];
};
struct Sequence {
    double base_p;
    double base_t;
    Note *notes;
    int   notes_num;
};

class V0 {
private:
    double x, a;
    double da;
    double t, t0;
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
        t = 0;
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
        p = 1000000.0;
        l = 0.0;
	t = t0 = 0.0;
        V0::dt = dt;
        amax = 0.90;
        //damax = (1.0 + 0.02/M_PI);
        //damin = (1.0 - 0.0001/(l * dt));
        damin = (1.0 - 0.0001/M_PI);
        reset();
    }
    void pick(double p, double l)
    {
        reset();
        V0::p = p;
        V0::l = l;
	V0::t0 = t;
	V0::a = amax;
        //V0::damin = (1.0 - 0.01/(l));
    }
    double timestep(void)
    {
        x = a * (
	          1.0/1.0  * sin(1.0/1.0  * t / p) 
//		+ 3.0/6.0  * sin(3.0/4.0  * t / p)
		+ 4.0/12.0 * sin(1.0/2.0  * t / p)
//		+ 0.4  * sin(t * 10.0 / p)
//		+ 0.1 * sin(t * 20.0 / p)

//		+ 0.8  * sin(t * 1.8 / p)
//		+ 0.55 * sin(t * 2.592592 / p)
//		+ 0.5  * sin(t * 3.3716475 / p)
//		+ 0.4  * sin(t * 4.137931 / p)

//		+ 0.01 * sin(t * 0.5 / p) 
//		+ 0.16 * sin(t * 2.0 / p) 
//		+ 0.0008 * sin(t * 3.0 / p) 
//		+ 0.08 * sin(t * 4.0 / p) 
//		+ 0.0004 * sin(t * 5.0 / p)
//		+ 0.0002 * sin(t * 6.0 / p)
//		+ 0.002 * sin(t * 8.0 / p) 
		//+ (pow(2.0, -abs(tan(t / p))) - 1.0)
		);
        t = t + dt;

        a = a * damin;
//((t - t0 < l) ? (1 - pow((t - t0) / l, 6.0)) : 1.0);


//(l / (l * pow(4.0,  (t + dt - t0) / l)));
//(1.0 - pow(2.0, -1.0 / ()))
	//( ( l / (l + t + dt - t0)) / ( l ) );
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
    V0 v0[9];
    int vcnt;
    Sequence *sequence;

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
        v0[4].init(dt);
        v0[5].init(dt);
        v0[6].init(dt);
        v0[7].init(dt);
        v0[8].init(dt);
        v0[9].init(dt);
    }
    void reset(void)
    {
    }
    void pick(double p, double l)
    {
        v0[vcnt++].pick(p, l);

        if (vcnt >= 9)
            vcnt = 0;
    }
    double timestep(void)
    {
        double a;
        a = (v0[0].timestep()+v0[1].timestep()+v0[2].timestep()+
             v0[3].timestep()+v0[4].timestep()+v0[5].timestep()+
             v0[6].timestep()+v0[7].timestep()+v0[8].timestep()
            ) / 9.0;
        return a;
    }
};
class Sequencer {
    Vibro vibro;
    Sequence *sequence;
    double pos_t;
    int pos_n;
    double dt;
    bool is_zero_timestep;
    bool is_end_of_sequence;
    sf::OutputSoundFile outfile;
    static const int file_buf_size = 1024 * 512;
    sf::Int16 file_buf[file_buf_size];
public:
    Sequencer(double dt) : vibro(dt)
    {
        Sequencer::dt = dt;
        is_zero_timestep = true;
    }
    void reset(void)
    {
        vibro.reset();
        pos_t = 0.0;
        pos_n = 0;
    }
    void save_sequence_to_file(Sequence *sequence, const char *filename, unsigned int sample_rate)
    {
        double a;
        int16_t n;
        uint32_t len, pos;
        outfile.openFromFile(filename, sample_rate, 1);
        start(sequence);
        pos = 0;
        while (!is_end_of_sequence) {
            a = timestep();
            n = (int16_t)(((double)0x7ffe) * a);
            file_buf[pos++] = n;
            if (pos == file_buf_size) {
                outfile.write(file_buf, pos);
                pos = 0;
            }
        }
        outfile.write(file_buf, pos);
    }
    void start(Sequence *sequence)
    {
        Sequencer::sequence = sequence;
        reset();
        is_zero_timestep = true;
        is_end_of_sequence = false;
    }
    double timestep(void)
    {
        if (is_end_of_sequence)
            return 0.0;
        if (is_zero_timestep) {
            play_next_note();
            is_zero_timestep = false;
        } else
        if (is_next_note_time(pos_t))
            play_next_note();
        pos_t += dt;
        return vibro.timestep();
    }
    bool is_next_note_time(double t)
    {
        if (pos_n >= sequence->notes_num)
            return false;
        if (pos_t >= sequence->base_t * ((double)sequence->notes[pos_n].pos))
            return true;
        return false;
    }
    bool play_next_note(void)
    {
        double p, l;
        Note *note;
        note = &sequence->notes[pos_n];
        if (note->len == 0) {
            std::cout << "note " << pos_n << "  pos " << sequence->notes[pos_n].pos << "\n";
            is_end_of_sequence = true;
            return false;
        }
	if (note->len < 0 ) {
            if (--note->pitch[3] > note->len) {
                std::cout << "note " << pos_n << "  pos " << sequence->notes[pos_n].pos << "\n";
		pos_n = note->pitch[0];
		pos_t = ((double)sequence->notes[pos_n].pos);
	        note = &sequence->notes[pos_n];
            } else {
                pos_n++;
                note = &sequence->notes[pos_n];                
            }
	}
		
        std::cout << "note " << pos_n << "  pos " << sequence->notes[pos_n].pos << "\n";
        p = sequence->base_p;
        if (note->pitch[0] > 0) {
            p *= ((double)note->pitch[0]) / ((double)note->pitch[1]);
        } else {
            p *= pow(2.0, ((double)note->pitch[0]) / ((double)note->pitch[1]));
        }
        if (note->pitch[2] > 0) {
            p *= ((double)note->pitch[2]) / ((double)note->pitch[3]);
        } else {
            p *= pow(2.0, ((double)note->pitch[2]) / ((double)note->pitch[3]));
        }
        l = sequence->base_t * ((double)note->len);
        std::cout << "base_p = " << sequence->base_p << "   p = " << p << "  l = " << l << "\n";
        if (note->len != 0)
            vibro.pick(p, l);
        pos_n++;
    }
};


// custom audio stream that plays a loaded buffer
class MyStream : public sf::SoundStream
{
public:
    MyStream() : sf::SoundStream(),
                    vibro(1.0/(double)SAMPLE_RATE),
                    sequencer(1.0/(double)SAMPLE_RATE)
    {
        initialize(1, SAMPLE_RATE);
        vibro.reset();
        dbgcnt = 0;
        memset(raw, 0, sizeof(raw));
        sequencer.reset();
    }
    void pick(double p, double l)
    {
        //vibro.reset();
        vibro.pick(p, l);
    }
    void save_sequence_to_file(Sequence *sequence, const char *filename, unsigned int sample_rate)
    {
        sequencer.save_sequence_to_file(sequence, filename, sample_rate);
    }
    void play_sequence(Sequence *sequence)
    {
        sequencer.start(sequence);
    }
    static const unsigned SAMPLE_RATE = 44100;
private:
    Vibro vibro;
    Sequencer sequencer;
    static const unsigned SAMPLES = 1024;
    sf::Int16 raw[SAMPLES];
    int dbgcnt;
    int zerocnt;

    virtual bool onGetData(Chunk& data)
    {
        sf::Int16 x, xmax = 0x7ffe;
        double a;
        for (int i=0; i < SAMPLES; i++) {
            a = (vibro.timestep() + sequencer.timestep());// / 2.0;
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

#define REAL    1
#define NATURAL 2

#define MODE NATURAL

#define	I1	{1,  1,  1, 1}
#define II1	{8,  9,  1, 1}
#define IIIm1	{5,  6,  1, 1}
#define III1	{4,  5,  1, 1}
#define IV1	{3,  4,  1, 1}
#define V1	{2,  3,  1, 1}
#define	VIm1	{5,  8,  1, 1}
#define	VIIm1	{5,  9,  1, 1}
#define	VI1	{3,  5,  1, 1}
#define	VII1	{8, 15,  1, 1}

#define	I2	{1,  1,  2, 1}
#define II2	{8,  9,  2, 1}
#define IIIm2	{5,  6,  2, 1}
#define III2	{4,  5,  2, 1}
#define IV2	{3,  4,  2, 1}
#define V2	{2,  3,  2, 1}
#define	VIm2	{5,  8,  2, 1}
#define	VIIm2	{5,  9,  2, 1}
#define	VI2	{3,  5,  2, 1}
#define	VII2	{8, 15,  2, 1}

#define	I3	{1,  1,  4, 1}
#define II3	{8,  9,  4, 1}
#define IIIm3	{5,  6,  4, 1}
#define III3	{4,  5,  4, 1}
#define IV3	{3,  4,  4, 1}
#define V3	{2,  3,  4, 1}
#define	VIm3	{5,  8,  4, 1}
#define	VIIm3	{5,  9,  4, 1}
#define	VI3	{3,  5,  4, 1}
#define	VII3	{8, 15,  4, 1}

#define	I4	{1,  1,  8, 1}
#define II4	{8,  9,  8, 1}
#define IIIm4	{5,  6,  8, 1}
#define III4	{4,  5,  8, 1}
#define IV4	{3,  4,  8, 1}
#define V4	{2,  3,  8, 1}
#define	VIm4	{5,  8,  8, 1}
#define	VIIm4	{5,  9,  8, 1}
#define	VI4	{3,  5,  8, 1}
#define	VII4	{8, 15,  8, 1}

/*
====================================
1, 2, 3,   5,  8, 13,   21,      34
    [+6]        [+26]          [+55]
     [6]  [11]   [32]           [87]
====================================

====================================
1, 2, 3,   5,  8, 13,   13, 8,   26
    [+6]        [+26]          [+47]
     [6]  [11]   [32]           [79]
====================================
*/
Note fnl_1_notes[] =
{
    {   0,  1,  I3	},
    {   1,  2,  IIIm3	},
    {   3,  3,  V3	},
//					{   6,  3,  {  -3,  4,   2,  1}	},
    {   6,  5,  IIIm2	},
    {  11,  7,  II2	},
//					{  18,  3,  {   1,  1,   2,  1}	},
    {  18, 11,  V3	},
//					{  29, 19,  {   4,  5,   2,  1}	},
    {  29, 19,  VIIm3	},
//					{  29,  6,  {   4,  5,   1,  1}	},
//					{  35,  6,  {   2,  3,   1,  1}	},
//					{  41,  6,  {   9, 10,   1,  2}	},
    {  48, 26,  I2	},
//					{  48,  7,  {   3,  4,   1,  1}	},
//					{  48,  7,  {  -3,  4,   1,  1}	},
//					{  55,  7,  {   1,  2,   1,  1}	},
//					{  62,  7,  {   4,  5,   1,  2}	},


    {  74,  1,  I3	},
    {  75,  2,  IIIm3	},
    {  77,  3,  V3	},
//					{  80,  5,  {   -3,  4,   2,  1} },
    {  80,  5,  IIIm2	},
    {  85,  7,  II2	},
//					{  42,  3,  {   1,  1,   2,  1}	},
    {  92, 11,  V3	},
//					{ 103, 20,  {   4,  5,   2,  1}	},
    { 103, 13,  VIIm3	},
    { 116,  7,  I2	},
//					{ 123, 29,  {   1,  1,   2,  1}	},
    { 123, 29,  V3	},


    { 152,  1,  I3	},
    { 153,  2,  IIIm3	},
    { 155,  3,  V3	},
//					{   0,  3,  {  -3,  4,   2,  1}	},
    { 158,  5,  IIIm2	},
    { 163,  7,  II2	},
//					{   6,  3,  {   1,  1,   2,  1}	},
    { 170, 13,  V3	},
//					{  12,  3,  {   4,  5,   2,  1}	},
    { 183, 17,  VIIm3},

    { 200, 19,  I2	},


    { 219,  6,  VIIm3	},
    { 225,  7,  VIm3	},

//					{ 108,  3,  {   1,  1,   2,  1}	},
    { 232, 17,  V3	},

//					{ 117,  3,  { -11, 12,   2,  1}	},
    { 249, 19,  IV3	},

//					{ 128,  3,  {  -3,  4,   2,  1}	},
    { 268, 30,  III3	},



//#if 1
// arp

    { 298,  1,  IV4	},
    { 299,  2,  I4	},
    { 301,  3,  IV3	},

//#else

{ 304,  4,  VIm3	},
{ 308,  4,  V3		},
{ 312,  4,  IV3	},
{ 316,  4,  V3		},
{ 320,  4,  VIm3	},
{ 324,  4,  VIIm3	},

{ 326,  1,  I3	},
{ 327,  1,  IIIm3	},
{ 328,  1,  VIm3	},

//{ 328,  4,  {   1,  1,   3,  4}	},

{ 329,  4,  I2	},
{ 333,  4,  VIIm3	},
{ 337,  4,  VIm3	},
{ 341,  4,  VIIm3	},
{ 345,  4,  I2		},
{ 349,  4,  II2		},


{ 351,  1,  I3	},
{ 352,  1,  IIIm3	},
{ 353,  1,  V3	},

//{ 352,  4,  {   1,  1, -21, 12}	},

{ 354,  4,  IIIm2	},
{ 358,  4,  II2		},
{ 362,  4,  I2		},
{ 366,  4,  II2		},
{ 370,  4,  IIIm2	},
{ 374,  4,  IV2		},
//.{ 362,  4,  {  10, 10,  -9, 12}	},
//.{ 366,  4,  {   9, 10,  -9, 12}	},
//.{ 370,  4,  {  -1,  4,  -9, 12}	},
//.{ 374,  4,  {   3,  4,  -9, 12}	},

//#endif

#if 1

{ 377,  1,  {   4,  5,   2,  1}	},
{ 378,  1,  { -11, 12,   2,  1}	},
{ 379,  1,  {   4,  5,   1,  1}	},
//{ 379,  1,  { -11, 12,   1,  1}	},

{ 380, 32,  {   2,  3,  -9, 12}	},


{ 412,  1,  {   2,  3,  18, 10}	},
{ 413,  2,  {   9, 10,  18, 10}	},
{ 415,  3,  {   2,  3,   9, 10}	},


{ 418,  4,  {  -1,  4,  -3,  4}	},
{ 422,  4,  {   9, 10,  -3,  4}	},
{ 426,  4,  {   1,  1,  -3,  4}	},
{ 430,  4,  {   9, 10,  -3,  4}	},
{ 434,  4,  {  -1,  4,  -3,  4}	},
{ 438,  4,  {   3,  4,  -3,  4}	},

{ 440,  1,  {   1,  1,   1,  1}	},
{ 441,  1,  {   4,  5,   1,  1}	},
{ 442,  1,  {   3,  3,   1,  1}	},

{ 443,  4,  {   4,  5,   1,  2} },
{ 447,  4,  {   9, 10,   1,  2} },
{ 451,  4,  {   1,  1,   1,  2} },
{ 455,  4,  {   9, 10,   1,  2} },
{ 459,  4,  {   4,  5,   1,  2} },
{ 463,  4,  { -11, 12,   2,  3} },

{ 465,  1,  {   1,  1,   4,  5}	},
{ 466,  1,  {  -1,  4,   4,  5}	},
{ 467,  1,  {   2,  3,   4,  5}	},

{ 468,  4,  {  -5,  6,  -3,  4}	},
{ 472,  4,  {  -3,  4,  -3,  4}	},
{ 476,  4,  {   2,  3,  -3,  4}	},
{ 480,  4,  {  -3,  4,  -3,  4}	},
{ 484,  4,  {  -5,  6,  -3,  4}	},
{ 488,  4,  {   1,  2,  -3,  4}	},

//{ 472,  2,  { -11, 12,   1,  2}	},
//{ 474,  2,  {   1,  2,   1,  2}	},

/* interesting version
{ 222,  2,  { -11, 12,   1,  2} },

{ 224,  2,  {  -5,  6,  -3,  4}	},
{ 226,  2,  {  -3,  4,  -3,  4}	},
{ 228,  2,  {   2,  3,  -3,  4}	},
{ 230,  2,  {  -3,  4,  -3,  4}	},
{ 232,  2,  {  -5,  6,  -3,  4}	},
{ 234,  2,  {   1,  2,  -3,  4}	},
*/

//{ 472, 28,  {   4,  5,  -3,  4}	},
//{ 500, 32,  {  -3,  4,  -3,  4}	},

{ 490,  1,  {   2,  1,  -11, 12}	},
{ 491,  2,  {   8,  5,  -11, 12}	},
{ 493,  3,  {   6,  3,  -11, 12}	},

{ 497, 29,  {   4,  5,  -11, 12}	},

{ 526,  1,  {   2,  1,   4,  5}	},
{ 527,  2,  {   8,  5,   4,  5}	},
{ 529,  3,  {   6,  3,   4,  5}	},

{ 532, 35,  {  -5,  6,   4,  5}	},
//{ 532, 38,  { -11, 12,   1,  1}	},

    { 567,  -2,  {   0,  0,   0,  0}	},

{ 567,  1,  {   2,  1,  -3,  4}	},
{ 568,  2,  {   8,  5,  -3,  4}	},
{ 570,  3,  {   4,  3,  -3,  4}	},
//{ 571,  4,  {   2,  3,  -3,  4}	},
{ 573, 76,  {   1,  1,  -3,  4}	},
{ 573, 76,  {   4,  5,  -3,  4}	},
//{ 576, 70,  {  19, 20,   1,  2}	},

//{ 576, 70,  {  19, 20,   1,  2}	},

#else

{ 376, 28,  {   2,  3,  -9, 12}	},


{ 404,  4,  {  -1,  4,  -3,  4}	},
{ 408,  4,  {   9, 10,  -3,  4}	},
{ 412,  4,  {   1,  1,  -3,  4}	},
{ 416,  4,  {   9, 10,  -3,  4}	},
{ 420,  4,  {  -1,  4,  -3,  4}	},
{ 424,  4,  {   3,  4,  -3,  4}	},

{ 428,  4,  {   4,  5,   1,  2} },
{ 432,  4,  {   9, 10,   1,  2} },
{ 436,  4,  {   1,  1,   1,  2} },
{ 440,  4,  {   9, 10,   1,  2} },
{ 444,  4,  {   4,  5,   1,  2} },
{ 448,  4,  { -11, 12,   2,  3} },

{ 452,  4,  {  -5,  6,  -3,  4}	},
{ 456,  4,  {  -3,  4,  -3,  4}	},
{ 460,  4,  {   2,  3,  -3,  4}	},
{ 464,  4,  {  -3,  4,  -3,  4}	},
{ 468,  4,  {  -5,  6,  -3,  4}	},
{ 472,  4,  {   1,  2,  -3,  4}	},

/* interesting version
{ 222,  2,  { -11, 12,   1,  2} },

{ 224,  2,  {  -5,  6,  -3,  4}	},
{ 226,  2,  {  -3,  4,  -3,  4}	},
{ 228,  2,  {   2,  3,  -3,  4}	},
{ 230,  2,  {  -3,  4,  -3,  4}	},
{ 232,  2,  {  -5,  6,  -3,  4}	},
{ 234,  2,  {   1,  2,  -3,  4}	},
*/

//{ 472, 28,  {   4,  5,  -3,  4}	},
//{ 500, 32,  {  -3,  4,  -3,  4}	},
{ 476, 28,  {  -1,  2,  -3,  4}	},

{ 504, 28,  {  -5,  6,   4,  5}	},
{ 504, 28,  { -11, 12,   1,  2}	},

    { 532,  -6,  {   0,  0,   0,  0}	},

{ 532, 76,  {  19, 20,   1,  2}	},
{ 532, 76,  {   1,  1,  -3,  4}	},




#endif
    { 651,   0,  {   0,  0,   0,  0}	},


};

Sequence fnl_1_sequence;

int main()
{
    //fnl_1_sequence.base_p = 2 * M_PI / (1573.2 * 32.0);
    fnl_1_sequence.base_p = 2.0/3.0 * M_PI / (432.0 * 32.0);
    fnl_1_sequence.base_t =  (18.5 / 12.0 * (3600.0 / (1080.0 * 76.0)));//(60.0 / 984.0);
    fnl_1_sequence.notes  = fnl_1_notes;
    fnl_1_sequence.notes_num = sizeof(fnl_1_notes)/sizeof(fnl_1_notes[0]);

    // load an audio buffer from a sound file
//    sf::SoundBuffer buffer;
//    buffer.loadFromFile("sound.wav");

    // initialize and play our custom stream
    MyStream stream;
//    stream.load(buffer);
    char c;
    double p0;
    p0 = 2.0/3.0 * M_PI / (432.0 * 32.0);
    double l;
    l = 1000.0 * M_PI*M_PI;
    double p;
    p = p0;

    //stream.save_sequence_to_file(&fnl_1_sequence, "fnl_5.wav", MyStream::SAMPLE_RATE);

    //stream.play_sequence(&fnl_1_sequence);
    stream.play();


    while (stream.getStatus() == MyStream::Playing)
    {
        std::cout << "p = " << p / M_PI << " l = " << l / (M_PI * M_PI) << "\n";
        c = getch();
        switch (c){
        //case 'z':
        //    stream.play_sequence(&fnl_1_sequence);
        //    break;

        //case 'p':
        //    break;
        case 'z':
            return 0;

        case '`':
        	p = p0;
        	break;
        case '1':
        	p = p0 * 1.0/1.0;
        	break;
        case '2':
        	p = p0 * 8.0/9.0;
        	break;
        case '3':
        	p = p0 * 4.0/5.0;
        	break;
        case '4':
        	p = p0 * 3.0/4.0;
        	break;
	case '5':
		p = p0 * 2.0/3.0;
		break;
	case '6':
		p = p0 * 3.0/5.0;
		break;
	case '7':
		p = p0 * 8.0/15.0;
		break;
	case '8':
		p = p0 * 1.0/2.0;
		break;
	case '9':
		p = p0 * 1.0/2.0 * 8.0/9.0;
		break;
	case '0':
		p = p0 * 1.0/2.0 * 4.0/5.0;
		break;
        case '-':
        	p = p0 * 1.0/2.0 * 3.0/4.0;
			//p = p0 * 89.0/100.0;
        	break;
        case '=':
        	p = p0 * 1.0/2.0 * 2.0/3.0;
			//p = p0 * 943.0/1000.0;
        	break;

        case 'q':
        	p = p0 * 1.0/1.0;
        	break;
        case 'w':
        	p = p0 * 8.0/9.0;
        	break;
        case 'e':
        	p = p0 * 5.0/6.0;
        	break;
        case 'r':
        	p = p0 * 3.0/4.0;
        	break;
	case 't':
		p = p0 * 2.0/3.0;
		break;
	case 'y':
		p = p0 * 5.0/8.0;
		break;
	case 'u':
		p = p0 * 5.0/9.0;
		break;
	case 'i':
		p = p0 * 1.0/2.0;
		break;
	case 'o':
		p = p0 * 1.0/2.0 * 8.0/9.0;
		break;
	case 'p':
		p = p0 * 1.0/2.0 * 5.0/6.0;
		break;
        case '[':
            p = p0 * 1.0/2.0 * 3.0/4.0;
			//p = p0 * 89.0/100.0;
            break;
        case ']':
            p = p0 * 1.0/2.0 * 2.0/3.0;
			//p = p0 * 943.0/1000.0;
            break;
        }
        stream.pick(p, l);
    }
        //sf::sleep(sf::seconds(0.1f));

    return 0;
}
