//////////////////////////////////////
// CMT Threshold Ratio Measuring tool
// written by Ilya Orlov (LetiMix) 
// https://www.youtube.com/channel/UCbS0EpAZytfizVvysUluEHQ
// St. Petersburg, Russia, october 2018
//////////////////////////////////////
// This plugin is free.
//////////////////////////////////////
// If you need support, bugfixes, new features,
// or if you're just feeling grateful,
// please consider a donation:
// PayPal: ilya_orlov@mail.ru
// Thank you!
//////////////////////////////////////

string name="Threshold Ratio Measuring Tool"; 
string description="Version 1.07 (2018-12-14)"; 
string author="Ilya Orlov (LetiMix)";

///////////////////////////
// HI and LOW working margins (in dB)
const double CEIL_LEVEL = 0;
double FLOOR_LEVEL = -60; // can change to 100

// value which is considered as "silence"
double SILENCE_LEVEL = -112; // in dB, signals below that are considered silence
double SILENCE_LEVEL_COMP = -112; // for compressed signal signals below that are considered silence
double SILENCE_VALUE, SILENCE_VALUE_COMP, FLOOR_VALUE; // the same value =~ 0.00000238; calc in initialize

///////////////////////////
// INPUT PARAMETERS
const int IP_MUTE_OUTPUT = 0;
const int IP_PAUSE = 1;
const int IP_THRESHOLD = 2;
const int IP_AUTO_THRESHOLD = 3;
const int IP_AUTO_GAIN_OFFSET = 4;
const int IP_CURVE_TYPE = 5;
const int IP_GAIN_OFFSET_MANUAL = 6;
const int IP_RATIO_MEASURE_POINT = 7;
const int IP_AUTO_RATIO = 8;
const int IP_SHOW_EXTRA_INFO = 9;
const int IP_INPUT_CHANNEL_COMPRESSED = 10;
const int IP_INPUT_CHANNEL_TESTTONE = 11;
const int IP_SAVE_SNAPSHOT = 12;
const int IP_SAVE_SNAPSHOT_MODE = 13;
const int IP_RANGE = 14;
const int IP_SAVE_DEBUG = 15;

array<string> inputParametersNames = { "Mute Output", "Pause", "Threshold", 
"Auto Threshold", "Enable Manual Gain Offset", "Curve type", "Man Gain Offset value",
"Ratio Measuring Point", "Auto Ratio", "Show Extra Info", 
"Input (compressed)", "Input (test tone)", "Save snapshot", "Save snapshot mode",
"Range", "Save debug" }; 
array<string> inputParametersUnits = {"", "", "dB", 
"", "", "", "dB", 
"dB", "", "",
"", "", "", "",
"", "" }; 
array<double> inputParameters(inputParametersNames.length); 
array<double> inputParametersMin = {0, 0, FLOOR_LEVEL, 
0, 0, 0, -30, 
FLOOR_LEVEL, 0, 0,
0, 0, SAVE_OFF, SNAPSHOT_MODE_NORMAL,
0, 0 }; 
array<double> inputParametersMax = {1, 1, CEIL_LEVEL, 
1, 1, 1, 30,
CEIL_LEVEL, 1, 1, 
2, 2, SAVE_SNAP_REF_REMOVE, SNAPSHOT_MODE_LIVE,
1, 1 }; 
array<double> inputParametersDefault = {1, 0, FLOOR_LEVEL/2, 
1, 0, 0, 0,
CEIL_LEVEL-5, 1, 0, 
0, 0, SAVE_OFF, SNAPSHOT_MODE_NORMAL,
0, 0 }; 
array<string> inputParametersEnums = {"mute output;muted", "", "",
"", "(auto):;(manual):", "realtime;settled", "",
"", "", "", 
"Left;Right;Stereo", "Ch. 3;Right;Sidechain Left", "--;save snapshot;save reference true;save reference compensated;remove reference", "normal;with raw data;live mode",
"60 dB;100 dB", ";saving..."};
array<string> inputParametersFormats = {"", "", "", 
"", "", "", "0.2",
"0.0", "", "", 
"", "", "", "",
"", "" };

// modes for snapshot & reference saving
const int SNAPSHOT_MODE_NORMAL = 0;
const int SNAPSHOT_MODE_NORMAL_WITH_RAWDATA = 1;
const int SNAPSHOT_MODE_LIVE = 2;
const int SNAPSHOT_MODE_REF_TRUE = 3;
const int SNAPSHOT_MODE_REF_COMP = 4;
const int SNAPSHOT_MODE_REMOVE_REF = 5;

// snapshot saving buttons
const int SAVE_OFF = 0;
const int SAVE_SNAP = 1;
const int SAVE_SNAP_REF_TRUE = 2;
const int SAVE_SNAP_REF_COMP = 3;
const int SAVE_SNAP_REF_REMOVE = 4;


///////////////////////////
// INPUT STRINGS (if we need some)
//array<string> inputStrings(2);
//array<string> inputStringsNames={"S1","S2"};

///////////////////////////
// OUTPUT PARAMETERS
const int OP_COMP_LEVEL = 0;
const int OP_ORIG_LEVEL = 1;
const int OP_FREQ = 2;
const int OP_STEP_DURATION = 3;
const int OP_ERR_MSG = 4;
const int OP_GAIN_OFFSET_CALCULATED = 5;
const int OP_THRESHOLD = 6;
const int OP_RATIO_CURRENT = 7;
const int OP_RATIO_CALC_MIN = 8;
const int OP_RATIO_CALC_MAX = 9;
const int OP_RATIO = 10;
const int OP_CURRENT_STEP = 11;
const int OP_COMP_LEVEL_REAL = 12;
const int OP_ORIG_LEVEL_REAL = 13;
const int OP_SNAPSHOT_STATE = 14;
const int OP_KNEE = 15;

array<string> outputParametersNames={"Level Comp", "Level Orig", "Frequency",
"Step duration", "Error Messages", "Gain Offset Calculated", "Threshold", 
"Ratio (current)", "Ratio Calc Min", "Ratio Calc Max", "Ratio Manual",
"Current step", "Level Comp (real)", "Level Orig (real)", "Snapshot saving",
"Knee" };
array<double> outputParameters(outputParametersNames.length);
array<string> outputParametersUnits={"dB", "dB", "Hz", 
"ms", "", "dB", "dB", 
":1", "", "", ":1", 
"", "dB", "dB", "",
"dB" };
array<string> outputParametersFormats={".1", ".1", ".1",
"", "", ".2", ".2", 
".2", ".2", ".2", ".2",
"", ".2", ".2", "",
".0" };
array<string> outputParametersEnums={"", "", "", 
"", ";Test tone missing (check input);Probably Step Duration is too short;Threshold autodetection did not succeed;Data capturing paused;You need at least two input channels;Please provide separate test tone signal", "", "",
"", "", "", "",
"", "", "", "Save snapshot:;Saving...;Saving...;Removing...",
""};
array<double> outputParametersMin={FLOOR_LEVEL, FLOOR_LEVEL, 0, 
100, 0, -60, FLOOR_LEVEL, 
-100, -100, -100, -100,
0, -400, -400, 0,
0 };
array<double> outputParametersMax={CEIL_LEVEL, CEIL_LEVEL, 100000,
2000, 6, 60, CEIL_LEVEL, 
10000, 10000, 10000, 10000,
10000000, 60, 60, 3,
-FLOOR_LEVEL };

// error constants
int ERROR_OK = 0;
int ERROR_TESTTONE = 1;
int ERROR_STEP_DURATION = 2;
int ERROR_THRESHOLD_AUTO_FAILED = 3;
int ERROR_DATA_PAUSED = 4;
int ERROR_SINGLE_CHANNEL = 5;
int ERROR_TESTTONE_SAME = 6;

// "window" to correctly calculate max peak value for hi frequencies
int SAMPLES_WINDOW = 28;  

///////////////////////////
// GLOBAL VARS

// for channel selection
int input_channel_comp;
int input_channel_testtone;
int INPUT_LEFT = 0;
int INPUT_RIGHT = 1;
int INPUT_CH_3 = 2;
int INPUT_SC_LEFT = audioInputsCount;

// for frequency detection
int freq_samples_count = 0;
int freq_signschange = 0; // how many times sign changed
int freq_fullperiod_smpcnt = 0; // counter 
int freq_fullperiod_samples = 2000; // detected length of full period in samples
int frequency = 0; // detected frequency
bool use_window_detector = false; // auto-enabled for high frequencies

// for compressed level detection
double comp_value, comp_abs_value, comp_max_abs_value, comp_level, prev_comp_level, prev_significant_comp_level, comp_level_set; // comp_level - in db
array<double> comp_values(SAMPLES_WINDOW); // keep tracking of max level for high frequencies
int comp_sign, comp_silent_samples_count, comp_samples_since_silence, comp_signal_state, comp_attack_not_ended_times, samples_since_last_comp_level_measure; // for silence and waveform state detection
int MILLISECONDS_WINDOW = 99;  // time in milliseconds for detecting attack end
int MW = MILLISECONDS_WINDOW; // shortcut
array<double> comp_levels(MILLISECONDS_WINDOW);

// for original level detection
double orig_value, orig_abs_value, orig_max_abs_value, orig_level, orig_level_ready, last_orig_level, prev_orig_level_ready, orig_level_set, orig_level_set_last, orig_level_change_db;
array<double> orig_values(SAMPLES_WINDOW);
double orig_level_min, orig_level_min_this_cycle, orig_level_max, orig_level_max_this_cycle, orig_level_diff;
int orig_sign, orig_silent_samples_count, orig_samples_since_silence, orig_signal_state, orig_signal_missing_smpls, samples_since_orig_level_state_change, samples_since_last_orig_level_measure, orig_level_state = 0;

// values in samples for time calculation (precalculated in initialize)
int s250ms, s10s, ms_from_start, onems_samples_count;
uint64 samples_from_start;
int s_silence_min_limit; // for how many samples signal must be low to be considered silence
array<int> ms_samplepoints(10); // keep here points in samples that equal to 1ms interval (calculate them on initialize)

// used for compressor "MakeUp Gain" comprensation
array<double> gain_diffs(-int(FLOOR_LEVEL)+1); // to keep data about orig_gain-comp_gain
double gain_offset, gain_offset_calculated, gain_diffs_points;
bool gain_offset_calc_done_this_cycle = false;

// knee size
double knee = 0;
double knee_start, knee_end;

// range (mode)
int tr_range; // auto-read from kuiml file

// used for error reporting
int error_no = 0;

// file saving
string curDir = "";
string snapTRDir = "";
string curFileName = "";
string skinFullFileName = "";
string skinData = "";

// snapshot
int current_saving_mode = 0, snapshot_mode = 0;
int remove_reference_in_ms = 0;
int ms_since_last_progress_save = 1000;



// for saving SVG
const double SVG_WIDTH = 1200;
int PROGRESS_SAVE = 1;
int PROGRESS_CLEAR = 2;

// for saving debug
string debug_data = "";

// flags
bool auto_threshold = true;
bool auto_ratio = true;
bool auto_gain_offset = true;
bool output_paused = false;
bool settled_curve = false;
bool flag_save_snapshot = false;
bool right_channel_as_testtone = false;
bool cycle_has_started = false;
bool debug_saving_started = false;
bool production_mode = false;
bool was_saving_live = false;

// for calculation
int MIN_STEP_DURATION_MS = 90; // minimal step duration in milliseconds
int STEP_DURATIONS_WINDOW = 5;
array<double> steps_duration(STEP_DURATIONS_WINDOW); // keep here last 5 steps duration
int step_samples_count, step_ms; // counter for step length
double step_duration; // keep here calculated (settled) step duration
int steps_count = 1; // how many steps in one cycle
int current_step = 0;
int current_cycle = 0;
int step_up_probability = 0, cycle_end_probability = 0;

