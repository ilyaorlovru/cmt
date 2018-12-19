//////////////////////////////////////
// CMT Attack Release Measuring tool
// written by Ilya Orlov (LetiMix) 
// https://www.youtube.com/channel/UCbS0EpAZytfizVvysUluEHQ
// St. Petersburg, Russia, 2018
//////////////////////////////////////
// This plugin is free.
//////////////////////////////////////
// If you need support, bugfixes, new features,
// or if you're just feeling grateful,
// please consider a donation:
// PayPal: ilya_orlov@mail.ru
// Thank you!
//////////////////////////////////////

string name="Attack Release Measuring Tool"; 
string description="Version 1.07 (2018-12-14)"; 
string author="Ilya Orlov (LetiMix)";

///////////////////////////
// HI and LOW working margins (in dB)
const double CEIL_LEVEL = 0;
const double FLOOR_LEVEL = -40;

// value which is considered as "silence"
double SILENCE_LEVEL = -112; // in dB, signals below that are considered silence
double SILENCE_VALUE, FLOOR_VALUE; // the same value =~ 0.00000238; calc in initialize

// preset parameters
const double MIN_ATTACK_JUMP_DB = 1.5; // level jump in 1dB means attack start
const double MIN_RELEASE_DROP_DB = -5; // level drop in 5dB meanse release start

const double MAX_LEVEL_DIFF_FOR_ATTACK_END = 0.15; // level diff within averaging "window" to consider attack finished
const double MAX_LEVEL_DIFF_FOR_RELEASE_END = 0.15;

const int ATTACK_MAX_AVERAGING_WINDOW_MS = 150; // in milliseconds
const int ATTACK_MIN_AVERAGING_WINDOW_MS = 50;
const int RELEASE_AVERAGING_WINDOW_MS = 250; 

const double ATTACK_DEFAULT_COEFF_PERC = 63; // default attack measure coeff (in percents)
const double RELEASE_DEFAULT_COEFF_PERC = 63;

// states
const int ST_UNDEFINED = 0;
const int ST_ATT_STARTED = 1;
const int ST_ATT_MEASURED = 2;
const int ST_ATT_ENDED = 3;
const int ST_REL_STARTED = 4;
const int ST_REL_MEASURED = 5;
const int ST_REL_ENDED = 6;

// presets
array<string> hr_att_len_presets = { "off", "5 ms", "10 ms", "15 ms", "20 ms", "30 ms", "40 ms", "50 ms", 
    "75 ms", "100 ms", "125 ns", "150 ms", "200 ms", "250 ms"};
array<string> hr_rel_len_presets = { "off", "10 ms", "20 ms", "50 ms", "75 ms", "100 ms", 
    "150 ms", "200 ms", "300 ms", "400 ms", "500 ms"};

const int IP_MUTE_OUTPUT = 0;
const int IP_SHOW_MORE_INFO  = 4;
const int IP_PAUSE = 5;

const int IP_INPUT_CHANNEL = 8;
const int IP_LOCK_MAXLEVEL = 6;
const int IP_GR_MODE = 9;

const int IP_ATT_COEFF = 2;
const int IP_REL_COEFF = 3;

const int IP_SAVE_SNAPSHOT = 1;
const int IP_SAVE_SNAPSHOT_MODE = 7;
const int IP_HR_ATT_LEN = 10;
const int IP_HR_REL_LEN = 11;

// const int IP_SAVE_SNAP_REF = 12;
const int IP_SAVE_DEBUG = 12;

// modes for snapshot saving
const int SNAPSHOT_MODE_NORMAL = 0;
const int SNAPSHOT_MODE_NORMAL_WITH_RAWDATA = 1;
const int SNAPSHOT_MODE_LIVE = 2;
const int SNAPSHOT_MODE_REFERENCE = 3;
const int SNAPSHOT_MODE_REMOVE_REF = 4;

// snapshot saving buttons
const int SAVE_OFF = 0;
const int SAVE_SNAP = 1;
const int SAVE_SNAP_REF = 2;
const int SAVE_SNAP_REF_REMOVE = 3;

// Input Parameters
array<string> inputParametersNames={"Mute Output", "Save snapshot", 
"Attack Coeff", "Release Coeff", "Show more", 
"Pause", "Lock Max Level", "Save snapshot mode",
"Input channel", "GR Mode", "HR Att Length", "HR Rel Length",
"Save debug" }; 
array<string> inputParametersUnits={"", "", 
"% GR", "% GR", "", 
"", "", "", 
"", "", "", "", 
"" }; 
array<double> inputParameters(inputParametersNames.length); 
array<double> inputParametersMin={0, SAVE_OFF, 
0, 0, 0, 
0, 0, SNAPSHOT_MODE_NORMAL, 
0, 0, 0, 0,
0}; 
array<double> inputParametersMax={1, SAVE_SNAP_REF_REMOVE, 
100, 100, 1, 
1, 1, SNAPSHOT_MODE_LIVE,
2, 2, hr_att_len_presets.length-1, hr_rel_len_presets.length-1, 
1}; 
array<double> inputParametersDefault={1, SAVE_OFF, 
ATTACK_DEFAULT_COEFF_PERC, RELEASE_DEFAULT_COEFF_PERC, 0, 
0, 0, SNAPSHOT_MODE_NORMAL,
0, 0, 0, 0, 
0}; 
array<string> inputParametersEnums={"", "---;save snapshot;save reference;remove reference",
"","","",
"","auto;locked","normal;with csv;live mode",
"Left;Right;Stereo", "max:;attack:;release:", join(hr_att_len_presets, ";"), join(hr_rel_len_presets, ";"), 
";saving...",
};
array<string> inputParametersFormats={"", "", 
".1", ".1", "", 
"", "", "",
"", "", "", "",
""};
array<int> inputParametersSteps={0, 0, 
0, 0, 0, 
0, 0, 0, 
0, 0, hr_att_len_presets.length, hr_rel_len_presets.length,
0}; 

// modes of GR
const int GR_MODE_MAX = 0;
const int GR_MODE_ATTACK = 1;
const int GR_MODE_RELEASE = 2;

//array<string> inputStrings(2);
//array<string> inputStringsNames={"S1","S2"};

// Output Parameters
const int OP_SIGNAL_LEVEL = 0;
const int OP_SIGNAL_LEVEL_REAL = 1;
const int OP_ATT_LEV = 2;
const int OP_REL_LEV = 3;
const int OP_MAX_LEV = 4;
const int OP_MIN_LEV = 5;
const int OP_ATT_TIME = 6;
const int OP_REL_TIME = 7;
const int OP_FREQ = 8;
const int OP_GR = 9;
const int OP_VERT_CURVE = 10;
const int OP_SNAPSHOT_STATE = 11;
const int OP_ATT_END_LVL = 12;
const int OP_ATT_END_TIME = 13;
const int OP_REL_END_LVL = 14;
const int OP_REL_END_TIME = 15;
const int OP_ERR_MSG = 16;
const int OP_PRECISION = 17;

//const uint OP_UPDATED = 14;

array<string> outputParametersNames={"Level dB", "Level dB Real", "Attack Level", "Release Level",
    "Max Level", "Min Level", "Attack", "Release", "Frequency",
    "GR max", "Vertical curve", "Save snapshot State", 
    "Att End Level", "Att End Time", "Rel End Level", "Rel End Time",
    "Error Messages", "Precision" };
array<double> outputParameters(outputParametersNames.length);
array<string> outputParametersUnits={"dB", "dB", "dB", "dB",
    "dB", "dB", "ms", "ms", "Hz",
    "dB", "dB", "", 
    "dB", "ms", "dB", "ms",
    "", "ms" };
array<string> outputParametersFormats={".1", ".2", ".2", ".2",
    ".1", ".1", ".2", ".2", ".1",
    ".1", ".1", "", 
    ".1", ".2", ".1", ".2",
    "", ".3" };
array<string> outputParametersEnums={"", "", "", "", 
"", "", "", "", "", 
"", "", "Save snapshot:;Saving...;Streaming...;Removing...",
"", "", "", "",
";Data output paused;Un-pause to enable saving;Locked max level is too low;High level time is probably too short;Low level time is probably too short;Signal too high;Signal too low", ""};
array<double> outputParametersMin={FLOOR_LEVEL, -300, FLOOR_LEVEL, FLOOR_LEVEL, 
SILENCE_LEVEL, SILENCE_LEVEL, 0, 0, 0, 
CEIL_LEVEL, FLOOR_LEVEL, 0, 
SILENCE_LEVEL, 0, SILENCE_LEVEL, 0,
0, -1000};
array<double> outputParametersMax={CEIL_LEVEL, +24, CEIL_LEVEL, CEIL_LEVEL, 
CEIL_LEVEL+24, CEIL_LEVEL, 20000, 50000, 100000, 
-FLOOR_LEVEL+24, CEIL_LEVEL, 3, 
CEIL_LEVEL, 20000, CEIL_LEVEL, 50000,
7, 1000};

// error constants
int ERROR_OK = 0;
int ERROR_DATA_PAUSED = 1;
int ERROR_NO_SAVING_ON_PAUSE = 2;
int ERROR_MAX_LEVEL_LOCKED = 3;
int ERROR_HI_LEVEL_TIME_SHORT = 4;
int ERROR_LOW_LEVEL_TIME_SHORT = 5;
int ERROR_SIGNAL_TOO_HIGH = 6;
int ERROR_SIGNAL_TOO_LOW = 7;
int error_no = ERROR_OK;

// progress saving
int PROGRESS_SAVE = 1;
int PROGRESS_CLEAR = 2;
int ms_since_last_progress_save = 1000;

///////////////////////////////////////////

// "window" to correctly calculate max peak value for hi frequencies
int SAMPLES_WINDOW = 28; 
int input_channel;
int INPUT_LEFT = 0;
int INPUT_RIGHT = 1;

// for frequency detection
int freq_samples_count = 0;
int freq_signschange = 0; // how many times sign changed
int freq_fullperiod_smpcnt = 0; // counter 
int freq_fullperiod_samples = 2000; // detected length of full period in samples
double freq_fullperiod_smp_calc = 0; // the same, but calculated from frequency
double frequency = 0; // detected frequency
bool use_window_detector = false, is_good_hi_freq = false;;
double precision_ms = 0; // calculated precision in ms
double precision_smp = 0;
int precision_smp_int = 0;

double signal_value, signal_abs_value, signal_max_abs_value, signal_level, old_signal_level; // signal_level - in db
array<double> signal_values(SAMPLES_WINDOW); // keep tracking of max level for high frequencies
int signal_sign, signal_silent_samples_count, signal_samples_since_silence, signal_state, samples_since_last_level_measure; // for silence and waveform state detection
int level_state = 0;
int samples_since_level_state_change = 0, samples_since_prev_level_state_change = 0;
double last_signal_level, level_change_db, level_max_diff;
array<float> levels_over_time(1500); // in ms, auto-adjusting
bool signal_sign_changed = false;
array<array<double>> levels_precise(2); // two switchable buffers to keep info about precise level
int cur_levprec_buf = 0, old_levprec_buf = 1; // flag to select which buffer to use

// arrays for saving SVG snapshots
array<string> svg_curve_points_ar_pre();
array<string> svg_curve_points_ar_post();
array<string> svg_live_curve_points_ar();

// values in samples for time calculation (precalculated in initialize)
int s10ms, s100ms, s250ms, s500ms, ms_from_start, onems_samples_count;
double ms_from_start_precise = 0;
int s_silence_min_limit; // for how many samples signal must be low to be considered silence
array<int> ms_samplepoints(10); // keep here points in samples that equal to 1ms interval (calculate them on initialize)

// minimum time of attack and release phases (from testtone generator)
int MIN_RELEASE_DUR_SAMPLES = roundDoubleToInt(99*sampleRate/1000); // 99ms in samples
int MIN_ATTACK_DUR_SAMPLES = roundDoubleToInt(0.9*sampleRate/1000); // 99ms in samples

// misc vars
int current_state = ST_UNDEFINED; // which state we're in (attack/release/undefined)
double att_measure_level = 0; // level in db where attack time is measured
double rel_measure_level = 0; // level in db where release time is measured
double att_coeff = ATTACK_DEFAULT_COEFF_PERC/100; // coeff to set att_measure_level
double rel_coeff = RELEASE_DEFAULT_COEFF_PERC/100;// coeff to set rel_measure_level
double att_end_level = 0; // level in db where attack ends
double rel_end_level = 0; // level in db where release ends

double max_level = FLOOR_LEVEL; // maximum level (at attack start)
double min_level = signal_level; // minimum level (at release start)
double true_max_level = FLOOR_LEVEL;
double max_level_time, min_level_time;

double att_start_time = 0; // attack start time (ms timestamp, precise)
double attack_time = 0; // detected attack time (length)
double attack_end_time = 0; // when attack phase seems to end (length)
double attack_phase_length_ms = 0;
double release_start_time = 0; // true timestamp in ms
double release_time = 0; // detected release time
double release_end_time = 0; // when release phase seems to end
double vert_curve_value = FLOOR_LEVEL; // value of vertical curve
double gr, att_gr, rel_gr;
int ms_from_attack_start = 0;
int attack_av_wind, release_av_wind;

