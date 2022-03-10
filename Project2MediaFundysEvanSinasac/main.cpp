//Evan Sinasac - 1081418
//INFO6046 Media Fundamentals (Project 2)
//main.cpp description:
//				For Project 2, the objective of this project is to make a sensible application that utilizes streamed
//				and sampled sounds that the user can control (or demonstrate) the manipulation of DSPs.
//				I have extended the simple sound board I made for project 1 and added channel groups and DSPs, as per
//				the assignment requirements as well as control to actively manipulate one DSP on each channel group

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <FMOD/fmod.hpp>
#include <rssgl/GLText.h>
#include <vector>

//Globals
GLuint _windowWidth = 1024;
GLuint _windowHeight = 720;

GLFWwindow* _window = NULL;
std::string _appName = "Project 2";

FMOD::System* _system = NULL;
FMOD_RESULT _result = FMOD_OK;

#define TOTAL_SOUNDS 11
#define BUFFER_SIZE 255

char _buffer[BUFFER_SIZE];

RSS::GLText* _text;
GLuint _textRowIndex = 2;


std::vector<FMOD::Sound*> _vecSounds;
std::vector<FMOD::Channel*> _vecChannels;
//FMOD::Sound* _sounds[TOTAL_SOUNDS];
//FMOD::Channel* _channels[TOTAL_SOUNDS];

//Channel groups
FMOD::ChannelGroup* _cgMusic = nullptr;
FMOD::ChannelGroup* _cgSE1 = nullptr;
FMOD::ChannelGroup* _cgSE2 = nullptr;
FMOD::ChannelGroup* _cgSE3 = nullptr;
FMOD::ChannelGroup* _cgMaster = nullptr;
FMOD::ChannelGroup* _selectedChannelGroup = nullptr;
FMOD::ChannelGroup* _playingChannelGroup = nullptr;

unsigned int _cgIndex = 0;
float _pan[5];					//0->Master		1->_cgMusic		2->_cgSE1		3->_cgSE2		4->_cgSE3

//_cgMusic
FMOD::DSP* _dspEcho = nullptr;
FMOD::DSP* _dspMixer = nullptr;
FMOD::DSP* _dspFFT = nullptr;
FMOD::DSP* _dspTransceiver = nullptr;

//_cgSE1
FMOD::DSP* _dspHighPass = nullptr;
FMOD::DSP* _dspLowPass = nullptr;
FMOD::DSP* _dspCompressor = nullptr;
FMOD::DSP* _dspMultibandEq = nullptr;

//_cgSE2
FMOD::DSP* _dspDistortion = nullptr;
FMOD::DSP* _dspNormalize = nullptr;
FMOD::DSP* _dspTremolo = nullptr;
FMOD::DSP* _dspDelay = nullptr;

//_cgSE3
FMOD::DSP* _dspFader = nullptr;
FMOD::DSP* _dspFlange = nullptr;
FMOD::DSP* _dspOscillator = nullptr;
FMOD::DSP* _dspPan = nullptr;

//_cgMaster
FMOD::DSP* _dspChorus = nullptr;
FMOD::DSP* _dspSFXReverb = nullptr;
FMOD::DSP* _dspLimiter = nullptr;
FMOD::DSP* _dspLoudnessMeter = nullptr;

FMOD::DSP* _dspTemp = nullptr;
FMOD_DSP_TYPE _dspType;	// = nullptr;

std::vector<std::string> songNames;