// threshold and ratio vars
double threshold, t_probable, threshold_calculated;
uint t_count;
array<double> thresholds(-int(FLOOR_LEVEL)+1);
double ratio_current, ratio_calc_min, ratio_calc_max, ratio_manual, r_probable, ratio_calculated, ratio;
double ratio_meas_point, ratio_meas_point_abs;
uint ratio_meas_point_abs_uint, r_count;
array<double> ratios(-int(FLOOR_LEVEL)+1);

/////////////// HEAD OVER ///////////////////////////

// initialize
bool initialize() {


    // init curDir
    curDir = scriptFilePath.substr(0, scriptFilePath.findLast("/")+1);
    curFileName = scriptFilePath.substr(curDir.length, scriptFilePath.length - curDir.length - 4);
    
    // check what mode we're in (inside PnS or as  Exported plugin)
    if (curFileName != "default") {
        // running inside Plug-N-Script (not as exported plugin)     
        snapTRDir = curDir + "Snapshot_TR/threshold_ratio_data/";
        skinFullFileName = curDir + curFileName + ".kuiml";
        production_mode = false;
    } else {
        // EXPORTED DLL version
        
        skinFullFileName = curDir + "../../Skins/custom_controls.kuiml.inc";
        production_mode = true;

        // if MAC version
        if (scriptFilePath.findFirst("Contents/Resources") > 0) {
            //print ("MAC Version");
            snapTRDir = curDir + "../../../../../../Snapshot_TR/threshold_ratio_data/";
        } else {
            //print ("Windows version");
            snapTRDir = curDir + "../../../Snapshot_TR/threshold_ratio_data/";
        }
    }

    //print("scriptFilePath: " + scriptFilePath);
    //print("curDir: " + curDir);
    //print("curFileName: " + curFileName);
    //print("snapTRDir: " + snapTRDir);
    

    // read range (60 or 100) from KUIML
    tr_range = getRangeFromKUIML();
    if (tr_range > 90) {
        inputParametersDefault[IP_RANGE] = 1;
    }
    //print("tr_range: " + tr_range);

    // print("curFileName: " + curFileName + " curDir: " + curDir + " snapTRDir: " + snapTRDir);

    // set input channels
    input_channel_comp = INPUT_LEFT;
    input_channel_testtone = INPUT_CH_3;

    if (auxAudioInputsCount == 0) {
        // if no AUX (SideChain) channels
        if (audioInputsCount == 1) {
            print("You need at least two channels to use this plugin.");
            outputParameters[OP_ERR_MSG] = ERROR_SINGLE_CHANNEL;
            return false;
        } else if (audioInputsCount == 2) {
            // only left and right channels
            inputParametersMax[IP_INPUT_CHANNEL_TESTTONE] = 1;
            inputParametersEnums[IP_INPUT_CHANNEL_TESTTONE] = "Right;Right";
        } else {
            // more than 2 channels, but no sidechain
            inputParametersMax[IP_INPUT_CHANNEL_TESTTONE] = 1;
            inputParametersEnums[IP_INPUT_CHANNEL_TESTTONE] = "Ch.3;Right";
        }
    } else {
        // if there are AUX inputs
        INPUT_SC_LEFT = audioInputsCount;
        inputParametersMax[IP_INPUT_CHANNEL_TESTTONE] = 2;
        if (audioInputsCount == 1) {
            // if it's a mono channel with sidechain, so no channel
            inputParametersMax[IP_INPUT_CHANNEL_COMPRESSED] = 1;
            inputParametersEnums[IP_INPUT_CHANNEL_COMPRESSED] = "Mono;Mono";
            inputParametersEnums[IP_INPUT_CHANNEL_TESTTONE] = "Sidechain;Sidechain;Sidechain";
        } else if (audioInputsCount == 2) {
            // if it's a stereo channel with sidechain
            inputParametersEnums[IP_INPUT_CHANNEL_TESTTONE] = "Right;Right;Sidechain Left";
        } else {
            // more than 2 channels with sidechain
            inputParametersEnums[IP_INPUT_CHANNEL_TESTTONE] = "Ch.3;Right;Sidechain Left";
        }
    }

    // print("audioInputsCount: "+audioInputsCount + " auxAudioInputsCount: " + auxAudioInputsCount);
    

    // calc sample points which correspond to 1ms points
    calculateSamplePoints();

    updateFloorLevel();

    // calc silence levels
    // SILENCE_LEVEL = round(20.0*log10(SILENCE), 3); // get level (in db) from float value (0 to 1)
    SILENCE_VALUE = pow(10, SILENCE_LEVEL / 20); // get value (float from 0 to 1) from level (in db)
    SILENCE_VALUE_COMP = pow(10, SILENCE_LEVEL_COMP / 20); // the same for compressed signal
    // print("SILENCE_LEVEL: " + SILENCE_LEVEL + ", SILENCE_VALUE: " + SILENCE_VALUE);
    FLOOR_VALUE = pow(10, FLOOR_LEVEL / 20);

    // set startup values
    comp_silent_samples_count = 0;
    comp_samples_since_silence = 0;
    comp_signal_state = -1; //  1 - rising, 0 = on peak, -1 = falling or static
    orig_silent_samples_count = s10s; // set to 10 seconds (for quick detection of "Sidechain Signal Missing")
    orig_samples_since_silence = 0;
    orig_signal_state = -1;
    onems_samples_count = 0;
    ms_from_start = 0;
    samples_from_start = 0;
    step_duration = 200;
    current_cycle = 0;
    current_step = 0;

    // set levels to zero
    comp_level = SILENCE_LEVEL;
    orig_level = SILENCE_LEVEL;

    orig_level_max = FLOOR_LEVEL;
    orig_level_min = CEIL_LEVEL;
    orig_level_min_this_cycle = CEIL_LEVEL;
    orig_level_max_this_cycle = SILENCE_LEVEL;

    prev_orig_level_ready = SILENCE_LEVEL;

    // clean up data array
    cleanUpGainDiff();

    gain_offset = 0;
    gain_offset_calculated = 0;

    outputParameters[OP_COMP_LEVEL] = comp_level;
    outputParameters[OP_ORIG_LEVEL] = orig_level;
    outputParameters[OP_COMP_LEVEL_REAL] = comp_level;
    outputParameters[OP_ORIG_LEVEL_REAL] = orig_level;

    // clear ratios array
    for (uint n=0; n < ratios.length; n++) { ratios[n] = 0; }
    r_count = 0; // how many ratius measured
    
    return true;
}

// per-sample processing
void processSample(array<double>& ioSample)
{
    // increase samples counter
    samples_from_start++;
    step_samples_count++;

    // SELECTING INPUT CHANNEL(s)

    // get sample values
    // for mono channels
    int ioSampleLength = ioSample.length;

    if (audioInputsCount == 1) {
        // we're on mono channel
        // select main (compressed) input
        comp_value = ioSample[0]; // MONO IN channel 
        // select testtone input
        if (ioSampleLength > 1) orig_value = ioSample[1]; // SideChain MONO IN (if SC present)
    } else {    
        // we're on stereo+ channel
        
        // use "Stereo" input for comp
        if (input_channel_comp == 2) { 
            comp_value = (ioSample[INPUT_LEFT] + ioSample[INPUT_RIGHT]) / 2; // sum of two channels
        } else {
            // or use Left or Right input for comp
            comp_value = ioSample[input_channel_comp];
        }

        // select testtone input
        orig_value = 0; // originally no signal
        if (audioInputsCount == 2) {
            // for stereo channels we can select eigher Right or SC (if present)
            if (input_channel_testtone < 2) {
                // select Right channel as TestTone input (bad idea, but possible)
                orig_value = ioSample[INPUT_RIGHT];
                right_channel_as_testtone = true; // mark this flag
            } else {
                // select sidechain if available
                right_channel_as_testtone = false; // reset flag
                if ((auxAudioInputsCount>0) && (INPUT_SC_LEFT <= (ioSampleLength-1))) orig_value = ioSample[INPUT_SC_LEFT];
            }
        } else {
            // for channels with more than 2 channels
            if (input_channel_testtone == 0) {
                orig_value = ioSample[INPUT_CH_3];
            } else if (input_channel_testtone == 1) {
                orig_value = ioSample[INPUT_RIGHT];
            } else {
                // use sidechain
                if ((auxAudioInputsCount>0) && (INPUT_SC_LEFT <= (ioSampleLength-1))) orig_value = ioSample[INPUT_SC_LEFT];
            }
        }
    }

    // measure frequency & levels
    measureFreqAndLevels();

    // if no original signal (sidechained/on input channels 3-4)
    // for more than 10 seconds
    if (orig_silent_samples_count > s10s) {
        // show warning "sidechain signal missing"
        error_no = ERROR_TESTTONE;
        // without sidechain signal we cannot detect step duration, etc
        // so take "set" levels as current, just to display at least something
        orig_level_set = round(orig_level_ready, 0);
        comp_level_set = comp_level;
    } else {
        // hide "sidechain signal missing" error
        if (error_no == ERROR_TESTTONE) { error_no = ERROR_OK; }
    }

    // detect if 1 millisecond passed
    onems_samples_count++;
    int onems_passed = 0;
    for( uint n = 0; n <= 9; n++ ) {
        if (onems_samples_count == ms_samplepoints[n]) { onems_passed = n+1; }
        if (onems_passed == 10) { onems_samples_count = 0; }
    }

    // if 1 millisecond passed
    if (onems_passed > 0) {
        ms_from_start++;
        step_ms++;
        ms_since_last_progress_save++;


        // every 1 millisecond add info about level
        comp_levels.removeAt(0);
        comp_levels.insertLast(comp_level);

        // in "realtime curve" mode display real (current) values
        // if not paused
        if ((!output_paused) && (!settled_curve)) {
            // print("realtime curve output");
            outputParameters[OP_COMP_LEVEL] = comp_level-gain_offset;
            outputParameters[OP_ORIG_LEVEL] = orig_level_ready;
            outputParameters[OP_COMP_LEVEL_REAL] = comp_level;
            outputParameters[OP_ORIG_LEVEL_REAL] = orig_level_ready;
        }

        // if flag is set to remove reference
        if (flag_save_snapshot && (current_saving_mode == SNAPSHOT_MODE_REMOVE_REF)) {
            remove_reference_in_ms--;
            if (remove_reference_in_ms == 0) {
                saveSnapShot();
                if (was_saving_live) {
                    flag_save_snapshot = true;
                    outputParameters[OP_SNAPSHOT_STATE] = 2;
                    current_saving_mode = SNAPSHOT_MODE_LIVE;
                }
            }
        }
    }


    // calculate level differences
    orig_level_diff = (orig_level_ready - prev_orig_level_ready);

    // detect STEP ENDED
    bool step_just_ended = false;
    bool cycle_ended = false;

    step_up_probability = 0;
    if (orig_level_diff > 0.9) step_up_probability = 100;

    // DETECT STEP UP (original level grows 1dB)
    
    if (step_up_probability > 99) {
        if (!cycle_has_started) {
            cycle_has_started = true;
            // print("cycle has started. orig_level_ready: "+ orig_level_ready);
        }

        // remember maximum level
        if (orig_level_ready > orig_level_max_this_cycle) {
            orig_level_max_this_cycle = orig_level_ready;
        }

        // if coming up from prev step (not from silence)
        if (prev_orig_level_ready >= FLOOR_LEVEL) { 
            // calculate static (and current) step duration
            calculateStepDuration();
            // print("step duration: "+ step_duration);
        }

        step_samples_count = 0; // starting a new step
        step_just_ended = true; // flag that previous step just ended

    } else // ended STEP UP DETECTION

    // DETECT CYCLE END
    cycle_end_probability = 0;
    if (cycle_has_started) {
        if (orig_level_diff < -2.9) cycle_end_probability = 100;
        if (orig_level_ready < SILENCE_LEVEL) cycle_end_probability = 200;
    }
    // print("cycle_end_probability: " + cycle_end_probability);
    
    if (cycle_end_probability > 99) {
        // cycle ended & prev step ended
        cycle_ended = true;
        step_just_ended = true;
        cycle_has_started = false;
    } 

    // if step just ended (started a new step, or whole cycle ended)
    if (step_just_ended) {
        
        orig_level_set = prev_orig_level_ready;

        // take an average level few milliseconds before
        comp_level_set = (comp_levels[MW-5]+comp_levels[MW-6]+comp_levels[MW-7]) / 3; 

        // parse passed step
        parseStepPassed();

        // if also cycle ended
        if (cycle_ended) {
            parseCycleEnded();
        }

        // try to detect if step duration is not enough for compressor attack to "settle"
        verifyStepDuration();
        // print("orig_level_diff_sum: " + orig_level_diff_sum);
    }

    // if we're in silent zone, still count steps
    if ((orig_level_ready < FLOOR_LEVEL) && (step_ms >= step_duration)) {
        orig_level_set = prev_orig_level_ready;

        // take average comp level
        comp_level_set = (comp_levels[MW-5]+comp_levels[MW-6]+comp_levels[MW-7]) / 3; 

        parseStepPassed();
    }

    // remember current original level for next iteration
    prev_orig_level_ready = orig_level_ready;
    prev_comp_level = comp_level;

    // mute output if needed
    if (inputParameters[IP_MUTE_OUTPUT] > 0.5) {
        for(uint channel=0;channel<audioInputsCount;channel++) { 
            ioSample[channel]=0; 
        }
    }
    
    // debug saving
    if (debug_saving_started) { debugAddLine(); }

    // processSample ENDED
}

