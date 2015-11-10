#include <stdlib.h>
#include <stdio.h>
#include "frac.h"
#include "vibro.h"

typedef struct {
	const char * name;
	frac_t       pitch;
	frac_t		 length;
} note_t;

typedef struct {
	const char   * name;
	note_t * notes;
	int notes_num; 
} score_t;

#define _4I   {"4I",   {4*1,1},
#define _4III {"4III", {4*4,5},
#define	_4IVp {"4VIp",  {15*4*3,16*4},
#define _4V   {"4V",   {4*2,3},
#define _4VI  {"4VI",  {4*3,5},
#define _4VII {"4VII", {4*27,50},
#define _2I   {"2I",   {2*1,1},
#define _2II  {"2II",  {2*9,10},
#define _2III {"2III", {2*4,5},
#define _2IV  {"2IV",  {2*3,4},
#define _1  {1,1}},

//Cello Suite #1
//*=2Hz I=108Hz II=9/10 III=4/5 IV=3/4 V=2/3 VI=3/5 VII=27/50

note_t suite1_notes[] = {
	_4I  _1  _4V   _1  _2III _1  _2II  _1  _2III _1  _4V   _1  _2III _1  _4V   _1
	_4I  _1  _4V   _1  _2III _1  _2II  _1  _2III _1  _4V   _1  _2III _1  _4V   _1
	_4I  _1  _4VI  _1  _2IV  _1  _2III _1  _2IV  _1  _4VI  _1  _2IV  _1  _4VI  _1
	_4I  _1  _4VI  _1  _2IV  _1  _2III _1  _2IV  _1  _4VI  _1  _2IV  _1  _4VI  _1
	_4I  _1  _4VII _1  _2IV  _1  _2III _1  _2IV  _1  _4VII _1  _2IV  _1  _4VII _1
	_4I  _1  _4VII _1  _2IV  _1  _2III _1  _2IV  _1  _4VII _1  _2IV  _1  _4VII _1
	_4I  _1  _2I   _1  _2III _1  _2II  _1  _2III _1  _2I   _1  _2III _1  _2I   _1
	_4I  _1  _2I   _1  _2III _1  _2II  _1  _2III _1  _2I   _1  _2III _1  _4VII _1
	_4I  _1  _4VI  _1  _2III _1  _2II  _1  _2III _1  _2I   _1  _4VII _1  _2I   _1
	_4VI _1  _2I   _1  _4VII _1  _2I   _1  _4III _1  _4V   _1  _4IVp _1  _4III _1
};

score_t suite1_score = {
	"Cello Suite #1",
	suite1_notes,
	sizeof(suite1_notes)/sizeof(note_t)
};

class VibroPlayer : public VibroStream{
private:
	score_t * score;
	float base_note_pitch;
	float base_note_length;
	int   score_pos_i;
	float next_note_t;
	float cur_t;
protected:
    virtual bool onGetData(Chunk& data)
    {
		if (is_playing)
		{
			if (cur_t >= next_note_t)
			{
				if (score_pos_i < score->notes_num)
				{
					pick_note(&score->notes[score_pos_i]);
					frac_t *len;
					len = &score->notes[score_pos_i].length;
					next_note_t += base_note_length * ((float)len->n) / ((float)len->d);
					score_pos_i++;
				}
				else
				{
					is_playing = false;
				}
			}
			//printf("cur_t = %f\n next_note_t = %f pos_i = %d", cur_t, next_note_t, score_pos_i);
			cur_t += 1.0f / ((float)SAMPLE_RATE);
		}
        VibroStream::onGetData(data);
        return true;
    }
public:
	bool is_playing;
	void set_base_note_length(float length)
	{
		base_note_length = length;
	}
	void set_base_note_pitch(float pitch)
	{
		base_note_pitch = pitch;
	}

	void pick_note(note_t * note)
	{
		pick(base_note_pitch * ((float)note->pitch.n) / ((float)note->pitch.d));
		printf("%s (%d/%d)\n", note->name, F(note->pitch));
	}

	void play_score(score_t * score_to_play)
	{
		is_playing = true;
		score = score_to_play;
		score_pos_i = 0;
		next_note_t = 0.0f;
		cur_t = 0.0f;
		play();
	}
	void save()
	{
		
	}
};

int main(void)
{
	VibroPlayer vp;
	vp.set_base_note_pitch(5.0/8.0 * M_PI / (216.0 * 64.0));
	vp.set_base_note_length(0.00033f);
	//vp.pick_note(&suite1_notes[0]);
	vp.play_score(&suite1_score);
	
	while (vp.is_playing)
		_sleep(1000);

	return 0;
}