// counters & flags
// uint64 samples_from_start = 0;
uint64 samples_from_start = 0;
int samples_since_attack_start = 0;
int samples_since_release_start = 0;
int samples_precise_counter = 0;
bool output_paused = false;
bool production_mode = false;
bool lock_max_level = false;
double sample_in_ms = 0; // value of one sample in milliseconds
int samples_in_ms_cnt = 0;
bool was_saving_live = false;

double attack_start_probability = 0;
double release_start_probability = 0;
int attack_not_ended_properly = 0, release_not_ended_properly = 0;
double arcycle_len_ms = 0, last_arcycle_len_ms = 0;
int gr_mode = 0;
bool is_hires_on = false;
double hr_att_len_ms = 0, hr_rel_len_ms = 0;
string gr_type_text = "max";
string live_curve_points, prev_live_curve_points;
int curve_points_start_ms, prev_curve_points_start_ms, curve_points_end_ms;

// file saving
file f;
string curDir = "";
string snapARDir = "";
string curFileName = "";

bool flag_save_snapshot = false; // flag to save snapshot
int snapshot_mode = 0, ref_mode = 0;
int current_saving_mode = 0;
int remove_reference_in_ms = 0;
int samples_till_save_snapshot = -1; // counter offset for saving snapshot
double SVG_Y_COEFF;

double snapshot_padding_ms = 100; // 100ms - how much time to add left and right while saving snapshot
double snapshot_padding_smpls = floor(sampleRate*snapshot_padding_ms/1000);

double ms_to_save_in_snapshot = 0; // how many milliseconds to save in snapshot
int onems_point = -1;
double last_max_level, last_att_time, last_att_start_time, last_att_measure_level;
double last_att_coeff, last_att_end_time, last_att_end_level, last_att_gr, last_gr;
double last_true_max_level, last_max_level_time;

// for saving debug
string debug_data = "";
bool debug_saving_started = false;
int debug_signal_level_the_same = 0;

// initialize
void initialize() {

    // init curDir
    curDir = scriptFilePath.substr(0, scriptFilePath.findLast("/")+1);
    curFileName = scriptFilePath.substr(curDir.length, scriptFilePath.length - curDir.length - 4);
    
    // check what mode we're in (inside PnS or as  Exported plugin)
    if (curFileName != "default") {
        // running inside Plug-N-Script (not as exported plugin)
        // writing WORKDIR to skin file
        // autoUpdateSkin();
        snapARDir = curDir + "Snapshot_AR/attack_release_data/";
        production_mode = false;
    } else {
        // EXPORTED DLL version
        snapARDir = curDir + "../../../Snapshot_AR/attack_release_data/";
        production_mode = true;
        // inputParameters.resize(inputParametersNames.length - 1); // remove debug parameter

        // if MAC version
        if (scriptFilePath.findFirst("Contents/Resources") > 0) {
            //print ("MAC Version");
            snapARDir = curDir + "../../../../../../Snapshot_AR/attack_release_data/";
        } else {
            //print ("Windows version");
            snapARDir = curDir + "../../../Snapshot_AR/attack_release_data/";
        }
    }

    if (audioInputsCount == 1) {
        // only mono channel
        inputParametersMax[IP_INPUT_CHANNEL] = 1;
        inputParametersEnums[IP_INPUT_CHANNEL] = "mono;mono";
    }

    // calc silence levels
    // SILENCE_LEVEL = round(20.0*log10(SILENCE), 3); // get level (in db) from float value (0 to 1)
    SILENCE_VALUE = pow(10, SILENCE_LEVEL / 20); // get value (float from 0 to 1) from level (in db)
    // print("SILENCE_LEVEL: " + SILENCE_LEVEL + ", SILENCE_VALUE: " + SILENCE_VALUE);
    FLOOR_VALUE = pow(10, FLOOR_LEVEL / 20);

    // calc sample points which correspond to 1ms points
    calculateSamplePoints();

    // signal_level=-(CEIL_LEVEL-FLOOR_LEVEL/2);
}

// per-sample processing.
void processSample(array<double>& ioSample)
{
    // increase sample counters
    samples_from_start++;
    onems_samples_count++;
    samples_in_ms_cnt++;
    samples_since_attack_start++;
    samples_since_release_start++; 
    samples_precise_counter++;
    ms_from_start_precise += sample_in_ms; // keeps precise timestamp

    // detect if one ms passed
    onems_point = -1;
    // we use this strange cycle to determine sample values that match 1ms value
    // like for sampleRate 44100 it will be points 44, 88, .... 441 
    for( uint n = 0; n < 10; n++ ) {
        // if it's 1ms point
        // if (onems_samples_count == roundDoubleToInt((sampleRate/1000* (n+1)) )) {
        if (onems_samples_count == ms_samplepoints[n]) {
            onems_point = n+1;
        }
        // if its point of tenth millisecond (which is always on integer sample, 
        // for example 10ms in 44100 = 441, 10 ms in 88200 = 882
        // so reset counter and start again
        if (onems_point == 10) {
            onems_samples_count = 0;
        }
    }

    if (onems_point > 0) {
        samples_in_ms_cnt = 0;
        ms_from_start++; // add info about runtime
        ms_from_start_precise = ms_from_start;
        ms_since_last_progress_save++;
        ms_from_attack_start++;
    }

    // get sample value
    if (audioInputsCount == 1) {
        // we're on mono channel
        // select main (compressed) input
        signal_value = ioSample[0]; // MONO IN channel 
    } else {    
        // we're on stereo+ channel
        // use "Stereo" input for comp
        if (input_channel == 2) { 
            signal_value = (ioSample[INPUT_LEFT] + ioSample[INPUT_RIGHT]) / 2; // sum of two channels
        } else {
            // or use Left or Right input for comp
            signal_value = ioSample[input_channel];
        }
    }

    // measure frequency & levels
    measureFreqAndLevel();
    
    // detect level change since last max level
    level_change_db = signal_level - last_signal_level;
    int last_level_state = level_state;
    if (level_change_db != 0) {
        level_state = (level_change_db < 0) ? -1 : 1;
        if (last_level_state != level_state) {
            samples_since_prev_level_state_change = samples_since_level_state_change;
            samples_since_level_state_change = 0;
        } else {
            samples_since_level_state_change++;
        }
    } else {
        samples_since_level_state_change++;
    }
    
    ///////////////////////////////
    // ATTACK START DETECTING
    ///////////////////////////////
    attack_start_probability = 0;
    // if level jumps it seems like attack start
    if (level_change_db > MIN_ATTACK_JUMP_DB) attack_start_probability = 100;
    // but if we're already attacking, it's not attack start
    if ((attack_start_probability > 0) &&
        ((current_state == ST_ATT_STARTED) 
        || (current_state == ST_ATT_MEASURED) 
        || (current_state == ST_ATT_ENDED))) attack_start_probability = 20;
    if ((attack_start_probability > 0) && 
        (samples_since_release_start < MIN_RELEASE_DUR_SAMPLES)) attack_start_probability = 40;
    if (signal_level < SILENCE_LEVEL) attack_start_probability = 30;

    // if seems like it's an attack
    if (attack_start_probability > 99) {

        // print ("release started, current_state: " +current_state);
        if (current_state != ST_REL_ENDED) {
            // if attack not ended properly, show message
            release_not_ended_properly++;
            if ((release_not_ended_properly > 3) && (error_no == ERROR_OK)) {
                error_no = ERROR_LOW_LEVEL_TIME_SHORT;
            }
        } else {
            release_not_ended_properly = 0;
            if (error_no == ERROR_LOW_LEVEL_TIME_SHORT) {
                error_no = ERROR_OK;
            }
        }

        // check if signal is too high or too low
        if (ms_from_start > 10000) {
            if (true_max_level > CEIL_LEVEL) {
                error_no = ERROR_SIGNAL_TOO_HIGH;
            } else if (min_level < FLOOR_LEVEL-5) {
                if (true_max_level < CEIL_LEVEL-10) {
                    error_no = ERROR_SIGNAL_TOO_LOW;
                }
            } else {
                if ((error_no == ERROR_SIGNAL_TOO_HIGH) || (error_no == ERROR_SIGNAL_TOO_LOW)) {
                    error_no = ERROR_OK;
                }
            }
        }

        current_state = ST_ATT_STARTED;
        ms_from_attack_start = 0;

        // fix for 0 release time
        if (((rel_gr < 0.2) && (release_not_ended_properly == 0)) 
            || ((gr_mode == GR_MODE_MAX) && (gr < 0.2))) {
            release_time = 0;
        }

        // switch levels_precise buffers
        switchLevPrecBuffers();

        // join curve points
        joinCurvePoints();

        // remember previous cycle's data for snapshot saving and other
        last_max_level = max_level;
        last_true_max_level = true_max_level;
        last_max_level_time = max_level_time;
        last_att_start_time = att_start_time;
        last_att_time = attack_time;
        last_att_measure_level = att_measure_level;
        last_att_coeff = att_coeff;
        last_att_end_time = attack_end_time;
        last_att_end_level = att_end_level;
        last_att_gr = att_gr;
        last_gr = gr;

        // remember this attack start time
        att_start_time = round(ms_from_start_precise, 3);
        // print("attack start time: " + int(att_start_time) + "." + fraction(att_start_time));

        // add precise info about attack start
        if (is_hires_on) {
            levels_precise[cur_levprec_buf].insertLast(ms_from_start_precise);
            samples_precise_counter = precision_smp_int; // force immediate saving of level info
        }

        // measure time from attack till new attack (full cycle length)
        if (samples_since_attack_start > s100ms) {
            last_arcycle_len_ms = arcycle_len_ms;
            arcycle_len_ms = (samples_since_attack_start*1000)/sampleRate;
            if (abs(last_arcycle_len_ms - arcycle_len_ms) > 10) {
                // if cycle length changed
                resizeLOTBuffer();
            }
            // how many milliseconds to save in snapshot (total period of attack + release + offset at start and end)
            ms_to_save_in_snapshot = (((samples_since_attack_start+snapshot_padding_smpls*2)*1000)/sampleRate);
            SVG_Y_COEFF = ms_to_save_in_snapshot/100;
        }

        // if set flag to save snapshot
        if (flag_save_snapshot && (current_saving_mode != SNAPSHOT_MODE_REMOVE_REF)) {
            // flag_save_snapshot = false;
            samples_till_save_snapshot = roundDoubleToInt(snapshot_padding_smpls);
        }

        // mark attack start in samples
        samples_since_attack_start = 0;
        
        // update max_level (new attack - new max level)
        true_max_level = signal_level;
        max_level_time = ms_from_start_precise;
        if (!lock_max_level) { max_level = signal_level; }

        // data for helper curve
        vert_curve_value = true_max_level;
    }

    // determine max and min level (if not "locked")
    
    if (signal_level > true_max_level) { 
        true_max_level = signal_level;
        max_level_time = ms_from_start_precise;
        if (!lock_max_level) { max_level = signal_level; }

        // if we're started attack and this is real max value, update it
        if (current_state == ST_ATT_STARTED) {
            vert_curve_value = true_max_level;
            if (!output_paused) outputParameters[OP_VERT_CURVE] = vert_curve_value;
        }
    } 

    if (signal_level < min_level) {
        min_level = signal_level;
        min_level_time = ms_from_start_precise;
        // print("min_level_time 2: " +min_level_time + " min_level:" +min_level);
        // if we're started release and this is real min value, update it
        if (current_state == ST_REL_STARTED) {
            vert_curve_value = min_level;
            if (!output_paused) outputParameters[OP_VERT_CURVE] = vert_curve_value;
        }
    }

    // detect release start
    release_start_probability = 0;
    if (level_change_db < MIN_RELEASE_DROP_DB) release_start_probability = 100;
    if ((release_start_probability>0) &&
        (samples_since_attack_start < MIN_ATTACK_DUR_SAMPLES)) release_start_probability = 40;
    if (signal_level < SILENCE_LEVEL) release_start_probability = 30;


    // let's say release starts when level drops
    if (release_start_probability > 99) {
        // fix 0 attack time
        if (((att_gr < 0.2) && (attack_not_ended_properly == 0)) 
            || ((gr_mode == GR_MODE_MAX) && (gr < 0.2))) {
            attack_time = 0;
        }

        // print ("release started, current_state: " +current_state);
        if (current_state != ST_ATT_ENDED) {
            // if attack not ended properly, show message
            attack_not_ended_properly++;
            if (attack_not_ended_properly > 3) {
                // though attack was abrupt, we can
                // try to take last level as att_end_level
                // att_end_level = levels_over_time[levels_over_time.length()-2];
                // and output error
                if (error_no == ERROR_OK) { error_no = ERROR_HI_LEVEL_TIME_SHORT; }
            }
        } else {
            attack_not_ended_properly = 0;
            if (error_no == ERROR_HI_LEVEL_TIME_SHORT) {
                error_no = ERROR_OK;
            }
        }

        release_start_time = ms_from_start_precise;
        current_state = ST_REL_STARTED;
        attack_phase_length_ms = (samples_since_attack_start*1000/sampleRate);
        samples_since_release_start = 0 ; // samples_since_level_state_change ; // or just 0;
        min_level = signal_level; // update min level on release start

        min_level_time = ms_from_start_precise;
        // adding live point
        svg_live_curve_points_ar.insertLast((ms_from_attack_start + (ms_from_start_precise-ms_from_start) +snapshot_padding_ms-1) + "," + round(-signal_level*SVG_Y_COEFF, 1));

        if (is_hires_on) {
            // add precise info about release start
            levels_precise[cur_levprec_buf].insertLast(ms_from_start_precise);
            samples_precise_counter = precision_smp_int; // force immediate saving of level info
        }
        // print("min_level_time 1: " +min_level_time + " min_level:" +min_level);
        // data for helper curve
        vert_curve_value = min_level;
    }

    // if we're in attack phase
    if ((current_state == ST_ATT_STARTED) or (current_state == ST_ATT_MEASURED)) {
        // if attack level reached
        if ((current_state == ST_ATT_STARTED) 
            and (signal_level-att_measure_level < 0)
            and (level_state < 0) // level must be falling
            ) {
            current_state = ST_ATT_MEASURED;
            attack_time = ((samples_since_attack_start*1000)/sampleRate);
            //if (gr < 0.2) attack_time = 0;
        }
    // if we're in release release phase
    } else if ((current_state == ST_REL_STARTED) or (current_state == ST_REL_MEASURED)) {
        // print("in release mode " +samples_since_release_start + " signal_level: " + signal_level + " rel_measure_level: "+rel_measure_level+" sig diff: " + (signal_level-rel_measure_level));
        // if release level reached
        if ((current_state == ST_REL_STARTED) 
            and ((signal_level-rel_measure_level) > 0) 
            and (level_state > 0) // level must be rising
            ) {
            
            current_state = ST_REL_MEASURED;
            release_time = ((samples_since_release_start*1000)/sampleRate);
            // print("release measured " +release_time + " signal_level: " + signal_level + " rel_measure_level: "+rel_measure_level+" sig diff: " + (signal_level-rel_measure_level));
            // fix for 0 release
            //if ((rel_gr < 0.2)) release_time = 0;
        }
    }

    // calc average level

    // if it's one millisecond point
    if (onems_point > 0) { 

        levels_over_time.removeAt(0);
        levels_over_time.insertLast(signal_level);

        svg_live_curve_points_ar.insertLast((ms_from_attack_start+snapshot_padding_ms-1) + "," + round(-signal_level*SVG_Y_COEFF, 1));

        double diff = 0;
        level_max_diff = 0;
        
        // for release
        release_av_wind = RELEASE_AVERAGING_WINDOW_MS;
        uint n_start_position = levels_over_time.length - release_av_wind;
        
        // for attack
        if ((current_state == ST_ATT_MEASURED) or (current_state == ST_ATT_STARTED)) {
            // taking averaging "window" depending on attack time
            attack_av_wind = int(3*attack_time + 42);
            if (attack_av_wind > ATTACK_MAX_AVERAGING_WINDOW_MS) attack_av_wind = ATTACK_MAX_AVERAGING_WINDOW_MS;
            if (attack_av_wind < ATTACK_MIN_AVERAGING_WINDOW_MS) attack_av_wind = ATTACK_MIN_AVERAGING_WINDOW_MS;
            n_start_position = levels_over_time.length - attack_av_wind;
        }

        // detect level difference (level changes in measuring "window")
        for( uint n = n_start_position; n < levels_over_time.length; n++ ) {
            diff = abs(signal_level - levels_over_time[n]);
            if (diff > level_max_diff) {
                level_max_diff = diff;
            }
        }
        
        // if we're in ATTACK state
        if ((current_state == ST_ATT_MEASURED) or (current_state == ST_ATT_STARTED)) {
            // if level is static enough
            if (level_max_diff < MAX_LEVEL_DIFF_FOR_ATTACK_END) {
                vert_curve_value = signal_level;
                current_state = ST_ATT_ENDED;
                
                // int ar_index = roundDoubleToInt(levels_over_time.length() - ATTACK_MAX_AVERAGING_WINDOW_MS/2);
                // att_end_level = levels_over_time[ar_index];
                att_end_level = signal_level; // we can take current level

                attack_end_time = ((samples_since_attack_start*1000)/sampleRate); // -ATTACK_MAX_AVERAGING_WINDOW_MS/2;
                att_gr = max_level - signal_level;
                updateGR();
                
                // if locked max level is lower
                if (max_level < true_max_level) {
                    // probably max level locking error
                    error_no = ERROR_MAX_LEVEL_LOCKED;
                } else {
                    if (error_no == ERROR_MAX_LEVEL_LOCKED) {
                        error_no = ERROR_OK;
                    }
                }
            }

        } else
        // if we're in RELEASE state
        if ((current_state == ST_REL_MEASURED) or (current_state == ST_REL_STARTED)){
            // if level is static enough
            if (level_max_diff < MAX_LEVEL_DIFF_FOR_RELEASE_END) {
                // release ended
                current_state = ST_REL_ENDED;
                vert_curve_value = signal_level;
                rel_end_level = signal_level;
                release_end_time = ((samples_since_release_start*1000)/sampleRate); // RELEASE_AVERAGING_WINDOW_MS/2;
                rel_gr = abs(min_level - signal_level); // GR is taken from attack GR
                updateGR();
            }
        }

        // if flag is set to remove reference
        if (flag_save_snapshot && (current_saving_mode == SNAPSHOT_MODE_REMOVE_REF)) {
            remove_reference_in_ms--;
            if (remove_reference_in_ms <= 0) {
                // print("remove snapshot"+ms_from_start);
                outputParameters[OP_SNAPSHOT_STATE] = 0;
                saveSnapShot();
                flag_save_snapshot = false;
                if (was_saving_live) {
                    flag_save_snapshot = true;
                    outputParameters[OP_SNAPSHOT_STATE] = 2;
                    current_saving_mode = SNAPSHOT_MODE_LIVE;
                }
            }
        }

        // save progress bar to svg if needed
        if ((flag_save_snapshot) && (current_saving_mode != SNAPSHOT_MODE_REMOVE_REF)) {
            saveProgress(PROGRESS_SAVE);
            // print("saving progress "+ ms_from_start);
        }

        // add info about ms to precise levels array
        if ((is_hires_on) && (precision_smp_int > 1)) {
            levels_precise[cur_levprec_buf].insertLast(ms_from_start);
            samples_precise_counter = precision_smp_int; // force immediate saving of level info
        }

    } // END if one ms point end

    
    
    // if we need to remember precise level info
    if (is_hires_on && (precision_smp_int > 1) && (samples_precise_counter >= precision_smp_int)) {
        samples_precise_counter = 0;
        levels_precise[cur_levprec_buf].insertLast(signal_level);
    }

    // snapshot saving
    if ((samples_till_save_snapshot == 0) && (current_saving_mode != SNAPSHOT_MODE_REMOVE_REF)) {
        int saveResult = 0;
        saveResult = saveSnapShot();
        // print("save snapshot " + ms_from_start);
        // if save snapshot continiously
        if ((current_saving_mode == SNAPSHOT_MODE_LIVE) || (was_saving_live)) {
            flag_save_snapshot = true;
            outputParameters[OP_SNAPSHOT_STATE] = 2;
            current_saving_mode = SNAPSHOT_MODE_LIVE;
        } else {
            if (saveResult == 1) {
                // print("stopped because current_saving_mode: " + current_saving_mode);
                flag_save_snapshot = false;
                outputParameters[OP_SNAPSHOT_STATE] = 0;
                saveProgress(PROGRESS_CLEAR);
            }
        }
    }
    if (samples_till_save_snapshot >= 0) samples_till_save_snapshot--;
    

    // remember last level
    last_signal_level = signal_level;

    // mute output
    if (inputParameters[IP_MUTE_OUTPUT] > 0.5) {
        for(uint channel=0;channel<audioInputsCount;channel++) { 
            ioSample[channel]=0; 
        }
    }

    // debug saving
    if (debug_saving_started) { debugAddLine(); }



} // end of process sample function