// called when step ended / also while silence called every "step_duration" period
void parseStepPassed(){
    // ignore if executed too often
    if (step_ms < MIN_STEP_DURATION_MS) {
        return;
    }

    current_step++;
    step_ms = 0;

    // detect minimal level
    if (cycle_has_started) {
        if (orig_level_set < orig_level_min_this_cycle) {
            orig_level_min_this_cycle = orig_level_set;
        }
        // print("orig_level_set: " + orig_level_set + " orig_level_min_this_cycle: "+orig_level_min_this_cycle);
    }

    if (output_paused) {
        // don't update data on pause
        return;
    }

    /*
    // if no changes to original signal since last call
    // maybe we're in "silence" steps, so just take current values
    if ((orig_level_set == orig_level_set_last) && (orig_level < FLOOR_LEVEL)) {
        orig_level_set = orig_level;
        comp_level_set = comp_level;
    }
    */

    // in "settled curve" mode display "settled" values
    if ((!output_paused) && (settled_curve)) {
        // print("settled curve output");
        outputParameters[OP_COMP_LEVEL] = comp_level_set-gain_offset;
        outputParameters[OP_ORIG_LEVEL] = orig_level_set;
        outputParameters[OP_COMP_LEVEL_REAL] = comp_level_set;
        outputParameters[OP_ORIG_LEVEL_REAL] = orig_level_set;
        // print("settled orig_level_set: " + orig_level_set + ", comp_level_set: " + (comp_level_set-gain_offset));
    }
    
    // output info about current "step"
    if (!output_paused) {
        outputParameters[OP_CURRENT_STEP] = current_step;
    }

    // adding step info into array (of level differences between original and compressed signals)
    // and try to calculate static gain offset
    uint gdindex = roundDoubleToInt(abs(orig_level_set));
    float this_gain_diff = round(orig_level_set,0) - comp_level_set;
    // print("gdindex: " + gdindex + ", orig_level_set: " + orig_level_set + ", ms_from_start: " + ms_from_start + " this_gain_diff: " + this_gain_diff);
    uint gdlength = gain_diffs.length;
    if ((gdindex >= 0) && (gdindex < gdlength)) {
        gain_diffs[gdindex] = this_gain_diff;
    }
    // used for checking if both compressed and testtone signals are equal
    if ((orig_level_set > FLOOR_LEVEL) && (comp_level_set > FLOOR_LEVEL)) {
        double abs_gain_diff = abs(orig_level_set-comp_level_set);
        if (abs_gain_diff > 1) gain_diffs_points += 1*abs_gain_diff;
    }

    // if we're passing manual ratio measurement point, calculate ratio
    if (gdindex == ratio_meas_point_abs_uint) {
        calculateManualRatio();
    }

    // trying to auto-detect static gain offset (static for last few steps)
    if ((gdindex < gdlength-3) && (!gain_offset_calc_done_this_cycle)) {
        if ((abs(this_gain_diff-gain_diffs[gdindex+1]) < 0.1)
            && (abs(this_gain_diff-gain_diffs[gdindex+2]) < 0.1)
            && (abs(this_gain_diff-gain_diffs[gdindex+3]) < 0.1)
            ) { 
            gain_offset_calculated = -this_gain_diff; 
            gain_offset_calc_done_this_cycle = true;
            }
    }
    // apply auto (calculated) gain offset or manual
    if (auto_gain_offset) {
        gain_offset = gain_offset_calculated;
    } else {
        gain_offset = inputParameters[IP_GAIN_OFFSET_MANUAL];
    }

    // calculate current ratio for this step (if it's over threshold)
    if (orig_level_set > threshold) {
    // if ((comp_level_set-gain_offset) > threshold) {
        double diff_o = orig_level_set-threshold;
        double diff_c = comp_level_set-gain_offset-threshold;
        ratio_current = diff_o/diff_c; 
        //if (ratio_current > 0.02) {
            if (!output_paused) {
                outputParameters[OP_RATIO_CURRENT] = ratio_current;
            }
            /* ratios[r_count] = ratio_current;
            r_count++; */
        //}
    }

    // save progress bar to svg if needed
    if ((flag_save_snapshot) && (current_saving_mode != SNAPSHOT_MODE_REMOVE_REF)) {
        saveProgress(PROGRESS_SAVE);
    }
    
    // print("step: " + current_step + ", ms: " + ms_from_start+ ", gdindex: " + gdindex + ", orig_level_set: " + orig_level_set + ", comp_level_set: " + comp_level_set);

    // remember last set original level (for next step)
    orig_level_set_last = orig_level_set;
}

// called every time whole "cycle" of steps ended
void parseCycleEnded(){
    current_cycle++;

    if (output_paused) {
        return;
    }

    // reset gain auto-offset calculation
    gain_offset_calc_done_this_cycle = false;
    
    // confirm last detected min and max levels
    if ((orig_level_min_this_cycle < orig_level_max_this_cycle)) {
        orig_level_min = round(orig_level_min_this_cycle, 0);
        orig_level_max = round(orig_level_max_this_cycle, 0);
    }

    //print("cycle ended after step " + current_step + ", orig_level_min: " + orig_level_min + ", orig_level_max: " + orig_level_max);

    // and reset them
    orig_level_min_this_cycle = CEIL_LEVEL;
    orig_level_max_this_cycle = SILENCE_LEVEL;



    if (current_cycle > 2) { // because max and min cycle levels are properly calculated on 2nd cycle
        // clean gain_diff array for values outside current cycle max and min
        cleanUpGainDiff(); 
    }

    // trying to autodetect threshold
    //if (current_cycle > 1) { // if it's not the first cycle from start
        autoDetectThreshold();
    //}

    // calculate ratio in automatic mode
    calculateAutoRatiosAndKnee();

    // saving snapshot
    if (flag_save_snapshot) {
        saveSnapShot();
        // if save snapshot continiously
        if ((current_saving_mode == SNAPSHOT_MODE_LIVE) || (was_saving_live)) {
            flag_save_snapshot = true;
            outputParameters[OP_SNAPSHOT_STATE] = 2;
            current_saving_mode = SNAPSHOT_MODE_LIVE;
        }
    }
    
    // check for possible error, where TestTone is same signal as main input
    if (right_channel_as_testtone) {
        if (gain_diffs_points < 10) { // if both testtone and compressed signal don't differ
            error_no = ERROR_TESTTONE_SAME;
            outputParameters[OP_ERR_MSG] = error_no;
            // print("error no testtone signal");
        } else {
            error_no = ERROR_OK;
        }
        // print("gain_diffs_points: "+ gain_diffs_points);
        gain_diffs_points = 0; 
    } else {
        // reset error
        if (error_no == ERROR_TESTTONE_SAME) {
            error_no = ERROR_OK;
        }
    }
}

// trying to autodetect threshold
void autoDetectThreshold() {
    // trying to auto-calculate threshold
    // formula is t = (o0*c1 - o1*c0) / (c1 - c0 + o0 - o1)
    double t, o0, o1, c0, c1;
    uint nceil, nfloor, i;
    uint gdlength = gain_diffs.length;

    t = FLOOR_LEVEL; // starting value
    nceil = roundDoubleToInt(abs(orig_level_max)); 
    nfloor = roundDoubleToInt(abs(orig_level_min)); 
    // print("orig_level_min: " + orig_level_min + ", nceil: " + nceil + ", nfloor: " + nfloor);

    // clear thresholds array
    for (uint n=0; n < thresholds.length; n++) { thresholds[n] = 0; }
    t_count = 0; // how many thresholds calculated

    // calculate several thresholds
    for (uint n=nceil; n<=nfloor; n++) {
        o0 = -n;
        o1 = o0-1;
        i = roundDoubleToInt(abs(o0));
        if ((i>=0) && (i<gdlength-1)) {
            c0 = o0-gain_diffs[i]-gain_offset;
            c1 = o1-gain_diffs[i+1]-gain_offset;
            if (((o1-c1)>1) && ((o1-c1)<100)) { // if difference in levels is more than 1db, use it
                t = round((o0*c1 - o1*c0) / (c1 - c0 + o0 - o1), 2);
                thresholds[t_count] = t; // add found threshold to array
                t_count++;
                //print("i: "+ i + " o0: " + o0 + " o1: " + o1 + " c0: " + c0 + " c1: " + c1 + " t: " + t );
            } else {
                //print("not used i: "+ i + " o0: " + o0 + " o1: " + o1 + " c0: " + c0 + " c1: " + c1 );
            }
        }
    }

    // trying to find most probable threshold
    findMostProbableThreshold();

    // apply threshold
    if (!auto_threshold) {
        // manual mode "threshold"
        threshold = inputParameters[IP_THRESHOLD];
        if (error_no == ERROR_THRESHOLD_AUTO_FAILED) { error_no = ERROR_OK; }
    } else {
        // auto threshold
        if (t_probable >= FLOOR_LEVEL) {
            // if detected successfully
            if (error_no == ERROR_THRESHOLD_AUTO_FAILED) { error_no = ERROR_OK; }
            threshold_calculated = t_probable;
            threshold = threshold_calculated;
            if (!auto_ratio) {
                calculateManualRatio();
            }
            
        } else {
            // failed to calculate threshold
            // print("auto-threshold failed. t_probable: "+t_probable);
            // print ("t_static: " + t_static);
            if (current_cycle > 2) {
                error_no = ERROR_THRESHOLD_AUTO_FAILED; // display "failed to calculate threshold"
            }
            threshold = threshold_calculated; // take previous calculated threshold
        }
    }
}

