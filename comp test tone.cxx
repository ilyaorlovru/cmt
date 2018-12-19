//////////////////////////////////////
// CMT Tone Generator
// written by Ilya Orlov (LetiMix) 
// https://www.youtube.com/channel/UCbS0EpAZytfizVvysUluEHQ
// St. Petersburg, Russia, 2018
//////////////////////////////////////
// This plugin is free.
//////////////////////////////////////
// Your donations are welcome:
// PayPal: ilya_orlov@mail.ru
// Thank you!
//////////////////////////////////////

string name="Test Tone Generator";
string description="Version 1.07 (2018-12-14)"; 
string author="Ilya Orlov (LetiMix)";

// predefined values
const array<string> freq_presets = {
    "40 Hz", "80 Hz", "150 Hz", "250 Hz", "500 Hz", "1000 Hz", 
    "1500 Hz", "2300 Hz", "3000 Hz", "5000 Hz", "7000 Hz", "9000 Hz", "12500 Hz", "15000 Hz",
    "24 smp", "20 smp", "16 smp", "12 smp", "8 smp", "           smp"
};

// durations presets for attack-release high level
const array<string> allowed_durs_high = {
    "Off", "              ms", "0.100 s", "0.150 s", "0.200 s", "0.250 s", "0.3 s", "0.4 s", "0.5 s", "0.6 s", 
    "0.7 s", "0.8 s", "0.9 s", "1 s", "1.2 s", "1.5 s", "1.8 s"};

// durations presets for attack-release low level
const array<string> allowed_durs_low = {
    "0.100", "0.200", "0.250", "0.3", "0.4", "0.5", "0.6", 
    "0.7", "0.8", "0.9", "1", "1.2", "1.5", "1.8", "2", "3", "4", 
    "5", "6", "7", "8", "9", "10", "15", "20", "30" };

// durations presets for threshold-ratio steps
const array<string> allowed_durs_thrrat_steps = {
    "0.100", "0.150", "0.200", "0.250", "0.3", "0.4", "0.5", "0.6", 
    "0.7", "0.8", "0.9", "1" };

// durations presets for frequency steps
const array<string> allowed_durs_freq_steps = {
    "0.100", "0.150", "0.200", "0.250", "0.3", "0.4", "0.5", "0.6", 
    "0.7", "0.8", "0.9", "1", "5", "10", "20" };

// levels
const int ST_FLOOR_DB = -200; // for static level
const int ST_CEIL_DB = +24;
const int AR_FLOOR_DB = -120; // for attack-release mode
const int AR_CEIL_DB = +24;
const int TR_FLOOR_DB = -100; // for threshold-ratio mode
const int TR_CEIL_DB = 0; 

// misc constants
const int TR_MAX_PAUSE_DURATION = 30; // in steps, for thr-ratio mode
const int FR_MAX_PAUSE_DURATION = 30; // the same for frequency mode

// generator modes
const int MODE_SIMPLE = 0;
const int MODE_ATT_REL = 1;
const int MODE_THR_RATIO = 2;
const int MODE_FREQ_RANGE = 3;

// for sinus generation
const double PI=3.141592653589793238462;
const double E=2.718281828459045235360;

// maximum frequency for current sample rate
double MAX_FREQUENCY = 0; // calculated in initialize
const double MIN_LEVEL = -300; // for output
const double MAX_LEVEL = +24; // for output

// predefined frequencies for freq range mode
array<string> fr_freqs_list = {
    20,   25, 31.5, 40,   50, 63, 80,   100, 125, 160, 
    200, 250, 315,   400, 500, 630,   800, 1000, 1250, 
    1600, 2000, 2500,    3150, 4000, 5000,   6300, 8000, 10000,    12500, 16000, 20000
};

/*
array<string> freqs_predef_short = {
    "31.5", "63", "125", "250", "500", "1000", "2000", "4000", "8000", "16000"
};
*/

