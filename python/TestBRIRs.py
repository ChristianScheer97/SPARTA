# NOTE: This script generates a dummy BRIR to check if the room_binauraliser works properly.
#       The dummy BRIR has onl three directions (0, 90, 180).
#       0 degree is the front direction, 90 degree is the left direction and 180 degree is the back direction.
#       Each direction consists of a simple FIR filter.
#       0 degree is a simple dirac impulse that does not change the signal.
#       90 degree is a simple highpass filter.
#       180 degree is a simple bandpass filter.
#       270 degree is a simple lowpass filter.

# STEP 1: import libraries
#%% 
import pyfar as pf
import sofar as sf
import numpy as np
import scipy.signal as signal

# STEP 2: Generate filter coefficients
#%%
fS = 48000  # Sampling rate.
fL = 480  # Cutoff frequency.
fH = 5000  # Cutoff frequency.
N = 128  # Filter length, must be odd.

# Dirac
h = np.zeros(N)
h[int((N - 1) / 2)] = 1
diracFilter = pf.FilterFIR(h, fS)

# Highpass
h_HP = np.sinc(2 * fL / fS * (np.arange(N) - (N - 1) / 2))
h_HP *= np.hamming(N)
h_HP /= np.sum(h_HP)
hpFilter = pf.FilterFIR(h_HP, fS)

# Lowpass
h_LP = np.sinc(2 * fH / fS * (np.arange(N) - (N - 1) / 2))
h_LP *= np.hamming(N)
h_LP /= np.sum(h_LP)
lpFilter = pf.FilterFIR(h_LP, fS)

# Bandpass
h_BP = signal.firwin(N, [500, 1000], pass_zero=False, fs=fS)
bpFilter = pf.FilterFIR(h_BP, fS)


# STEP 3: Generate dummy BRIR
#%%

brir = sf.Sofa(convention='SingleRoomMIMOSRIR')

# Set attributes
brir.ListenerPosition = np.repeat([[0, 0, 0]], 4, axis=0)
brir.ListenerView = np.repeat([[1, 0, 0]], 4, axis=0)
brir.ListenerUp = np.repeat([[0, 0, 1]], 4, axis=0)
brir.SourcePosition = np.repeat([[0, 0, 0]], 4, axis=0)
brir.ReceiverPosition = np.array([[0, 0, 0], [0, 0, 0]])
brir.ReceiverDescriptions = np.array(['L', 'R'])   
brir.ReceiverView = np.array([[1, 0, 0], [1, 0, 0]])
brir.ReceiverUp = np.array([[0, 0, 1], [0, 0, 1]])
brir.EmitterPosition = np.array([[45, 0, 0], [-45, 0, 0]])
brir.EmitterDescriptions = np.array(['L', 'R'])
brir.EmitterView = np.array([[-1, 0, 0], [-1, 0, 0]])
brir.EmitterUp = np.array([[0, 0, 1], [0, 0, 1]])
brir.MeasurementDate = np.repeat([0], 4, axis=0)
brir.Data_Delay = np.zeros((4, 2, 1)) # mrne: 4 directions (measurements), 2 ears (receivers), 1 speaker (emitter)  
brir.Data_SamplingRate = fS

# initialise Data_IR
brir.Data_IR = np.zeros((4, 2, N, 2)) # mrne: 4 directions (measurements), 2 ears (receivers), N samples, 2 speakers (emitters)

# 0 degree 
brir.Data_IR[0, 0, :, 0] = diracFilter.coefficients
brir.Data_IR[0, 1, :, 0] = diracFilter.coefficients
brir.Data_IR[0, 0, :, 1] = diracFilter.coefficients
brir.Data_IR[0, 1, :, 1] = diracFilter.coefficients

# 90 degree
brir.Data_IR[1, 0, :, 0] = hpFilter.coefficients
brir.Data_IR[1, 1, :, 0] = hpFilter.coefficients
brir.Data_IR[1, 0, :, 1] = hpFilter.coefficients
brir.Data_IR[1, 1, :, 1] = hpFilter.coefficients

# 180 degree
brir.Data_IR[2, 0, :, 0] = bpFilter.coefficients
brir.Data_IR[2, 1, :, 0] = bpFilter.coefficients
brir.Data_IR[2, 0, :, 1] = bpFilter.coefficients
brir.Data_IR[2, 1, :, 1] = bpFilter.coefficients

# 270 degree
brir.Data_IR[3, 0, :, 0] = lpFilter.coefficients
brir.Data_IR[3, 1, :, 0] = lpFilter.coefficients
brir.Data_IR[3, 0, :, 1] = lpFilter.coefficients
brir.Data_IR[3, 1, :, 1] = lpFilter.coefficients

# STEP 4: Save dummy BRIR
#%%
sf.write_sofa('dummyBRIR.sofa', brir)