// find the most probably threshold
double findMostProbableThreshold(){
    t_probable = FLOOR_LEVEL-1;
    
    // compare every number with every other
    double this_t, this_prob_diff, prob_value, min_prob_value, prob_coeff;
    min_prob_value = 100;
    for (uint n=0; n < t_count; n++) { 
        this_t = thresholds[n];
        prob_value = 0;
        prob_coeff = 1; // coeff of probability
        for (uint m=0; m < t_count; m++) {
            if (m==n) continue;
            this_prob_diff = abs(this_t - thresholds[m]);
            prob_value += this_prob_diff;
            if (this_prob_diff < 0.1) {
                prob_coeff++; // for really close values
            }
        }
        prob_value = (prob_value/t_count) / prob_coeff;

        if (prob_value < min_prob_value) {
            min_prob_value = prob_value;
            t_probable = this_t;
        } else if (prob_value == min_prob_value) {
            t_probable = (t_probable + this_t)/2;
        }

        //print("Thr"+n+": " + thresholds[n] +" prob_coeff: "+prob_coeff + " prob_value: "+prob_value );
    }

    //print("MOST PROBABLE THRESHOLD: " + t_probable);
    return t_probable;
}

// trying to select most probable ratios from the list of calculation
// also calculate knee
void calculateAutoRatiosAndKnee() {

    int nceil = roundDoubleToInt(abs(orig_level_max)); 
    int nfloor = roundDoubleToInt(abs(orig_level_min)); 
    
    // if not yet detected min and max levels, return
    if (nfloor < nceil) {
        return;
    } 

    uint i;
    double r_this, orig_lvl, comp_lvl, diff_o, diff_c;
    int gdlength = gain_diffs.length;

    // clear ratios array
    for (uint n=0; n < ratios.length; n++) { ratios[n] = 0; }
    r_count = 0; // how many ratius measured

    // calculate all possible ratios
    for (int n=nceil; n<=nfloor; n++) {
        orig_lvl = -n;
        if ((n>=0) && (n<gdlength)) {
            comp_lvl = orig_lvl-gain_diffs[n]-gain_offset;

            if (orig_lvl > threshold) {
                diff_o = orig_lvl-threshold;
                diff_c = comp_lvl-threshold;
                r_this = diff_o/diff_c; 

                //if (r_this > 0.02) {
                    ratios[r_count] = r_this;
                    r_count++; 
                    // print("n: "+n + " orig_lvl: " + orig_lvl + " comp_lvl: " + comp_lvl + " r_this: " + r_this);
                //}
            }
        }
    }

    // selecting most probable ratio
    r_probable = 0;

    double this_r, this_prob_diff, prob_value, min_prob_value, prob_coeff;
    min_prob_value = 2000;
    ratio_calc_min = 9999;
    ratio_calc_max = 0;
    for (uint n=0; n < r_count; n++) { 
        this_r = ratios[n];
        prob_value = 0;
        prob_coeff = 1; // coeff of probability
        // compare with every other ratio
        for (uint m=0; m < r_count; m++) {
            if (m==n) continue;
            this_prob_diff = abs(this_r - ratios[m]);
            prob_value += this_prob_diff;
            if (this_prob_diff < 0.01) {
                prob_coeff++; // for really close values
            }
        }
        prob_value = (prob_value/r_count) / prob_coeff;

        if (prob_value < min_prob_value) {
            min_prob_value = prob_value;
            r_probable = this_r;
        } else if (prob_value == min_prob_value) {
            r_probable = (r_probable + this_r)/2;
        }

        // print("ratios["+n+"]: " + ratios[n] +" prob_coeff: "+prob_coeff + " prob_value: "+prob_value );
    }

    ratio_calculated = r_probable;

    // once again passing through ratios to find adequate min and max values 
    double ratio_filtering_coeff = 4;
    for (uint n=0; n < r_count; n++) { 
        this_r = ratios[n];
        if ((r_probable > 0) && (this_r < 0)) continue; // ignore negative values
        if (this_r > r_probable*ratio_filtering_coeff) continue; // ignore too big values
        if (this_r < r_probable/ratio_filtering_coeff) continue; // ignore too small values
        if (this_r > ratio_calc_max) { ratio_calc_max = this_r; }
        if (this_r < ratio_calc_min) { ratio_calc_min = this_r; }
    }

    // output calculated min and max
    if ((ratio_calc_min < ratio_calc_max) && (!output_paused)) {
        outputParameters[OP_RATIO_CALC_MIN] = ratio_calc_min;
        outputParameters[OP_RATIO_CALC_MAX] = ratio_calc_max;
    }
    
    // print("MOST PROBABLE RATIO: " + r_probable + " ("+ratio_calc_min+"-"+ratio_calc_max+"): ");
    

    // apply ratio
    if (!auto_ratio) {
        // manual mode "ratio"
        ratio = ratio_manual;
    } else {
        // auto ratio
        ratio = ratio_calculated;
    }

    // trying to calculate knee
    knee_start = 1000;
    knee_end = 1000;
    double r_last = 1;
    for (int n = nfloor; n >= nceil; n--) {
        orig_lvl = -n;
        if ((n>=0) && (n<gdlength)) {
            comp_lvl = orig_lvl-gain_diffs[n]-gain_offset;
            if (abs(orig_lvl - comp_lvl) > 0.1) {
                if (knee_start > 100) {
                    knee_start = orig_lvl-0.5;
                }
                diff_o = orig_lvl-threshold;
                diff_c = comp_lvl-threshold;
                r_this = round(diff_o/diff_c, 2); 
                // print("n: "+n + " orig_lvl: " + orig_lvl + " comp_lvl: " + comp_lvl + " r_this: " + r_this);
                if ((r_last < r_this) && (abs(r_last - r_this) > 0.1)) {
                    knee_end = orig_lvl+0.5;
                } 
                if ((ratio_calculated > 1) && (abs(r_this - ratio_calculated) < 0.1)) {
                    knee_end = orig_lvl+0.5;
                    break;
                }
                r_last = r_this;
            } else {
                // if compressor is in linear mode (orig_lvl - comp_lvl < 0.2)
                if (knee_start < 1) {
                    knee_start = orig_lvl-0.5;
                }
            }
        }
    }

    // first way to calculate knee - (start .. end) / 2
    double mknee = 0;
    double mknee_prec = 0;
    if ((knee_end < 1) && (knee_start > FLOOR_LEVEL)) {
        mknee_prec = (knee_end - knee_start)/2;
    }
    /*
        if (mknee_prec < 2) {
            mknee = floor(mknee_prec*0.9);
        } else if (mknee_prec < 7) {
            mknee = ceil(mknee_prec);
        } else {
            // adding rounding coeff to match Oxford Dynamics
            mknee = ceil(mknee_prec*1.1);
        } 
    } else {
        mknee = 0;
    }
    */

    // second way to calculate knee - take difference between threshold and knee_start
    double tknee_prec = 0;
    if ((knee_start > FLOOR_LEVEL) && (threshold < -3)) {
        tknee_prec = (threshold - knee_start);
    }

    // now let's select which knee to take
    double knee_prec = 0;
    double rounding_coeff = 1;
    if (tknee_prec < 1) {
        knee_prec = 0; // ignore mknee
        knee = 0;
    } else {
        // if mknee and tknee are close, take average
        if (abs(tknee_prec - mknee_prec) < 4) {
            knee_prec = (tknee_prec + mknee_prec)/2;
        } else {
            // if they are too far, take the smallest
            if (tknee_prec < mknee_prec) {
                knee_prec = tknee_prec;
            } else {
                knee_prec = mknee_prec;
            }
        }
        
        // just to better match Oxford Dynamics/ReaComp ratios we add "rounding coefficient"
        if (knee_prec<6) {
            rounding_coeff = 1;
        } else if (knee_prec < 8) {
            rounding_coeff = 1.1;
        } else if (knee_prec < 12) {
            rounding_coeff = 1.18;
        } else {
            rounding_coeff = 1.2;
        }
        // for higher ratios lower rounding coeff
        if (ratio_calculated > 5) {
            rounding_coeff = rounding_coeff*0.96;
        } 
        if (ratio_calculated < 3) {
            rounding_coeff = rounding_coeff*1.06;
        }

        knee = round(knee_prec * rounding_coeff, 0);
    }

    // print("knee_start: " +knee_start + " knee_end: " + knee_end + " mknee: " + mknee + " tknee: " + tknee + " knee_prec: " + knee_prec + " rounding_coeff: " + rounding_coeff + " knee: " + knee);
    // print("mknee_prec: " + mknee_prec + " tknee_prec: " + tknee_prec + " knee_prec: " + knee_prec + " rounding_coeff: " + rounding_coeff + " knee: " + knee);
}

// function to calculate ratio with manual set ratio measuring point
void calculateManualRatio(){
    double orig_lvl = -ratio_meas_point_abs;
    double diff_o = orig_lvl-threshold;
    double comp_lvl = -ratio_meas_point_abs-gain_diffs[ratio_meas_point_abs_uint];
    double diff_c = comp_lvl-gain_offset-threshold;
    ratio_manual = diff_o/diff_c; 
    // print("orig_lvl: " + orig_lvl + " comp_lvl: " + comp_lvl + " diff_o/diff_c: " + diff_o + "/" + diff_c + " = " + ratio_manual);
    /* if (ratio_manual < 0.1) {
        ratio_manual = 0;
    } */
    if (!auto_ratio) {
        ratio = ratio_manual;
    }
}

// calculating step duration (in milliseconds)
void calculateStepDuration(){
    // calculate previous (just ended) step duration
    double this_step_duration = (step_samples_count*1000)/sampleRate;

    // filter improper durations (too short)
    if (this_step_duration > MIN_STEP_DURATION_MS) { 

        // fill array of step durations
        steps_duration.removeAt(0);
        steps_duration.insertLast(this_step_duration);

        // detect if step duration is (not) "static" for some "window"
        bool is_static = true;
        for( int n = 0; n < STEP_DURATIONS_WINDOW; n++ ) {
            if (steps_duration[n] != this_step_duration) { is_static = false; }
        }
        // if last few step durations are the same, lets confirm it
        if (is_static) {
            step_duration = this_step_duration;
        }
    }
    // print ("step duration: " + step_duration);   
}

// try to detect if attack has ended on previous step, or step was too short
// display warning if so
void verifyStepDuration() {

    double max_diff = 0;
    double this_diff = 0;
    uint n_start = 0;

    // for short step duration use little window 
    if (step_duration < 101) {
        n_start = MILLISECONDS_WINDOW-70;
    }
    // calculate level differences in level
    for( int n = n_start; n < MILLISECONDS_WINDOW-1; n++ ) {
        this_diff = abs(comp_level_set - comp_levels[n]);
        if (this_diff > max_diff) { max_diff = this_diff; }
    }
    // if compressed signal is "floating" in last window, maybe it's because attack hasn't ended yet
    if (max_diff >= 0.1) { // or 0.075
        if (comp_attack_not_ended_times < 10) {
            comp_attack_not_ended_times++;
        }
    } else {
        if (comp_attack_not_ended_times > 0) {
            comp_attack_not_ended_times--;
        }
    }
    if (comp_attack_not_ended_times > 5) {
        if (error_no == ERROR_OK) {
            error_no = ERROR_STEP_DURATION; // display "step duration too short"
            // print("on step: " + current_step + ", max_diff: " + max_diff);
        }
    }
    // reset "step duration too short" message
    if (comp_attack_not_ended_times == 0) {
        if (error_no == ERROR_STEP_DURATION) {
            error_no = ERROR_OK;
        }
    }
}