//Callbacks
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float volume = 0.0f;
	bool isPaused = false;
	float stepSize = 0.02f;
	float volumeMax = 1.0f;
	float volumeMin = 0.0f;
	float panMax = 1.0f;
	float panMin = -1.0f;
	int numDSPs = 0;
	bool dspActive = false;
	bool dspBypass = false;

	//dsp modification values
	float echoDelayMin = 10.0f;
	float echoDelayMax = 5000.0f;
	float echoDelay = 0.0f;

	float highpassMin = 1.0f;
	float highpassMax = 22000.0;
	float highpass = 0.0f;

	float tremoloMin = 0.0f;
	float tremoloMax = 20.0f;
	float tremoloFrequency = 0.0f;

	float oscillatorMin = 0.0f;
	float oscillatorMax = 22000.0f;
	float oscillatorRate = 0.0f;

	float chorusMin = 0.0f;
	float chorusMax = 20.0f;
	float chorusRate = 0.0f;

	//Close the window when users press ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	//Keys 1 through 0 and SPACE play the 11 various sounds we've loaded
	//Error checking here to make sure we don't try to reference a sound that hasn't been loaded 
	//i.e. if there were only 5 audio files, only keys 1-5 should work
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgMusic || _selectedChannelGroup == _cgMaster)
		{
			if (0 < _vecSounds.size())
			{
				_playingChannelGroup = _cgMusic;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[0], _cgMusic, false, &_vecChannels[0]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 1
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgMusic || _selectedChannelGroup == _cgMaster)
		{
			if (1 < _vecSounds.size())
			{
				_playingChannelGroup = _cgMusic;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[1], _cgMusic, false, &_vecChannels[1]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 2
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgMusic || _selectedChannelGroup == _cgMaster)
		{
			if (2 < _vecSounds.size())
			{
				_playingChannelGroup = _cgMusic;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[2], _cgMusic, false, &_vecChannels[2]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 3
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgSE1 || _selectedChannelGroup == _cgMaster)
		{
			if (3 < _vecSounds.size())
			{
				_playingChannelGroup = _cgSE1;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[3], _cgSE1, false, &_vecChannels[3]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 4
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgSE1 || _selectedChannelGroup == _cgMaster)
		{
			if (4 < _vecSounds.size())
			{
				_playingChannelGroup = _cgSE1;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[4], _cgSE1, false, &_vecChannels[4]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 5
	else if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgSE1 || _selectedChannelGroup == _cgMaster)
		{
			if (5 < _vecSounds.size())
			{
				_playingChannelGroup = _cgSE1;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[5], _cgSE1, false, &_vecChannels[5]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 6
	else if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgSE2 || _selectedChannelGroup == _cgMaster)
		{
			if (6 < _vecSounds.size())
			{
				_playingChannelGroup = _cgSE2;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[6], _cgSE2, false, &_vecChannels[6]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 7
	else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgSE2 || _selectedChannelGroup == _cgMaster)
		{
			if (7 < _vecSounds.size())
			{
				_playingChannelGroup = _cgSE2;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[7], _cgSE2, false, &_vecChannels[7]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 8
	else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgSE2 || _selectedChannelGroup == _cgMaster)
		{
			if (8 < _vecSounds.size())
			{
				_playingChannelGroup = _cgSE2;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[8], _cgSE2, false, &_vecChannels[8]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 9
	else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgSE3 || _selectedChannelGroup == _cgMaster)
		{
			if (9 < _vecSounds.size())
			{
				_playingChannelGroup = _cgSE3;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[9], _cgSE3, false, &_vecChannels[9]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of 0
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (_selectedChannelGroup == _cgSE3 || _selectedChannelGroup == _cgMaster)
		{
			if (10 < _vecSounds.size())
			{
				_playingChannelGroup = _cgSE3;
				_playingChannelGroup->stop();	//keeps the program only playing one sound on a group at a time
				_result = _system->playSound(_vecSounds[10], _cgSE3, false, &_vecChannels[10]);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to play sound");
				}
			}
		}
	} //end of space

	//Pause if TAB is pressed
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		_result = _selectedChannelGroup->getPaused(&isPaused);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to read pause");
		}
		_result = _selectedChannelGroup->setPaused(!isPaused);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to set pause");
		}
	} //end of P

	//Keypad input (copied from channel-groups sample code)
	if (key == GLFW_KEY_UP /*&& action == GLFW_PRESS*/) {
		_result = _selectedChannelGroup->getVolume(&volume);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to read volume");
		}
		//Clamp volume
		volume = (volume >= volumeMax) ? volumeMax : (volume + stepSize);
		_result = _selectedChannelGroup->setVolume(volume);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to set volume");
		}
	} //end of UP
	else if (key == GLFW_KEY_DOWN /*&& action == GLFW_PRESS*/) {
		_result = _selectedChannelGroup->getVolume(&volume);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to read volume");
		}
		//Clamp volume
		volume = (volume <= volumeMin) ? volumeMin : (volume - stepSize);
		_result = _selectedChannelGroup->setVolume(volume);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to set volume");
		}
	} //end of DOWN
	else if (key == GLFW_KEY_RIGHT /*&& action == GLFW_PRESS*/) {
		_pan[_cgIndex] = (_pan[_cgIndex] >= panMax) ? panMax : (_pan[_cgIndex] + 0.1f);
		_result = _selectedChannelGroup->setPan(_pan[_cgIndex]);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set pan!");
		}
	} //end of RIGHT
	else if (key == GLFW_KEY_LEFT /*&& action == GLFW_PRESS*/) {
		_pan[_cgIndex] = (_pan[_cgIndex] <= panMin) ? panMin : (_pan[_cgIndex] - 0.1f);
		_result = _selectedChannelGroup->setPan(_pan[_cgIndex]);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set pan!");
		}
	} //end of LEFT

	//Changing channel group
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		_selectedChannelGroup = _cgMusic;
		_cgIndex = 1;
	} //end of Z
	else if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		_selectedChannelGroup = _cgSE1;
		_cgIndex = 2;
	} //end of X
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		_selectedChannelGroup = _cgSE2;
		_cgIndex = 3;
	} //end of C
	else if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		_selectedChannelGroup = _cgSE3;
		_cgIndex = 4;
	} //end of V
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		_selectedChannelGroup = _cgMaster;
		_cgIndex = 0;
	} //end of M

	//Changing DSPs active
	//switch active for the first dsp on the selected channel group
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (0 < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
		{
			_result = _selectedChannelGroup->getDSP(0, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getActive(&dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP active from current dsp");
			}
			_result = _dspTemp->setActive(!dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP active from current dsp");
			}
		}
	}
	//switch active for the second dsp on the selected channel group
	else if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (1 < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
		{
			_result = _selectedChannelGroup->getDSP(1, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getActive(&dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP active from current dsp");
			}
			_result = _dspTemp->setActive(!dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP active from current dsp");
			}
		}
	}
	//switch active for the third dsp on the selected channel group
	else if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (2 < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
		{
			_result = _selectedChannelGroup->getDSP(2, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getActive(&dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP active from current dsp");
			}
			_result = _dspTemp->setActive(!dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP active from current dsp");
			}
		}
	}
	//switch active for the fourth dsp on the selected channel group
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (3 < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
		{
			_result = _selectedChannelGroup->getDSP(3, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getActive(&dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP active from current dsp");
			}
			_result = _dspTemp->setActive(!dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP active from current dsp");
			}
		}
	}
	//switch active for every dsp on the selected channel group
	else if (key == GLFW_KEY_T && action == GLFW_PRESS) 
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (_result != FMOD_OK)	
		{
			fprintf(stderr, "Unable to get number of DSPs");
		}
		for (unsigned int i = 0; i < numDSPs; i++)	//go through all the DSPs on the selected channel group and switch the active state
		{
			_result = _selectedChannelGroup->getDSP(i, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getActive(&dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP active from current dsp");
			}
			_result = _dspTemp->setActive(!dspActive);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP active from current dsp");
			}
		}
	}

	//Changing DSPs bypass
	//switch bypass for the first dsp on the selected channel group
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (0 < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
		{
			_result = _selectedChannelGroup->getDSP(0, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getBypass(&dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP Bypass from current dsp");
			}
			_result = _dspTemp->setBypass(!dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP Bypass from current dsp");
			}
		}
	}
	//switch bypass for the second dsp on the selected channel group
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (1 < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
		{
			_result = _selectedChannelGroup->getDSP(1, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getBypass(&dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP Bypass from current dsp");
			}
			_result = _dspTemp->setBypass(!dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP Bypass from current dsp");
			}
		}
	}
	//switch bypass for the third dsp on the selected channel group
	else if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (2 < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
		{
			_result = _selectedChannelGroup->getDSP(2, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getBypass(&dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP Bypass from current dsp");
			}
			_result = _dspTemp->setBypass(!dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP Bypass from current dsp");
			}
		}
	}
	//switch bypass for the fourth dsp on the selected channel group
	else if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (3 < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
		{
			_result = _selectedChannelGroup->getDSP(3, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getBypass(&dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP Bypass from current dsp");
			}
			_result = _dspTemp->setBypass(!dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP Bypass from current dsp");
			}
		}
	}
	//switch bypass for every dsp on the selected channel group
	else if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get number of DSPs");
		}
		for (unsigned int i = 0; i < numDSPs; i++)	//go through all the DSPs on the selected channel group and switch the bypass state
		{
			_result = _selectedChannelGroup->getDSP(i, &_dspTemp);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP from selected channel group");
			}
			_result = _dspTemp->getBypass(&dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to get DSP Bypass from current dsp");
			}
			_result = _dspTemp->setBypass(!dspBypass);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to set DSP Bypass from current dsp");
			}
		}
	}

	//Changing specific dsp parameters
	//dsp echo delay
	if (key == GLFW_KEY_U /*&& action == GLFW_PRESS*/)
	{
		_result = _dspEcho->getParameterFloat(FMOD_DSP_ECHO_DELAY, &echoDelay, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get echo delay");
		}
		echoDelay -= 10.0f;
		echoDelay = (echoDelay <= echoDelayMin) ? echoDelayMin : echoDelay;
		_result = _dspEcho->setParameterFloat(FMOD_DSP_ECHO_DELAY, echoDelay);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set echo delay");
		}
	}
	else if (key == GLFW_KEY_I /* && action == GLFW_PRESS */ )
	{
		_result = _dspEcho->getParameterFloat(FMOD_DSP_ECHO_DELAY, &echoDelay, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get echo delay");
		}
		echoDelay += 10.0f;
		echoDelay = (echoDelay >= echoDelayMax) ? echoDelayMax : echoDelay;
		_result = _dspEcho->setParameterFloat(FMOD_DSP_ECHO_DELAY, echoDelay);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set echo delay");
		}
	} 
	//dsp highpass cutoff
	else if (key == GLFW_KEY_O /* && action == GLFW_PRESS */ )
	{
		_result = _dspHighPass->getParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, &highpass, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get highpass cutoff");
		}
		highpass -= 100.0f;
		highpass = (highpass <= highpassMin) ? highpassMin : highpass;
		_result = _dspHighPass->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, highpass);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set highpass");
		}
	}
	else if (key == GLFW_KEY_P /*&& action == GLFW_PRESS*/)
	{
		_result = _dspHighPass->getParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, &highpass, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get highpass cutoff");
		}
		highpass += 100.0f;
		highpass = (highpass >= highpassMax) ? highpassMax : highpass;
		_result = _dspHighPass->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, highpass);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set highpass");
		}
	}
	//dsp tremolo frequency
	else if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		_result = _dspTremolo->getParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, &tremoloFrequency, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get tremolo");
		}
		tremoloFrequency -= 1.0f;
		tremoloFrequency = (tremoloFrequency <= tremoloMin) ? tremoloMin : tremoloFrequency;
		_result = _dspTremolo->setParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, tremoloFrequency);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set tremolo");
		}
	}
	else if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		_result = _dspTremolo->getParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, &tremoloFrequency, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get tremolo");
		}
		tremoloFrequency += 1.0f;
		tremoloFrequency = (tremoloFrequency >= tremoloMax) ? tremoloMax : tremoloFrequency;
		_result = _dspTremolo->setParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, tremoloFrequency);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set tremolo");
		}
	}
	// dsp oscillator frequency
	else if (key == GLFW_KEY_K /*&& action == GLFW_PRESS*/)
	{
		_result = _dspOscillator->getParameterFloat(FMOD_DSP_OSCILLATOR_RATE, &oscillatorRate, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get oscillator rate");
		}
		oscillatorRate -= 100.0f;
		oscillatorRate = (oscillatorRate <= oscillatorMin) ? oscillatorMin : oscillatorRate;
		_result = _dspOscillator->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, oscillatorRate);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set oscillator rate");
		}
	}
	else if (key == GLFW_KEY_L /*&& action == GLFW_PRESS*/)
	{
		_result = _dspOscillator->getParameterFloat(FMOD_DSP_OSCILLATOR_RATE, &oscillatorRate, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get oscillator rate");
		}
		oscillatorRate += 100.0f;
		oscillatorRate = (oscillatorRate >= oscillatorMax) ? oscillatorMax : oscillatorRate;
		_result = _dspOscillator->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, oscillatorRate);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set oscillator rate");
		}
	}
	//dsp chorus rate
	else if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
	{
		_result = _dspChorus->getParameterFloat(FMOD_DSP_CHORUS_RATE, &chorusRate, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get chorus");
		}
		chorusRate -= 0.1f;
		chorusRate = (chorusRate <= chorusMin) ? chorusMin : chorusRate;
		_result = _dspChorus->setParameterFloat(FMOD_DSP_CHORUS_RATE, chorusRate);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set chorus");
		}
	}
	else if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)
	{
		_result = _dspChorus->getParameterFloat(FMOD_DSP_CHORUS_RATE, &chorusRate, nullptr, 0);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get chorus");
		}
		chorusRate += 0.1f;
		chorusRate = (chorusRate >= chorusMax) ? chorusMax : chorusRate;
		_result = _dspChorus->setParameterFloat(FMOD_DSP_CHORUS_RATE, chorusRate);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to set chorus");
		}
	}

} //end of key_callback

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "FMOD Error(%d): %s", error, description);
}

