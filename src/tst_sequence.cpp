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
        x = a * (sin(t / p) 
		+ 0.5 * sin(t * 2.0 / p) 
		+ 0.005 * sin(t * 3.0 / p) 
		+ 0.03 * sin(t * 4.0 / p) 
		+ 0.005 * sin(t * 5.0 / p)
		+ 0.005 * sin(t * 6.0 / p)
		+ 0.01 * sin(t * 8.0 / p) 
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
    V0 v0[6];
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
    }
    void reset(void)
    {
    }
    void pick(double p, double l)
    {
        v0[vcnt++].pick(p, l);

        if (vcnt >= 6)
            vcnt = 0;
    }
    double timestep(void)
    {
        double a;
        a = (v0[0].timestep()+v0[1].timestep()+v0[2].timestep()+v0[3].timestep()+v0[4].timestep()+v0[5].timestep()) / 6.0;
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
    void start(Sequence *sequence)
    {
        Sequencer::sequence = sequence;
        reset();
        is_zero_timestep = true;
    }
    double timestep(void)
    {
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
	if (note->len == -1) {
		pos_n = 0;
		pos_t = 0.0;
	        note = &sequence->notes[pos_n];
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
    void play_sequence(Sequence *sequence)
    {
        sequencer.start(sequence);
    }
private:
    Vibro vibro;
    Sequencer sequencer;
    static const unsigned SAMPLES = 1024;
    static const unsigned SAMPLE_RATE = 44100;
    sf::Int16 raw[SAMPLES];
    int dbgcnt;
    int zerocnt;

    virtual bool onGetData(Chunk& data)
    {
        sf::Int16 x, xmax = 0x7ffe;
        double a;
        for (int i=0; i < SAMPLES; i++) {
            a = (vibro.timestep() + sequencer.timestep()) / 2.0;
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

#if (MODE == REAL)
Note fnl_1_notes[] =
{
    {   0,  3 + 24,  {   1,  2,   1,  1}	},
    {   3,  3 + 21,  { -11, 12,   1,  1}	},
    {   6,  6 + 18,  {   4,  5,   1,  1}	},
    {  12,  9 +  3,  {   2,  3,   1,  1}	},
    {  21, 12 + 12,  {  -3,  4,   1,  1}	},

    {  33,  3,  {   1,  2,   1,  1}	},
    {  36,  3,  { -11, 12,   1,  1}	},
    {  39,  6,  {   4,  5,   1,  1}	},
    {  45,  6,  {   2,  3,   1,  1}	},
    {  51,  3,  {  -3,  4,   1,  1}	},
    {  54, 12,  {   4,  5,   1,  1}	},

    {  66,  3,  {   1,  2,   1,  1}	},
    {  69,  3,  { -11, 12,   1,  1}	},
    {  72,  6,  {   4,  5,   1,  1}	},
    {  78,  9,  {   2,  3,   1,  1}	},
    {  87, 12,  {  -3,  4,   1,  1}	},

    {  93,  3,  {   2,  3,   1,  1}	},
    {  96,  3,  {   3,  4,   1,  1}	},
    {  99,  9,  {   4,  5,   1,  1}	},
    { 108,  9,  {   9, 10,   1,  1}	},
    { 117, 11,  {  19, 20,   1,  1}	},


{ 128,  2,  {  -1,  4,   9, 10}	},
{ 130,  2,  {   9, 10,   9, 10}	},
{ 132,  2,  {  10, 10,   9, 10}	},
{ 134,  2,  {   9, 10,   9, 10}	},
{ 136,  2,  {  -1,  4,   9, 10}	},
{ 138,  2,  {   3,  4,   9, 10}	},

/*
{ 140,  2,  {   2,  3,   9, 10}	},
{ 142,  2,  {   3,  4,   9, 10}	},
{ 144,  2,  {  -1,  4,   9, 10}	},
{ 146,  2,  {   3,  4,   9, 10}	},
{ 148,  2,  {   2,  3,   9, 10}	},
{ 150,  2,  {  -3,  4,   9, 10} },
*/
{ 140,  2,  {   4,  5,   9, 12}	},
{ 142,  2,  {   3,  4,   9, 12}	},
{ 144,  2,  {   4,  4,   9, 12}	},
{ 146,  2,  {   3,  4,   9, 12}	},
{ 148,  2,  {   4,  5,   9, 12}	},
{ 150,  2,  {  -1,  2,   9, 12} },

{ 152,  2,  {  -1,  4,  -9, 12}	},
{ 154,  2,  {   9, 10,  -9, 12}	},
{ 156,  2,  {  10, 10,  -9, 12}	},
{ 158,  2,  {   9, 10,  -9, 12}	},
{ 160,  2,  {  -1,  4,  -9, 12}	},
{ 162,  2,  {   3,  4,  -9, 12}	},

{ 164, 12,  {   9, 10,   9, 20}	},


{ 128,  2,  {  -1,  4,  -3,  4}	},
{ 130,  2,  {   9, 10,  -3,  4}	},
{ 132,  2,  {   1,  1,  -3,  4}	},
{ 134,  2,  {   9, 10,  -3,  4}	},
{ 136,  2,  {  -1,  4,  -3,  4}	},
{ 138,  2,  {   3,  4,  -3,  4}	},
/*
{ 140,  2,  {   2,  3,  -3,  4}	},
{ 142,  2,  {   3,  4,  -3,  4}	},
{ 144,  2,  {  -1,  4,  -3,  4}	},
{ 146,  2,  {   3,  4,  -3,  4}	},
{ 148,  2,  {   2,  3,  -3,  4}	},
{ 150,  2,  {  -3,  4,  -3,  4} },
*/
{ 140,  2,  {   4,  5,  3,  4}	},
{ 142,  2,  {   3,  4,  3,  4}	},
{ 144,  2,  {   4,  4,  3,  4}	},
{ 146,  2,  {   3,  4,  3,  4}	},
{ 148,  2,  {   4,  5,  3,  4}	},
{ 150,  2,  {  -1,  2,  3,  4} },

{ 152,  2,  {  -5,  6,  -3,  4}	},
{ 154,  2,  {  -3,  4,  -3,  4}	},
{ 156,  2,  {   2,  3,  -3,  4}	},
{ 158,  2,  {  -3,  4,  -3,  4}	},
{ 160,  2,  {  -5,  6,  -3,  4}	},
{ 162,  2,  {   1,  2,  -3,  4}	},

{ 164, 12,  {   9, 10,  -3,  4}	},


    { 128,  0,  {   1,  1,   1,  1}	}
};
#elif (MODE == NATURAL)
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
					{   0,  3,  {  -3,  4,   2,  1}	},
    {   0,  3,  {   1,  2,   1,  1}},
    {   3,  3,  { -11, 12,   1,  1}},
					{   6,  3,  {   1,  1,   2,  1}	},
    {   6,  6,  {   4,  5,   1,  1}},
					{  12,  3,  {   4,  5,   2,  1}	},
    {  12,  9,  {   2,  3,   1,  1}},
					{  21,  3,  {   3,  4,   2,  1}	},
    {  21, 15,  {  -3,  4,   1,  1}},

					{  36,  3,  {   -3,  4,   2,  1}	},
    {  36,  3,  {   1,  2,   1,  1}	},
    {  39,  3,  { -11, 12,   1,  1}	},
					{  42,  3,  {   1,  1,   2,  1}	},
    {  42,  6,  {   4,  5,   1,  1}	},
					{  48,  3,  {   4,  5,   2,  1}	},
    {  48,  6,  {   2,  3,   1,  1}	},
    {  54,  3,  {  -3,  4,   1,  1}	},
					{  57,  3,  {   1,  1,   2,  1}	},
    {  57, 15,  {   4,  5,   1,  1}	},

					{  72,  3,  {  -3,  4,   2,  1}	},
    {  72,  3,  {   1,  2,   1,  1}	},
    {  75,  3,  { -11, 12,   1,  1}	},
					{  78,  3,  {   1,  1,   2,  1}	},
    {  78,  6,  {   4,  5,   1,  1}	},
					{  84,  3,  {   4,  5,   2,  1}	},
    {  84,  9,  {   2,  3,   1,  1}	},
					{  93,  3,  {   3,  4,   2,  1}	},
    {  93,  9,  {  -3,  4,   1,  1}	},

    { 102,  3,  {   2,  3,   1,  1}	},
    { 105,  3,  {   3,  4,   1,  1}	},

					{ 108,  3,  {   1,  1,   2,  1}	},
    { 108,  9,  {   4,  5,   1,  1}	},
					{ 117,  3,  { -11, 12,   2,  1}	},
    { 117, 12,  {   9, 10,   1,  1}	},

					{ 128,  3,  {  -3,  4,   2,  1}	},
    { 129, 21,  {  19, 20,   1,  1}	},
    //{ 168, 12,  {   0,  0,   0,  0}	}


{ 150,  2,  {  -1,  4,   9, 10}	},
{ 152,  2,  {   9, 10,   9, 10}	},
{ 154,  2,  {  10, 10,   9, 10}	},
{ 156,  2,  {   9, 10,   9, 10}	},
{ 158,  2,  {  -1,  4,   9, 10}	},
{ 160,  2,  {   3,  4,   9, 10}	},

{ 162,  2,  {   4,  5,   3,  4}	},
{ 164,  2,  {   9, 10,   3,  4}	},
{ 166,  2,  {  10, 10,   3,  4}	},
{ 168,  2,  {   9, 10,   3,  4}	},
{ 170,  2,  {   4,  5,   3,  4}	},
{ 172,  2,  { -11, 12,   1,  1} },

{ 174,  2,  {   1,  2,   1,  1}	},
{ 176,  2,  { -11, 12,   1,  1}	},
{ 178,  2,  {  10, 10,  -9, 12}	},
{ 180,  2,  {   9, 10,  -9, 12}	},
{ 182,  2,  {  -1,  4,  -9, 12}	},
{ 184,  2,  {   3,  4,  -9, 12}	},

{ 186, 16,  {   2,  3,  -9, 12}	},


{ 202,  2,  {  -1,  4,  -3,  4}	},
{ 204,  2,  {   9, 10,  -3,  4}	},
{ 206,  2,  {   1,  1,  -3,  4}	},
{ 208,  2,  {   9, 10,  -3,  4}	},
{ 210,  2,  {  -1,  4,  -3,  4}	},
{ 212,  2,  {   3,  4,  -3,  4}	},

{ 214,  2,  {   4,  5,   1,  2} },
{ 216,  2,  {   9, 10,   1,  2} },
{ 218,  2,  {   1,  1,   1,  2} },
{ 220,  2,  {   9, 10,   1,  2} },
{ 222,  2,  {   4,  5,   1,  2} },
{ 224,  2,  { -11, 12,   2,  3} },

{ 226,  2,  {  -5,  6,  -3,  4}	},
{ 228,  2,  {  -3,  4,  -3,  4}	},
{ 230,  2,  {   2,  3,  -3,  4}	},
{ 232,  2,  {  -3,  4,  -3,  4}	},
{ 234,  2,  {  -5,  6,  -3,  4}	},
{ 236,  2,  {   1,  2,  -3,  4}	},

/* interesting version
{ 222,  2,  { -11, 12,   1,  2} },

{ 224,  2,  {  -5,  6,  -3,  4}	},
{ 226,  2,  {  -3,  4,  -3,  4}	},
{ 228,  2,  {   2,  3,  -3,  4}	},
{ 230,  2,  {  -3,  4,  -3,  4}	},
{ 232,  2,  {  -5,  6,  -3,  4}	},
{ 234,  2,  {   1,  2,  -3,  4}	},
*/

{ 238, 16,  {   4,  5,  -3,  4}	},
{ 254, 16,  {  -3,  4,  -3,  4}	},


    { 270,  -1,  {   0,  0,   0,  0}	}
};
#else

#endif

Sequence fnl_1_sequence;

int main()
{
    //fnl_1_sequence.base_p = 2 * M_PI / (1573.2 * 32.0);
    fnl_1_sequence.base_p = 2 * M_PI / (440.0 * 32.0);
    fnl_1_sequence.base_t =  (2.0 * 60.0 / 984.0);
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
    p0 = 2 * M_PI / (440.0 * 32.0);
    double l;
    l = 1000.0 * M_PI*M_PI;
    double p;
    p = p0;

    stream.play_sequence(&fnl_1_sequence);
    stream.play();


    while (stream.getStatus() == MyStream::Playing)
    {
        std::cout << "p = " << p / M_PI << " l = " << l / (M_PI * M_PI) << "\n";
        c = getch();
        switch (c){
        case 'z':
            stream.play_sequence(&fnl_1_sequence);
            break;

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
		case '5':
			p = p0 * 0.52973154717964763228091264747317;
			break;
		case '6':
			p = p0 * 0.56123102415468649071676652483959;
			break;
		case '7':
			p = p0 * 0.59460355750136053335874998528024;
			break;
		case '8':
			p = p0 * 0.62996052494743658238360530363911;
			break;
		case '9':
			p = p0 * 0.70710678118654752440084436210485;
			break;
		case '0':
			p = p0 * 0.84089641525371454303112547623321;
			break;
        case '-':
            p = p0 * 9.0/10.0;
			//p = p0 * 89.0/100.0;
            break;
        case '=':
            p = p0 * 19.0/20.0;
			//p = p0 * 943.0/1000.0;
            break;
        }
        stream.pick(p, l);
    }
        //sf::sleep(sf::seconds(0.1f));

    return 0;
}