// measure frequency & peak levels
void measureFreqAndLevels() {
    bool comp_sign_changed = false;

    // increase samples counter
    freq_samples_count++;
    freq_fullperiod_smpcnt++;
    samples_since_last_comp_level_measure++;
    samples_since_last_orig_level_measure++;

    // measure freq and level of compressed signal
    comp_abs_value = abs(comp_value);

    // remember abs values for measuring higher frequencies using "window"
    comp_values.removeAt(0);
    comp_values.insertLast(comp_abs_value);

    // if signal is present and loud enough
    if (comp_abs_value > SILENCE_VALUE_COMP) {

        // silence/unsilence counters
        comp_silent_samples_count = 0;
        comp_samples_since_silence++;

        // detect signal change
        if (comp_value < -SILENCE_VALUE) {
            if (comp_sign > 0) {
                comp_sign = -1;
                comp_sign_changed = true;
                // calculate length of one period in samples
                freq_fullperiod_samples = freq_fullperiod_smpcnt;
                freq_fullperiod_smpcnt = 0;
                // print ("s_silence_min_limit: "+s_silence_min_limit+", freq_fullperiod_samples: " + freq_fullperiod_samples );
            }
        } else {
            if (comp_sign <= 0) {
                comp_sign = 1;
                comp_sign_changed = true;
            }
        }
    } else {
        // if signal is low, count silent samples
        comp_silent_samples_count++;
        if (comp_silent_samples_count > s_silence_min_limit) {
            // if silent samples last long enough, lets consider it real silence
            comp_samples_since_silence = 0;
        }
    }

    // if sign change (of compressed signal) detected
    if (comp_sign_changed) {
        // detect frequency
        freq_signschange++; // count sign changes qty
        if (freq_samples_count >= s250ms) {
            // if signal is present for long enough time, measure frequency
            if (comp_samples_since_silence >= s250ms) {
                frequency = freq_signschange*2; // because of 250ms window
                // decide if we use "window" or peak detector for level measuring
                // "peak" detector works well for freqs with length of 25+ samples (~ <2000Hz @ 44.1kHz)
                // "window" detector works better for higher frequencies
                if (freq_fullperiod_samples < SAMPLES_WINDOW) {
                    use_window_detector = true;
                } else {
                    use_window_detector = false;
                }
            } 
            freq_samples_count = 0;
            freq_signschange = 0;
        }
    }

    // if silence detected
    if (comp_samples_since_silence == 0) {
        comp_max_abs_value = 0; // reset max_abs_value
        // comp_level = SILENCE_LEVEL_COMP; // set level to silence 
        comp_level = round(20.0*log10(comp_abs_value), 3);
        if (comp_level < -200) comp_level = -200;
    } else {
        // if signal present, detect level

        // if sign change (zero crossing) detected
        if (comp_sign_changed) {
            comp_max_abs_value = comp_abs_value; // reset comp_max_abs_value
        } else {
            // sign not changed, let's check maybe it has'n changed for a long time
            if (freq_fullperiod_smpcnt > freq_fullperiod_samples) {
                // print("suspicios! orig_level_ready: " + orig_level_ready);
                // fix for detection of compressed levels over or above 0 without crossing 0
                // when testtone < SILENCE_LEVEL
                if (orig_level_ready < SILENCE_LEVEL) {
                    // print("orig_level: "+ orig_level + " comp_level: "+comp_level);
                    comp_level = round(20.0*log10(comp_abs_value), 3);
                }
            }
        }

        // detect signal rising or falling
        if (comp_abs_value > comp_max_abs_value) {
            // signal rising (in this half-period)
            comp_max_abs_value = comp_abs_value;
            comp_signal_state = 1; // signal rising
        } else {
            // signal on peak or falling (in this half-period)
            if (comp_signal_state == 1) { // was rising
                comp_signal_state = 0; // on peak now

                double comp_value_to_db = comp_max_abs_value;
                // if frequency is high and samplerate low, 
                // peak measuring is unreliable
                if (use_window_detector) {
                    comp_value_to_db = getMaxValue(comp_values);
                }
                // when on peak, calculate peak level
                comp_level = round(20.0*log10(comp_value_to_db), 3);
                samples_since_last_comp_level_measure = 0;
            } else {
                comp_signal_state = -1; // signal falling or static
            }

            // to fix level detection on hi freqs, where sample_period is about 2 samples)
            if ((use_window_detector) && (samples_since_last_comp_level_measure > freq_fullperiod_samples)) {
                double signal_value_to_db = getMaxValue(comp_values);
                comp_level = round(20.0*log10(signal_value_to_db), 3);
                samples_since_last_comp_level_measure = 0;
                // print("hard detection" + ms_from_start);
            }
        }
    }

    // NOW let's measure original signal level
    bool orig_sign_changed = false;
    last_orig_level = orig_level;

    // remember abs values for measuring higher frequencies using "window"
    orig_abs_value = abs(orig_value);
    orig_values.removeAt(0);
    orig_values.insertLast(orig_abs_value);

    // if signal is present and loud enough
    if (orig_abs_value > SILENCE_VALUE) {
        // silence/unsilence counters
        orig_silent_samples_count = 0;
        orig_samples_since_silence++;

        // detect signal change
        if (orig_value < -SILENCE_VALUE) {
            // waveform negative
            if (orig_sign > 0) {
                orig_sign = -1;
                orig_sign_changed = true;
            }
        } else {
            // waveform positive
            if (orig_sign <= 0) {
                orig_sign = 1;
                orig_sign_changed = true;
            }
        }
    } else {
        // if signal is low, count silent samples
        orig_silent_samples_count++;
        if (orig_silent_samples_count > 1) {
            // if silent samples last long enough, lets consider it real silence
            orig_samples_since_silence = 0;
            // print("ms: " + ms_from_start + ", s_silence_min_limit reached: "+s_silence_min_limit);
        }
    }

    // if silence detected
    if (orig_samples_since_silence == 0) {
        orig_max_abs_value = 0; // reset max_abs_value
        // orig_level = SILENCE_LEVEL; // set level to silence
        orig_level = round(20.0*log10(orig_abs_value), 3);
        if (orig_level < -200) orig_level = -200;
    } else {
        // if signal present, detect level

        // if sign change (zero crossing) detected
        if (orig_sign_changed) {
            orig_max_abs_value = orig_abs_value; // reset
        }

        // detect signal rising or falling
        if (orig_max_abs_value < orig_abs_value) {
            // signal rising (in this half-period)
            orig_max_abs_value = orig_abs_value;
            orig_signal_state = 1; // signal rising
        } else {
            // signal on peak or falling (in this half-period)
            if (orig_signal_state == 1) { // was rising
                orig_signal_state = 0; // on peak now or falling

                double orig_value_to_db = orig_max_abs_value;
                // if frequency is high and samplerate low, 
                // peak measuring is unreliable
                // so use "window" detection of last max_value
                if (use_window_detector) {
                    orig_value_to_db = getMaxValue(orig_values);
                    // print("w: " +orig_value_to_db);
                }

                // when on peak, calculate peak level
                orig_level = round(20.0*log10(orig_value_to_db), 3);
                samples_since_last_orig_level_measure = 0;
            } else {
                orig_signal_state = -1; // signal falling or static
            }

            // to fix level detection on hi freqs, where sample_period is about 2 samples)
            if ((use_window_detector) && (samples_since_last_orig_level_measure > freq_fullperiod_samples)) {
                double signal_value_to_db = getMaxValue(orig_values);
                orig_level = round(20.0*log10(signal_value_to_db), 3);
                samples_since_last_orig_level_measure = 0;
                // print("here "+ms_from_start);
            } 
        }
    }

    // let's detect if level is growing or falling
    orig_level_change_db = orig_level - last_orig_level;
    int last_orig_level_state = orig_level_state;
    if (orig_level_change_db != 0) {
        orig_level_state = (orig_level_change_db < 0) ? -1 : 1;
        if (last_orig_level_state != orig_level_state) {
            // samples_since_prev_level_state_change = samples_since_level_state_change;
            samples_since_orig_level_state_change = 0;
        } else {
            samples_since_orig_level_state_change++;
        }
    } else {
        // level not changing
        samples_since_orig_level_state_change++;
    }

    // proper detection of changes in original level for "window" detector
    if (use_window_detector) {
        // if level is growing, let's wait till it's grown, or till it's too long
        if (orig_level_state > 0) {
            // if signal is growing for too long - it's suspicious
            if (samples_since_orig_level_state_change > SAMPLES_WINDOW*4) {
                orig_level_ready = orig_level;
            }
        } else {
            // if level is falling, lets take it as it is
            orig_level_ready = orig_level;
        }

    } else {
        // for peak detector just take as is
        orig_level_ready = orig_level;
    }
}


// calc misc samplepoints which will be used in later calculations
void calculateSamplePoints(){
    // calc sample points which correspond to 1ms (because of approximity of 1ms at 44100 and 88200)
    for( uint n = 0; n <= 9; n++ ) {
        ms_samplepoints[n] = roundDoubleToInt((sampleRate/1000*(n+1)) );
    }
    // samplepoints for milliseconds
    s250ms = roundDoubleToInt(sampleRate/4);
    s10s = roundDoubleToInt(sampleRate*10);

    // for how many samples signal must be low to be considered silence
    s_silence_min_limit = roundDoubleToInt(sampleRate/10000); // 0.1ms
}

// clean up gain_diffs array that is over or below min and max of current cycle
void cleanUpGainDiff(){   
    
    uint nceil = roundDoubleToInt(abs(orig_level_max));
    uint nfloor = roundDoubleToInt(abs(orig_level_min));
    for (uint n=0; n<gain_diffs.length; n++) {
        if ((n<nceil) or (n>nfloor)) {
            gain_diffs[n] = 1000; // set really impossible gain difference
            // print("cleared "+n);
        }
    }


    // FOR DEBUGGING
    /*
    string sout;
    sout = "";
    for (uint n=0; n<gain_diffs.length; n++) {
        sout += gain_diffs[n] + ", ";
        if (fraction((n+1)/ 10.0) == 0) {
            sout += "\n";
        }
    }
    print("------------");
    print("cycle: " + current_cycle);
    print(sout);
    */
}

// save progress bar
void saveProgress(int mode = PROGRESS_SAVE){
    double x_max = SVG_WIDTH;
    double y_max = x_max;
    double XY_COEFF = SVG_WIDTH/-FLOOR_LEVEL;

    double width = round((orig_level_set - orig_level_min)*XY_COEFF, 0);
    // double xstart = x_max + orig_level_min*XY_COEFF;
    double xstart = x_max + round(orig_level_min_this_cycle, 0)*XY_COEFF;
    double height = 10;
    double ystart = 0; // y_max - height;

    if (width < 0) return;

    if (((xstart<x_max)) || (mode == PROGRESS_CLEAR)) {
        string t = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" +
        "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 " + x_max + " " + y_max + "\">\n";
        if (mode != PROGRESS_CLEAR) t += "<rect fill=\"#00bb00\" class=\"progress\" width=\""+width+"px\" height=\""+height+"px\" x=\""+xstart+"px\" y=\""+ystart+"px\" />";
        t += "</svg>";

        if ((ms_since_last_progress_save > 499) || (mode == PROGRESS_CLEAR)) {
            file f;
            if(f.open(snapTRDir+"progress.svg","w")>=0) {
                f.writeString(t); f.close();
                ms_since_last_progress_save = 0;
                // print("saved progress " + xstart + " " +width);
            }
        }
    }
}