/////////////////////////////////

// Input Parameters Shortcuts
const int IP_ONOFF = 0;
const int IP_GEN_MODE = 1;
const int IP_STATIC_LEVEL = 2;

const int IP_FREQ_MODE = 3;
const int IP_FREQ_PRESETS = 4;
const int IP_FREQ_FREE = 5;
const int IP_FREQ_SAMPLES = 6;

const int IP_AR_HIGH_LEVEL = 7;
const int IP_AR_HIGH_DUR = 8;
const int IP_AR_HIGH_DUR_MS = 9;
const int IP_AR_LOW_LEVEL = 10;
const int IP_AR_LOW_DUR = 11;

const int IP_TR_BOTTOM_LEVEL = 12;
const int IP_TR_TOP_LEVEL = 13;
const int IP_TR_STEP_DUR = 14;
const int IP_TR_PAUSE_DUR = 15;

const int IP_FR_START_FREQ = 16;
const int IP_FR_END_FREQ = 17;
const int IP_FR_STEP_DUR = 18;
const int IP_FR_PAUSE_DUR = 19;

// input parameters
// NAMES
array<string> inputParametersNames={"Enable", "Gen Mode", "Static Level"
"Stat Freq Mode", "Stat Freq Presets", "Stat Frequency", "Stat Freq Samples",
"AR High Level", "AR High Time", "AR High Time ms", "AR Low Level", "AR Low Time", 
"TR Min Level", "TR Max Level", "TR Step Duration", "TR Pause Steps",
"FR Start freq", "FR End freq", "FR Step Duration", "FR Pause Steps",
};
array<double> inputParameters(inputParametersNames.length); 
// UNITS
array<string> inputParametersUnits={"", "", "dB",
"", "Hz", "Hz", "smp",
"dB", "", "ms", "dB", "s",
"dB", "dB", "s", "steps",
"Hz", "Hz", "s", "steps"
};
// ENUMS 
array<string> inputParametersEnums={"Off;On", "Simple;Att / Rel;Thr / Ratio;Freq range", "", 
"Freq set;Frequency", join(freq_presets, ";"), "", "",
"", join(allowed_durs_high, ";"), "", "", join(allowed_durs_low, ";"), 
"", "", join(allowed_durs_thrrat_steps, ";"), "",
join(fr_freqs_list, ";"), join(fr_freqs_list, ";"), join(allowed_durs_freq_steps, ";"), ""
};
// FORMAT
array<string> inputParametersFormats={".0", ".0", "+.1",
".0", ".0", ".1", ".0",
".1", ".0", ".2", ".1", ".0", 
".0", ".0", ".0", ".0", 
".0", ".0", ".0", ".0"
};
// MIN
array<double> inputParametersMin={0, MODE_SIMPLE, ST_FLOOR_DB,
0, 0, 0, 4,
AR_FLOOR_DB, 0, 0, AR_FLOOR_DB, 0, 
TR_FLOOR_DB, TR_FLOOR_DB, 0, 1,
0, 0, 0, 0
};
// MAX
array<double> inputParametersMax={1, MODE_THR_RATIO, ST_CEIL_DB,
1, freq_presets.length-1, 384000, 40000,
AR_CEIL_DB, allowed_durs_high.length-1, 100, AR_CEIL_DB, allowed_durs_low.length-1, 
TR_CEIL_DB, TR_CEIL_DB, allowed_durs_thrrat_steps.length-1, TR_MAX_PAUSE_DURATION,
fr_freqs_list.length-1, fr_freqs_list.length-1, allowed_durs_freq_steps.length-1, FR_MAX_PAUSE_DURATION
}; 
// STEPS
array<int> inputParametersSteps={0, roundDoubleToInt(inputParametersMax[IP_GEN_MODE]-inputParametersMin[IP_GEN_MODE]) + 1, ST_CEIL_DB-ST_FLOOR_DB+1,
0, freq_presets.length, 10, roundDoubleToInt(inputParametersMax[IP_FREQ_SAMPLES])/4,
AR_CEIL_DB-AR_FLOOR_DB+1, allowed_durs_high.length, roundDoubleToInt(inputParametersMax[IP_AR_HIGH_DUR_MS])+1, AR_CEIL_DB-AR_FLOOR_DB+1, allowed_durs_low.length,
TR_CEIL_DB-TR_FLOOR_DB+1, TR_CEIL_DB-TR_FLOOR_DB+1, allowed_durs_thrrat_steps.length, roundDoubleToInt(inputParametersMax[IP_TR_PAUSE_DUR])+1,
fr_freqs_list.length, fr_freqs_list.length, allowed_durs_freq_steps.length, roundDoubleToInt(inputParametersMax[IP_FR_PAUSE_DUR])+1
};
// DEFAULT
array<double> inputParametersDefault={1, MODE_ATT_REL, -12,
0, roundDoubleToInt(freq_presets.length-3), 3300, 4,
-5, floor(allowed_durs_high.length / 2 ), 50, -25, floor(allowed_durs_low.length/2-1),
-60, TR_CEIL_DB, floor(allowed_durs_thrrat_steps.length / 4), 4,
0, fr_freqs_list.length-1, floor(allowed_durs_freq_steps.length / 4), 4
}; 