// measure frequency & peak level
void measureFreqAndLevel() {

    // increase samples counter
    freq_samples_count++;
    freq_fullperiod_smpcnt++;
    samples_since_last_level_measure++;

    // measure freq and level of compressed signal
    signal_abs_value = abs(signal_value);

    // remember abs values for measuring higher frequencies using "window"
    signal_values.removeAt(0);
    signal_values.insertLast(signal_abs_value);

    signal_sign_changed = false;
    // if signal is present and loud enough
    if (signal_abs_value > SILENCE_VALUE) {

        // silence/unsilence counters
        signal_silent_samples_count = 0;
        signal_samples_since_silence++;

        // detect signal change
        if (signal_value < -SILENCE_VALUE) {
            if (signal_sign >= 0) {
                signal_sign = -1;
                signal_sign_changed = true;
                // calculate length of one period in samples
                freq_fullperiod_samples = freq_fullperiod_smpcnt;
                freq_fullperiod_smpcnt = 0;
                // print ("s_silence_min_limit: "+s_silence_min_limit+", freq_fullperiod_samples: " + freq_fullperiod_samples );
            }
        } else {
            if (signal_sign <= 0) {
                signal_sign = 1;
                signal_sign_changed = true;
            }
        }
    } else {
        // if signal is low, count silent samples
        signal_silent_samples_count++;
        if (signal_silent_samples_count > s_silence_min_limit) {
            // if silent samples last long enough, lets consider it real silence
            signal_samples_since_silence = 0;
        }
    }

    // if sign change detected
    if (signal_sign_changed) {
        // detect frequency
        freq_signschange++; // count sign changes qty
        if (freq_samples_count >= sampleRate) {
            // if signal is present for long enough time, measure frequency
            if (signal_samples_since_silence >= sampleRate) {
                frequency = freq_signschange/2.0;
                freq_fullperiod_smp_calc = sampleRate / frequency;
                
                is_good_hi_freq = false;

                // use "window" detector for hi freqs
                if (freq_fullperiod_samples < SAMPLES_WINDOW) {
                    use_window_detector = true;
                    
                    // high frequency is good when samplelength is close to integer
                    // we can use peak detector on such freqs
                    double sldiff = abs(round(freq_fullperiod_smp_calc, 0) - freq_fullperiod_smp_calc);
                    // print ("sl: " + " " + freq_fullperiod_smp_calc +" diff: " + sldiff);
                    is_good_hi_freq = (sldiff < 0.02);
                    if (is_good_hi_freq) { use_window_detector = false; }

                } else {
                    // for lower freqs use peak detector
                    use_window_detector = false;
                }

                // let's calculate precision
                if (use_window_detector) {
                    precision_smp = SAMPLES_WINDOW; // half of wavelength in samples
                    precision_smp_int = SAMPLES_WINDOW;
                    precision_ms = (SAMPLES_WINDOW*1000) / sampleRate;
                } else {
                    // for peak detector
                    precision_ms = 500 / frequency; // half of wavelength in ms
                    precision_smp = freq_fullperiod_smp_calc / 2; // half of wavelength in samples
                    precision_smp_int = roundDoubleToInt(precision_smp);
                }

                // debugFreq();
            } 
            freq_samples_count = 0;
            freq_signschange = 0;
        }
    }

    // if silence detected
    if (signal_samples_since_silence == 0) {
        signal_max_abs_value = 0; // reset max_abs_value
        // signal_level = SILENCE_LEVEL; // set level to silence
        signal_level = round(20.0*log10(signal_abs_value), 3);
        if (signal_level < -300) signal_level = -300;
    } else {
        // if signal present, detect level

        // if sign change (zero crossing) detected
        if (signal_sign_changed) {
            if (signal_state != 1) { // update max value if prev max value was used
                signal_max_abs_value = signal_abs_value; // reset signal_max_abs_value
            }
        }

        // detect signal rising or falling
        if (signal_abs_value >= signal_max_abs_value) {
            // signal rising or on top (in this half-period)
            signal_max_abs_value = signal_abs_value;
            signal_state = 1; // signal rising or on peak
        } else {
            // signal falling (in this half-period)
            if (signal_state == 1) { // if was on peak (one sample before)
                signal_state = 0; 

                double signal_value_to_db = signal_max_abs_value;
                // if frequency is high and samplerate low, 
                // peak measuring is unreliable
                if (use_window_detector) {
                    signal_value_to_db = getMaxValue(signal_values);
                }
                // calculate last peak level
                signal_level = round(20.0*log10(signal_value_to_db), 3);
                samples_since_last_level_measure = 0;
            } else {
                signal_state = -1; // signal now falling
            }

            // to fix level detection on hi freqs, where sample_period is about 2 samples)
            if ((use_window_detector) && (samples_since_last_level_measure > freq_fullperiod_samples)) {
                double signal_value_to_db = getMaxValue(signal_values);
                signal_level = round(20.0*log10(signal_value_to_db), 3);
                samples_since_last_level_measure = 0;
            }
        }

    }

    //debugMinMax();
}