// save data for analysis
void saveSnapShot(){

    // print ("ms: " + ms_to_save_in_snapshot);

    double x_max = SVG_WIDTH;
    double y_max = x_max;
    double XY_COEFF = SVG_WIDTH/-FLOOR_LEVEL; // size of 1 Db for positioning

    double y_offset = 0; // 10*(x_max/100)/2;
    double x, y, x1, y1, x2, y2;
    string t, t1, t1grid, t1testtone, t2, t3, t4, t5, tref; // pieces of code for export
    
    
    double xmoreinfo = XY_COEFF*2.5; // x position for more-info block

    t += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" +
    "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 " + x_max + " " + y_max + "\">\n";

    t += "\n<style>\n" +
        "\t"+"rect.wholespace {fill:#EEEEEE;stroke-width:0.5;stroke:rgb(0,0,0);}\n" +
        
        "\t"+"text {font: 120% sans-serif;}\n" +
        "\t"+"text.gridtext {font: 100% sans-serif;fill: #aaaaaa;}\n" +
        "\t"+"text.title {fill: #004040; font: 300% bold sans-serif; text-anchor: middle; opacity: 0.9; }\n" +
        "\t"+"text.text_b {fill: #015965; font: 185% sans-serif; opacity: 1; }\n" +
        "\t"+"text.text_go {fill: #5875B7; font: 140% sans-serif; opacity: 0.8; }\n" +
        "\t"+"text.text_ref { fill: #7A0000; font: 140% sans-serif; opacity: 0.9; }\n" +
        "\t"+"text.text_c {fill: #444; font: 150% sans-serif; opacity: 0.9;  }\n" +
        "\t"+"text.text_d {fill: #015965; font: 140% sans-serif; opacity: 0.6;  }\n" +

        "\t"+"line {stroke-width:3;}\n" +
        "\t"+"line.gridda { vector-effect: non-scaling-stroke; stroke:#555555; opacity: 0.4; stroke-width:1;stroke-dasharray:10,3; }\n" +
        "\t"+"line.griddb { vector-effect: non-scaling-stroke; stroke:#555555; opacity: 0.4; stroke-width:1;stroke-dasharray:2,6; }\n" +
        "\t"+"line.thr_man {stroke:#FF0000; opacity: 0.85; stroke-width:2; }\n" +
        "\t"+"line.thr_auto {stroke:#00DD00; opacity: 0.85; stroke-width:2; }\n" +
        "\t"+"circle.dot { fill: #015965; }\n"+

        "\t"+"line.att_mes { stroke:#FF1300; opacity: 0.6; stroke-width:2; }\n" +
        "\t"+"line.ratio { stroke:#015965; opacity: 0.3; stroke-width:2; }\n" +
        "\t"+"line.knee { stroke:#CD0074; opacity: 0.5; stroke-width:2; }\n" +

        "\t"+"polyline { fill: transparent; stroke-linejoin:round; stroke-width:2; }\n" +
        "\t"+"polyline.testtone { stroke: #AAAAAA; stroke-width:1; }\n" +
        "\t"+"polyline.curve_true { stroke: #5875B7; }\n" +
        "\t"+"polyline.curve_offset_compensated { stroke: #06266F; }\n" +
        "\t"+"polyline.curve_ref { stroke: #DD0000; stroke-width:3; opacity: 0.5; }\n" +
    "</style>\n\n";
    
    string tend = "\n</svg>";

    t1 += "<rect class=\"wholespace\" width=\"100%\" height=\"100%\" />\n";

    t1grid += "\n<!-- Grid background -->\n";
    // draw db grid
    string classchar = "a";
    double db_divs = (floor(-FLOOR_LEVEL/10)*2);
    double db_text = 0;
    double grid_y = y_offset;
    double grid_x = x_max;
    for( int n = 1; n <= db_divs+1; n++ ) {
        if (!((n==1) && (y_offset == 0))) { // skip first line if y_offset==0
            classchar = "b";
            if ((n % 2) != 0) {
                classchar = "a";
                t1grid += "<text class='gridtext' x=\""+(x_max-57)+"\" y=\""+(grid_y-8)+"\">"+db_text+" dB</text>\n";
                if ((n-1) % 4 == 0) { t1grid += "<text class='gridtext' y=\""+(y_max-8)+"\" x=\""+(grid_x+8)+"\">"+db_text+" dB</text>\n"; }
            }
            t1grid += "<line class=\"gridd"+classchar+"\" x1=\""+0+"\" x2=\""+x_max+"\" y1=\""+grid_y+"\" y2=\""+grid_y+"\" />\n";
            t1grid += "<line class=\"gridd"+classchar+"\" y1=\""+0+"\" y2=\""+y_max+"\" x1=\""+grid_x+"\" x2=\""+grid_x+"\" />\n";
            }
        
        grid_y += (y_max/db_divs);
        grid_x -= (x_max/db_divs);
        db_text -= 5;
    }


    // prepare curve
    string orig_points = "";
    string comp_points_true = "";
    string comp_points_offset_compensated = "";
    string y_val, x_val;

    int nstart = roundDoubleToInt(orig_level_min);
    int nend = roundDoubleToInt(orig_level_max);
    int i;
    int gdlength = gain_diffs.length;
    

    // nstart = -50;
    // nend = -10;
    int floor_lvl = roundDoubleToInt(FLOOR_LEVEL);
    double orig_lvl, comp_lvl_offset_compensated, comp_lvl_true;
    int gainoffsetpos = 6;
    if (gainoffsetpos < -nend) {
        gainoffsetpos = -nend;
    }
    double ygainoffset = 0;
    double xgainoffset = x_max-XY_COEFF*(gainoffsetpos+2);
    double kneestartx = -100; 
    double kneeendx = -100;
    string raw_data = "";
    double this_r = 0;
    raw_data += "Test tone (dB);Comp-offset ("+gain_offset+" dB);Over threshold (dB);Gain diff (dB);Ratio;Comp true (dB);\n";

    bool rawdata_threshold_added = false;

    for( int n = nstart; n <= nend; n++ ) {

        orig_lvl = n;
        i = -n;
        if ((i>=0) && (i<gdlength)) {
            comp_lvl_offset_compensated = orig_lvl-gain_diffs[i]-gain_offset;
            comp_lvl_true = orig_lvl-gain_diffs[i];
            if (i == gainoffsetpos) {
                ygainoffset = -comp_lvl_true*XY_COEFF+y_offset-XY_COEFF;
            }
        

            x_val = (orig_lvl-floor_lvl)*XY_COEFF;
            y_val = y_max-(comp_lvl_true-floor_lvl)*XY_COEFF;
            comp_points_true = comp_points_true + x_val + "," + y_val + " ";
            y_val = y_max-(comp_lvl_offset_compensated-floor_lvl)*XY_COEFF;
            comp_points_offset_compensated = comp_points_offset_compensated + x_val + "," + y_val + " ";
            y_val = y_max-(orig_lvl-floor_lvl)*XY_COEFF;
            orig_points = orig_points + x_val + "," + y_val + " ";
            
            if (abs(knee_start-orig_lvl) < 1) {
                if (kneestartx < 0) {
                    kneestartx = (orig_lvl-floor_lvl)*XY_COEFF;;
                }
            }
            if (abs((knee_start+knee*2)-orig_lvl) < 1) {
                if (kneeendx < 0) {
                    kneeendx = (orig_lvl-floor_lvl)*XY_COEFF;;
                }
            }

            // adding RAW DATA line
            if (current_saving_mode == SNAPSHOT_MODE_NORMAL_WITH_RAWDATA) {
                raw_data += round(orig_lvl) + ";" + round(comp_lvl_offset_compensated) + ";"; // test tone level
                this_r = round(orig_lvl-threshold)/(comp_lvl_offset_compensated-threshold); // comp level-offset
                raw_data += round(orig_lvl-threshold)+";"; // over threshold
                raw_data += round(orig_lvl-comp_lvl_offset_compensated)+";"; // gain diff
                raw_data += round(this_r)+";"; // ratio
                raw_data += round(comp_lvl_true) + ";"; // comp level true
                if ((knee>0) && (orig_lvl > knee_start) && ((orig_lvl-knee_start)<1)) {
                    raw_data += "KNEE START (size ~" + knee + " dB);";
                }
                if ((orig_lvl >= threshold) && !rawdata_threshold_added) {
                    raw_data += "THRESHOLD " + threshold + " dB;";
                    rawdata_threshold_added = true;
                }
                raw_data += "\n";
            }
        }
    }

    t1testtone += "\n<!-- Level curves -->\n<polyline class=\"testtone\" vector-effect=\"non-scaling-stroke\" points=\"" + orig_points + "\" />";
    t2 += "\n\n<polyline class=\"curve_true\" vector-effect=\"non-scaling-stroke\"  points=\"" + comp_points_true + "\" />";
    
    t2 += "\n<text class='text_go' x='"+(xgainoffset)+"' y='"+(ygainoffset)+"'>";
    // if (auto_gain_offset) { t2 += "(auto)"; } else { t2 += "(manual)"; }
    // Gain offset 
    if (gain_offset>0) t2 +="+";
    t2 += "" + round(gain_offset)+" dB";
    if (!auto_gain_offset) t2 += " (man)";
    t2 += "</text>";

    t3 += "\n\n<polyline class=\"curve_offset_compensated\" vector-effect=\"non-scaling-stroke\" points=\"" + comp_points_offset_compensated + "\" />";
    
    double y_ref_text = XY_COEFF*9;
    if (current_saving_mode == SNAPSHOT_MODE_REF_TRUE) {
        tref += "\n\n<polyline class=\"curve_ref\" vector-effect=\"non-scaling-stroke\"  points=\"" + comp_points_true + "\" />\n";
    }
    if (current_saving_mode == SNAPSHOT_MODE_REF_COMP) {
        tref = "\n\n<polyline class=\"curve_ref\" vector-effect=\"non-scaling-stroke\" points=\"" + comp_points_offset_compensated + "\" />\n";
    }

    if ((current_saving_mode == SNAPSHOT_MODE_REF_COMP) || (current_saving_mode == SNAPSHOT_MODE_REF_TRUE)) {
        tref += "\n<text class='text_ref' x='"+xmoreinfo+"' y='"+(y_ref_text)+"'>Gain offset (ref) ";
        if (!auto_gain_offset) tref +="(man)";
        tref += ": ";
        // if (gain_offset>0) tref +="+";
        tref += "" + round(gain_offset)+" dB</text>";
        y_ref_text+=35;
    }


    // text and line threshold
    double yt = (threshold*-XY_COEFF) + y_offset;
    double ytt = yt-20;
    double xtt = x_max - XY_COEFF*(abs(threshold)) - XY_COEFF*16;
    if (!auto_threshold) {  xtt = xtt-XY_COEFF*4; }
    if (xtt < (XY_COEFF)) {
        xtt = XY_COEFF;
    }

    // ratio
    double yrt = ytt+65;
    if (knee > 1) {
        yrt += XY_COEFF*(knee/6);
    }
    double xrt = x_max - XY_COEFF*(abs(threshold)) + XY_COEFF*1;
    double xrtr = xrt;
    double yrtr = yrt+35;

    if (threshold < 0) {
        // for low thresholds
        if (threshold < FLOOR_LEVEL+5) {
            yrt = ytt;
            yrtr = ytt;
            xrt = xtt + XY_COEFF*16;
            if (!auto_threshold) {  xrt = xrt+XY_COEFF*4; }
            xrtr = xrt + XY_COEFF*10;
        } else if (threshold > -12) {
            xtt = xtt - XY_COEFF;
            xrt = xtt;
            xrtr = xrt;
            if (threshold < 5) {
                ytt = yt+40;
                yrt = ytt+40;
                yrtr = yrt+35;
            }
        }
        if (tr_range == 100) {
            xtt = xtt - XY_COEFF*9;
        }

        // manual ratio measurement point
        x = x_max - floor(ratio_meas_point_abs)*XY_COEFF;
        y = ratio_meas_point_abs_uint*XY_COEFF + y_offset;
   
        // after calculation of positions, display em
        t4 += "\n\n<!-- Threshold -->\n<text class='text_b' x='"+xtt+"' y='"+(ytt)+"'>Threshold";
        tref += "\n<text class='text_ref' x='"+xmoreinfo+"' y='"+(y_ref_text)+"'>Threshold (ref)";
        if (!auto_threshold) { t4 += " (manual)"; tref += " (man)"; }
        t4 += ": "+round(threshold)+" dB</text>";
        tref += ": " + round(threshold)+" dB</text>\n";
        y_ref_text+=35;

        t4 += "\n<line class='thr_";
        if (!auto_threshold) { t4 += "man"; } else { t4 += "auto"; }
        t4 += "' x1='0' x2='"+x_max+"' y1='"+yt+"' y2='"+yt+"' />";

        tref += "<text class='text_ref' x='"+xmoreinfo+"' y='"+(y_ref_text)+"'>Ratio (ref)";
        if (!auto_ratio) {
            // for manual ratio
            // y1 - vert point for comp level at ratio_meas_point
            y1 = abs(ratio_meas_point-gain_diffs[ratio_meas_point_abs_uint]-gain_offset)*XY_COEFF + y_offset;
            y2 = y;
            if (y1 > yt) { y2 = y1; }
            double yy1 = y1;
            if (y<yy1) yy1=y;
            if (yt<yy1) yy1=yt;
            double yy2 = y1;
            if (y>yy2) yy2=y;
            if (yt>yy2) yy2=yt;
            t4 += "\n\n<!-- Ratio manual -->\n<circle class='dot' cx='"+x+"' cy='"+y+"' r='4'/>\n";
            t4 += "<circle class='dot' cx='"+x+"' cy='"+y1+"' r='4'/>\n";
            t4 += "<circle class='dot' cx='"+x+"' cy='"+yt+"' r='4'/>\n";
            t4 += "<line class='ratio' x1='"+x+"' x2='"+x+"' y1='"+yy1+"' y2='"+yy2+"' />";
            // if we like arrows on ends of line
            // t4 += "<line class='ratio' marker-start='url(#arrow_a)' marker-end='url(#arrow_b)' x1='"+x+"' x2='"+x+"' y1='"+y+"' y2='"+y1+"' />\n\n";
            // t4 += "  <defs><marker id='arrow_a' markerWidth='10' markerHeight='10' refX='10' refY='3' orient='270' markerUnits='strokeWidth'><path d='M0,0 L0,6 L9,3 z' fill='#015965' /></marker><marker id='arrow_b' markerWidth='10' markerHeight='10' refX='10' refY='3' orient='auto' markerUnits='strokeWidth'><path d='M0,0 L0,6 L9,3 z' fill='#015965' /></marker></defs>";
            
            /* x1= x+10;
            if (x1 > (x_max-200)) { x1 = x-250; }
            y1 = y+30+y_offset;
            if (y1>(y_max-50)) { y1 = y_max-50; } 
            */
            t4 += "\n<text class='text_b' x='"+xrt+"' y='"+(yrt)+"'>Ratio";
            t4 += ": "+round(ratio_manual)+":1</text>";
            t4 += "\n<text class='text_c' x='"+xrtr+"' y='"+(yrtr)+"'>Measured ";
            t4 +=" @ "+(-ratio_meas_point_abs_uint)+"dB</text>\n";

            tref += " (@ "+(-ratio_meas_point_abs_uint)+"dB)";
            tref += ": "+round(ratio_manual)+":1";
            tref += "</text>\n";
            

        } else {
        // for automatic ratio
            t4 += "\n\n<!-- Ratio -->\n<text class='text_b' x='"+xrt+"' y='"+(yrt)+"'>Ratio";
            t4 += ": "+round(ratio_calculated)+":1</text>\n";

            tref += ": "+round(ratio_calculated)+":1</text>\n";
        }
    }

    // knee marks
    double kneemarkwidth = 1;
    if (knee > 0.5) {
        if (kneestartx > 0) {
            double kneestarty = (-knee_start)*XY_COEFF;
            double kneesx1 = (kneestartx-XY_COEFF*(kneemarkwidth-0.5));
            double kneesx2 = (kneestartx+XY_COEFF*(kneemarkwidth+0.5));
            t5 += "\n<!-- Knee marks -->\n<line class='knee";
            t5 += "' x1='"+kneesx1+"' x2='"+kneesx2+"' y1='"+kneestarty+"' y2='"+kneestarty+"' />\n";
        }
        if (kneeendx > 0) {
            double kneeendy = (-knee_start-knee*2)*XY_COEFF;
            double kneeex1 = (kneeendx-XY_COEFF*(kneemarkwidth-0.5));
            double kneeex2 = (kneeendx+XY_COEFF*(kneemarkwidth+0.5));
            t5 += "<line class='knee";
            t5 += "' x1='"+kneeex1+"' x2='"+kneeex2+"' y1='"+kneeendy+"' y2='"+kneeendy+"' />\n";
        }
    }

    // more info
    double running_time = round((round(ms_from_start)/1000));
    string running_time_hms = getHMS();

    t5 += "\n<!-- Extra info -->\n<text class='text_d' x='"+xmoreinfo+"' y='"+(y_offset+60)+"'>Timestamp: " + running_time_hms + "</text>";
    t5 += "\n<text class='text_d' x='"+xmoreinfo+"' y='"+(y_offset+95)+"'>Frequency: "+frequency+" Hz</text>";
    t5 += "\n<text class='text_d' x='"+xmoreinfo+"' y='"+(y_offset+130)+"'>Step duration: "+step_duration+" ms</text>\n";

    double xsi = x_max - XY_COEFF*19;
    double ysi = y_max - XY_COEFF*7;
    if (tr_range == 100) {
        xsi = x_max - XY_COEFF*29;
        ysi = y_max - XY_COEFF*9;
    }
    
    /* 
    if (!auto_ratio) {
        t4 += "\n<text class='text_c' x='"+xsi+"' y='"+ysi+"'>Auto measured ratio: ";
        t4 += round(ratio_calculated) +"</text>\n";
        ysi+=35;
    } */
    if ((ratio_calc_min<ratio_calc_max) || (!auto_ratio)) {
        t5 += "\n<text class='text_c' x='"+xsi+"' y='"+ysi+"'>Ratios: ";
        t5 += round(ratio_calc_min) +" .. "+round(ratio_calc_max);
        if (!auto_ratio) { t5 += " &#160;&#160;Auto: " + round(ratio_calculated) + ":1" + ""; }
        t5 += "</text>";
        ysi+=35;
    }
    t5 += "\n<text class='text_c' x='"+xsi+"' y='"+(ysi)+"'>Knee (approx)";
    t5 += ": " + round(knee,0)+" dB</text>";
    ysi+=35;

    t5 += "\n<text class='text_c' x='"+xsi+"' y='"+(ysi)+"'>Gain offset ";
    if (auto_gain_offset) { t5 += "(auto)"; } else { t5 += "(manual)"; }
    t5 += ": ";
    // if (gain_offset>0) t5 +="+";
    t5 += round(gain_offset)+" dB</text>\n";
    ysi+=35;

    file f;

    // normal and continuous modes
    if ((current_saving_mode == SNAPSHOT_MODE_NORMAL) || (current_saving_mode == SNAPSHOT_MODE_NORMAL_WITH_RAWDATA) || (current_saving_mode == SNAPSHOT_MODE_LIVE)) {
        if(f.open(snapTRDir+"threshold_ratio.svg","w")>=0) {
            string snapShotTitle = "\n<!-- The title -->\n<text class='title' x='600' y='90'>Compressor snapshot</text>\n";
            f.writeString(t+t1+snapShotTitle+t1grid+t1testtone+t2+t3+t4+t5+tend);
            f.close();
        } 
        if(f.open(snapTRDir+"background.svg","w")>=0) {
            f.writeString(t+t1+t1grid+t1testtone+tend); f.close();
        }
        if(f.open(snapTRDir+"curve_true.svg","w")>=0) {
            f.writeString(t+t2+tend); f.close();
        }
        if(f.open(snapTRDir+"curve_compensated.svg","w")>=0) {
            f.writeString(t+t3+tend); f.close();
        }
        if(f.open(snapTRDir+"info_a.svg","w")>=0) {
            f.writeString(t+t4+tend); f.close();
        }
        if(f.open(snapTRDir+"info_b.svg","w")>=0) {
            f.writeString(t+t5+tend); f.close();
        }
        if (current_saving_mode == SNAPSHOT_MODE_NORMAL_WITH_RAWDATA) {
            if(f.open(snapTRDir+"raw_data.csv","w")>=0) {
                f.writeString(raw_data); f.close();
            }
        }
    } else {
        // reference true or compensated or remove ref
        if(f.open(snapTRDir+"curve_ref.svg","w")>=0) {
            if (current_saving_mode == SNAPSHOT_MODE_REMOVE_REF) {
                // removing reference
                f.writeString(""); 
            } else {
                // saving reference
                f.writeString(t+tref+tend); 
            }
            f.close();
        }
    }

    outputParameters[OP_SNAPSHOT_STATE] = 0;
    flag_save_snapshot = false;

    // clear progressBar
    saveProgress(PROGRESS_CLEAR);
}

