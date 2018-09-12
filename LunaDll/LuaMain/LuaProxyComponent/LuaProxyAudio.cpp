#include "LuaProxyAudio.h"

#include "../../SdlMusic/MusicManager.h"
#include "../../SdlMusic/SdlMusPlayer.h"
#include "../LuaProxy.h"
#include "../../Misc/MiscFuncs.h"
#include "../../GlobalFuncs.h"
#include "../../MciEmulator/mciEmulator.h"

#include "../../SMBXInternal/Level.h"

#include <memory>
#include <mutex>

void LuaProxy::Audio::MusicOpen(const std::string& filename)
{
#ifndef NO_SDL
    std::string full_paths = getSfxPath(filename);
    PGE_MusPlayer::MUS_openFile(full_paths.c_str());
#endif
}


void LuaProxy::Audio::MusicPlay()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_playMusic();
#endif
}


void LuaProxy::Audio::MusicPlayFadeIn(int ms)
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_playMusicFadeIn(ms);
#endif
}


void LuaProxy::Audio::MusicStop()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_stopMusic();
#endif
}


void LuaProxy::Audio::MusicStopFadeOut(int ms)
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_stopMusicFadeOut(ms);
#endif
}


void LuaProxy::Audio::MusicVolume(int vlm)
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_changeVolume(vlm);
#endif
}


void LuaProxy::Audio::MusicPause()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_pauseMusic();
#endif
}


void LuaProxy::Audio::MusicResume()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_playMusic();
#endif
}


bool LuaProxy::Audio::MusicIsPlaying()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_IsPlaying();
#else
    return false;
#endif
}


bool LuaProxy::Audio::MusicIsPaused()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_IsPaused();
#endif
}


bool LuaProxy::Audio::MusicIsFading()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_IsFading();
#endif
}

std::string LuaProxy::Audio::MusicTitle()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_MusicTitle();
#endif
}

std::string LuaProxy::Audio::MusicTitleTag()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_MusicTitleTag();
#endif
}

std::string LuaProxy::Audio::MusicArtistTag()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_MusicArtistTag();
#endif
}

std::string LuaProxy::Audio::MusicAlbumTag()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_MusicAlbumTag();
#endif
}

std::string LuaProxy::Audio::MusicCopyrightTag()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_MusicCopyrightTag();
#endif
}

void LuaProxy::Audio::MusicSetPosition(double seconds)
{
    Mix_SetMusicPosition(seconds);
}

double LuaProxy::Audio::MusicGetPosition()
{
    return Mix_GetMusicPosition(PGE_MusPlayer::currentMusic());
}

double LuaProxy::Audio::MusicGetLoopStart()
{
    return Mix_GetMusicLoopStartTime(PGE_MusPlayer::currentMusic());
}

double LuaProxy::Audio::MusicGetLoopEnd()
{
    return Mix_GetMusicLoopEndTime(PGE_MusPlayer::currentMusic());
}

double LuaProxy::Audio::MusicGetLoopLength()
{
    return Mix_GetMusicLoopLengthTime(PGE_MusPlayer::currentMusic());
}

void LuaProxy::Audio::seizeStream(int section)
{
    MusicManager::setSeized(section, true);
}

void LuaProxy::Audio::releaseStream(int section)
{
    MusicManager::setSeized(section, false);
}


void LuaProxy::Audio::changeMusic(int section, const std::string &filename)
{
    changeMusic(section, filename, -1);
}

void LuaProxy::Audio::changeMusic(int section, int musicId)
{
    changeMusic(section, musicId, -1);
}

void LuaProxy::Audio::changeMusic(int section, const std::string &filename, int fadeInDelayMs)
{
    if(filename.empty())
    {
        // If file path is empty, set to silence
        changeMusic(section, 0);
        return;
    }

    if((section < 0) || (section > 20))
        return;

    // music paths, set music id to custom and set path
    GM_SEC_MUSIC_TBL[section] = 24;
    GM_MUSIC_PATHS_PTR[section] = filename;

    // If the specified section's music is what's
    // currently playing, force the change in config to be picked up
    if(section == GM_SEC_CURRENT_MUSIC)
    {
        playMusic(section);
        if(fadeInDelayMs > 0)
        {
            MusicStop();
            MusicPlayFadeIn(fadeInDelayMs);
        }
    }
}