// calc misc samplepoints which will be used in later calculations
void calculateSamplePoints(){
    // calc sample points which correspond to 1ms (because of approximity of 1ms at 44100 and 88200)
    for( uint n = 0; n <= 9; n++ ) {
        ms_samplepoints[n] = roundDoubleToInt((sampleRate/1000*(n+1)) );
    }
    // samplepoints for milliseconds
    s10ms = roundDoubleToInt(sampleRate/100);
    s100ms = roundDoubleToInt(sampleRate/10);
    s250ms = roundDoubleToInt(sampleRate/4);
    s500ms = roundDoubleToInt(sampleRate/2);
    sample_in_ms = 1000/sampleRate;

    // for how many samples signal must be low to be considered silence
    s_silence_min_limit = roundDoubleToInt(sampleRate/10000); // 0.1ms
}

// save progress bar
void saveProgress(int mode = PROGRESS_SAVE){
    double x_max = ms_to_save_in_snapshot;
    double y_max = x_max*0.45;

    double width=450;
    double xstart = ceil((samples_since_attack_start/sampleRate) * 1000) + snapshot_padding_ms-width/2;
    double height = ceil(y_max*1.2/100);
    double ystart = 0; // y_max - height;
    if (width<0) return;

    if (((xstart>=0) && (xstart<x_max)) || (mode == PROGRESS_CLEAR)) {
        string t = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" +
        "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 " + x_max + " " + y_max + "\"";
        t += " preserveAspectRatio=\"none\" >\n";
        if (mode != PROGRESS_CLEAR) {
            t += "<linearGradient id='linear-gradient'><stop offset='0%' stop-color='#EEEEEE'/><stop offset='50%' stop-color='#00DD00'/><stop offset='100%' stop-color='#EEEEEE'/></linearGradient>";
            t += "<rect fill=\"url(#linear-gradient)\" class=\"progress\" width=\""+width+"px\" height=\""+height+"px\" x=\""+xstart+"px\" y=\""+ystart+"px\" opacity='0.75'/>";
            }
        t += "</svg>";

        if ((ms_since_last_progress_save > 250) || (mode == PROGRESS_CLEAR)) {
            file f;
            if(f.open(snapARDir+"progress.svg","w")>=0) {
                f.writeString(t); 
                f.close();
                ms_since_last_progress_save = 0;
                // print("saved progress " + xstart + " " +width);
            }
        }
    }
}


// updating current GR mode && measuring levels
void updateGR(){
    if (gr_mode == GR_MODE_MAX) {
        gr_type_text = "max";
        gr = att_gr;
        if (rel_gr > gr) gr = rel_gr;
    } else if (gr_mode == GR_MODE_ATTACK) {
        gr_type_text = "attack";
        gr = att_gr;
    } else {
        gr_type_text = "release";
        gr = rel_gr;
    }

    // calculating measure levels
    att_measure_level = att_end_level - (1-att_coeff)*-gr;
    rel_measure_level = rel_end_level + (1-rel_coeff)*-gr;
    if (attack_not_ended_properly > 1) {
        // if att_end_level is not correct
        att_measure_level = max_level - gr*att_coeff;
    }
    if (release_not_ended_properly > 1) {
        // if release_end_level is not correct
        rel_measure_level = min_level + gr*rel_coeff;
    }

    if (!output_paused) {
        outputParameters[OP_GR] = gr;
    }
}


// auto-adjusting length of main buffer
void resizeLOTBuffer(){
    // if cycle length changed
    // adjusting levels_over_time array (keeping every 1 ms of data)
    int arcycle_len_ms_int = roundDoubleToInt(round(arcycle_len_ms/50,0)*50);
    int lol_set_length = arcycle_len_ms_int + int(snapshot_padding_ms*2) + 100; // 100 just for so
    if (lol_set_length < 1000) { lol_set_length = 1000; }
    int lotlen = levels_over_time.length;
    if (lotlen != lol_set_length) {
        if (lol_set_length>lotlen) {
            // insert empty data at beginning of array
            for (int n=0;n<lol_set_length-lotlen;n++) { levels_over_time.insertAt(0,0); }
        } else {
            // remove part of data at beginning of array
            for (int n=0;n<lotlen-lol_set_length;n++){ levels_over_time.removeAt(0); }
        }
        levels_over_time.resize(lol_set_length);
    } else {
        // no need to resize level_over_time
        // print("no need to resize level_over_time");
    }


}

// switching precise level data buffer (called on attack start)
void switchLevPrecBuffers(){
    // switch level_precise buffer
    old_levprec_buf = cur_levprec_buf;
    cur_levprec_buf = int(abs(cur_levprec_buf-1));

    // copying last data from current buffer (that we just switched to) to another buffer
    // it's data before previous attack start
    int lpclen = levels_precise[cur_levprec_buf].length;
    int npos = lpclen-100;
    if (npos > 0) {
        for (int n=lpclen-1;n>=npos;n--) {
            levels_precise[old_levprec_buf].insertAt(0, levels_precise[cur_levprec_buf][n]);
        }
    }

    // clearing current buffer for new cycle
    levels_precise[cur_levprec_buf].resize(0);
}

// making string from points from attack to attack
void joinCurvePoints(){
    // adding live curve points
    live_curve_points = join(svg_live_curve_points_ar, " ");
    //prev_live_curve_points = live_curve_points;

    curve_points_start_ms = prev_curve_points_start_ms;
    curve_points_end_ms = int(ms_from_start_precise) - 1;
    prev_curve_points_start_ms = int(ms_from_start_precise);

    svg_live_curve_points_ar.resize(0);
}


int getTailSize()
{
    return 0;
}

void reset(){}


/* called when input parameters change */
void updateInputParametersForBlock(const TransportInfo@ info)
{

    // input attack and release measure coeff
    att_coeff = inputParameters[IP_ATT_COEFF]/100;
    rel_coeff = inputParameters[IP_REL_COEFF]/100;

    // lock max level flag
    lock_max_level = (inputParameters[IP_LOCK_MAXLEVEL] > 0.5) ;
    
    // gr mode
    int prev_gr_mode = gr_mode;
    gr_mode = roundDoubleToInt(inputParameters[IP_GR_MODE]);
    if (prev_gr_mode != gr_mode) updateGR();

    // snapshot saving mode
    int prev_snapshot_mode = snapshot_mode;
    snapshot_mode = int(round(inputParameters[IP_SAVE_SNAPSHOT_MODE]));

    if ((snapshot_mode != prev_snapshot_mode) && ((flag_save_snapshot) || (samples_till_save_snapshot >= 0))) {
        // prevent saving when changing mode
        flag_save_snapshot = false;
        outputParameters[OP_SNAPSHOT_STATE] = 0;
        // print("stopped because samples_till_save_snapshot: " + samples_till_save_snapshot);
        samples_till_save_snapshot = -1; // another flag for snapshot saving
        was_saving_live = false; // another flag
        // clear progressBar
        saveProgress(PROGRESS_CLEAR);
    }

    // print("snapshot_mode: " + snapshot_mode);

    // hires mode settings
    int pos = roundDoubleToInt(inputParameters[IP_HR_ATT_LEN]);
    hr_att_len_ms = parseInt(hr_att_len_presets[pos]);
    pos = roundDoubleToInt(inputParameters[IP_HR_REL_LEN]);
    hr_rel_len_ms = parseInt(hr_rel_len_presets[pos]);
    is_hires_on = ((hr_att_len_ms > 0) || (hr_rel_len_ms > 0));
    // print("is_hires_on: " + is_hires_on + " hr_att_len_ms: " + hr_att_len_ms + " hr_rel_len_ms: " + hr_rel_len_ms);

    // paused switch
    bool output_was_paused = output_paused;
    output_paused = false;
    if (inputParameters[IP_PAUSE] > 0.75) { 
        output_paused = true;
        flag_save_snapshot = false;
        outputParameters[OP_SNAPSHOT_STATE] = 0;
        samples_till_save_snapshot = -1; // another flag for snapshot saving
        if (!output_was_paused) {
            error_no = ERROR_DATA_PAUSED;
        }
    } else {
        if ((error_no == ERROR_DATA_PAUSED) || (error_no == ERROR_NO_SAVING_ON_PAUSE)) {
            error_no = ERROR_OK;  
        }
    }

    int do_save_snap = roundDoubleToInt(inputParameters[IP_SAVE_SNAPSHOT]);

    // click on save snapshot
    if (do_save_snap == SAVE_SNAP) {
        was_saving_live = false;
        // saving in pause mode
        if (output_paused) {
            // saving on pause is disabled
            error_no = ERROR_NO_SAVING_ON_PAUSE;
        } else {
            // print("current_saving_mode: " + current_saving_mode);
            // saving in normal (not paused) mode
            if (flag_save_snapshot != true) {
                // if not saving now, start saving
                current_saving_mode = snapshot_mode;
                flag_save_snapshot = true;
                outputParameters[OP_SNAPSHOT_STATE] = 1;
                if (current_saving_mode == SNAPSHOT_MODE_LIVE) {
                    outputParameters[OP_SNAPSHOT_STATE] = 2;
                } else if (current_saving_mode == SNAPSHOT_MODE_REMOVE_REF) {
                    outputParameters[OP_SNAPSHOT_STATE] = 3; // remove ref
                    remove_reference_in_ms = 300;
                    // saveSnapShot();
                }
            } else {
                // print("stopped because of clicking button");
                outputParameters[OP_SNAPSHOT_STATE] = 0;
                flag_save_snapshot = false;
                samples_till_save_snapshot = -1; // another flag for snapshot saving
                // clear progressBar
                saveProgress(PROGRESS_CLEAR);
                // print("saved "+ms_from_start);
            }
        }
        // print("Saving snapshot..." + ms_from_start);


    } else if (do_save_snap == SAVE_SNAP_REF) { // save reference
        was_saving_live = ((current_saving_mode == SNAPSHOT_MODE_LIVE) && (flag_save_snapshot));
        flag_save_snapshot = true;
        current_saving_mode = SNAPSHOT_MODE_REFERENCE;
        outputParameters[OP_SNAPSHOT_STATE] = 1;

    } else if (do_save_snap == SAVE_SNAP_REF_REMOVE) { // remove reference
        was_saving_live = ((current_saving_mode == SNAPSHOT_MODE_LIVE) && (flag_save_snapshot));
        flag_save_snapshot = true;
        current_saving_mode = SNAPSHOT_MODE_REMOVE_REF;
        outputParameters[OP_SNAPSHOT_STATE] = 3; // remove ref
        remove_reference_in_ms = 300;

    }

    // input channel
    input_channel = roundDoubleToInt(inputParameters[IP_INPUT_CHANNEL]);

    // save debug
    if (!production_mode) {
        if (inputParameters[IP_SAVE_DEBUG] > 0.5) {
            if (!debug_saving_started) {
                debug_saving_started = true;
                debugSaveStart();
            }
        } else {
            if (debug_saving_started) {
                debug_saving_started = false;
                debugSaveEnd();
            }
        }
    }

}

void computeOutputData()
{

    if (!output_paused) {
        outputParameters[OP_ATT_TIME] = attack_time;
        outputParameters[OP_REL_TIME] = release_time;

        outputParameters[OP_ATT_LEV] = att_measure_level;
        outputParameters[OP_REL_LEV] = rel_measure_level;
        outputParameters[OP_ATT_END_LVL] = att_end_level;
        outputParameters[OP_ATT_END_TIME] = attack_end_time;
        outputParameters[OP_REL_END_LVL] = rel_end_level;
        outputParameters[OP_REL_END_TIME] = release_end_time;

        outputParameters[OP_MAX_LEV] = max_level;
        outputParameters[OP_GR] = gr;
        outputParameters[OP_SIGNAL_LEVEL] = signal_level;
        outputParameters[OP_SIGNAL_LEVEL_REAL] = signal_level;
        outputParameters[OP_MIN_LEV] = min_level;
        outputParameters[OP_FREQ] = frequency;
        if (is_good_hi_freq) {
            outputParameters[OP_PRECISION] = -precision_ms; // negative is a KUIML mark for good freq
        } else {
            outputParameters[OP_PRECISION] = precision_ms;
        }
        outputParameters[OP_VERT_CURVE] = vert_curve_value;
    }

    outputParameters[OP_ERR_MSG] = error_no;

}


///////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////

// return max_value from last max_values_array (to detect proper max level for high frequencies)
double getMaxValue(array<double> data_array)
{
    double this_max_value = 0;
    uint array_length = SAMPLES_WINDOW; // data_array.length(); 
    for( uint n = 0; n < array_length; n++ ) {
        if (data_array[n] > this_max_value) {
            this_max_value = data_array[n];
        }
    }
    return this_max_value;
}

int roundDoubleToInt(double d)
{
 if(d<0)
     return int(d-.5);
 else
     return int(d+.5);
}

double round(double d, double p = 2) {
    double x = pow(10, p);
    double r = floor(d*x+0.5)/x ;
    //print("x: " + x + ", r: " + r);
    return r;
}

string getHMS(){
    datetime d;
    string s = formatFloat(d.get_hour(),"0", 2, 0) + ":" + formatFloat(d.get_minute(),"0", 2, 0) + ":" + formatFloat(d.get_second(),"0", 2, 0);
    // print("time: " + s);
    return s;
}

