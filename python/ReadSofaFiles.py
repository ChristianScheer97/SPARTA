# -*- coding: utf-8 -*-
"""
Created on Sun Mar 19 10:32:53 2023

@author: schee
"""
#%%
import sofar as sf

#%%
folder = "assets/BRIRs/WDRBroadcastStudios/"
filenames = ["BRIR_CR1_KU_MICS_multiSpeakerBRIR.sofa", "BRIR_CR1_KU_MICS_SingleRoomMIMOSRIR_1.0.sofa"]
multispeakerBRIR = sf.io.read_sofa(folder + filenames[0])
singleRoomMIMOSROR = sf.io.read_sofa(folder + filenames[1])

#%%
singleSpeaker_left = sf.io.read_sofa(folder + "BRIR_CR1_KU_MICS_L.sofa")
singleSpeaker_right = sf.io.read_sofa(folder + "BRIR_CR1_KU_MICS_R.sofa")