// output parameters
const int OP_MODE = 0;
const int OP_SAMPLERATE = 1;
const int OP_FREQ = 2;
const int OP_LEVEL = 3;
const int OP_PERIOD_LENGTH_SAMPLES = 4;

array<string> outputParametersNames={"Mode", "SampleRate", 
"Frequency", "Level", "Period length"};
array<double> outputParameters(outputParametersNames.length); 
array<string> outputParametersUnits={"", "", 
"Hz", "dB", "smp"};
array<string> outputParametersEnums={"SIMPLE;ATT-RELEASE;THR-RATIO;FREQ RANGE", "", 
"", "", ""};
array<string> outputParametersFormats={"", ".0", 
".1", ".1", ".0"};
array<double> outputParametersMin={MODE_SIMPLE, 1, 
0, -300, 0};
array<double> outputParametersMax={MODE_FREQ_RANGE, 384000, 
192000, MAX_LEVEL, 384000}; 
array<double> outputParametersDefault={MODE_ATT_REL, 1, 
0, 0, 0};

///////////////////////////////////////////////

// switches
bool gen_enabled = false; // is generator enabled (on/off)
bool get_enable_button_on = false; // the state of on/off button
int gen_mode = MODE_ATT_REL; // default generator mode

// Attack Release mode
double ar_high_gain, ar_low_gain; 

int ar_high_dur = 1000; // duration time in samples
int ar_low_dur = 1000;
bool ar_in_high_mode = true;

// Threshold Ratio 
double tr_bottom_level, tr_top_level, tr_current_level;
int tr_step_dur_samples, tr_pause_dur_samples;
bool tr_in_pause = false;

// Frequency
int fr_start_i, fr_end_i;
int fr_step_dur_samples, fr_pause_dur_samples, fr_cur_freq_index;
bool fr_in_pause = false;

// common
double omega, currentPhase, frequency, gain, static_level;
int samples_counter = 0;
int sign = 1; // is signal positive or negative
const double period2pi = 2*PI;
int period_samples_int, period_samples_counter;
double SR;

// flags
bool first_inputparams_update = true;
bool flag_go_off = false;