//Function signatures
bool init();
bool initGLFW();
bool initGL();
bool initFMOD();
void shutdown();

bool loadSounds();

int main(int argc, char** argv)
{
	if (!init())	//if any part doesn't initialize properly we don't want to run the program
	{
		fprintf(stderr, "Unable to initalize app");
		exit(EXIT_FAILURE);
	}
	//Could remove this line since there's apparently a 25 line limit
	_text->addLine("Hello!  Welcome to simple sound board V2.0!", _textRowIndex++);

	GLuint lastKnownRowIndex = _textRowIndex;

	//variables to temporarily store values for displaying purposes
	std::stringstream ss;
	float volume = 0.f;
	float pitch = 0.f;
	bool isPaused = false;

	int numDSPs = 0;
	unsigned int dspIndex = 0;
	bool dspActive = false;
	bool dspBypass = false;

	FMOD::Sound* currentSound = nullptr;
	FMOD::Channel* currentChannel = nullptr;

	float echoDelay = 0.0f;
	float highpassCutoff = 0.0f;
	float tremoloFrequency = 0.0f;
	float oscillatorRate = 0.0f;
	float chorusRate = 0.0f;

	//Main Loop
	while (!glfwWindowShouldClose(_window))
	{
		glClearColor(0.2f, 0.1f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lastKnownRowIndex = _textRowIndex;
		//_text->addLine("-------------------------------------------------------", lastKnownRowIndex++);

		//Go through the songs that were loaded and print them to the screen with the assigned buttons
		if (_selectedChannelGroup == _cgMaster)
		{
			for (unsigned int index = 0; index < TOTAL_SOUNDS; index++)
			{
				if (index < 9)
				{
					ss << "Press key " << (index + 1) << " to play: " << songNames[index];
					_text->addLine(ss.str(), lastKnownRowIndex++);
				}
				else if (index == 9)
				{
					ss << "Press key 0 to play: " << songNames[index];
					_text->addLine(ss.str(), lastKnownRowIndex++);
				}
				else if (index == 10)
				{
					ss << "Press space to play: " << songNames[index];
					_text->addLine(ss.str(), lastKnownRowIndex++);
				}
				else
				{
					ss << "You also added: " << songNames[index];
					_text->addLine(ss.str(), lastKnownRowIndex++);
				}
				ss.str("");
			} //end for
		}
		else if (_selectedChannelGroup == _cgMusic)
		{
			for (unsigned int index = 0; index < 3; index++)
			{
				ss << "Press key " << (index + 1) << " to play: " << songNames[index];
				_text->addLine(ss.str(), lastKnownRowIndex++);
				ss.str("");
			}
		}
		else if (_selectedChannelGroup == _cgSE1)
		{
			for (unsigned int index = 3; index < 6; index++)
			{
				ss << "Press key " << (index + 1) << " to play: " << songNames[index];
				_text->addLine(ss.str(), lastKnownRowIndex++);
				ss.str("");
			}
		}
		else if (_selectedChannelGroup == _cgSE2)
		{
			for (unsigned int index = 6; index < 9; index++)
			{
				ss << "Press key " << (index + 1) << " to play: " << songNames[index];
				_text->addLine(ss.str(), lastKnownRowIndex++);
				ss.str("");
			}
		}
		else if (_selectedChannelGroup == _cgSE3)
		{
			ss << "Press key 0 to play: " << songNames[9];
			_text->addLine(ss.str(), lastKnownRowIndex++);
			ss.str("");
			ss << "Press space to play: " << songNames[10];
			_text->addLine(ss.str(), lastKnownRowIndex++);
			ss.str("");
		}


		_text->addLine("-------------------------------------------------------", lastKnownRowIndex++);
		
		//Get selected channel group name
		_result = _selectedChannelGroup->getName(_buffer, BUFFER_SIZE);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to read selected channel group name");
		}
		ss.str("");
		ss << "Selected channel group name: " << _buffer;
		_text->addLine(ss.str(), lastKnownRowIndex++);

		//Get if selected channel is paused
		_result = _selectedChannelGroup->getPaused(&isPaused);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to read selected channel group pause");
		}
		ss.str("");
		ss << "Selected channel group is paused: " << ((isPaused) ? "Yes" : "No");
		_text->addLine(ss.str(), lastKnownRowIndex++);

		//Get the volume of the selected channel
		_result = _selectedChannelGroup->getVolume(&volume);
		if (_result != FMOD_OK) {
			fprintf(stderr, "Unable to read selected channel group volume");
		}
		ss.str("");
		ss.precision(2);
		ss << "Selected channel group volume: " << volume;
		_text->addLine(ss.str(), lastKnownRowIndex++);

		//Get the pan of the current channel
		ss.str("");
		ss.precision(2);
		ss << "Selected channel group pan: " << _pan[_cgIndex];
		_text->addLine(ss.str(), lastKnownRowIndex++);

		//get the most recently played song/sound effect
		_result = _playingChannelGroup->getChannel(0, &currentChannel);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get channel from channel group\n");
		}
		_result = currentChannel->getCurrentSound(&currentSound);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get current sound from current channel\n");
		}
		_result = currentSound->getName(_buffer, BUFFER_SIZE);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to get name from current sound\n");
		}
		else
		{
			ss.str("");
			ss << "Most Recent Song: " << _buffer;
			_text->addLine(ss.str(), lastKnownRowIndex++);
		}

		_result = _selectedChannelGroup->getNumDSPs(&numDSPs);
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "No DSPs on channel group\n");
		}
		else
		{
			if (dspIndex < numDSPs)		//error check in case DSPs don't load properly but we got here somehow
			{
				ss.str("");
				_result = _selectedChannelGroup->getDSP(dspIndex, &_dspTemp);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to get DSP from current CG\n");
				}
				else
				{
					_dspTemp->getType(&_dspType);
					_dspTemp->getActive(&dspActive);
					_dspTemp->getBypass(&dspBypass);
					if (_selectedChannelGroup == _cgMusic)
					{
						ss.precision(5);
						_result = _dspEcho->getParameterFloat(FMOD_DSP_ECHO_DELAY, &echoDelay, nullptr, 0);
						if (_result != FMOD_OK)
						{
							fprintf(stderr, "Unable to get echo delay");
						}
						ss << "DSP Echo (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled")
							<< " and Delay is: " << echoDelay;
					}
					else if (_selectedChannelGroup == _cgSE1)
					{
						ss.precision(6);
						_result = _dspHighPass->getParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, &highpassCutoff, nullptr, 0);
						if (_result != FMOD_OK)
						{
							fprintf(stderr, "Unable to get highpass cutoff");
						}
						ss << "DSP High Pass (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled")
							<< " and Cutoff is: " << highpassCutoff;
					}
					else if (_selectedChannelGroup == _cgSE2)
					{
						ss << "DSP Distortion (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE3)
					{
						ss << "DSP Fader (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgMaster)
					{
						ss.precision(3);
						_result = _dspChorus->getParameterFloat(FMOD_DSP_CHORUS_RATE, &chorusRate, nullptr, 0);
						if (_result != FMOD_OK)
						{
							fprintf(stderr, "Unable to get chorus rate");
						}
						ss << "DSP Chorus (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled")
							<< " and rate is: " << chorusRate;
					}
					_text->addLine(ss.str(), lastKnownRowIndex++);
				}
			}
			dspIndex++;
			
			if (dspIndex < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
			{
				ss.str("");
				_result = _selectedChannelGroup->getDSP(dspIndex, &_dspTemp);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to get DSP from current CG\n");
				}
				else
				{
					_dspTemp->getType(&_dspType);
					_dspTemp->getActive(&dspActive);
					_dspTemp->getBypass(&dspBypass);
					if (_selectedChannelGroup == _cgMusic)
					{
						ss << "DSP Mixer (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE1)
					{
						ss << "DSP Low Pass (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE2)
					{
						ss << "DSP Normalize (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE3)
					{
						ss << "DSP Flange (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgMaster)
					{
						ss << "DSP SFX Reverb (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					_text->addLine(ss.str(), lastKnownRowIndex++);
				}
			}
			dspIndex++;

			if (dspIndex < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
			{
				ss.str("");
				_result = _selectedChannelGroup->getDSP(dspIndex, &_dspTemp);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to get DSP from current CG\n");
				}
				else
				{
					_dspTemp->getType(&_dspType);
					_dspTemp->getActive(&dspActive);
					_dspTemp->getBypass(&dspBypass);
					if (_selectedChannelGroup == _cgMusic)
					{
						ss << "DSP FFT (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE1)
					{
						ss << "DSP Compressor (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE2)
					{
						ss.precision(3);
						_result = _dspTremolo->getParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, &tremoloFrequency, nullptr, 0);
						if (_result != FMOD_OK)
						{
							fprintf(stderr, "Unable to get tremolo frequency");
						}
						ss << "DSP Tremolo (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled")
							<< "and frequency is: " << tremoloFrequency;
					}
					else if (_selectedChannelGroup == _cgSE3)
					{
						ss.precision(6);
						_result = _dspOscillator->getParameterFloat(FMOD_DSP_OSCILLATOR_RATE, &oscillatorRate, nullptr, 0);
						if (_result != FMOD_OK)
						{
							fprintf(stderr, "Unable to get oscillator rate");
						}
						ss << "DSP Oscillator (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled")
							<< " and rate is: " << oscillatorRate;
					}
					else if (_selectedChannelGroup == _cgMaster)
					{
						ss << "DSP Limiter (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					_text->addLine(ss.str(), lastKnownRowIndex++);
				}
			}
			dspIndex++;

			if (dspIndex < numDSPs)	//error check in case DSPs don't load properly but we got here somehow
			{
				ss.str("");
				_result = _selectedChannelGroup->getDSP(dspIndex, &_dspTemp);
				if (_result != FMOD_OK)
				{
					fprintf(stderr, "Unable to get DSP from current CG\n");
				}
				else
				{
					_dspTemp->getType(&_dspType);
					_dspTemp->getActive(&dspActive);
					_dspTemp->getBypass(&dspBypass);
					if (_selectedChannelGroup == _cgMusic)
					{
						ss << "DSP Transceiver (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE1)
					{
						ss << "DSP Multiband EQ (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE2)
					{
						ss << "DSP Delay (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgSE3)
					{
						ss << "DSP Pan (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					else if (_selectedChannelGroup == _cgMaster)
					{
						ss << "DSP Loudness Meter (= " << _dspType << ") is: " << ((dspActive) ? "active" : "not active")
							<< " and Bypass is: " << ((dspBypass) ? "enabled" : "disabled");
					}
					_text->addLine(ss.str(), lastKnownRowIndex++);
				}
			}
			dspIndex = 0;
		}

		_text->addLine("-------------------------------------------------------", lastKnownRowIndex++);

		ss.str("");
		//Clean up the screen
		while (lastKnownRowIndex < 25)
		{
			_text->addLine("", lastKnownRowIndex++);
		}
		_text->render();

		glfwSwapBuffers(_window);
		glfwPollEvents();
	} //end of while

	shutdown();
	system("pause");

} //end of main

//Function Definitions
bool init()	//unchanged
{
	if (!initGLFW())
	{
		return false;
	}
	if (!initGL())
	{
		return false;
	}

	_text = new RSS::GLText(_appName, _windowWidth, _windowHeight);

	if (!initFMOD())
	{
		return false;
	}

	return true;
} //end of init

bool initGLFW()	//unchanged
{
	if (!glfwInit())
	{
		fprintf(stderr, "Unable to initialize GLFW");
		return false;
	}

	//set error callbacks

	glfwSetErrorCallback(glfw_error_callback);

	_window = glfwCreateWindow(_windowWidth, _windowHeight, _appName.c_str(), NULL, NULL);

	if (!_window)
	{
		fprintf(stderr, "Unable to create GLFW window");
		return false;
	}

	glfwSetKeyCallback(_window, glfw_key_callback);
	glfwMakeContextCurrent(_window);
	glfwSwapInterval(1);

	return true;
} //end of initGLFW

bool initGL()	//unchanged
{
	if (!gladLoadGL(glfwGetProcAddress))
	{
		fprintf(stderr, "Unable to initialize GLAD");
		return false;
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
} //end of initGL

bool initFMOD()
{
	_result = FMOD::System_Create(&_system);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create FMOD system");
		return false;
	}

	_result = _system->init(32, FMOD_INIT_NORMAL, NULL);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to initialize FMOD system");
		return false;
	}

	//Load and make sounds
	if (!loadSounds())
	{
		fprintf(stderr, "Something went wrong with sounds");
		return false;
	}

	//Create groups
	_result = _system->createChannelGroup("Music Group", &_cgMusic);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create channel group");
		return false;
	}

	_result = _system->createChannelGroup("Sound Group 1", &_cgSE1);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create channel group");
		return false;
	}

	_result = _system->createChannelGroup("Sound Group 2", &_cgSE2);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create channel group");
		return false;
	}

	_result = _system->createChannelGroup("Sound Group 3", &_cgSE3);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create channel group");
		return false;
	}

	//Get master channel
	_result = _system->getMasterChannelGroup(&_cgMaster);
	if (_result != FMOD_OK) {
		fprintf(stderr, "Unable to get master channel group");
		return false;
	}

	//Initialize channels and channelgroups(this was the only way I could get it to display the currently playing sound without it getting reeeeeeally mad when you start the program
	_selectedChannelGroup = _cgSE1;
	_result = _system->playSound(_vecSounds[3], _selectedChannelGroup, false, &_vecChannels[3]);

	_selectedChannelGroup = _cgSE2;
	_result = _system->playSound(_vecSounds[6], _selectedChannelGroup, false, &_vecChannels[6]);

	_selectedChannelGroup = _cgSE3;
	_result = _system->playSound(_vecSounds[9], _selectedChannelGroup, false, &_vecChannels[9]);

	_selectedChannelGroup = _cgMaster;
	_result = _system->playSound(_vecSounds[3], _selectedChannelGroup, false, &_vecChannels[3]);

	//Set initial value of _selectedChannelGroup
	_selectedChannelGroup = _cgMusic;		//_cgMaster;
	_playingChannelGroup = _cgMusic;
	_cgIndex = 1;
	
	//Have to populate the channels and channel groups
	_result = _system->playSound(_vecSounds[0], _selectedChannelGroup, false, &_vecChannels[0]);

	//fill the pan values
	for (unsigned int i = 0; i < 5; i++)
	{
		_pan[i] = 0.0f;
	}

	//Create dsp echo on _cgMusic
	_result = _system->createDSPByType(FMOD_DSP_TYPE_ECHO, &_dspEcho);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp echo");
			return false;
	}
	//Append dsp to channel group
	_result = _cgMusic->addDSP(0, _dspEcho);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgMusic\n");
		return false;
	}
	//Enable dsp
	_result = _dspEcho->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspEcho");
		return false;
	}
	//Bypass dsp for now
	_result = _dspEcho->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp mixer on _cgMusic
	_result = _system->createDSPByType(FMOD_DSP_TYPE_MIXER, &_dspMixer);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp mixer");
		return false;
	}
	//Append dsp to channel group
	_result = _cgMusic->addDSP(1, _dspMixer);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgMusic\n");
		return false;
	}
	//Enable dsp
	_result = _dspMixer->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspMixer");
		return false;
	}
	//Bypass dsp for now
	_result = _dspMixer->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp FFT on _cgMusic
	_result = _system->createDSPByType(FMOD_DSP_TYPE_FFT, &_dspFFT);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp FFT");
		return false;
	}
	//Append dsp to channel group
	_result = _cgMusic->addDSP(2, _dspFFT);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgMusic\n");
		return false;
	}
	//Enable dsp
	_result = _dspFFT->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspFFT");
		return false;
	}
	//Bypass dsp for now
	_result = _dspFFT->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp Transceiver on _cgMusic
	_result = _system->createDSPByType(FMOD_DSP_TYPE_TRANSCEIVER, &_dspTransceiver);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp transceiver");
		return false;
	}
	//Append dsp to channel group
	_result = _cgMusic->addDSP(3, _dspTransceiver);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgMusic\n");
		return false;
	}
	//Enable dsp
	_result = _dspTransceiver->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspTransceiver");
		return false;
	}
	//Bypass dsp for now
	_result = _dspTransceiver->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}


	//Create dsp HighPass on _cgSE1
	_result = _system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &_dspHighPass);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp high pass");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE1->addDSP(0, _dspHighPass);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE1\n");
		return false;
	}
	//Enable dsp
	_result = _dspHighPass->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspHighPass");
		return false;
	}
	//Bypass dsp for now
	_result = _dspHighPass->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp LowPass on _cgSE1
	_result = _system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &_dspLowPass);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp low pass");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE1->addDSP(1, _dspLowPass);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE1\n");
		return false;
	}
	//Enable dsp
	_result = _dspLowPass->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspLowPass");
		return false;
	}
	//Bypass dsp for now
	_result = _dspLowPass->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp Compressor on _cgSE1
	_result = _system->createDSPByType(FMOD_DSP_TYPE_COMPRESSOR, &_dspCompressor);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp compressor");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE1->addDSP(2, _dspCompressor);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE1\n");
		return false;
	}
	//Enable dsp
	_result = _dspCompressor->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspCompressor");
		return false;
	}
	//Bypass dsp for now
	_result = _dspCompressor->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp MultibandEQ on _cgSE1
	_result = _system->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &_dspMultibandEq);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp multiband eq");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE1->addDSP(3, _dspMultibandEq);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE1\n");
		return false;
	}
	//Enable dsp
	_result = _dspMultibandEq->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspMultibandEq");
		return false;
	}
	//Bypass dsp for now
	_result = _dspMultibandEq->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}


	//Create dsp distortion on _cgSE2
	_result = _system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &_dspDistortion);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp distortion");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE2->addDSP(0, _dspDistortion);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE2\n");
		return false;
	}
	//Enable dsp
	_result = _dspDistortion->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspDistortion");
		return false;
	}
	//Bypass dsp for now
	_result = _dspDistortion->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp normalize on _cgSE2
	_result = _system->createDSPByType(FMOD_DSP_TYPE_NORMALIZE, &_dspNormalize);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp normalize");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE2->addDSP(1, _dspNormalize);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE2\n");
		return false;
	}
	//Enable dsp
	_result = _dspNormalize->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspNormalize");
		return false;
	}
	//Bypass dsp for now
	_result = _dspNormalize->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp tremolo on _cgSE2
	_result = _system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &_dspTremolo);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp tremolo");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE2->addDSP(2, _dspTremolo);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE2\n");
		return false;
	}
	//Enable dsp
	_result = _dspTremolo->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspTremolo");
		return false;
	}
	//Bypass dsp for now
	_result = _dspTremolo->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp delay on _cgSE2
	_result = _system->createDSPByType(FMOD_DSP_TYPE_DELAY, &_dspDelay);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp delay");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE2->addDSP(3, _dspDelay);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE2\n");
		return false;
	}
	//Enable dsp
	_result = _dspDelay->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspDelay");
		return false;
	}
	//Bypass dsp for now
	_result = _dspDelay->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}


	//Create dsp fader on _cgSE3
	_result = _system->createDSPByType(FMOD_DSP_TYPE_FADER, &_dspFader);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp fader");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE3->addDSP(0, _dspFader);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE3\n");
		return false;
	}
	//Enable dsp
	_result = _dspFader->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspFader");
		return false;
	}
	//Bypass dsp for now
	_result = _dspFader->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp flange on _cgSE3
	_result = _system->createDSPByType(FMOD_DSP_TYPE_FLANGE, &_dspFlange);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp flange");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE3->addDSP(1, _dspFlange);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE2\n");
		return false;
	}
	//Enable dsp
	_result = _dspFlange->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspFlange");
		return false;
	}
	//Bypass dsp for now
	_result = _dspFlange->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp oscillator on _cgSE3
	_result = _system->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &_dspOscillator);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp oscillator");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE3->addDSP(2, _dspOscillator);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE3\n");
		return false;
	}
	//Enable dsp
	_result = _dspOscillator->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspOscillator");
		return false;
	}
	//Bypass dsp for now
	_result = _dspOscillator->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp pan on _cgSE3
	_result = _system->createDSPByType(FMOD_DSP_TYPE_PAN, &_dspPan);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp pan");
		return false;
	}
	//Append dsp to channel group
	_result = _cgSE3->addDSP(3, _dspPan);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgSE3\n");
		return false;
	}
	//Enable dsp
	_result = _dspPan->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspPan");
		return false;
	}
	//Bypass dsp for now
	_result = _dspPan->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}


	//Create dsp chorus on _cgMaster
	_result = _system->createDSPByType(FMOD_DSP_TYPE_CHORUS, &_dspChorus);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp chorus");
		return false;
	}
	//Append dsp to channel group
	_result = _cgMaster->addDSP(0, _dspChorus);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgMaster\n");
		return false;
	}
	//Enable dsp
	_result = _dspChorus->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspChorus");
		return false;
	}
	//Bypass dsp for now
	_result = _dspChorus->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp SFX reverb on _cgMaster
	_result = _system->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &_dspSFXReverb);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp sfx reverb");
		return false;
	}
	//Append dsp to channel group
	_result = _cgMaster->addDSP(1, _dspSFXReverb);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgMaster\n");
		return false;
	}
	//Enable dsp
	_result = _dspSFXReverb->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspSFXReverb");
		return false;
	}
	//Bypass dsp for now
	_result = _dspSFXReverb->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp limiter on _cgSE3
	_result = _system->createDSPByType(FMOD_DSP_TYPE_LIMITER, &_dspLimiter);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp limiter");
		return false;
	}
	//Append dsp to channel group
	_result = _cgMaster->addDSP(2, _dspLimiter);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgMaster\n");
		return false;
	}
	//Enable dsp
	_result = _dspLimiter->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspLimiter");
		return false;
	}
	//Bypass dsp for now
	_result = _dspLimiter->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}

	//Create dsp loudness meter on _cgMaster
	_result = _system->createDSPByType(FMOD_DSP_TYPE_LOUDNESS_METER, &_dspLoudnessMeter);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to create dsp loudness meter");
		return false;
	}
	//Append dsp to channel group
	_result = _cgMaster->addDSP(3, _dspLoudnessMeter);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to add dsp to _cgMaster\n");
		return false;
	}
	//Enable dsp
	_result = _dspLoudnessMeter->setActive(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to enable _dspLoudnessMeter");
		return false;
	}
	//Bypass dsp for now
	_result = _dspLoudnessMeter->setBypass(true);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to set dsp bypass");
		return false;
	}
	

	//_selectedChannelGroup = _cgMaster;
	return true;
} //end of initFMOD

