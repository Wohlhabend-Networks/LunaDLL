#ifndef NO_SDL


#ifndef SDL_MUS_PLAYER
#define SDL_MUS_PLAYER

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#undef main
#include <map>

class PGE_SDL_Manager
{
public:
	static bool isInit;
	static void initSDL();

};


class PGE_MusPlayer
{
public:
	static std::string currentTrack;
    static void MUS_playMusic();
	static void MUS_playMusicFadeIn(int ms);
	static void MUS_pauseMusic();
	static void MUS_stopMusic();
	static void MUS_stopMusicFadeOut(int ms);

    static void MUS_changeVolume(int vlm);
    static void MUS_openFile(const char *musFile);
    static void setSampleRate(int sampleRate);
    static int sampleRate();
    static int currentVolume();

private:
    static Mix_Music *play_mus;
    static int volume;
    static int sRate;
	static bool showMsg;
	static std::string showMsg_for;
};


class PGE_Sounds
{
public:
    static void SND_PlaySnd(const char *sndFile);
	static void clearSoundBuffer();
private:
	static std::map<std::string, Mix_Chunk* > chunksBuffer;
    static Mix_Chunk *sound;
    static char *current;
};

#endif

#endif