// initialize values (on start)
void initialize(){
     SR = sampleRate;
     // SR = 88200;

     MAX_FREQUENCY = (SR < 88200) ? 20000.0 : (SR == 88200) ? 40000.0 : 45000.0;

     inputParametersMax[IP_FREQ_FREE] = MAX_FREQUENCY;
     inputParametersSteps[IP_FREQ_FREE] = roundDoubleToInt(MAX_FREQUENCY/100) +1;

     gen_enabled = false;
     gain = 0;
     tr_bottom_level = inputParametersMin[IP_TR_BOTTOM_LEVEL];
     resetToStartValues();
     period_samples_counter = 0;

     if (SR > 48000) {
         fr_freqs_list.insertLast("25000");
         fr_freqs_list.insertLast("32000");
         fr_freqs_list.insertLast("40000");
         // freqs_predef_short.insertLast("32000");
         inputParametersMax[IP_FR_END_FREQ] = fr_freqs_list.length-1;
         inputParametersSteps[IP_FR_END_FREQ] = fr_freqs_list.length;
         inputParametersEnums[IP_FR_END_FREQ] = join(fr_freqs_list, ";");
         inputParametersMax[IP_FR_START_FREQ] = fr_freqs_list.length-1;
         inputParametersSteps[IP_FR_START_FREQ] = fr_freqs_list.length;
         inputParametersEnums[IP_FR_START_FREQ] = join(fr_freqs_list, ";");
    }


}