void shutdown()
{
	glfwTerminate();

	//release sounds
	for (unsigned int index = 0; index != _vecSounds.size(); index++)
	{
		_result = _vecSounds[index]->release();
		if (_result != FMOD_OK)
		{
			fprintf(stderr, "Unable to release sound!\n");
		}
	}

	//Remove and release everything on _cgMusic
	_cgMusic->removeDSP(_dspEcho);
	_result = _dspEcho->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgMusic->removeDSP(_dspMixer);
	_result = _dspMixer->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgMusic->removeDSP(_dspFFT);
	_result = _dspFFT->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgMusic->removeDSP(_dspTransceiver);
	_result = _dspTransceiver->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}

	//Remove and release everything on _cgSE1
	_cgSE1->removeDSP(_dspHighPass);
	_result = _dspHighPass->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE1->removeDSP(_dspLowPass);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE1->removeDSP(_dspCompressor);
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE1->removeDSP(_dspMultibandEq);
	_result = _dspMultibandEq->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}

	//Remove and release everything on _cgSE2
	_cgSE2->removeDSP(_dspDistortion);
	_result = _dspDistortion->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE2->removeDSP(_dspNormalize);
	_result = _dspNormalize->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE2->removeDSP(_dspTremolo);
	_result = _dspTremolo->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE2->removeDSP(_dspDelay);
	_result = _dspDelay->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}

	//Remove and release everything on _cgSE3
	_cgSE3->removeDSP(_dspFader);
	_result = _dspFader->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE3->removeDSP(_dspFlange);
	_result = _dspFlange->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE3->removeDSP(_dspOscillator);
	_result = _dspOscillator->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgSE3->removeDSP(_dspPan);
	_result = _dspPan->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}

	//Remove and release everything on _cgMaster
	_cgMaster->removeDSP(_dspChorus);
	_result = _dspChorus->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgMaster->removeDSP(_dspSFXReverb);
	_result = _dspSFXReverb->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgMaster->removeDSP(_dspLimiter);
	_result = _dspLimiter->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release dsp");
	}
	_cgMaster->removeDSP(_dspLoudnessMeter);
	_result = _dspLoudnessMeter->release();

	//Release channel groups
	_result = _cgMusic->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release _cgMusic");
	}
	_result = _cgSE1->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release _cgSE1");
	}
	_result = _cgSE2->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release _cgSE2");
	}
	_result = _cgSE3->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release _cgSE3");
	}


	_result = _system->close();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to close system");
	}

	_result = _system->release();
	if (_result != FMOD_OK)
	{
		fprintf(stderr, "Unable to release system");
	}

	if (_text) {
		delete _text;
		_text = nullptr;
	}

	exit(EXIT_SUCCESS);
} //end of shutdown

