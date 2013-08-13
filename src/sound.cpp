// sound.cpp: uses fmod on windows and sdl_mixer on unix (both had problems on the other platform)

#include "cube.h"

//#ifndef WIN32    // NOTE: fmod not being supported for the moment as it does not allow stereo pan/vol updating during playback
//#define USE_MIXER
//#endif

VARP(soundvol, 0, 255, 255);
VARP(musicvol, 0, 128, 255);
bool nosound = false;

#define MAXCHAN 32
#define SOUNDFREQ 22050

struct soundloc { vec loc; bool inuse; } soundlocs[MAXCHAN];

#ifdef USE_MIXER
    #include <SDL/SDL_mixer.h>
    #define MAXVOL MIX_MAX_VOLUME
    Mix_Music *mod = NULL;
    void *stream = NULL;
#else
    #define MAXVOL 1
    void * mod = NULL;
    void * stream = NULL;
#endif

void stopsound()
{
    if(nosound) return;
    if(mod)
    {
        #ifdef USE_MIXER
            Mix_HaltMusic();
            Mix_FreeMusic(mod);
        #else
        #endif
        mod = NULL;
    };
    if(stream)
    {
        #ifndef USE_MIXER
        #endif
        stream = NULL;
    };
};

VAR(soundbufferlen, 128, 1024, 4096);

void initsound()
{
    memset(soundlocs, 0, sizeof(soundloc)*MAXCHAN);
    #ifdef USE_MIXER
        if(Mix_OpenAudio(SOUNDFREQ, MIX_DEFAULT_FORMAT, 2, soundbufferlen)<0)
        {
            conoutf("sound init failed (SDL_mixer): %s", (size_t)Mix_GetError());
            nosound = true;
        };
	    Mix_AllocateChannels(MAXCHAN);
    #else
    #endif
};

void music(char *name)
{
    if(nosound) return;
    stopsound();
    if(soundvol && musicvol)
    {
        string sn;
        strcpy_s(sn, "packages/");
        strcat_s(sn, name);
        #ifdef USE_MIXER
            if(mod = Mix_LoadMUS(path(sn)))
            {
                Mix_PlayMusic(mod, -1);
                Mix_VolumeMusic((musicvol*MAXVOL)/255);
            };
        #else
        #endif
    };
};

COMMAND(music, ARG_1STR);

#ifdef USE_MIXER
vector<Mix_Chunk *> samples;
#else
vector<void *> samples;
#endif

cvector snames;

int registersound(char *name)
{
    loopv(snames) if(strcmp(snames[i], name)==0) return i;
    snames.add(newstring(name));
    samples.add(NULL);
    return samples.length()-1;
};

COMMAND(registersound, ARG_1EST);

void cleansound()
{
    if(nosound) return;
    stopsound();
    #ifdef USE_MIXER
        Mix_CloseAudio();
    #else
    #endif
};

VAR(stereo, 0, 1, 1);

void updatechanvol(int chan, vec *loc)
{
    int vol = soundvol, pan = 255/2;
    if(loc)
    {
        vdist(dist, v, *loc, player1->o);
        vol -= (int)(dist*3*soundvol/255); // simple mono distance attenuation
        if(stereo && (v.x != 0 || v.y != 0))
        {
            float yaw = -atan2(v.x, v.y) - player1->yaw*(PI / 180.0f); // relative angle of sound along X-Y axis
            pan = int(255.9f*(0.5*sin(yaw)+0.5f)); // range is from 0 (left) to 255 (right)
        };
    };
    vol = (vol*MAXVOL)/255;
    #ifdef USE_MIXER
        Mix_Volume(chan, vol);
        Mix_SetPanning(chan, 255-pan, pan);
    #else
    #endif
};

void newsoundloc(int chan, vec *loc)
{
    assert(chan>=0 && chan<MAXCHAN);
    soundlocs[chan].loc = *loc;
    soundlocs[chan].inuse = true;
};

void updatevol()
{
    if(nosound) return;
    loopi(MAXCHAN) if(soundlocs[i].inuse)
    {
        #ifdef USE_MIXER
            if (Mix_Playing(i))
        #else
            if (true)
        #endif
                updatechanvol(i, &soundlocs[i].loc);
            else soundlocs[i].inuse = false;
    };
};

void playsoundc(int n) { addmsg(0, 2, SV_SOUND, n); playsound(n); };

int soundsatonce = 0, lastsoundmillis = 0;

void playsound(int n, vec *loc)
{
    if(nosound) return;
    if(!soundvol) return;
    if(lastmillis==lastsoundmillis) soundsatonce++; else soundsatonce = 1;
    lastsoundmillis = lastmillis;
    if(soundsatonce>5) return;  // avoid bursts of sounds with heavy packetloss and in sp
    if(n<0 || n>=samples.length()) { conoutf("unregistered sound: %d", n); return; };

    if(!samples[n])
    {
        sprintf_sd(buf)("packages/sounds/%s.wav", snames[n]);

        #ifdef USE_MIXER
            samples[n] = Mix_LoadWAV(path(resourcepath(buf)));
        #else
        #endif

        if(!samples[n]) { conoutf("failed to load sample: %s", buf); return; };
    };

    #ifdef USE_MIXER
        int chan = Mix_PlayChannel(-1, samples[n], 0);
    #else
        int chan = 0;
    #endif
    if(chan<0) return;
    if(loc) newsoundloc(chan, loc);
    updatechanvol(chan, loc);
    #ifndef USE_MIXER
    #endif
};

void sound(int n) { playsound(n, NULL); };
COMMAND(sound, ARG_1INT);
