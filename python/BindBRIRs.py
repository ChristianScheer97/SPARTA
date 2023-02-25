# NOTE: You need two different versions of the sofar package to run this script, as exporting
#       multiSpeakerBRIR is only possible up to version (0.3.1). You should create two invoronments.
#       One having version (0.3.1) and another one having verion (1.0.0) installed.
#       Let's start with version (0.3.1)...


# STEP 1: import libraries
#%% 
import pyfar as pf
import sofar as sf
import numpy as np

#######################################################################
#             CONVENTION - multiSpeakerBRIR (SOFAR version (0.3.1))
#######################################################################

# STEP 2: Load the data
#%% load sofa files to bind
path = "../assets/BRIRs/WDRBroadcastStudios/"
filenames = ["\BRIR_CR1_KU_MICS_L.sofa", "\BRIR_CR1_KU_MICS_R.sofa"]
brirL = sf.read_sofa(path+filenames[0])
brirR = sf.read_sofa(path+filenames[1])
print(np.shape(brirL.Data_IR))

# STEP 3: Do necessary bind Data_IR and update necessary data elements
#%% 
multiSpeakerBRIR_LR = brirL.copy()
multiSpeakerBRIR_LR.Data_IR = np.append(multiSpeakerBRIR_LR.Data_IR[..., :, :], brirR.Data_IR[..., :, :], axis=2)
multiSpeakerBRIR_LR.EmitterPosition = np.squeeze(np.array([brirL.EmitterPosition, brirR.EmitterPosition]), axis=1)
multiSpeakerBRIR_LR.EmitterUp = np.squeeze(np.array([brirL.EmitterUp, brirR.EmitterUp]), axis=1)
multiSpeakerBRIR_LR.EmitterView = np.squeeze(np.array([brirL.EmitterView, brirR.EmitterView]), axis=1)
multiSpeakerBRIR_LR.Data_Delay = np.append(brirL.Data_Delay, brirR.Data_Delay, axis=2)

# STEP 4: Write bounded files as multiSpeakerBRIR convention
#%%
sf.write_sofa(str(path + "\BRIR_CR1_KU_MICS_multiSpeakerBRIR.sofa"), multiSpeakerBRIR_LR)

#######################################################################
#             CONVENTION - singleRoomMIMOSRIR (SOFAR version (1.0.0))
#######################################################################

# NOTE: Now change to the second environment that holds SOFAR version (1.0.0). Repeat STEP 1-3 (leave out STEP 4) 
#       and continue with the following steps.



# STEP 5: Preprocessing for conversion to singleRoomMIMOSRIR
#%% 
singleRoomMIMOSRIR_LR = multiSpeakerBRIR_LR.copy()
singleRoomMIMOSRIR_LR.delete("GLOBAL_ListenerDescription")
singleRoomMIMOSRIR_LR.delete("GLOBAL_ReceiverDescription")
singleRoomMIMOSRIR_LR.delete("GLOBAL_SourceDescription")
singleRoomMIMOSRIR_LR.delete("GLOBAL_EmitterDescription")
singleRoomMIMOSRIR_LR.ListenerPosition = np.repeat(singleRoomMIMOSRIR_LR.ListenerPosition, 360, axis=0)
singleRoomMIMOSRIR_LR.SourcePosition = np.repeat(singleRoomMIMOSRIR_LR.SourcePosition, 360, axis=0)
singleRoomMIMOSRIR_LR.Data_Delay = singleRoomMIMOSRIR_LR.Data_Delay[:, :, 0:1]

# STEP 6: Convert to newer convention (singleRoomMIMOSRIR)
#%% 
singleRoomMIMOSRIR_LR.upgrade_convention('SingleRoomMIMOSRIR_1.0', True)

# STEP 7: Write singelRoomMIMOSRIR
#%% 
sf.write_sofa(str(path + "\BRIR_CR1_KU_MICS_SingleRoomMIMOSRIR_1.0.sofa"),singleRoomMIMOSRIR_LR)