//Modification of loadSounds from project 1, 
bool loadSounds()
{
	std::stringstream ss;
	std::stringstream sFile;
	ss << SOLUTION_DIR << "common\\assets\\audio\\audioList.txt";

	FMOD::Sound* sound;
	FMOD::Channel* channel;

	std::ifstream theFile(ss.str());
	ss.str("");

	if (!theFile.is_open())
	{
		fprintf(stderr, "Could not open audioList.txt");
		return false;
	}
	std::string nextToken;
	while (theFile >> nextToken)
	{
		if (nextToken == "end")
		{
			break;
		}

		if (nextToken.find("mp3") != std::string::npos || nextToken.find("wav") != std::string::npos)
		{
			sFile << nextToken.c_str();
			ss << SOLUTION_DIR << "common\\assets\\audio\\" << sFile.str().c_str();
			songNames.push_back(sFile.str().c_str());
			_vecChannels.push_back(channel);
			_result = _system->createSound(ss.str().c_str(), FMOD_LOOP_OFF, 0, &sound);
			if (_result != FMOD_OK)
			{
				fprintf(stderr, "Unable to create a sound: %s", ss.str().c_str());
			}
			else
			{
				_vecSounds.push_back(sound);
			}
			ss.str("");
			sFile.str("");
		}
		else
		{
			//in case audio file name contains spaces
			sFile << nextToken.c_str() << " ";
		}
	} //end while
	theFile.close();
	return true;
} //end of loadSounds