void LuaProxy::Audio::changeMusic(int section, int musicId, int fadeInDelayMs)
{
    if((section < 0) || (section > 20))
        return;

    GM_SEC_MUSIC_TBL[section] = short(musicId >= 0 ? musicId : 0);

    // If the specified section's music is what's
    // currently playing, force the change in config to be picked up
    if(section == GM_SEC_CURRENT_MUSIC)
    {
        if(fadeInDelayMs <= 0)
            playMusic(section);
        else
        {
            if(GM_SEC_MUSIC_TBL[section] != 0)
            {
                playMusic(section);
                MusicStop();
                MusicPlayFadeIn(fadeInDelayMs);
            }
            else
            {
                /* TODO: Replace this shit with own "playMusic(section)"
                 * implementation to avoid MCI calls and let SMBX organize
                 * it's internals correct
                 */
                seizeStream(section);
                playMusic(section);
                releaseStream(section);

                MusicStopFadeOut(fadeInDelayMs);
            }
        }
    }
}

void LuaProxy::Audio::musicFadeOut(int section, int fadeInDelayMs)
{
    changeMusic(section, 0, fadeInDelayMs);
}


Mix_Chunk* LuaProxy::Audio::newMix_Chunk()
{
    return NULL;
}



void LuaProxy::Audio::clearSFXBuffer()
{
    PGE_Sounds::clearSoundBuffer();
}


void LuaProxy::Audio::playSFX(const std::string& filename)
{
#ifndef NO_SDL
    std::string full_paths = getSfxPath(filename);
    PGE_Sounds::SND_PlaySnd(full_paths.c_str());
#else
    ::LuaProxy::playSFX(filename);
#endif
}


Mix_Chunk *LuaProxy::Audio::SfxOpen(const std::string& filename)
{
    std::string full_paths = getSfxPath(filename);
    return PGE_Sounds::SND_OpenSnd(full_paths.c_str());
}


int LuaProxy::Audio::SfxPlayCh(int channel, Mix_Chunk *chunk, int loops)
{
    return Mix_PlayChannel(channel, chunk, loops);
}


int LuaProxy::Audio::SfxPlayChTimed(int channel, Mix_Chunk *chunk, int loops, int ticks)
{
    return Mix_PlayChannelTimed(channel, chunk, loops, ticks);
}


int LuaProxy::Audio::SfxFadeInCh(int channel, Mix_Chunk *chunk, int loops, int ms)
{
    return Mix_FadeInChannel(channel, chunk, loops, ms);
}


int LuaProxy::Audio::SfxFadeInChTimed(int channel, Mix_Chunk *chunk, int loops, int ms, int ticks)
{
    return Mix_FadeInChannelTimed(channel, chunk, loops, ms, ticks);
}


void LuaProxy::Audio::SfxPause(int channel)
{
    Mix_Pause(channel);
}


void LuaProxy::Audio::SfxResume(int channel)
{
    Mix_Resume(channel);
}


int LuaProxy::Audio::SfxStop(int channel)
{
    return Mix_HaltChannel(channel);
}


int LuaProxy::Audio::SfxExpire(int channel, int ticks)
{
    return Mix_ExpireChannel(channel, ticks);
}


int LuaProxy::Audio::SfxFadeOut(int channel, int ms)
{
    return Mix_FadeOutChannel(channel, ms);
}


int LuaProxy::Audio::SfxIsPlaying(int channel)
{
    return Mix_Playing(channel);
}

int LuaProxy::Audio::SfxIsPaused(int channel)
{
    return Mix_Paused(channel);
}



std::string LuaProxy::Audio::getSfxPath(const std::string& filename)
{
    return resolveIfNotAbsolutePath(filename);
}


void LuaProxy::Audio::setMciSection(int section)
{
    MusicManager::setCurrentSection(section);
}


void LuaProxy::Audio::resetMciSections()
{
    MusicManager::resetSeizes();
}


int LuaProxy::Audio::SfxVolume(int channel, int vlm)
{
    return Mix_Volume(channel, vlm);
}