// read current value of FLOOR_DB from KUIML
int getRangeFromKUIML(){
    int range_read = 60, pos_var, pos_val;
    file f;
    if( f.open(skinFullFileName, "r") >= 0 ) {
      skinData = f.readString(f.getSize()); f.close();
    }
    // if skin file is read
    if (skinData.length > 512) {
        pos_var = skinData.findFirst("<VARIABLE id=\"FLOOR_DB\"", 0);
        if (pos_var > 0) {
            pos_val = skinData.findFirst("value=\"", pos_var);
            range_read = -(parseInt(skinData.substr(pos_val+7, 4)));
        }
    } else {
        // print("failed to read skin file in getRangeFromKUIML");
    }
    if (range_read <= 99) range_read = 60;
    if (range_read > 99) range_read = 100;

    return range_read;
}

// writing some variables to skin file for KUIML to use
void updateKUIML() {
    
    file f;
    // Open the file in 'read' mode
    if (skinData == "") {
        // Read the whole file into the string buffer
        if( f.open(skinFullFileName, "r") >= 0 ) {
          skinData = f.readString(f.getSize()); f.close();
        }
    }


    bool rewrite_needed = false;
    int pos_var, pos_val, pos_var_end, pos_val_end;

    // if skin file is read
    if (skinData.length > 512) {

        /*
        // replacing WORKDIR
        pos_var = skinData.findFirst("<VARIABLE id=\"WORKDIR\"", 0);
        if (pos_var > 0) {
            pos_val = skinData.findFirst("value=\"", pos_var);
            pos_val_end = skinData.findFirst("\"", pos_val+7);
            string curWorkDir = skinData.substr(pos_val+7, pos_val_end-pos_val-7);
            // if working dir in skin doesn't match curdir, replace it in skin
            if (curWorkDir != curDir) {
                pos_var_end = skinData.findFirst("/>", pos_var);
                skinData.erase(pos_var, pos_var_end-pos_var+2);
                skinData.insert(pos_var, "<VARIABLE id=\"WORKDIR\" value=\""+curDir+"\" />");
                rewrite_needed = true;
            } 
        } */

        // replacing range vars
        pos_var = skinData.findFirst("<VARIABLE id=\"FLOOR_DB\"", 0);
        if (pos_var > 0) {
            pos_val = skinData.findFirst("value=\"", pos_var);
            pos_val_end = skinData.findFirst("\"", pos_val+7);
            int curVal = parseInt(skinData.substr(pos_val+7, pos_val_end-pos_val-7));
            if (curVal == -tr_range) {
                // all ok
                // print("NO NEED TO REPLACE RANGE");
            } else {
                // replacing
                int pos_range_end = skinData.findFirst("<!-- range vars end -->", pos_val+7);
                if (pos_range_end > 0) {
                    skinData.erase(pos_var, pos_range_end-pos_var);
                    string s = "<VARIABLE id=\"FLOOR_DB\" value=\""+-tr_range+"\" />";
                    s+= "<VARIABLE id=\"MAN_THR_STEPS\" value=\""+(tr_range*4 + 1)+"\" />";
                    s+= "<VARIABLE id=\"DB_POS_COUNT\" value=\""+(tr_range + 1)+"\" />";
                    s+= "<VARIABLE id=\"GRID_YPOS_COUNT\" value=\""+(int(tr_range/10) + 1)+"\" />"; 
                    skinData.insert(pos_var, s);
                    rewrite_needed = true;
                }
            }
        }

        if (rewrite_needed) {
            if( f.open(skinFullFileName, "w") >= 0 ) {
              f.writeString(skinData); f.close();
              //print("rewritten skin file | reload plugin");
            } else {
              //print("failed to rewrite skin file");
            }
        } else {
            // print("no rewrite needed");
        }

    } else {
        //print("failed to read skin file");
    }

    // print("skinData: " + skinData);
}

