/*
 ==============================================================================
 
 This file is part of SPARTA; a suite of spatial audio plug-ins.
 Copyright (c) 2018 - Leo McCormack.
 
 SPARTA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 SPARTA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with SPARTA.  If not, see <http://www.gnu.org/licenses/>.
 
 ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h" 

PluginProcessor::PluginProcessor() : 
	AudioProcessor(BusesProperties()
		.withInput("Input", AudioChannelSet::discreteChannels(64), true)
	    .withOutput("Output", AudioChannelSet::discreteChannels(2), true))
{
	roombinauraliser_create(&hBin);
    
    /* specify here on which UDP port number to receive incoming OSC messages */
    osc_port_ID = DEFAULT_OSC_PORT;
    osc_connected = osc.connect(osc_port_ID);
    /* tell the component to listen for OSC messages */
    osc.addListener(this);
    refreshWindow = true;
    startTimer(TIMER_PROCESSING_RELATED, 80);
}

PluginProcessor::~PluginProcessor()
{
    osc.disconnect();
    osc.removeListener(this);
    
	roombinauraliser_destroy(&hBin);
}

void PluginProcessor::oscMessageReceived(const OSCMessage& message)
{
    /* if rotation angles are sent as an array \ypr[3] */
    if (message.size() == 3 && message.getAddressPattern().toString().compare("ypr")) {
        if (message[0].isFloat32())
            roombinauraliser_setYaw(hBin, message[0].getFloat32());
        if (message[1].isFloat32())
            roombinauraliser_setPitch(hBin, message[1].getFloat32());
        if (message[2].isFloat32())
            roombinauraliser_setRoll(hBin, message[2].getFloat32());
        return;
    }
    
    /* if rotation angles are sent individually: */
    if(message.getAddressPattern().toString().compare("yaw"))
        roombinauraliser_setYaw(hBin, message[0].getFloat32());
    else if(message.getAddressPattern().toString().compare("pitch"))
        roombinauraliser_setPitch(hBin, message[0].getFloat32());
    else if(message.getAddressPattern().toString().compare("roll"))
        roombinauraliser_setRoll(hBin, message[0].getFloat32());
}

void PluginProcessor::setParameter (int index, float newValue)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_enableRotation:  roombinauraliser_setEnableRotation(hBin, (int)(newValue + 0.5f)); break;
            case k_useRollPitchYaw: roombinauraliser_setRPYflag(hBin, (int)(newValue + 0.5f)); break;
            case k_yaw:             roombinauraliser_setYaw(hBin, (newValue-0.5f)*360.0f ); break;
            case k_pitch:           roombinauraliser_setPitch(hBin, (newValue - 0.5f)*180.0f); break;
            case k_roll:            roombinauraliser_setRoll(hBin, (newValue - 0.5f)*180.0f); break;
            case k_flipYaw:         roombinauraliser_setFlipYaw(hBin, (int)(newValue + 0.5f));  break;
            case k_flipPitch:       roombinauraliser_setFlipPitch(hBin, (int)(newValue + 0.5f)); break;
            case k_flipRoll:        roombinauraliser_setFlipRoll(hBin, (int)(newValue + 0.5f)); break;
            case k_numInputs:       roombinauraliser_setNumSources(hBin, (int)(newValue*(float)(MAX_NUM_INPUTS)+0.5)); break;
        }
    }
    /* source direction parameters */
    else{
        index-=k_NumOfParameters;
        float newValueScaled;
        if (!(index % 2)){
            newValueScaled = (newValue - 0.5f)*360.0f;
            if (newValueScaled != roombinauraliser_getSourceAzi_deg(hBin, index/2)){
                roombinauraliser_setSourceAzi_deg(hBin, index/2, newValueScaled);
                refreshWindow = true;
            }
        }
        else{
            newValueScaled = (newValue - 0.5f)*180.0f;
            if (newValueScaled != roombinauraliser_getSourceElev_deg(hBin, index/2)){
                roombinauraliser_setSourceElev_deg(hBin, index/2, newValueScaled);
                refreshWindow = true;
            }
        }
    }
}

void PluginProcessor::setCurrentProgram (int /*index*/)
{
}