string trim(string s) {
    int from = s.findFirstNotOf(" ");
    if (from < 0) {
        return s;
    }
    int to = s.length;
    int this_to;
    for(uint n=from;n<s.length;n++) {
        this_to = s.findFirstOf(" ", n);
        if (this_to < 0) { // if no more spaces
            break;
        } else {
            // found next space
            n = this_to;
            // find if only spaces after
            if (s.findFirstNotOf(" ", this_to) < 0) {
                to = this_to;
                break;
            }
        }
    }
    if (to < from) to = s.length;
    string s2 = s;
    // if (to > 0) 
    s2 = s.substr(from, to-from);
    // print("s: [" + s + "] s2: ["+s2+"] from: " +from + " to: "+ to);
    return s2;
}

// returns rounded value as string
string roundTrim(double d, double p = 2) {
    double resd = round(d, p);
    string ress = trim(formatFloat(resd,"l", 16, int(p)));
    return ress;
}


////////////////////////////////////////
// SNAPSHOT SAVING
////////////////////////////////////////


// save data for analysis
int saveSnapShot(){
    
    int saveResult = 1; // by default saved OK

    // taking ms from previous attack - padding
    int true_ms = int(last_att_start_time) - int(snapshot_padding_ms);
    if (true_ms < 0) {
        saveResult = 0; // need to resave cause not enough data
    }

    double lev_val = 0;
    string svg_head, svg_style, svg_bg_rect, svg_attrel, 
        svg_extrainfo, svg_curve, svg_end, svg_curve_points, 
        svg_hr_att_data, svg_hr_rel_data, svg_hr_att_ref_data, svg_hr_rel_ref_data,
        raw_data, raw_hr_att_data, raw_hr_rel_data; // pieces of code for export

    double x_max = ms_to_save_in_snapshot; // is not rounded
    double y_max = x_max*0.45;

    // scaling font-sizes
    double font_size = 16;
    double line_height = 10;

    // scaling elements depending on length of cycle
    if (arcycle_len_ms < 301) {
        font_size = 7;
        line_height = 10;
    } else {
        font_size = floor(0.01384615385*arcycle_len_ms + 2.846153846);
        line_height = ceil(font_size*1.4);
    }
 
    // flag to save raw data
    bool save_raw_data = (current_saving_mode == SNAPSHOT_MODE_NORMAL_WITH_RAWDATA);

    // prepare curve and raw data
    int lotlen = levels_over_time.length;
    uint n_start = lotlen - (int(ms_from_start) - int(last_att_start_time)) - int(snapshot_padding_ms);
    if (n_start < 0) n_start = 0;
    uint n_end = lotlen;
    // print("n_start: " + n_start);
    // uint n_start = lotlen - roundDoubleToInt(ms_to_save_in_snapshot); // how many milliseconds to save
    
    int ss_attack_start_ms = roundDoubleToInt(snapshot_padding_ms);
    int ss_attack_measured_ms = roundDoubleToInt(ss_attack_start_ms+last_att_time);
    int ss_attack_ended_ms = roundDoubleToInt(ss_attack_start_ms+last_att_end_time);
    if (attack_not_ended_properly > 0) { ss_attack_ended_ms = 100000000; }
    int ss_release_start_ms = roundDoubleToInt(ss_attack_start_ms+attack_phase_length_ms);
    int ss_release_measured_ms = roundDoubleToInt(ss_release_start_ms+release_time);
    int ss_release_ended_ms = roundDoubleToInt(ss_release_start_ms+release_end_time);
    if (release_not_ended_properly > 0) { ss_release_ended_ms = 100000000; }

    raw_data = "SampleRate:;"+sampleRate+"\nFrequency:;"+frequency+"\nResolution:;1ms\n\nms;level;phase ms;GR;hints;\n";

    int svg_ms, rd_phase_ms;
    double svg_ms_att_start = snapshot_padding_ms;

    // svg_curve_points = live_curve_points;

    svg_curve_points = "";
    svg_curve_points_ar_pre.resize(0);
    svg_curve_points_ar_post.resize(0);
    string svg_point;

    // we have two ways to generate level curve
    // 1) generate in right now, using data from levels_over_time array
    //    it is straightforward, but causes CPU spike because of too much string/array manipulation
    // 2) use generated "live" curve from last pass (from attack to attack)
    //    we also will add and start to that curve from levels_over_time
    //    but it's much less CPU intensive
    //    * note - if cycle length (graph length/length of high/low periods) changed - 
    //      than we have to use first way 

    bool use_live_points = false;

    // using live mode points in live mode only (for decreasing CPU usage)
    if (current_saving_mode == SNAPSHOT_MODE_LIVE) {
        use_live_points = true; // flag to use live points (generated on last pass)
        // if cycle length changed, don't use live points
        if (last_arcycle_len_ms != arcycle_len_ms) use_live_points = false;
    }

    for( uint n = n_start; n < n_end; n++ ) {
        true_ms++;

        if (((use_live_points)
            && ((true_ms <= curve_points_start_ms) || (true_ms > curve_points_end_ms)))
            || (!use_live_points)) {

            lev_val = levels_over_time[n];
            svg_ms = n - n_start;

            svg_point = svg_ms + "," + round(-lev_val*SVG_Y_COEFF, 1);

            // bottlekneck - requires too much CPU, think about optimising (esp. in live mode)
            if (true_ms == int(last_max_level_time)) {
                // adding true max level point (previous)
                svg_ms_att_start = (svg_ms + fraction(last_max_level_time));
                // svg_curve_points += svg_ms_att_start + "," + round(-last_true_max_level*SVG_Y_COEFF, 1) + " ";
                svg_point += " " + svg_ms_att_start + "," + round(-last_true_max_level*SVG_Y_COEFF, 1);
            } else if (true_ms == int(max_level_time)) {
                // adding true max level point (recent)
                // svg_curve_points += (svg_ms + fraction(max_level_time)) + "," + round(-true_max_level*SVG_Y_COEFF, 1) + " ";
                svg_point += " " + (svg_ms + fraction(max_level_time)) + "," + round(-true_max_level*SVG_Y_COEFF, 1);
            } else if (true_ms == int(min_level_time)) {
                // adding true min level point
                // svg_curve_points += (svg_ms + fraction(min_level_time)) + "," + round(-min_level*SVG_Y_COEFF, 1) + " ";
                svg_point += " " + (svg_ms + fraction(min_level_time)) + "," + round(-min_level*SVG_Y_COEFF, 1);
            }

            if (!use_live_points) {
                svg_curve_points += svg_point + " ";
            } else {
                if (true_ms <= curve_points_start_ms) {
                    svg_curve_points_ar_pre.insertLast(svg_point);
                } else if (true_ms > curve_points_end_ms) {
                    svg_curve_points_ar_post.insertLast(svg_point);
                }
            }

            // preparing raw_data
            if (save_raw_data) {
                rd_phase_ms = (svg_ms - ss_attack_start_ms);
                if (rd_phase_ms < -5) continue;
                if (svg_ms >= ss_release_start_ms) rd_phase_ms = (svg_ms - ss_release_start_ms);
                raw_data += true_ms + ";" + lev_val + ";";
                if ((svg_ms >= ss_release_start_ms) && (svg_ms <= ss_release_ended_ms)) {
                    raw_data += rd_phase_ms+";"+roundTrim(rel_end_level-lev_val, 3);
                } else if ((svg_ms >= ss_attack_start_ms) && (svg_ms <= ss_attack_ended_ms)) {
                    if (svg_ms < ss_release_start_ms) raw_data += rd_phase_ms+";"+roundTrim(last_max_level-lev_val, 3);
                } else {
                    raw_data += ";;";
                }

                if (svg_ms == ss_attack_start_ms) {
                    raw_data += ";ATTACK STARTED @ "+roundTrim(last_att_start_time,2)+" ms;MAX LEVEL: " +last_max_level + " dB";
                    if (lock_max_level) { 
                        raw_data += " (LOCKED);"+last_true_max_level+" dB (TRUE @ "+roundTrim(last_max_level_time, 2)+" ms)";
                    } else {
                        raw_data += " @ "+roundTrim(last_max_level_time, 2)+" ms";
                    }
                }

                if (svg_ms == ss_attack_measured_ms) raw_data += ";ATTACK MEASURED;"+last_att_time+" ms;"+"Level: "+round(last_att_measure_level)+" dB ("+(last_att_coeff*100)+"% of "+gr_type_text+" GR "+round(gr)+" dB)";
                if (svg_ms == ss_attack_ended_ms) raw_data += ";ATTACK ENDED;"+"GR att: "+last_att_gr+" dB";
                if (svg_ms == ss_release_start_ms) {
                    raw_data += ";RELEASE STARTED @ "+roundTrim(release_start_time,2)+" ms;MIN LEVEL: " +min_level + " dB @ "+roundTrim(min_level_time,2)+" ms;GR rel max: "+rel_gr+" dB";
                }
                if (svg_ms == ss_release_measured_ms) raw_data += ";RELEASE MEASURED;"+release_time+" ms;Level: "+round(rel_measure_level)+" dB ("+(rel_coeff*100)+"% of "+gr_type_text+" GR "+round(gr)+" dB)";
                if (svg_ms == ss_release_ended_ms) raw_data += ";RELEASE ENDED;";
                raw_data += "\n";
            }
        }
    }

    string pre_attack_points = join(svg_curve_points_ar_pre, " ");
    string post_attack_points = join(svg_curve_points_ar_post, " ");;
    string full_live_curve_points = pre_attack_points + " " + live_curve_points + " " + post_attack_points;

    svg_head += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" +
    "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 " + x_max + " " + y_max + "\" font-size='"+font_size+"px'";
    string svg_head_proportional = svg_head + ">\n";
    svg_head += " preserveAspectRatio=\"none\" >\n";

    svg_style = "<style>\n" +
        "\t"+"rect.wholespace {fill:#EEEEEE;stroke-width:0.5;stroke:#AAA;vector-effect: non-scaling-stroke; }\n" +
        "\t"+"text {font: 120% sans-serif;}\n" +
        "\t"+"text.title {fill: #004040; font: 230% bold sans-serif; text-anchor: middle; opacity: 0.9; }\n" +
        "\t"+"text.gridtext {font: 90% sans-serif;fill: #aaaaaa;}\n" +
        "\t"+"text.text_a {fill: #004040; font: 140% bold sans-serif; }\n" +
        "\t"+"text.text_b {fill: #015965; font: 90% sans-serif; opacity: 0.9; }\n" +
        "\t"+"text.text_c {fill: #015965; font: 90% sans-serif; opacity: 0.8;  }\n" +
        "\t"+"text.text_d {fill: #015965; font: 90% sans-serif; opacity: 0.6;  }\n" +
        "\t"+"text.text_ref { fill: #7A0000; font: 90% sans-serif; opacity: 0.9; }\n" +

        "\t"+"line {stroke-width:3; vector-effect: non-scaling-stroke; }\n" +
        "\t"+"line.gta { vector-effect: non-scaling-stroke; stroke:#555555; opacity: 0.15; stroke-width:3; }\n" +
        "\t"+"line.gtb { vector-effect: non-scaling-stroke; stroke:#555555; opacity: 0.3; stroke-width:2;stroke-dasharray:3,3; }\n" +
        "\t"+"line.gtc { vector-effect: non-scaling-stroke; stroke:#555555; opacity: 0.4; stroke-width:1;stroke-dasharray:3,3; }\n" +
        "\t"+"line.gda { vector-effect: non-scaling-stroke; stroke:#555555; opacity: 0.4; stroke-width:1;stroke-dasharray:10,3; }\n" +
        "\t"+"line.gdb { vector-effect: non-scaling-stroke; stroke:#555555; opacity: 0.4; stroke-width:1;stroke-dasharray:2,6; }\n" +
        "\t"+"line.line_a {stroke:#D28035; opacity: 0.85; stroke-width:1; }\n" +
        "\t"+"line.line_r {stroke:#03899C; opacity: 0.85; stroke-width:1; }\n" +
        "\t"+"line.att_mes {stroke:#FF1300; opacity: 0.6; stroke-width:2; }\n" +
        "\t"+"line.rel_mes {stroke:#007F16; opacity: 0.7; stroke-width:2; }\n" +
        
        "\t"+"polyline { vector-effect: non-scaling-stroke; fill: transparent; stroke-linejoin:round; stroke-width:2; }\n" +
        "\t"+"polyline.curve {stroke: #06266F; }\n" +
        "\t"+"polyline.curve_ref { stroke: #DD0000; stroke-width:3; opacity: 0.5; }\n" +
    "</style>\n\n";

    svg_end = "\n</svg>";

    svg_bg_rect = "<!-- Gray background -->\n<rect class=\"wholespace\" width=\"100%\" height=\"100%\" />\n";

    // text: max level
    double astart_x = snapshot_padding_ms;
    double maxlt_x = astart_x - line_height*5;
    if (maxlt_x < line_height) maxlt_x = line_height;
    double maxl_y = -last_max_level*SVG_Y_COEFF;
    double maxlt_y = maxl_y - line_height*0.625;
    double maxl_w = (astart_x + font_size*5.5);
    if (last_max_level>-2) {
        maxlt_y = maxlt_y + line_height;
        maxlt_x = maxl_w + font_size;
    }

    // text: max level
    if (maxlt_y < font_size) maxlt_y = font_size;
    svg_extrainfo += "\n\n<!-- Max level & attack end -->\n<text class='text_b' x='"+maxlt_x+"' y='"+(maxlt_y)+"'>Max level";
    if (lock_max_level) svg_extrainfo += " (locked)";
    svg_extrainfo += ": "+round(last_max_level)+" dB";
    if (lock_max_level) svg_extrainfo += ", true: "+round(last_true_max_level)+" dB";
    svg_extrainfo += "</text>";
    svg_extrainfo += "\n<line class='line_a' x1='0' x2='"+maxl_w+"' y1='"+maxl_y+"' y2='"+maxl_y+"' />";

    // text: attack (measured)
    double am_x = astart_x+last_att_time;
    double amt_x = am_x+line_height*0.6;
    double am_y = -last_att_measure_level*(x_max/100);
    double amt_y = am_y-line_height*1.7;
    if ((last_att_measure_level < 0) && (last_att_time > 0)) {
        svg_attrel += "\n\n<!-- Attack -->\n<text class='text_a' x='"+amt_x+"' y='"+amt_y+"'>Attack: "+round(last_att_time)+" ms </text>";
        svg_attrel += "\n<text class='text_b' x='"+amt_x+"' y='"+(amt_y+line_height)+"'>Level: "+round(last_att_measure_level)+" dB ("+(last_att_coeff*100)+"% of "+gr_type_text+" GR "+round(gr)+" dB)</text>"+
        "\n<line class='att_mes' x1='"+am_x+"' x2='"+am_x+"' y1='"+(am_y-line_height*3)+"' y2='"+am_y+"' />"+
        "\n<line class='att_mes' x1='"+am_x+"' x2='"+(am_x+line_height*12.5)+"' y1='"+am_y+"' y2='"+am_y+"' />";
    }

    // text: attack ended
    double a3 = astart_x+last_att_end_time;
    double a3y = -last_att_end_level*(x_max/100);
    if ((last_att_end_time > 0) && (attack_not_ended_properly == 0) && (last_att_gr>0.1)) {
        svg_extrainfo += "\n<line class='line_a' x1='"+a3+"' x2='"+a3+"' y1='"+(a3y-line_height*1.3)+"' y2='"+(a3y+line_height/2)+"' />";
        svg_extrainfo += "\n<text class='text_c' x='"+(a3+font_size/3)+"' y='"+(a3y-line_height/3)+"'>"+round(last_att_end_time)+" ms, level: "+round(last_att_end_level)+" dB, GR: "+round(last_att_gr)+" dB</text>";
    }

    // text: min level 
    double r_x = astart_x+attack_phase_length_ms;
    double minlt_x = r_x;
    double minl_y = -min_level*SVG_Y_COEFF;
    svg_extrainfo += "\n\n<!-- Min level & release end -->\n<text class='text_b' text-anchor='middle' x='"+(minlt_x)+"' y='"+(minl_y+line_height)+"'>Min level: "+round(min_level)+" dB</text>";
    svg_extrainfo += "\n<line class='line_r' x1='"+(r_x-font_size*5.5)+"' x2='"+(r_x+font_size*5.5)+"' y1='"+minl_y+"' y2='"+minl_y+"' />";
    
    // text: release
    double rm_x = r_x+release_time;
    double rmt_x = rm_x+line_height*0.6;
    double rm_y = -rel_measure_level*(x_max/100);
    double rmt_y = rm_y+line_height*1.3;
    if ((rel_measure_level < 0) && (release_time>0)) {
        svg_attrel += "\n\n<!-- Release -->\n<text class='text_a' x='"+rmt_x+"' y='"+rmt_y+"'>Release: "+round(release_time)+" ms </text>";
        svg_attrel += "\n<text class='text_b' x='"+rmt_x+"' y='"+(rmt_y+line_height)+"'>Level: "+round(rel_measure_level)+" dB ("+(rel_coeff*100)+"% of "+gr_type_text+" GR "+round(gr)+" dB)</text>"+
        "\n<line class='rel_mes' x1='"+rm_x+"' x2='"+rm_x+"' y1='"+(rm_y+line_height*3)+"' y2='"+rm_y+"' />"+
        "\n<line class='rel_mes' x1='"+rm_x+"' x2='"+(rm_x+line_height*12.5)+"' y1='"+rm_y+"' y2='"+rm_y+"' />";
    }

    // text: release ended
    double r3 = r_x+release_end_time;
    double r3y = -rel_end_level*(x_max/100);
    if ((release_end_time > 0) && (release_not_ended_properly == 0) && (rel_gr>0.1)) {
        svg_extrainfo += "\n<line class='line_r' x1='"+r3+"' x2='"+r3+"' y1='"+(r3y-line_height/2)+"' y2='"+(r3y+line_height*1.3)+"' />";
        svg_extrainfo += "\n<text class='text_c' x='"+(r3+font_size/3)+"' y='"+(r3y+line_height*0.8)+"'>"+round(release_end_time)+" ms, level: "+round(rel_end_level)+" dB, GR: "+round(rel_gr)+" dB</text>";
    }

    // more info
    double running_time = round((round(ms_from_start)/1000));
    string running_time_hms = getHMS();

    double ex_x = floor(x_max - font_size*13);
    double ts_y = line_height*2;
    double fr_y = ts_y + font_size*1.4;
    double res_y = fr_y + font_size*1.4;
    svg_extrainfo += "\n\n<!-- Extra info -->\n<text class='text_d' x='"+(ex_x)+"' y='"+ts_y+"'>Timestamp: " + running_time_hms + "</text>";
    svg_extrainfo += "\n<text class='text_d' x='"+(ex_x)+"' y='"+fr_y+"'>Frequency: "+frequency+" Hz</text>\n";
    // svg_extrainfo += "\n<text class='text_d' x='"+(ex_x)+"' y='"+res_y+"'>Resolution: "+round(precision_ms, 3)+" ms</text>\n";

    // draw time grid
    double time_divs = (x_max-(snapshot_padding_ms))/1000;
    int time_subdivs = 10; // divide 1 second into subdivs
    int msgrid_coef = 1;
    int msgrid_subdiv_coef = 2; // how often to put text "x00ms"
    if (arcycle_len_ms > 1800) msgrid_subdiv_coef = 5;
    if (arcycle_len_ms > 7000) {
        msgrid_subdiv_coef = 10;
    }
    if (arcycle_len_ms > 11000) {
        time_subdivs = 2;
        msgrid_coef = 2;
    }
    if (arcycle_len_ms > 20000) {
        msgrid_coef = 4;
    }
    double x_pos = svg_ms_att_start;
    double ms_text = 0;
    string classchar = "a";
    string svg_grid = "<!-- Grid lines -->\n";
    for( int n = 0; n < time_divs; n++ ) {
        for( int m = 0; m < time_subdivs; m++ ) {
            classchar = "a";
            if (m>0) classchar = "c";
            if (m == 5) classchar = "b";
            svg_grid += "<line class=\"gt"+classchar+"\" x1=\""+x_pos+"\" x2=\""+x_pos+"\" y1=\"0\" y2=\""+y_max+"\" />\n";
            if (((m % msgrid_subdiv_coef) == 0) && ((n % msgrid_coef) == 0)) {
                svg_grid += "<text class='gridtext' x=\""+(x_pos+font_size*0.4)+"\" y=\""+(y_max-line_height*0.4)+"\">"+ms_text+" ms</text>\n";
            }
            x_pos += roundDoubleToInt(1000/time_subdivs);
            ms_text += roundDoubleToInt(1000/time_subdivs);
            if (x_pos >= x_max) {
                break;
            }
        }
    }

    // draw db grid
    double db_divs = (floor(-FLOOR_LEVEL/10)*2);
    double db_text = 0;
    double grid_y = 0;
    for( int n = 1; n <= db_divs+1; n++ ) {
        if (!((n==1))) { // skip first line if ==0
            classchar = "b";
            if ((n % 2) != 0) {
                classchar = "a";
                svg_grid += "<text class='gridtext' x=\""+(x_max - font_size*3.5)+"\" y=\""+(grid_y - font_size/2)+"\">"+db_text+" dB</text>\n";
            }
            svg_grid += "<line class=\"gd"+classchar+"\" x1=\""+0+"\" x2=\""+x_max+"\" y1=\""+grid_y+"\" y2=\""+grid_y+"\" />\n";
            }
        grid_y += 5*SVG_Y_COEFF;
        db_text -= 5;
    }



    // preparing hires RAW data
    if (is_hires_on) {
        
        // link to buffer
        array<double>@ lev_prec_buf=levels_precise[old_levprec_buf];
        uint lplen = lev_prec_buf.length;

        // init variables for hires saving
        string raw_hr_data = "",
            hr_att_curve_points = "", hr_rel_curve_points = "",
            svg_hr_head, svg_hr_style, svg_hr_grid, svg_hr_extrainfo, 
            svg_hr_att_curve, svg_hr_att_title, svg_hr_rel_curve, svg_hr_rel_title, 
            hr_running_time_hms;

        string hr_rel_title = "Hi-res release", hr_att_title = "Hi-res attack";
        hr_rel_title = "";
        hr_att_title = "";

        double hr_x_max, hr_y_max, HIRES_SVG_Y_COEFF;
        double hr_font_size, hr_line_height, hr_running_time;
        
        bool newmsdata = false;
        double rd_gr = 0, hr_x = 0;
        double msdelta = precision_smp_int*sample_in_ms; // precision in ms
        double ntosearchfor;
        int nfrom, nto;

        //////////////////////////////////
        // preparing HIRES ATTACK DATA
        //////////////////////////////////

        if (hr_att_len_ms > 0) {
            // taking data from attack start - 5 ms
            // double att_ms_start = ms_from_start - ms_to_save_in_snapshot + snapshot_padding_ms;
            double att_ms_start = floor(last_att_start_time);
            double hr_rd_att_ms_pre = 3; // how many ms pre-attack include
            double hr_rd_att_ms_post = 3; 
            double svg_hr_att_ms_pre = 2;
            double svg_hr_att_ms_post = 2;

            // determine which part of data to take
            nfrom = lev_prec_buf.find(att_ms_start - ceil(hr_rd_att_ms_pre));
            if (nfrom < 0) { nfrom = 0; }

            ntosearchfor = att_ms_start + hr_att_len_ms + hr_rd_att_ms_post;
            nto = lev_prec_buf.find(ntosearchfor);   
            if (nto < 0) { nto = lev_prec_buf.length-1; }
            // if (nto >= int(lev_prec_buf.length())) { nto = int(lev_prec_buf.length()); }

            // if found points from -> to
            if (nto > nfrom) {
                // hires graph saving params
                hr_x_max = svg_hr_att_ms_pre + hr_att_len_ms + svg_hr_att_ms_post;
                // if (hr_x_max > hires_length_ms + hr_rd_att_ms_pre) hr_x_max = hires_length_ms + hr_rd_att_ms_pre;
                hr_y_max = hr_x_max*0.45;
                HIRES_SVG_Y_COEFF = hr_x_max/100;
                hr_font_size = 0.01384615385*hr_x_max;
                hr_line_height = ceil(hr_font_size*1.4);

                svg_hr_head = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" +
                "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 " + hr_x_max + " " + hr_y_max + "\" font-size='"+hr_font_size+"px' preserveAspectRatio=\"none\" >\n";

                float val;
                
                raw_hr_data = "ATTACK DATA;\n\nSampleRate:;"+sampleRate+"\nFrequency:;"+frequency+"\nResolution:;"+precision_smp_int+" smp;"+msdelta+" ms;\n\nms;level (dB);att ms;GR (dB)\n";
                double last_ms = 0;
                double last_ms_printed = 0;
                double cur_prec_ms = nfrom;
                int state = -2;
                double rd_att_start_time = 0;
                double rd_att_phase_time = 0;
                // double substrms = floor(att_ms_start/1000)*1000;
                
                // print("precision_smp_int: " + precision_smp_int + " msdelta: " + msdelta + " precision_ms: " +precision_ms);
                double att_delta_ms = last_att_start_time; // relative ms
                for( int n = nfrom; n < nto; n++ ) {
                    val = lev_prec_buf[n];
                    if (val > 60) { // if it's ms value
                        cur_prec_ms = val;
                        newmsdata = true;
                        continue;
                    } else {
                        if (save_raw_data) { 
                            if (newmsdata) {
                                newmsdata = false;
                                if (fraction(cur_prec_ms) != 0) {
                                    // must be attack start
                                    state = -1;
                                    raw_hr_data += trim(formatFloat(cur_prec_ms,"l",16,2))+";"; 
                                } else {
                                   raw_hr_data += trim(formatFloat(cur_prec_ms,"l",16,1))+";"; 
                                }
                            } else {
                                raw_hr_data += " ;"; 
                            }
                            raw_hr_data += formatFloat(val,"",6,3);
                            }
                        hr_x = (cur_prec_ms-att_delta_ms+svg_hr_att_ms_pre);
                        if ((hr_x >=0) && (hr_x<=hr_x_max)) {
                            hr_att_curve_points += hr_x +","+round(-val*HIRES_SVG_Y_COEFF, 3)+" ";
                        }
                    }

                    if (save_raw_data) { 
                        if (state == -1) {
                            raw_hr_data += ";   0.0;"+formatFloat(last_max_level-val, "", 7, 3)+";ATTACK START TIME";
                            state = 0;
                            rd_att_start_time = cur_prec_ms;
                            // print("last_max_level: " + last_max_level);
                        } else if (state >= -1) {
                            rd_att_phase_time = round(cur_prec_ms-rd_att_start_time, 2);
                            raw_hr_data += ";"+formatFloat(rd_att_phase_time,"",7,2);
                            //if (fraction(rd_att_phase_time) == 0) raw_hr_data +=".0";
                            rd_gr = last_max_level - val;
                            raw_hr_data += ";"+formatFloat(rd_gr, "", 7, 3);
                            // if (fraction(rd_gr) == 0) raw_hr_data +=".0";

                            if ((state == 1) && (last_att_measure_level >= val)) {
                                raw_hr_data += ";ATTACK MEASURED;"+round(last_att_time, 3)+" ms;"+"ATT MEASURE LEVEL:;"+round(last_att_measure_level)+" dB;("+(last_att_coeff*100)+"% of "+gr_type_text+" GR "+round(gr)+" dB)";
                                state = 2;
                                // print("last_att_gr: " + last_att_gr);
                            }
                            if ((state == 2) && (abs(rd_gr-last_att_gr))<0.0005) {
                                raw_hr_data += ";ATT GR REACHED;"+last_att_gr+" dB";
                                state = 3;

                            } 
                        }

                        // attack started, now find max level
                        if (state == 0) {
                            if (lock_max_level) {
                                raw_hr_data += ";MAX LEVEL (LOCKED):;" +last_max_level + " dB";
                                raw_hr_data += "; (TRUE): " +last_true_max_level + " dB";
                                state = 1;
                            } else if (abs(val-last_max_level)<0.01) {
                                raw_hr_data += ";MAX LEVEL:;" +last_max_level + " dB";
                                state = 1;
                            }
                        }


                        raw_hr_data+="\n";
                    }
                    cur_prec_ms += msdelta;
                }

                raw_hr_att_data = raw_hr_data;
                raw_hr_data.resize(0);


                // preparing hires ATTACK graph

                // draw time grid
                double hr_time_divs = ceil(hr_x_max/10);
                int hr_time_subdivs = 10; // divide 10 ms into subdivs
                int hr_msgrid_coef = 1;
                if (hr_x_max > 30) hr_time_subdivs = 2;
                if (hr_x_max > 120) { hr_msgrid_coef = 2; }
                if (hr_x_max > 200) { hr_msgrid_coef = 4; }
                int hr_msgrid_subdiv_coef = 5; // how often to put text "x ms"
                double hr_x_pos = svg_hr_att_ms_pre;
                double hr_ms_text = 0;
                string hr_classchar = "a";
                svg_hr_grid += "\n<!-- Grid lines -->\n";
                for( int n = 0; n < hr_time_divs; n++ ) {
                    for( int m = 0; m < hr_time_subdivs; m++ ) {
                        hr_classchar = "a";
                        if (m > 0) hr_classchar = "c";
                        if (m == 5) hr_classchar = "b";
                        svg_hr_grid += "<line class=\"gt"+hr_classchar+"\" x1=\""+hr_x_pos+"\" x2=\""+hr_x_pos+"\" y1=\"0\" y2=\""+hr_y_max+"\" />\n";
                        if (((m % hr_msgrid_subdiv_coef) == 0) && ((n % hr_msgrid_coef) == 0)) {
                            svg_hr_grid += "<text class='gridtext' x=\""+(hr_x_pos+hr_font_size*0.4)+"\" y=\""+(hr_y_max-hr_font_size)+"\">"+hr_ms_text+" ms</text>\n";
                        }
                        hr_ms_text += 10/hr_time_subdivs;
                        hr_x_pos += 10/hr_time_subdivs;
                        if (hr_x_pos >= hr_x_max) {
                            break;
                        }
                    }
                }

                // draw db grid
                db_divs = (floor(-FLOOR_LEVEL/10)*2);
                db_text = 0;
                grid_y = 0;
                for( int n = 1; n <= db_divs+1; n++ ) {
                    if (!((n==1))) { // skip first line
                        classchar = "b";
                        if ((n % 2) != 0) {
                            classchar = "a";
                            svg_hr_grid += "<text class='gridtext' x=\""+(hr_x_max - hr_font_size*3.5)+"\" y=\""+(grid_y - hr_font_size/2)+"\">"+db_text+" dB</text>\n";
                        }
                        svg_hr_grid += "<line class=\"gd"+classchar+"\" x1=\""+0+"\" x2=\""+hr_x_max+"\" y1=\""+grid_y+"\" y2=\""+grid_y+"\" />\n";
                        }
                    grid_y += 5*HIRES_SVG_Y_COEFF;
                    db_text -= 5;
                }


                svg_hr_att_title = "\n<!-- The title -->\n<text class='title' x='"+ceil(hr_x_max/2)+"' y='"+(hr_font_size*5)+"'>"+hr_att_title+"</text>\n";

                hr_running_time = round((round(ms_from_start)/1000));
                hr_running_time_hms = getHMS();
                ex_x = floor(hr_x_max - hr_font_size*9.5);
                ts_y = hr_font_size*2;
                fr_y = ts_y + hr_font_size*1.2;
                res_y = fr_y + hr_font_size*1.2;
                svg_hr_extrainfo = "\n<!-- Extra info -->\n<text class='text_d' x='"+(ex_x)+"' y='"+ts_y+"'>Timestamp: " + hr_running_time_hms + "</text>";
                svg_hr_extrainfo += "\n<text class='text_d' x='"+(ex_x)+"' y='"+fr_y+"'>Frequency: "+frequency+" Hz</text>";
                svg_hr_extrainfo += "\n<text class='text_d' x='"+(ex_x)+"' y='"+res_y+"'>Resolution: "+round(precision_ms, 3)+" ms</text>\n";

                if (current_saving_mode == SNAPSHOT_MODE_REFERENCE) {
                    string svg_hr_att_ref_curve = "\n<!-- Reference curve -->\n<polyline class=\"curve_ref\" style=\"opacity: 0.5\" points=\"" + hr_att_curve_points + "\" />\n";
                    svg_hr_att_ref_data = svg_hr_head + svg_style + svg_hr_att_ref_curve + svg_end;
                } else {
                    svg_hr_att_curve = "\n<!-- Level curve -->\n<polyline class=\"curve\" points=\"" + hr_att_curve_points + "\" />\n";
                    svg_hr_att_data = svg_hr_head + svg_style + svg_bg_rect + svg_hr_att_title + svg_hr_grid + svg_hr_extrainfo + svg_hr_att_curve + svg_end;
                }

                //print("Precise raw data saved");
            } else {
                saveResult = -1; // we need to re-save, cause not enough data yet
                //print("Attack precision data lookup not successful");
            }
        }




        //////////////////////////////////
        // PREPARING HIRES RELEASE DATA
        //////////////////////////////////

        if (hr_rel_len_ms > 0) {
            double rel_ms_start = floor(release_start_time);
            double hr_rd_rel_ms_pre = 3; // how many ms of pre-data include
            double hr_rd_rel_ms_post = 3; 
            double svg_hr_rel_ms_pre = 2;
            double svg_hr_rel_ms_post = 2;

            // determine which part of data to take
            nfrom = lev_prec_buf.find(rel_ms_start - ceil(hr_rd_rel_ms_pre));
            if (nfrom < 0) { nfrom = 0; }

            ntosearchfor = rel_ms_start + hr_rel_len_ms + hr_rd_rel_ms_post;
            nto = lev_prec_buf.find(ntosearchfor);   
            if (nto < 0) { nto = lev_prec_buf.length-1; }
            // if (nto >= int(lev_prec_buf.length())) { nto = int(lev_prec_buf.length()); }

            if (nto > nfrom) {
                // hires graph saving params
                hr_x_max = svg_hr_rel_ms_pre + hr_rel_len_ms + svg_hr_rel_ms_post;
                // if (hr_x_max > hr_graph_max_rellenms + hr_rd_rel_ms_pre) hr_x_max = hr_graph_max_rellenms + hr_rd_rel_ms_pre;
                hr_y_max = hr_x_max*0.45;
                HIRES_SVG_Y_COEFF = hr_x_max/100;
                hr_font_size = 0.01384615385*hr_x_max;
                hr_line_height = ceil(hr_font_size*1.4);

                svg_hr_head = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" +
                "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 " + hr_x_max + " " + hr_y_max + "\" font-size='"+hr_font_size+"px'>\n";


                float val;
                if (save_raw_data) {  raw_hr_data = "RELEASE DATA;\n\nSampleRate:;"+sampleRate+"\nFrequency:;"+frequency+"\nResolution:;"+precision_smp_int+" smp;"+msdelta+" ms;\n\nms;level (dB);rel ms;GR (dB)\n"; }
                double last_ms = 0;
                double last_ms_printed = 0;
                double cur_prec_ms = nfrom;
                int state = -2;
                double rd_rel_start_time = 0;
                double rd_rel_phase_time = 0;
                newmsdata = false;
                rd_gr = 0;
                double rel_delta_ms = release_start_time; // relative ms

                for( int n = nfrom; n < nto; n++ ) {
                    val = lev_prec_buf[n];
                    if (val > 60) { // if it's ms value
                        cur_prec_ms = val;
                        newmsdata = true;
                        continue;
                    } else {
                        if (save_raw_data) { 
                            if (newmsdata) {
                                newmsdata = false;
                                if (fraction(cur_prec_ms) != 0) {
                                    // must be release start
                                    state = -1;
                                    raw_hr_data += trim(formatFloat(cur_prec_ms,"l",16,2))+";";
                                } else {
                                    raw_hr_data += trim(formatFloat(cur_prec_ms,"l",16,1))+";";
                                }
                            } else {
                                raw_hr_data += " ;";
                            }
                        raw_hr_data += formatFloat(val,"",6,3);
                        }
                        hr_x = (cur_prec_ms-rel_delta_ms+svg_hr_rel_ms_pre);
                        if ((hr_x >=0) && (hr_x<=hr_x_max)) {
                            hr_rel_curve_points += hr_x +","+round(-val*HIRES_SVG_Y_COEFF, 3)+" ";
                        }
                    }

                    if (save_raw_data) { 
                        if (state == -1) {
                            raw_hr_data += ";   0.0;"+round(rel_end_level-val, 3)+";RELEASE START TIME";
                            state = 0;
                            rd_rel_start_time = cur_prec_ms;
                            // print("last_max_level: " + last_max_level);
                        } else if (state >= -1) {
                            rd_rel_phase_time = round(cur_prec_ms-rd_rel_start_time, 2);
                            raw_hr_data += ";"+formatFloat(rd_rel_phase_time,"",7,2);
                            //if (fraction(rd_att_phase_time) == 0) raw_hr_data +=".0";
                            rd_gr = rel_end_level-val;
                            raw_hr_data += ";"+formatFloat(rd_gr, "", 7, 3);

                            if ((state == 1) && (rel_measure_level <= val)) {
                                raw_hr_data += ";RELEASE MEASURED;"+round(release_time, 3)+" ms;"+"REL MEASURE LEVEL:;"+round(rel_measure_level)+" dB;("+(rel_coeff*100)+"% of "+gr_type_text+" GR "+round(gr)+" dB)";
                                state = 2;
                                // print("last_att_gr: " + last_att_gr);
                            }
                            if ((state == 2) && (abs(rel_end_level-val))<0.0005) {
                                raw_hr_data += ";REL END REACHED;";
                                state = 3;

                            } 
                        }

                        // release started, now find min level
                        if (state == 0) {
                            if (abs(val-min_level)<0.01) {
                                raw_hr_data += ";MIN LEVEL:;" +min_level + " dB";
                                state = 1;
                            }
                        }

                        raw_hr_data+="\n";
                    }
                    cur_prec_ms += msdelta;
                }

                raw_hr_rel_data = raw_hr_data;


                // preparing hires RELEASE graph

                // draw time grid
                svg_hr_grid = "";
                double hr_time_divs = ceil(hr_x_max/10);
                int hr_time_subdivs = 10; // divide 10 ms into subdivs
                int hr_msgrid_coef = 1;
                if (hr_x_max > 30) { hr_time_subdivs = 2; }
                if (hr_x_max > 120) { hr_msgrid_coef = 2; }
                if (hr_x_max > 200) { hr_msgrid_coef = 4; }
                if (hr_x_max > 300) { hr_time_subdivs = 1; }
                int hr_msgrid_subdiv_coef = 5; // how often to put text "x ms"
                double hr_x_pos = svg_hr_rel_ms_pre;
                double hr_ms_text = 0;
                string hr_classchar = "a";
                svg_hr_grid += "<!-- Grid lines -->\n";
                for( int n = 0; n < hr_time_divs; n++ ) {
                    for( int m = 0; m < hr_time_subdivs; m++ ) {
                        hr_classchar = "a";
                        if (m > 0) hr_classchar = "c";
                        if (m == 5) hr_classchar = "b";
                        svg_hr_grid += "<line class=\"gt"+hr_classchar+"\" x1=\""+hr_x_pos+"\" x2=\""+hr_x_pos+"\" y1=\"0\" y2=\""+hr_y_max+"\" />\n";
                        if (((m % hr_msgrid_subdiv_coef) == 0) && ((n % hr_msgrid_coef) == 0)) {
                            svg_hr_grid += "<text class='gridtext' x=\""+(hr_x_pos+hr_font_size*0.4)+"\" y=\""+(hr_y_max-hr_font_size)+"\">"+hr_ms_text+" ms</text>\n";
                        }
                        hr_ms_text += 10/hr_time_subdivs;
                        hr_x_pos += 10/hr_time_subdivs;
                        if (hr_x_pos >= hr_x_max) {
                            break;
                        }
                    }
                }

                // draw db grid
                db_divs = (floor(-FLOOR_LEVEL/10)*2);
                db_text = 0;
                grid_y = 0;
                for( int n = 1; n <= db_divs+1; n++ ) {
                    if (!((n==1))) { // skip first line
                        classchar = "b";
                        if ((n % 2) != 0) {
                            classchar = "a";
                            svg_hr_grid += "<text class='gridtext' x=\""+(hr_x_max - hr_font_size*3.5)+"\" y=\""+(grid_y - hr_font_size/2)+"\">"+db_text+" dB</text>\n";
                        }
                        svg_hr_grid += "<line class=\"gd"+classchar+"\" x1=\""+0+"\" x2=\""+hr_x_max+"\" y1=\""+grid_y+"\" y2=\""+grid_y+"\" />\n";
                        }
                    grid_y += 5*HIRES_SVG_Y_COEFF;
                    db_text -= 5;
                }

                svg_hr_rel_title = "\n<!-- The title -->\n<text class='title' x='"+ceil(hr_x_max/2)+"' y='"+(hr_font_size*5)+"'>"+hr_rel_title+"</text>\n\n";

                hr_running_time = round((round(ms_from_start)/1000));
                hr_running_time_hms = getHMS();
                ex_x = floor(hr_x_max - hr_font_size*9.5);
                ts_y = hr_font_size*2;
                fr_y = ts_y + hr_font_size*1.2;
                res_y = fr_y + hr_font_size*1.2;
                svg_hr_extrainfo = "\n<!-- Extra info -->\n<text class='text_d' x='"+(ex_x)+"' y='"+ts_y+"'>Timestamp: " + hr_running_time_hms + "</text>";
                svg_hr_extrainfo += "\n<text class='text_d' x='"+(ex_x)+"' y='"+fr_y+"'>Frequency: "+frequency+" Hz</text>";
                svg_hr_extrainfo += "\n<text class='text_d' x='"+(ex_x)+"' y='"+res_y+"'>Resolution: "+round(precision_ms, 3)+" ms</text>\n";


                if (current_saving_mode == SNAPSHOT_MODE_REFERENCE) {
                    string svg_hr_rel_ref_curve = "\n<!-- Reference curve -->\n<polyline class=\"curve_ref\" style=\"opacity: 0.5\" points=\"" + hr_rel_curve_points + "\" />\n";
                    svg_hr_rel_ref_data = svg_hr_head + svg_style + svg_hr_rel_ref_curve + svg_end;
                } else {
                    svg_hr_rel_curve = "\n<!-- Level curve -->\n<polyline class=\"curve\" points=\"" + hr_rel_curve_points + "\" />\n";
                    svg_hr_rel_data = svg_hr_head + svg_style + svg_bg_rect + svg_hr_rel_title + svg_hr_grid + svg_hr_extrainfo + svg_hr_rel_curve + svg_end;
                }

                
                /*
                if(f.open(snapARDir+"hires_release.svg","w")>=0) {
                    f.writeString(svg_hr_head + svg_style + hires_rect + svg_hr_grid + svg_hr_title + hires_curve + svg_end); f.close();
                }
                */

            } else {
                // failed to find release data
            }

        } // end if hr_rel_len_ms > 0

    } // end if is_hires_on

    // creating curve data from live (or just generated) points
    if (use_live_points) {
        svg_curve = "\n<!-- Level curve (live) -->\n<polyline class=\"curve\" points=\"" + full_live_curve_points + "\" />";
    } else {
        svg_curve = "\n<!-- Level curve -->\n<polyline class=\"curve\" points=\"" + svg_curve_points + "\" />";
    }

    // normal and live modes
    if ((current_saving_mode == SNAPSHOT_MODE_NORMAL) 
        || (current_saving_mode == SNAPSHOT_MODE_NORMAL_WITH_RAWDATA) 
        || (current_saving_mode == SNAPSHOT_MODE_LIVE)) {

        if(f.open(snapARDir + "attack_release.svg", "w")>=0) {
            string snapShotTitle = "\n<!-- The title -->\n<text class='title' x='"+ceil(x_max/2)+"' y='"+(line_height*3)+"'>Compressor snapshot</text>\n\n";
            f.writeString(svg_head_proportional + svg_style + svg_bg_rect + snapShotTitle + svg_grid + 
                svg_curve + svg_attrel + svg_extrainfo + svg_end); 
            f.close();
        }

        if(f.open(snapARDir + "background.svg", "w")>=0) {
            f.writeString(svg_head + svg_style + svg_bg_rect + svg_grid + svg_end); 
            f.close();
        }

        if(f.open(snapARDir + "curve.svg", "w")>=0) {
            f.writeString(svg_head + svg_style + svg_curve + svg_end); 
            f.close();
        }

        if(f.open(snapARDir + "info_a.svg", "w")>=0) {
            f.writeString(svg_head + svg_style + svg_attrel + svg_end); 
            f.close();
        }

        if(f.open(snapARDir + "info_b.svg", "w")>=0) {
            f.writeString(svg_head + svg_style + svg_extrainfo + svg_end); 
            f.close();
        }

        if (is_hires_on) {
            // saving hires attack & release graphs
            if (hr_att_len_ms > 0) {
                // saving attack hires graph
                if(f.open(snapARDir+"hires_attack.svg","w")>=0) {
                    f.writeString(svg_hr_att_data); f.close();
                }
            } else {
                // erasing attack hires graph
                if(f.open(snapARDir+"hires_attack.svg","w")>=0) { f.writeString(""); f.close(); }
            }
            if (hr_rel_len_ms > 0) {
                // saving release hires graph
                if(f.open(snapARDir+"hires_release.svg","w")>=0) {
                    f.writeString(svg_hr_rel_data); f.close();
                }
            } else {
                // erasing release hires graph
                if(f.open(snapARDir+"hires_release.svg","w")>=0) { f.writeString(""); f.close(); }
            }
        } else {
            // erasing hires data
            if(f.open(snapARDir+"hires_attack.svg","w")>=0) { f.writeString(""); f.close(); }
            if(f.open(snapARDir+"hires_release.svg","w")>=0) { f.writeString(""); f.close(); }
        }

        // if we need to save raw data
        if (save_raw_data) {
            if(f.open(snapARDir + "raw_data.csv", "w")>=0) {
               f.writeString(raw_data); 
               f.close();
            }

            if (is_hires_on) {
                if (hr_att_len_ms > 0) {
                    // saving hires attack & release raw data
                    if(f.open(snapARDir+"hires_attack_data.csv","w")>=0) {
                        f.writeString(raw_hr_att_data); f.close();
                    }
                }
                if (hr_rel_len_ms > 0) {
                    if(f.open(snapARDir+"hires_release_data.csv","w")>=0) {
                        f.writeString(raw_hr_rel_data); f.close();
                    }
                }
            }
        }

    } else {
        // reference save or remove
        if(f.open(snapARDir + "curve_ref.svg","w")>=0) {
            if (current_saving_mode == SNAPSHOT_MODE_REMOVE_REF) {
                // removing reference
                f.writeString(""); 
            } else {
                // saving reference
                double reftx = line_height;

                string svg_ref = "<!-- Reference data -->\n<text y='"+(y_max-SVG_Y_COEFF*8)+"' class='text_ref'>\n<tspan x='"+reftx+"'>Ref attack: "+round(last_att_time)+" ms</tspan>"+
                "\n<tspan x='"+reftx+"' dy='"+line_height+"'>Ref release: "+round(release_time)+" ms</tspan>"+
                "\n<tspan x='"+reftx+"' dy='"+line_height+"'>Ref max GR: "+round(gr)+" dB</tspan>\n</text>"+
                "\n\n<!-- Reference curve-->\n<polyline class=\"curve_ref\" points=\"" + svg_curve_points + "\" vector-effect=\"non-scaling-stroke\" />";

                f.writeString(svg_head + svg_style + svg_ref + svg_end); 
            }
            f.close();
        }

        // HI-RES references
        
        // saving attack hires reference
        if ((hr_att_len_ms > 0) || (current_saving_mode == SNAPSHOT_MODE_REMOVE_REF)) {
            if(f.open(snapARDir+"hires_attack_ref.svg","w")>=0) {
                if (current_saving_mode != SNAPSHOT_MODE_REMOVE_REF) {
                    f.writeString(svg_hr_att_ref_data); 
                } else {
                    f.writeString(""); 
                }
                f.close();
            }
        }

        // saving release hires reference
        if ((hr_rel_len_ms > 0) || (current_saving_mode == SNAPSHOT_MODE_REMOVE_REF)) {
            if(f.open(snapARDir+"hires_release_ref.svg","w")>=0) {
                if (current_saving_mode != SNAPSHOT_MODE_REMOVE_REF) {
                    f.writeString(svg_hr_rel_ref_data); 
                } else {
                    f.writeString(""); 
                }
                f.close();
            }
        }
    }

    return saveResult;
}