// process block of data (main function)
void processBlock(BlockData& data)
{
    // if generator is off
    if (!gen_enabled) return;

    // print("gain: "+gain);
    double sampleValue;
    bool zero_crossing;

    // parse block of samples
    for(uint i = 0; i<data.samplesToProcess; i++) {

        // COMMON FOR ALL MODES

        // increase samples counter
        samples_counter++;
        period_samples_counter++;

        // compute sample value
        sampleValue = gain * sin(currentPhase);

        // detect signal sign (polarity) change / zero crossing
        zero_crossing = false;
        if (sampleValue < 0) {
            if (sign > 0) { 
                sign = -1; zero_crossing = true;
                // counting period length in samples
                period_samples_int = period_samples_counter;
                period_samples_counter = 0;
            }
        } else {
            if (sign < 0) { sign = 1; zero_crossing = true; }
        }

        // if we need to turn generator off - on zero crossing or zero level
        if (flag_go_off) {
            if (zero_crossing || (gain == 0)) {
                flag_go_off = false;
                gen_enabled = false;
                break; // quit current samples block
            }
        }

        // DIFFERENT FOR DIFFERENT MODES

        if (gen_mode == MODE_ATT_REL) {
        // ATTACK RELEASE mode
            // if crossing zero, we can change high/low gain modes
            if (zero_crossing) {
                if (ar_in_high_mode) {
                    // if it's time to switch to low gain mode
                    if ((ar_low_dur > 0) and (samples_counter >= ar_high_dur)) {
                       ar_in_high_mode = false;
                       samples_counter = 0;
                       gain = ar_low_gain; // get gain for current AR mode 
                       sampleValue = gain * sin(currentPhase); // recalculate current sample
                    } 
                } else {
                    // if it's time to switch to high gain mode
                    if ((ar_high_dur > 0) and (samples_counter >= ar_low_dur)) {
                       ar_in_high_mode = true;
                       samples_counter = 0;
                       gain = ar_high_gain;
                       sampleValue = gain * sin(currentPhase); // recalculate current sample
                    }
                }
            } 
        } else

        if (gen_mode == MODE_THR_RATIO) {
        // THRESHOLD RATIO mode
            // if crossing zero, of in pause zone, changes can happen
            if ((zero_crossing) or (gain == 0)) {
                // pause mode
                if (tr_in_pause) {
                    if (samples_counter >= tr_pause_dur_samples) {
                        tr_in_pause = false;
                        samples_counter = 0;
                        tr_current_level = tr_bottom_level;
                        gain = pow(10, tr_current_level/20); 
                    }
                } else {
                    // normal generate mode
                    if (samples_counter >= tr_step_dur_samples) {
                        samples_counter = 0;
                        // step is over, add another decibel
                        tr_current_level = tr_current_level + 1;
                        
                        if (tr_current_level > tr_top_level+0.1) {
                            // if signal is over top
                            if (tr_pause_dur_samples > 0) {
                                tr_in_pause = true;
                                gain = 0;
                                sampleValue = 0; // update sample value
                            } else {
                                // if not in pause
                                tr_current_level = tr_bottom_level;
                                gain = pow(10, tr_current_level/20); 
                                sampleValue = gain * sin(currentPhase); // update sample value
                            }
                        } else {
                           // if signal is not over top
                           // recalculate sample
                           gain = pow(10, tr_current_level/20); 
                           sampleValue = gain*sin(currentPhase); 
                        }
                    }
                }
            } // end of if zero_crossing of zero level

        } else if (gen_mode == MODE_FREQ_RANGE) {
            // FREQ RANGE MODE
            // if crossing zero, of in pause zone, changes can happen
            if ((zero_crossing) or (gain == 0)) {
                // pause mode
                if (fr_in_pause) {
                    // if pause is over
                    if (samples_counter >= fr_pause_dur_samples) {
                        fr_in_pause = false;
                        samples_counter = 0;
                        gain = pow(10, static_level / 20); 
                        fr_cur_freq_index++;
                        if (fr_cur_freq_index > fr_end_i) {
                            fr_cur_freq_index = fr_start_i;
                        }
                        // print("fr_cur_freq_index: "+ fr_cur_freq_index);
                        frequency = parseFloat(fr_freqs_list[fr_cur_freq_index]);
                        omega = 2 * PI * frequency / SR;
                    }
                } else {
                    // if step is over
                    if (samples_counter >= fr_step_dur_samples) {
                        fr_in_pause = true;
                        gain = 0;
                        sampleValue = 0;
                        samples_counter = 0;

                        /*
                        if (fr_pause_dur_samples > 0) {
                                fr_in_pause = true;
                                gain = 0;
                                sampleValue = 0; // update sample value
                            } else {
                                // if not in pause
                                fr_current_freq = 0;
                                gain = pow(10, static_level / 20); 
                                sampleValue = gain * sin(currentPhase); // update sample value
                            }

                        samples_counter = 0;
                        fr_current_freq += 1; // go to next frequency
                        
                        // if end freq is reached
                        if (fr_current_freq >= fr_freqs_list.length) {
                            
                        } else {
                           // if signal is not over top
                           // recalculate sample
                           gain = pow(10, tr_current_level/20); 
                           sampleValue = gain*sin(currentPhase); 
                        }
                        */
                    }
                }
            } // end of if zero_crossing of zero level

        }

    // COMMON FOR ALL MODES

    // if frequency is set to zero
    if (frequency == 0) {
        period_samples_int = 0;
        sampleValue = 0;
    }

    // add current sample to all audio outputs
    for(uint ch = 0; ch < audioOutputsCount; ch++) data.samples[ch][i] = sampleValue;
    // update phase
    currentPhase += omega;
    // to avoid overflow, reduce phase
    while(currentPhase > period2pi) currentPhase -= period2pi;

    } // end of loop of parsing block of samples
}

int getTailSize()
{
    return -1;
}