void updateFloorLevel(){
    if (tr_range == 100) {
        FLOOR_LEVEL = -100;
    } else {
        FLOOR_LEVEL = -60;
    }
    
    inputParametersMin[IP_THRESHOLD] = FLOOR_LEVEL;
    inputParametersMin[IP_RATIO_MEASURE_POINT] = FLOOR_LEVEL;
    outputParametersMin[OP_COMP_LEVEL] = FLOOR_LEVEL;
    outputParametersMin[OP_ORIG_LEVEL] = FLOOR_LEVEL;
    outputParametersMin[OP_THRESHOLD] = FLOOR_LEVEL;
    outputParametersMax[OP_KNEE] = -FLOOR_LEVEL;
    FLOOR_VALUE = pow(10, FLOOR_LEVEL / 20);


    gain_diffs.resize(-int(FLOOR_LEVEL)+1);
    thresholds.resize(-int(FLOOR_LEVEL)+1);
    ratios.resize(-int(FLOOR_LEVEL)+1);
    
    reset();
}

////////////////////////////////////////
// INPUT-OUTPUT PARAMS READ-WRITE

/* called when input parameters change */
void updateInputParametersForBlock(const TransportInfo@ info)
{
    // auto threshold switch
    bool auto_threshold_before = auto_threshold;
    auto_threshold = false;
    if (inputParameters[IP_AUTO_THRESHOLD] > 0.75) {
        auto_threshold = true;
        threshold = threshold_calculated;
        outputParameters[OP_THRESHOLD] = threshold;
    } 
    // if changed
    if (auto_threshold_before != auto_threshold) {
        calculateAutoRatiosAndKnee();
        if (!auto_threshold) {
            calculateManualRatio();
        }
        auto_threshold_before = auto_threshold;
    }

    // auto ratio switch
    bool auto_ratio_before = auto_ratio;
    auto_ratio = false;
    if (inputParameters[IP_AUTO_RATIO] > 0.75) {
        auto_ratio = true;
        ratio = ratio_calculated;
        outputParameters[OP_RATIO] = ratio;
    }
    // if changed
    if (auto_ratio_before != auto_ratio) {
        if (!auto_ratio) {
            calculateManualRatio();
        }
        auto_ratio_before = auto_ratio;
    }

    // auto gain switch
    auto_gain_offset = false;
    if (inputParameters[IP_AUTO_GAIN_OFFSET] < 0.5) {
        auto_gain_offset = true;
        gain_offset = gain_offset_calculated;
    } else {
        gain_offset = inputParameters[IP_GAIN_OFFSET_MANUAL];
    }

    // read "threshold" in manual mode
    if (!auto_threshold) {
        if (threshold != inputParameters[IP_THRESHOLD]) {
            // if we moved threshold knob
            threshold = inputParameters[IP_THRESHOLD];
            calculateAutoRatiosAndKnee();
            if (ratio_calc_min < ratio_calc_max) {
                outputParameters[OP_RATIO_CALC_MIN] = ratio_calc_min;
                outputParameters[OP_RATIO_CALC_MAX] = ratio_calc_max;
            }
            if (!auto_ratio) {
                calculateManualRatio();
            }
            
        }
        if (error_no == ERROR_THRESHOLD_AUTO_FAILED) { error_no = ERROR_OK; } // clear error "threshold autodetection failed"
    } else {
        threshold = threshold_calculated;
    }

    // read "ratio measure point"
    if (ratio_meas_point != inputParameters[IP_RATIO_MEASURE_POINT]) {
        ratio_meas_point = inputParameters[IP_RATIO_MEASURE_POINT];
        ratio_meas_point_abs = floor(abs(ratio_meas_point));
        ratio_meas_point_abs_uint = roundDoubleToInt(ratio_meas_point_abs);
        calculateManualRatio();
        // print("rmp changed: "+inputParameters[IP_RATIO_MEASURE_POINT] + " ms: " + ms_from_start);
    }
    
    // settled or realtime curve
    settled_curve = false;
    if (inputParameters[IP_CURVE_TYPE] > 0.5) { settled_curve = true; }

    // input channels
    input_channel_comp = roundDoubleToInt(inputParameters[IP_INPUT_CHANNEL_COMPRESSED]);
    input_channel_testtone = roundDoubleToInt(inputParameters[IP_INPUT_CHANNEL_TESTTONE]);

    // paused switch
    output_paused = false;
    if (inputParameters[IP_PAUSE] > 0.75) { 
        output_paused = true;
        flag_save_snapshot = false;
        outputParameters[OP_SNAPSHOT_STATE] = 0;
        outputParameters[OP_ERR_MSG] = ERROR_DATA_PAUSED;
    }

    // snapshot saving mode
    int prev_snapshot_mode = snapshot_mode;
    snapshot_mode = int(round(inputParameters[IP_SAVE_SNAPSHOT_MODE]));

    if ((snapshot_mode != prev_snapshot_mode) && (flag_save_snapshot)) {
        // prevent saving when changing mode
        flag_save_snapshot = false;
        outputParameters[OP_SNAPSHOT_STATE] = 0;
        // clear progressBar
        saveProgress(PROGRESS_CLEAR);
    }
    
    // save snapshot
    int do_save_snap = roundDoubleToInt(inputParameters[IP_SAVE_SNAPSHOT]);
    // print("do_save_snapshot: " + do_save_snapshot);

    if (do_save_snap == SAVE_SNAP) {
        was_saving_live = false;

        // saving in pause mode
        if (output_paused) {
            outputParameters[OP_SNAPSHOT_STATE] = 1;
            saveSnapShot();
            outputParameters[OP_SNAPSHOT_STATE] = 0;
        } else {
            // saving in normal (not paused) mode
            if (flag_save_snapshot != true) {
                // if not saving now, start saving
                current_saving_mode = snapshot_mode;
                flag_save_snapshot = true;
                outputParameters[OP_SNAPSHOT_STATE] = 1;
                if (current_saving_mode == SNAPSHOT_MODE_LIVE) {
                    outputParameters[OP_SNAPSHOT_STATE] = 2;
                } else if (current_saving_mode == SNAPSHOT_MODE_REMOVE_REF) {
                    outputParameters[OP_SNAPSHOT_STATE] = 3; 
                    remove_reference_in_ms = 300;
                    // saveSnapShot();
                }
            } else {
                outputParameters[OP_SNAPSHOT_STATE] = 0;
                flag_save_snapshot = false;
                // clear progressBar
                saveProgress(PROGRESS_CLEAR);
                // print("saved "+ms_from_start);
            }
        }
        // print("Saving snapshot..." + ms_from_start);
    } else if (do_save_snap == SAVE_SNAP_REF_TRUE) { // save reference (true level)
        was_saving_live = ((current_saving_mode == SNAPSHOT_MODE_LIVE) && (flag_save_snapshot));
        flag_save_snapshot = true;
        current_saving_mode = SNAPSHOT_MODE_REF_TRUE;
        outputParameters[OP_SNAPSHOT_STATE] = 1;

    } else if (do_save_snap == SAVE_SNAP_REF_COMP) { // save reference (compensated)
        was_saving_live = ((current_saving_mode == SNAPSHOT_MODE_LIVE) && (flag_save_snapshot));
        flag_save_snapshot = true;
        current_saving_mode = SNAPSHOT_MODE_REF_COMP;
        outputParameters[OP_SNAPSHOT_STATE] = 1;

    } else if (do_save_snap == SAVE_SNAP_REF_REMOVE) { // remove reference
        was_saving_live = ((current_saving_mode == SNAPSHOT_MODE_LIVE) && (flag_save_snapshot));
        flag_save_snapshot = true;
        current_saving_mode = SNAPSHOT_MODE_REMOVE_REF;
        outputParameters[OP_SNAPSHOT_STATE] = 3; // remove ref
        remove_reference_in_ms = 300;
    }

    // change range (floor level)
    int prev_tr_range = tr_range;
    tr_range = (inputParameters[IP_RANGE] > 0.5) ? 100 : 60;
    if (prev_tr_range != tr_range) {
        updateFloorLevel();
        if ((prev_tr_range == 60) || (prev_tr_range == 100)) {
            updateKUIML();
        }
    }

/*
    // reference saving
    int ref_save_mode = roundDoubleToInt(inputParameters[IP_REF_SAVE]);
    if (ref_save_mode == REF_SAVE_TRUE) {
        print ("save ref true");
    } else if (ref_save_mode == REF_SAVE_COMPENSATED) {
        print ("save ref comp");
    } if (ref_save_mode == REF_REMOVE) {
        print ("remove ref");
    }
*/

    // save debug
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

void computeOutputData()
{
    if (!output_paused) {
        outputParameters[OP_FREQ] = frequency;
        outputParameters[OP_ERR_MSG] = error_no;
        outputParameters[OP_GAIN_OFFSET_CALCULATED] = gain_offset_calculated;
        outputParameters[OP_THRESHOLD] = threshold;
        outputParameters[OP_STEP_DURATION] = step_duration;
        outputParameters[OP_KNEE] = knee;
    }
    outputParameters[OP_RATIO] = ratio;
    if (!auto_threshold) {
        outputParameters[OP_THRESHOLD] = threshold;
    }

}


int getTailSize()
{
    return 0;
}

void reset(){

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
    // print("x: " + x + ", r: " + r);
    return r;
}

string getHMS(){
    datetime d;
    string s = formatFloat(d.get_hour(),"0", 2, 0) + ":" + formatFloat(d.get_minute(),"0", 2, 0) + ":" + formatFloat(d.get_second(),"0", 2, 0);
    // print("time: " + s);
    return s;
}

/////////////////////////////////
// DEBUG FUNCTIONS
/////////////////////////////////

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
double old_orig_level;

void debugAddLine(){
    if (production_mode) return; // debug disabled in production mode

    string ms_from_start_text, orig_level_txt;

    ms_from_start_text = "";
    if (debug_old_ms_from_start != ms_from_start) {
        ms_from_start_text = ms_from_start;
        debug_old_ms_from_start = ms_from_start;
    } 

    orig_level_txt = "";
    if (old_orig_level != orig_level) {
        orig_level_txt = orig_level;
        old_orig_level = orig_level;
    }
    debug_data += ms_from_start_text+";"+samples_from_start;
    debug_data += ";"+orig_level_txt+";"+orig_level_ready; // orig_value;
    // debug_data += ";"+samples_since_orig_level_state_change+";"+orig_level_state;
    debug_data += ";"+step_up_probability+";"+cycle_end_probability+";"+orig_level_diff;
    // debug_data += ";"+comp_level+";"+comp_value;
    debug_data += "\n";
}

void debugSaveEnd(){
    if (production_mode) return; // debug disabled in production mode
    
    print("Debug saved " + ms_from_start);
    file f;
    if(f.open(curDir+"debug_tr.csv","w")>=0) {
        f.writeString(debug_data); f.close();
    }
}