float PluginProcessor::getParameter (int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_enableRotation:  return (float)roombinauraliser_getEnableRotation(hBin);
            case k_useRollPitchYaw: return (float)roombinauraliser_getRPYflag(hBin);
            case k_yaw:             return (roombinauraliser_getYaw(hBin)/360.0f) + 0.5f;
            case k_pitch:           return (roombinauraliser_getPitch(hBin)/180.0f) + 0.5f;
            case k_roll:            return (roombinauraliser_getRoll(hBin)/180.0f) + 0.5f;
            case k_flipYaw:         return (float)roombinauraliser_getFlipYaw(hBin);
            case k_flipPitch:       return (float)roombinauraliser_getFlipPitch(hBin);
            case k_flipRoll:        return (float)roombinauraliser_getFlipRoll(hBin);
            case k_numInputs:       return (float)(roombinauraliser_getNumSources(hBin))/(float)(MAX_NUM_INPUTS);
            default: return 0.0f;
        }
    }
    /* source direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return (roombinauraliser_getSourceAzi_deg(hBin, index/2)/360.0f) + 0.5f;
        else
            return (roombinauraliser_getSourceElev_deg(hBin, (index-1)/2)/180.0f) + 0.5f;
    }
}

int PluginProcessor::getNumParameters()
{
	return k_NumOfParameters + 2*MAX_NUM_INPUTS;
}

const String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

const String PluginProcessor::getParameterName (int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_enableRotation:  return "enable_rotation";
            case k_useRollPitchYaw: return "use_rpy";
            case k_yaw:             return "yaw";
            case k_pitch:           return "pitch";
            case k_roll:            return "roll";
            case k_flipYaw:         return "flip_yaw";
            case k_flipPitch:       return "flip_pitch";
            case k_flipRoll:        return "flip_roll";
            case k_numInputs:       return "num_sources";
            default: return "NULL";
        }
    }
    /* source direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return TRANS("Azim_") + String(index/2);
        else
            return TRANS("Elev_") + String((index-1)/2);
    }
}

const String PluginProcessor::getParameterText(int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_enableRotation:  return !roombinauraliser_getEnableRotation(hBin) ? "Off" : "On";
            case k_useRollPitchYaw: return !roombinauraliser_getRPYflag(hBin) ? "YPR" : "RPY";
            case k_yaw:             return String(roombinauraliser_getYaw(hBin));
            case k_pitch:           return String(roombinauraliser_getPitch(hBin));
            case k_roll:            return String(roombinauraliser_getRoll(hBin));
            case k_flipYaw:         return !roombinauraliser_getFlipYaw(hBin) ? "No-Flip" : "Flip";
            case k_flipPitch:       return !roombinauraliser_getFlipPitch(hBin) ? "No-Flip" : "Flip";
            case k_flipRoll:        return !roombinauraliser_getFlipRoll(hBin) ? "No-Flip" : "Flip";
            case k_numInputs:       return String(roombinauraliser_getNumSources(hBin));
            default: return "NULL";
        }
    }
    /* source direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return String(roombinauraliser_getSourceAzi_deg(hBin, index/2));
        else
            return String(roombinauraliser_getSourceElev_deg(hBin, (index-1)/2));
    }
}

const String PluginProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PluginProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 0;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

const String PluginProcessor::getProgramName (int /*index*/)
{
    return String(); 
}


bool PluginProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool PluginProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}


bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

void PluginProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	nHostBlockSize = samplesPerBlock;
    nNumInputs =  getTotalNumInputChannels();
    nNumOutputs = getTotalNumOutputChannels();
	nSampleRate = (int)(sampleRate + 0.5);
    
	roombinauraliser_init(hBin, nSampleRate);
    AudioProcessor::setLatencySamples(roombinauraliser_getProcessingDelay());
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& /*midiMessages*/)
{
    int nCurrentBlockSize = nHostBlockSize = buffer.getNumSamples();
    nNumInputs = jmin(getTotalNumInputChannels(), buffer.getNumChannels());
    nNumOutputs = jmin(getTotalNumOutputChannels(), buffer.getNumChannels());
    float** bufferData = (float**)buffer.getArrayOfWritePointers();
    float* pFrameData[MAX_NUM_CHANNELS];
    int frameSize = roombinauraliser_getFrameSize();

    if((nCurrentBlockSize % frameSize == 0)){ /* divisible by frame size */
        for (int frame = 0; frame < nCurrentBlockSize/frameSize; frame++) {
            for (int ch = 0; ch < buffer.getNumChannels(); ch++)
                pFrameData[ch] = &bufferData[ch][frame*frameSize];

            /* perform processing */
            roombinauraliser_process(hBin, pFrameData, pFrameData, nNumInputs, nNumOutputs, frameSize);
        }
    }
    else
        buffer.clear();
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; 
}

AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (this);
}