// read input parameters
void updateInputParameters() 
{ 
    int pos;
    
    // on/off 
    bool gen_was_enabled = gen_enabled;
    get_enable_button_on = (inputParameters[IP_ONOFF] > 0.5);
    if (get_enable_button_on != gen_was_enabled) {
        if (!get_enable_button_on) {
            flag_go_off = true; // turn off on zero crossing
        } else {
            resetToStartValues(); // reset all values to start
            gen_enabled = true;
            flag_go_off = false;
        }
    }

    // gen mode
    int old_gen_mode = gen_mode;
    gen_mode = roundDoubleToInt(inputParameters[IP_GEN_MODE]);
    if (gen_mode != old_gen_mode) {
        // on changing mode reset values to start
        resetToStartValues();
        old_gen_mode = gen_mode;
    }
    // print("gen_mode: " +gen_mode);

    // frequency
    double prev_frequency = frequency;
    if (gen_mode != MODE_FREQ_RANGE) {   
        if (inputParameters[IP_FREQ_MODE] < 0.5) {
            // freq presets mode
            pos = roundDoubleToInt(inputParameters[IP_FREQ_PRESETS]);
            if (pos == abs(freq_presets.length-1)) {
                // freq set in samples
                frequency = SR/round(inputParameters[IP_FREQ_SAMPLES]);
            } else {
                // freq preset
                frequency = parseFloat(freq_presets[pos]);
                if (freq_presets[pos].findFirst("smp") > 0) {
                    int smpls = parseInt(freq_presets[pos]);
                    frequency = SR/smpls; // sampleRate/samplePeriod
                }
            }

        } else {
            // free frequency mode
            frequency = round(inputParameters[IP_FREQ_FREE], 1);
        }
        // with known frequency we can calculate omega (angular velocity)
        omega = 2 * PI * frequency / SR;
    }

    // if frequency changed, start from 0 phase
    if (prev_frequency != frequency) {
        currentPhase = 0;
        // print("phase reset to zero on freq "+frequency);
    }


    // level (static)
    if ((gen_mode == MODE_SIMPLE) || (gen_mode == MODE_FREQ_RANGE)) {
        static_level = round(inputParameters[IP_STATIC_LEVEL],1);
        gain = pow(10,static_level/20); 
    }

    // attack release durations
    pos = roundDoubleToInt(inputParameters[IP_AR_HIGH_DUR]);
    if (pos > 0) {
        if (pos == 1) {
            // fine selected duration in ms
            ar_high_dur = roundDoubleToInt(inputParameters[IP_AR_HIGH_DUR_MS] * SR  / 1000);
            // print("ar_high_dur: " + ar_high_dur);
        } else {
            ar_high_dur = roundDoubleToInt(parseFloat(allowed_durs_high[pos])*SR);
        }
    } else {
        ar_high_dur = 0;
    }
    pos = roundDoubleToInt(inputParameters[IP_AR_LOW_DUR]);
    ar_low_dur = roundDoubleToInt(parseFloat(allowed_durs_low[pos])*SR);
    
    // attack release levels
    // convert from db to level using reverse dB formula: gain=10^(gaindB/20)*/ 
    ar_high_gain = pow(10,inputParameters[IP_AR_HIGH_LEVEL]/20); 
    ar_low_gain = pow(10,inputParameters[IP_AR_LOW_LEVEL]/20); 
    if ((gen_mode == MODE_ATT_REL) && (ar_high_dur == 0)) {
        gain = ar_low_gain; // apply low gain if high is off
    } 

    // threshold ratio settings
    tr_bottom_level = round(inputParameters[IP_TR_BOTTOM_LEVEL]);
    tr_top_level = round(inputParameters[IP_TR_TOP_LEVEL]);
    pos = roundDoubleToInt(inputParameters[IP_TR_STEP_DUR]);
    tr_step_dur_samples = roundDoubleToInt(parseFloat(allowed_durs_thrrat_steps[pos])*SR);
    pos = roundDoubleToInt(inputParameters[IP_TR_PAUSE_DUR]);
    tr_pause_dur_samples = tr_step_dur_samples * pos;
    // print("tr_bottom_level: " + tr_bottom_level + " tr_top_level: " + tr_top_level);

    // frequency range settings
    fr_start_i = roundDoubleToInt(inputParameters[IP_FR_START_FREQ]);
    fr_end_i = roundDoubleToInt(inputParameters[IP_FR_END_FREQ]);
    pos = roundDoubleToInt(inputParameters[IP_FR_STEP_DUR]);
    fr_step_dur_samples = roundDoubleToInt(parseFloat(allowed_durs_freq_steps[pos])*SR);
    pos = roundDoubleToInt(inputParameters[IP_FR_PAUSE_DUR]);
    fr_pause_dur_samples = fr_step_dur_samples * pos;
    // print("fr_start_i: " + fr_start_i + " fr_end_i: " + fr_end_i + " fr_step_dur_samples: " + fr_step_dur_samples + " fr_pause_dur_samples: " + fr_pause_dur_samples);

    // if just 
    if (first_inputparams_update) {
        resetToStartValues();
        first_inputparams_update = false;
    }
} 

