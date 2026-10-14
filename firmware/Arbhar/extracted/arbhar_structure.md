#define NO_OF_GRAINS 44

Grain_data:

float duration
float position
float windowShape
float speed
float spray
float amp
float panning
float velocity
int direction
int layer (classic selection)
float layerInterpolate (new feature)
bool addRandomTiming
bool addRandomAmp

std::array<Grain, NO_OF_GRAINS>

Grain:

private:

    int g_id
    bool g_active

    float g_duration
    float g_startPosition
    float g_curentPos
    float g_windowShape
    float g_windowPos
    float g_speed
    float g_amp
    float g_panning
    float g_ampL
    float g_ampR
    float g_velocity
    int g_delayStart
    float g_direction

    int g_kill
    float g_killVol

    float g_distanceToRecHead


    calc_panning(float pan){
        g_panning = pan;
        g_panL = 1 - g_panning;
        g_panR = g_panning;
        // add random distribution
    }

    calc_playDataFwd(const GrainData& data){
        g_startPosition=position;
        g_duration=duration;
        g_windowShape=windowShape;
    }

    calc_playDataRev(const GrainData& data){}

    calc_speed(){}

public:

    process_start(){}
    process_play(){}


******************
    GrainData
    GrainBuffers
    GrainRecorder
    GrainPlayer
    ---\
    ---\grain