int LuaProxy::Audio::SfxIsFading(int channel)
{
    if(channel<0) channel=1;//Anti-crash protection
    return Mix_FadingChannel(channel);
}


int LuaProxy::Audio::SfxSetPanning(int channel, int left, int right)
{
    return Mix_SetPanning(channel, (Uint8)left, (Uint8)right);
}

int LuaProxy::Audio::SfxSetDistance(int channel, int distance)
{
    return Mix_SetDistance(channel, (Uint8)distance);
}

int LuaProxy::Audio::SfxSet3DPosition(int channel, int angle, int distance)
{
    return Mix_SetPosition(channel, (Sint16)angle, (Uint8)distance);
}

int LuaProxy::Audio::SfxReverseStereo(int channel, int flip)
{
    return Mix_SetReverseStereo(channel, flip);
}

double LuaProxy::Audio::AudioClock()
{
#ifndef NO_SDL
    return ((double)PGE_MusPlayer::sampleCount()) / PGE_MusPlayer::sampleRate();
#else
    return 0;
#endif
}

double LuaProxy::Audio::MusicClock()
{
#ifndef NO_SDL
    return ((double)PGE_MusPlayer::MUS_sampleCount()) / PGE_MusPlayer::sampleRate();
#else
    return 0;
#endif
}

void LuaProxy::Audio::__setOverrideForAlias(const std::string& alias, Mix_Chunk* chunk)
{
#ifndef NO_SDL
    PGE_Sounds::setOverrideForAlias(alias, chunk);
#endif
}

Mix_Chunk* LuaProxy::Audio::__getChunkForAlias(const std::string& alias)
{
#ifndef NO_SDL
    return PGE_Sounds::getChunkForAlias(alias);
#else
    return nullptr;
#endif
}

void LuaProxy::Audio::__setMuteForAlias(const std::string& alias, bool muted)
{
#ifndef NO_SDL
    PGE_Sounds::setMuteForAlias(alias, muted);
#endif
}

bool LuaProxy::Audio::__getMuteForAlias(const std::string& alias)
{
#ifndef NO_SDL
    return PGE_Sounds::getMuteForAlias(alias);
#else
    return false;
#endif
}

// Mutex for making sure this is safely handled
static std::mutex g_SfxPlayObjMutex;

// Map of channels with objects associated currently
static std::unordered_map<int, std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance>> g_SfxPlayObjMap;

static bool g_SfxPlayObjCallbackSet = false;
static void SfxPlayObjCallback(int channel)
{
    std::unique_lock<std::mutex> lck(g_SfxPlayObjMutex);
    auto it = g_SfxPlayObjMap.find(channel);
    if (it != g_SfxPlayObjMap.end())
    {
        it->second->OnChannelFinished();
        g_SfxPlayObjMap.erase(channel);
    }
}

std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> LuaProxy::Audio::SfxPlayObj(Mix_Chunk &chunk, int loops)
{
    std::unique_lock<std::mutex> lck(g_SfxPlayObjMutex);

    if (!g_SfxPlayObjCallbackSet)
    {
        Mix_ChannelFinished(SfxPlayObjCallback);
        g_SfxPlayObjCallbackSet = true;
    }

    lck.unlock();
    int channel = Mix_PlayChannel(-1, &chunk, loops);
    lck.lock();
    std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> ret = std::make_shared<LuaProxy::Audio::PlayingSfxInstance>(channel);
    if (channel != -1) g_SfxPlayObjMap[channel] = ret;
    return ret;
}

std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> LuaProxy::Audio::SfxPlayObjTimed(Mix_Chunk &chunk, int loops, int ticks)
{
    std::unique_lock<std::mutex> lck(g_SfxPlayObjMutex);

    if (!g_SfxPlayObjCallbackSet)
    {
        Mix_ChannelFinished(SfxPlayObjCallback);
        g_SfxPlayObjCallbackSet = true;
    }

    lck.unlock();
    int channel = Mix_PlayChannelTimed(-1, &chunk, loops, ticks);
    lck.lock();
    std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> ret = std::make_shared<LuaProxy::Audio::PlayingSfxInstance>(channel);
    if (channel != -1) g_SfxPlayObjMap[channel] = ret;
    return ret;
}