// send output parameters
void computeOutputData() {
    outputParameters[OP_MODE] = gen_mode;
    outputParameters[OP_FREQ] = (omega * SR) / (2 * PI);
    outputParameters[OP_LEVEL] = round(20.0*log10(gain), 1);
    // outputParameters[OP_FREQ] = frequency;
    outputParameters[OP_SAMPLERATE] = SR;
    outputParameters[OP_PERIOD_LENGTH_SAMPLES] = period_samples_int;
}

// sets values to start values
void resetToStartValues() {
    currentPhase = 0;

    // ar reset
    samples_counter = 0;
    
    // set gain depending on mode
    if (gen_mode == MODE_ATT_REL) {
        gain = ar_low_gain;
        ar_in_high_mode = false;

    } else if (gen_mode == MODE_THR_RATIO) {
        tr_in_pause = false;
        tr_current_level = tr_bottom_level;
        gain = pow(10, tr_current_level/20); 
        
    } else if (gen_mode == MODE_FREQ_RANGE) {
        fr_cur_freq_index = fr_start_i;
        fr_in_pause = false;
        frequency = parseFloat(fr_freqs_list[fr_cur_freq_index]);
        omega = 2 * PI * frequency / SR;
        gain = pow(10, static_level/20); 
    }
}

//////////////////////////
// Utils
//////////////////////////

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




// A way to get frequencies list
void getFreqs(){
    string freqs = "";
    for(uint x = 0; x<34; x++) {
        double f = 19.85940038 * pow(E, 0.2304988929 * x);
        double fr = round(f, 0);
        freqs += fr+ " ";
    }
    print("long list: \n" + freqs);
    freqs = "";
    for(uint x = 1; x<12; x++) {
        double f = 15.63816517 * pow(E, 0.693056315 * x);
        double fr = round(f, 0);
        freqs += fr+ " ";
    }
    print("short list: \n" + freqs);
}


/*
// Experiments with finding best frequencies

double h44, h48, h88, h96;
for (double i=4;i<=48000;i+=4){
    h44 = (44100/i);
    h48 = (48000/i);
    h88 = (88200/i);
    h96 = (96000/i);
    if (
        ((fraction(h44) == 0) || (fraction(h44) == 0.5)) &&
        ((fraction(h48) == 0) || (fraction(h48) == 0.5)) && 
        ((fraction(h88) == 0) || (fraction(h88) == 0.5)) && 
        ((fraction(h96) == 0) || (fraction(h96) == 0.5))
        ) {
        print("i: "+i+ " smp " + h44 + " Hz " + h48 + " Hz " + h88 + " Hz " + h96 + " Hz" );
    }
}

double this_f, close_f;
for (uint i=0;i<freq_presets.length;i++){
    this_f = parseFloat(freq_presets[i]);
    close_f = findCloseFreq(this_f);
    print("f: " + this_f + " " + close_f + " " + (SR/close_f) + " smp");
}

double findCloseFreq(double ofreq){
    double freq_dist = 1000000, this_dist = 1000000, found_freq = 0, this_freq = 0;

    // double prev_freq_dist = ;
    // searching for freq with length *4 samples
    
    for (double i=4;i<=48000;i++){
        this_freq = (SR/i);
        this_dist = abs(this_freq - ofreq);
        if ((this_dist < freq_dist)) {
            freq_dist = abs(ofreq - this_freq);
            found_freq = this_freq;
        } else {
            break;
        }
    }

    return found_freq;
}
*/