////////////////////////////////////////
// DEBUG FUNCTIONS 
////////////////////////////////////////

void debugSaveStart(){
    if (production_mode) return; // debug disabled in production mode

    debug_data = "";
    print("Debug started " + ms_from_start);
    debug_data += "Frequency: " + frequency + "\n";
        debug_data += "Full period samples: " + freq_fullperiod_samples + "\n";
    if (use_window_detector) {
        debug_data += "Detector: window" + "\n";
    } else {
        debug_data += "Detector: peak" + "\n";
    }
    debug_data += "\n";
}

int debug_old_ms_from_start;

void debugAddLine(){
    if (production_mode) return; // debug disabled in production mode

    string ms_from_start_text, signal_level_txt;

    ms_from_start_text = "";
    if (debug_old_ms_from_start != ms_from_start) {
        ms_from_start_text = ms_from_start;
        debug_old_ms_from_start = ms_from_start;
    } 

    signal_level_txt = "";
    if (old_signal_level != signal_level) {
        signal_level_txt = signal_level;
        old_signal_level = signal_level;
        debug_signal_level_the_same = 0;
    } else {
        debug_signal_level_the_same++;
        if (debug_signal_level_the_same > 9) {
            debug_signal_level_the_same=0;
            signal_level_txt += "("+signal_level+")";
        }
    }

    debug_data += ms_from_start_text+";"+samples_from_start;
    debug_data += ";"+signal_level_txt+";"+signal_value+";"+signal_state; // orig_value;

    double momentary_level = round(20.0*log10(signal_abs_value), 3);
    string momentary_level_txt = "";
    if (momentary_level > -120) momentary_level_txt = momentary_level;
    //debug_data += ";"+momentary_level_txt;

    debug_data += ";"+current_state;
    debug_data += ";"+samples_since_attack_start;

    //debug_data += ";"+samples_since_level_state_change+";"+level_state;
    // string signal_sign_changed_txt = "";
    // if (signal_sign_changed) signal_sign_changed_txt = "changed "+signal_sign;
    // debug_data += ";"+signal_sign_changed_txt+";"+signal_state;
    // debug_data += ";"+samples_since_last_level_measure;
    debug_data += "\n";
}

void debugSaveEnd(){
    if (production_mode) return; // debug disabled in production mode
    print("Debug saved " + ms_from_start);
    file f;
    if(f.open(curDir+"debug_ar.csv","w")>=0) {
        f.writeString(debug_data); f.close();
    }
}

double lev_min, lev_max, last_max_sample, last_min_sample, distance;
void debugMinMax(){
    double this_distance = distance;
    if (signal_level >= lev_max) {
        lev_max = signal_level;
        last_max_sample = samples_from_start;
        this_distance = last_max_sample - last_min_sample;
    }
    if (signal_level <= lev_min) {
        lev_min = signal_level;
        last_min_sample = samples_from_start;
        this_distance = last_min_sample - last_max_sample;
    }
    if (this_distance > distance) {
         distance = this_distance;
    }
}

void debugFreq(){
    double diff = lev_max - lev_min;
    string goodfreq = "";
    if (is_good_hi_freq) {
        goodfreq = "[GOOD]";
    }
    print("freq " + goodfreq + ": "+ frequency + " diff: " + diff + " (" + lev_max + " .. " + lev_min + ") distance: "+distance + " period calc: "+freq_fullperiod_smp_calc+" smp");
    lev_min = 0;
    lev_max = -120;
    distance = 0;
}