std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> LuaProxy::Audio::SfxFadeInObj(Mix_Chunk &chunk, int loops, int ms)
{
    std::unique_lock<std::mutex> lck(g_SfxPlayObjMutex);

    if (!g_SfxPlayObjCallbackSet)
    {
        Mix_ChannelFinished(SfxPlayObjCallback);
        g_SfxPlayObjCallbackSet = true;
    }

    lck.unlock();
    int channel = Mix_FadeInChannel(-1, &chunk, loops, ms);
    lck.lock();
    std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> ret = std::make_shared<LuaProxy::Audio::PlayingSfxInstance>(channel);
    if (channel != -1) g_SfxPlayObjMap[channel] = ret;
    return ret;
}

std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> LuaProxy::Audio::SfxFadeInObjTimed(Mix_Chunk &chunk, int loops, int ms, int ticks)
{
    std::unique_lock<std::mutex> lck(g_SfxPlayObjMutex);

    if (!g_SfxPlayObjCallbackSet)
    {
        Mix_ChannelFinished(SfxPlayObjCallback);
        g_SfxPlayObjCallbackSet = true;
    }

    lck.unlock();
    int channel = Mix_FadeInChannelTimed(-1, &chunk, loops, ms, ticks);
    lck.lock();
    std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> ret = std::make_shared<LuaProxy::Audio::PlayingSfxInstance>(channel);
    if (channel != -1) g_SfxPlayObjMap[channel] = ret;
    return ret;
}

//    int mChannel;
//    bool mFinished;
LuaProxy::Audio::PlayingSfxInstance::PlayingSfxInstance(int channel) :
    mChannel(channel), mFinished(channel == -1)
{
}

LuaProxy::Audio::PlayingSfxInstance::~PlayingSfxInstance()
{
}

// Callback when the channel is finished
void LuaProxy::Audio::PlayingSfxInstance::OnChannelFinished()
{
    mFinished = true;
}

void LuaProxy::Audio::PlayingSfxInstance::Pause()
{
    if (mFinished) return;
    Mix_Pause(mChannel);
}

void LuaProxy::Audio::PlayingSfxInstance::Resume()
{
    if (mFinished) return;
    Mix_Resume(mChannel);
}
void LuaProxy::Audio::PlayingSfxInstance::Stop()
{
    if (mFinished) return;
    Mix_HaltChannel(mChannel);
}

void LuaProxy::Audio::PlayingSfxInstance::Expire(int ms)
{
    if (mFinished) return;
    Mix_ExpireChannel(mChannel, ms);
}

void LuaProxy::Audio::PlayingSfxInstance::FadeOut(int ms)
{
    if (mFinished) return;
    Mix_FadeOutChannel(mChannel, ms);
}

bool LuaProxy::Audio::PlayingSfxInstance::IsPlaying()
{
    if (mFinished) return false;
    return Mix_Playing(mChannel) != 0;
}

bool LuaProxy::Audio::PlayingSfxInstance::IsPaused()
{
    if (mFinished) return false;
    return Mix_Paused(mChannel) != 0;
}

bool LuaProxy::Audio::PlayingSfxInstance::IsFading()
{
    if (mFinished) return false;
    return Mix_Fading(mChannel) != 0;
}

void LuaProxy::Audio::PlayingSfxInstance::Volume(int vlm)
{
    if (mFinished) return;
    Mix_Volume(mChannel, vlm);
}

void LuaProxy::Audio::PlayingSfxInstance::SetPanning(uint8_t left, uint8_t right)
{
    if (mFinished) return;
    Mix_SetPanning(mChannel, left, right);
}

void LuaProxy::Audio::PlayingSfxInstance::SetDistance(uint8_t distance)
{
    if (mFinished) return;
    Mix_SetDistance(mChannel, distance);
}

void LuaProxy::Audio::PlayingSfxInstance::Set3DPosition(int16_t angle, uint8_t distance)
{
    if (mFinished) return;
    Mix_SetPosition(mChannel, (Sint16)angle, (Uint8)distance);
}

void LuaProxy::Audio::PlayingSfxInstance::SetReverseStereo(bool flip)
{
    if (mFinished) return;
    Mix_SetReverseStereo(mChannel, flip ? 1 : 0);
}
