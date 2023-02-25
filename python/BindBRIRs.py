import pyfar as pf
import sofar as sf
import numpy as np
#%%
path = "../assets/BRIRs/WDRBroadcastStudios/"
filenames = ["\BRIR_CR1_KU_MICS_L.sofa", "\BRIR_CR1_KU_MICS_R.sofa"]

brirL = sf.read_sofa(path+filenames[0])
brirR = sf.read_sofa(path+filenames[1])

# testBRIR = sf.read_sofa("D:\BRIRs\SBSBRIR_x-0pt5y-0pt5.sofa")
print(np.shape(brirL.Data_IR))
#%%
multiSpeakerBRIR = brirL
multiSpeakerBRIR.Data_IR = np.append(multiSpeakerBRIR.Data_IR[..., :, :], brirR.Data_IR[..., :, :], axis=2)
multiSpeakerBRIR.EmitterPosition = np.squeeze(np.array([brirL.EmitterPosition, brirR.EmitterPosition]), axis=1)
multiSpeakerBRIR.EmitterUp = np.squeeze(np.array([brirL.EmitterUp, brirR.EmitterUp]), axis=1)
multiSpeakerBRIR.EmitterView = np.squeeze(np.array([brirL.EmitterView, brirR.EmitterView]), axis=1)
multiSpeakerBRIR.Data_Delay = np.append(brirL.Data_Delay, brirR.Data_Delay, axis=2)
#%%
sf.write_sofa(str(path + "\BRIR_CR1_KU_MICS_multiSpeakerBRIR.sofa"), multiSpeakerBRIR)
#%%
singleRoomMIMOSRIR = multiSpeakerBRIR

singleRoomMIMOSRIR.delete("GLOBAL_ListenerDescription")
singleRoomMIMOSRIR.delete("GLOBAL_ReceiverDescription")
singleRoomMIMOSRIR.delete("GLOBAL_SourceDescription")
singleRoomMIMOSRIR.delete("GLOBAL_EmitterDescription")
singleRoomMIMOSRIR = multiSpeakerBRIR.upgrade_convention('SingleRoomMIMOSRIR_1.0', True)
print(np.shape(singleRoomMIMOSRIR.Data_IR))
#%%
sf.write_sofa(str(path + "\BRIR_CR1_KU_MICS_SingleRoomMIMOSRIR_1.0.sofa"),singleRoomMIMOSRIR)
#%%

def combineEmittersBRIR(input: np.ndarray):
    '''
    Combine single emitter BRIRs in the SOFA format to a
    :param input: Numpy array with the SOFA-Files that shall be binded. Make sure that they are in the
    right order and all follow the same SOFA-convention
    :return: sofar.Sofa file
    '''
    data =  input[0]
    data.Data_IR = np.append(data.Data_IR[..., :, :], input[1:].Data_IR[..., :, :], axis=2)
    return data
#%%

