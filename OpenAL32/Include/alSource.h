#ifndef _AL_SOURCE_H_
#define _AL_SOURCE_H_

#define MAX_SENDS                 4

#include "alMain.h"
#include "alu.h"
#include "hrtf.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ALbuffer;
struct ALsource;
struct ALsourceProps;


typedef struct ALbufferlistitem {
    struct ALbuffer *buffer;
    struct ALbufferlistitem *volatile next;
} ALbufferlistitem;


typedef struct ALvoice {
    struct ALsource *volatile Source;

    /** Current target parameters used for mixing. */
    ALint Step;

    ALboolean Looping;

    /* If not 'moving', gain/coefficients are set directly without fading. */
    ALboolean Moving;

    ALboolean IsHrtf;

    ALuint Offset; /* Number of output samples mixed since starting. */

    alignas(16) ALfloat PrevSamples[MAX_INPUT_CHANNELS][MAX_PRE_SAMPLES];

    BsincState SincState;

    DirectParams Direct;
    SendParams Send[MAX_SENDS];
} ALvoice;


struct ALsourceProps {
    ATOMIC(ALfloat)   Pitch;
    ATOMIC(ALfloat)   Gain;
    ATOMIC(ALfloat)   OuterGain;
    ATOMIC(ALfloat)   MinGain;
    ATOMIC(ALfloat)   MaxGain;
    ATOMIC(ALfloat)   InnerAngle;
    ATOMIC(ALfloat)   OuterAngle;
    ATOMIC(ALfloat)   RefDistance;
    ATOMIC(ALfloat)   MaxDistance;
    ATOMIC(ALfloat)   RollOffFactor;
    ATOMIC(ALfloat)   Position[3];
    ATOMIC(ALfloat)   Velocity[3];
    ATOMIC(ALfloat)   Direction[3];
    ATOMIC(ALfloat)   Orientation[2][3];
    ATOMIC(ALboolean) HeadRelative;
    ATOMIC(ALboolean) Looping;
    ATOMIC(enum DistanceModel) DistanceModel;
    ATOMIC(ALboolean) DirectChannels;

    ATOMIC(ALboolean) DryGainHFAuto;
    ATOMIC(ALboolean) WetGainAuto;
    ATOMIC(ALboolean) WetGainHFAuto;
    ATOMIC(ALfloat)   OuterGainHF;

    ATOMIC(ALfloat) AirAbsorptionFactor;
    ATOMIC(ALfloat) RoomRolloffFactor;
    ATOMIC(ALfloat) DopplerFactor;

    ATOMIC(ALfloat) StereoPan[2];

    ATOMIC(ALfloat) Radius;

    /** Direct filter and auxiliary send info. */
    struct {
        ATOMIC(ALfloat) Gain;
        ATOMIC(ALfloat) GainHF;
        ATOMIC(ALfloat) HFReference;
        ATOMIC(ALfloat) GainLF;
        ATOMIC(ALfloat) LFReference;
    } Direct;
    struct {
        ATOMIC(struct ALeffectslot*) Slot;
        ATOMIC(ALfloat) Gain;
        ATOMIC(ALfloat) GainHF;
        ATOMIC(ALfloat) HFReference;
        ATOMIC(ALfloat) GainLF;
        ATOMIC(ALfloat) LFReference;
    } Send[MAX_SENDS];

    ATOMIC(struct ALsourceProps*) next;
};

typedef struct ALsource {
    /** Source properties. */
    volatile ALfloat   Pitch;
    volatile ALfloat   Gain;
    volatile ALfloat   OuterGain;
    volatile ALfloat   MinGain;
    volatile ALfloat   MaxGain;
    volatile ALfloat   InnerAngle;
    volatile ALfloat   OuterAngle;
    volatile ALfloat   RefDistance;
    volatile ALfloat   MaxDistance;
    volatile ALfloat   RollOffFactor;
    volatile ALfloat   Position[3];
    volatile ALfloat   Velocity[3];
    volatile ALfloat   Direction[3];
    volatile ALfloat   Orientation[2][3];
    volatile ALboolean HeadRelative;
    volatile ALboolean Looping;
    volatile enum DistanceModel DistanceModel;
    volatile ALboolean DirectChannels;

    volatile ALboolean DryGainHFAuto;
    volatile ALboolean WetGainAuto;
    volatile ALboolean WetGainHFAuto;
    volatile ALfloat   OuterGainHF;

    volatile ALfloat AirAbsorptionFactor;
    volatile ALfloat RoomRolloffFactor;
    volatile ALfloat DopplerFactor;

    /* NOTE: Stereo pan angles are specified in radians, counter-clockwise
     * rather than clockwise.
     */
    volatile ALfloat StereoPan[2];

    volatile ALfloat Radius;

    /** Direct filter and auxiliary send info. */
    struct {
        ALfloat Gain;
        ALfloat GainHF;
        ALfloat HFReference;
        ALfloat GainLF;
        ALfloat LFReference;
    } Direct;
    struct {
        struct ALeffectslot *Slot;
        ALfloat Gain;
        ALfloat GainHF;
        ALfloat HFReference;
        ALfloat GainLF;
        ALfloat LFReference;
    } Send[MAX_SENDS];

    /**
     * Last user-specified offset, and the offset type (bytes, samples, or
     * seconds).
     */
    ALdouble Offset;
    ALenum   OffsetType;

    /** Source type (static, streaming, or undetermined) */
    volatile ALint SourceType;

    /** Source state (initial, playing, paused, or stopped) */
    volatile ALenum state;
    ALenum new_state;

    /**
     * Source offset in samples, relative to the currently playing buffer, NOT
     * the whole queue, and the fractional (fixed-point) offset to the next
     * sample.
     */
    ATOMIC(ALuint) position;
    ATOMIC(ALuint) position_fraction;

    /** Source Buffer Queue info. */
    ATOMIC(ALbufferlistitem*) queue;
    ATOMIC(ALbufferlistitem*) current_buffer;
    RWLock queue_lock;

    /** Current buffer sample info. */
    ALuint NumChannels;
    ALuint SampleSize;

    ATOMIC(struct ALsourceProps*) Update;
    ATOMIC(struct ALsourceProps*) FreeList;

    /** Self ID */
    ALuint id;
} ALsource;

inline void LockSourcesRead(ALCcontext *context)
{ LockUIntMapRead(&context->SourceMap); }
inline void UnlockSourcesRead(ALCcontext *context)
{ UnlockUIntMapRead(&context->SourceMap); }
inline void LockSourcesWrite(ALCcontext *context)
{ LockUIntMapWrite(&context->SourceMap); }
inline void UnlockSourcesWrite(ALCcontext *context)
{ UnlockUIntMapWrite(&context->SourceMap); }

inline struct ALsource *LookupSource(ALCcontext *context, ALuint id)
{ return (struct ALsource*)LookupUIntMapKeyNoLock(&context->SourceMap, id); }
inline struct ALsource *RemoveSource(ALCcontext *context, ALuint id)
{ return (struct ALsource*)RemoveUIntMapKeyNoLock(&context->SourceMap, id); }

void UpdateSourceProps(ALsource *source, ALuint num_sends, ALCcontext *context);
void UpdateAllSourceProps(ALCcontext *context);
ALvoid SetSourceState(ALsource *Source, ALCcontext *Context, ALenum state);
ALboolean ApplyOffset(ALsource *Source);

ALvoid ReleaseALSources(ALCcontext *Context);

#ifdef __cplusplus
}
#endif

#endif