//==============================================================================
void PluginProcessor::getStateInformation (MemoryBlock& destData)
{
    XmlElement xml("roombinauraliserPLUGINSETTINGS");
    for(int i=0; i<roombinauraliser_getMaxNumSources(); i++){
        xml.setAttribute("SourceAziDeg" + String(i), roombinauraliser_getSourceAzi_deg(hBin,i));
        xml.setAttribute("SourceElevDeg" + String(i), roombinauraliser_getSourceElev_deg(hBin,i));
    }
    xml.setAttribute("nSources", roombinauraliser_getNumSources(hBin));
    
    if(!roombinauraliser_getUseDefaultHRIRsflag(hBin))
        xml.setAttribute("SofaFilePath", String(roombinauraliser_getSofaFilePath(hBin)));
    
    xml.setAttribute("JSONFilePath", lastDir.getFullPathName());
    
    xml.setAttribute("INTERP_MODE", roombinauraliser_getInterpMode(hBin));
    xml.setAttribute("ENABLE_ROT", roombinauraliser_getEnableRotation(hBin));
    xml.setAttribute("YAW", roombinauraliser_getYaw(hBin));
    xml.setAttribute("PITCH", roombinauraliser_getPitch(hBin));
    xml.setAttribute("ROLL", roombinauraliser_getRoll(hBin));
    xml.setAttribute("FLIP_YAW", roombinauraliser_getFlipYaw(hBin));
    xml.setAttribute("FLIP_PITCH", roombinauraliser_getFlipPitch(hBin));
    xml.setAttribute("FLIP_ROLL", roombinauraliser_getFlipRoll(hBin));
    xml.setAttribute("RPY_FLAG", roombinauraliser_getRPYflag(hBin));
    //xml.setAttribute("HRIRdiffEQ", roombinauraliser_getEnableHRIRsDiffuseEQ(hBin));
    
    xml.setAttribute("OSC_PORT", osc_port_ID);
    
    copyXmlToBinary(xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr) {
        if (xmlState->hasTagName("roombinauraliserPLUGINSETTINGS")) {
            for(int i=0; i<roombinauraliser_getMaxNumSources(); i++){
                if(xmlState->hasAttribute("SourceAziDeg" + String(i)))
                    roombinauraliser_setSourceAzi_deg(hBin, i, (float)xmlState->getDoubleAttribute("SourceAziDeg" + String(i), 0.0f));
                if(xmlState->hasAttribute("SourceElevDeg" + String(i)))
                    roombinauraliser_setSourceElev_deg(hBin, i, (float)xmlState->getDoubleAttribute("SourceElevDeg" + String(i), 0.0f));
            }
            if(xmlState->hasAttribute("nSources"))
               roombinauraliser_setNumSources(hBin, xmlState->getIntAttribute("nSources", 1)); 
            
            if(xmlState->hasAttribute("SofaFilePath")){
                String directory = xmlState->getStringAttribute("SofaFilePath", "no_file");
                const char* new_cstring = (const char*)directory.toUTF8();
                roombinauraliser_setSofaFilePath(hBin, new_cstring);
            }
            
            if(xmlState->hasAttribute("JSONFilePath"))
                lastDir = xmlState->getStringAttribute("JSONFilePath", "");
            
            if(xmlState->hasAttribute("INTERP_MODE"))
                roombinauraliser_setInterpMode(hBin, xmlState->getIntAttribute("INTERP_MODE", 1));
            if(xmlState->hasAttribute("ENABLE_ROT"))
                roombinauraliser_setEnableRotation(hBin, xmlState->getIntAttribute("ENABLE_ROT", 0));
            if(xmlState->hasAttribute("YAW"))
                roombinauraliser_setYaw(hBin, (float)xmlState->getDoubleAttribute("YAW", 0.0f));
            if(xmlState->hasAttribute("PITCH"))
                roombinauraliser_setPitch(hBin, (float)xmlState->getDoubleAttribute("PITCH", 0.0f));
            if(xmlState->hasAttribute("ROLL"))
                roombinauraliser_setRoll(hBin, (float)xmlState->getDoubleAttribute("ROLL", 0.0f));
            if(xmlState->hasAttribute("FLIP_YAW"))
                roombinauraliser_setFlipYaw(hBin, xmlState->getIntAttribute("FLIP_YAW", 0));
            if(xmlState->hasAttribute("FLIP_PITCH"))
                roombinauraliser_setFlipPitch(hBin, xmlState->getIntAttribute("FLIP_PITCH", 0));
            if(xmlState->hasAttribute("FLIP_ROLL"))
                roombinauraliser_setFlipRoll(hBin, xmlState->getIntAttribute("FLIP_ROLL", 0));
            if(xmlState->hasAttribute("RPY_FLAG"))
                roombinauraliser_setRPYflag(hBin, xmlState->getIntAttribute("RPY_FLAG", 0));
            //if(xmlState->hasAttribute("HRIRdiffEQ"))
                //roombinauraliser_setEnableHRIRsDiffuseEQ(hBin, xmlState->getIntAttribute("HRIRdiffEQ", 1));

            if(xmlState->hasAttribute("OSC_PORT")){
                osc_port_ID = xmlState->getIntAttribute("OSC_PORT", DEFAULT_OSC_PORT);
                osc.connect(osc_port_ID);
            }
            
            roombinauraliser_refreshSettings(hBin);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

/* Adapted from the AllRADecoder by Daniel Rudrich, (c) 2017 (GPLv3 license) */
void PluginProcessor::saveConfigurationToFile (File destination)
{
    sources.removeAllChildren(nullptr);
    for (int i=0; i<roombinauraliser_getNumSources(hBin);i++)
    {
        sources.appendChild (ConfigurationHelper::
                             createElement(roombinauraliser_getSourceAzi_deg(hBin, i),
                                          roombinauraliser_getSourceElev_deg(hBin, i),
                                          1.0f, i+1, false, 1.0f), nullptr);
    }
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("Name", var("SPARTA roombinauraliser source directions."));
    char versionString[10];
    strcpy(versionString, "v");
    strcat(versionString, JucePlugin_VersionString);
    jsonObj->setProperty("Description", var("This configuration file was created with the SPARTA roombinauraliser " + String(versionString) + " plug-in. " + Time::getCurrentTime().toString(true, true)));
    jsonObj->setProperty ("GenericLayout", ConfigurationHelper::convertElementsToVar (sources, "Source Directions"));
    //jsonObj->setProperty ("LoudspeakerLayout", ConfigurationHelper::convertLoudspeakersToVar (sources, "Source Directions"));
    Result result = ConfigurationHelper::writeConfigurationToFile (destination, var (jsonObj));
}

/* Adapted from the AllRADecoder by Daniel Rudrich, (c) 2017 (GPLv3 license) */
void PluginProcessor::loadConfiguration (const File& configFile)
{
    int channelIDs[MAX_NUM_INPUTS+1] = {0};
    int virtual_channelIDs[MAX_NUM_INPUTS+1] = {0};
    sources.removeAllChildren(nullptr);
    Result result = ConfigurationHelper::parseFileForGenericLayout (configFile, sources, nullptr);
    //Result result = ConfigurationHelper::parseFileForLoudspeakerLayout (configFile, sources, nullptr);
    if(result.wasOk()){
        int num_srcs, num_virtual_srcs, src_idx, jj;
        num_srcs = num_virtual_srcs = src_idx = jj = 0;
        /* get Channel IDs and find number of directions and virtual directions */
        for (ValueTree::Iterator it = sources.begin(); it != sources.end(); ++it){
            if ( !((*it).getProperty("Imaginary"))){
                num_srcs++; channelIDs[jj] = (*it).getProperty("Channel");
            }
            else{
                virtual_channelIDs[num_virtual_srcs] = (*it).getProperty("Channel");
                num_virtual_srcs++; channelIDs[jj] = -1;
            }
            jj++;
        }
        /* remove virtual channels and shift the channel indices down */
        for(int i=0; i<num_virtual_srcs; i++)
            for(int j=0; j<num_srcs+num_virtual_srcs; j++)
                if(channelIDs[j] == -1)
                    for(int k=j; k<num_srcs+num_virtual_srcs; k++)
                        channelIDs[k] = channelIDs[k+1];
        
        /* then decriment the channel IDs to remove the gaps */
        for(int i=0; i<num_virtual_srcs; i++)
            for(int j=0; j<num_srcs+num_virtual_srcs; j++)
                if( channelIDs[j] > virtual_channelIDs[i]-i )
                    channelIDs[j]--;
        
        /* update with the new configuration  */
        roombinauraliser_setNumSources(hBin, num_srcs);
        for (ValueTree::Iterator it = sources.begin() ; it != sources.end(); ++it){
            if ( !((*it).getProperty("Imaginary"))){
                roombinauraliser_setSourceAzi_deg(hBin, channelIDs[src_idx]-1, (*it).getProperty("Azimuth"));
                roombinauraliser_setSourceElev_deg(hBin, channelIDs[src_idx]-1, (*it).getProperty("Elevation"));
                src_idx++;
            }
        }
    }
}


