# **************************************************************
# Python Example for Elsys TpcAccess API
# Copyright 2017 Elsys AG
# www.elsys-instruments.com
# Author: Roman Bertschi
# info@elsys-instruments.com
#
# For full documentation of all TpcAccess Parameter see the C++ API 
# documention available on www.elsys-instruments.com
# ***************************************************************
import TpcAccess as EL
import sys
import numpy as np
import time
import TpcAccess_lib as tpc

# Set some constants for this example
BLOCK_LENGTH        = 8192
NUMBER_OF_BLOCKS    = 10

# Set your number of boards and inputs per boards
NumberOfBoards = 1
NumberOfInputs = 8

# Define Device Connection
EL.TPC_BeginSystemDefinition()
deviceId = EL.TPC_AddDevice("192.168.0.126:10010")  # Change to what ever your device IP is
EL.TPC_EndSystemDefinition(5000)

# Reset all settings
EL.TPC_ResetConfiguration()

# Set some of the input settings, see TpcAccess Parameter list for a full list of parameters
for board in range(0, NumberOfBoards):
    for input in range(0, NumberOfInputs):
        EL.TPC_SetParameter(deviceId, board, input, EL.tpc_parInputCoupling,    EL.tpc_inpCouplingAC)   # Set to AC coupling
        EL.TPC_SetParameter(deviceId, board, input, EL.tpc_parRange,            5)    # Set 5V Input Range
        EL.TPC_SetParameter(deviceId, board, input, EL.tpc_parOffset,           40)  # Set 40% Offset
        EL.TPC_SetParameter(deviceId, board, input, EL.tpc_parPhysFactor,        100)  # Scale data with cactor 100
        EL.TPC_SetParameter(deviceId, board, input, EL.tpc_parPhysConstant,      100)  # Add offset
        

        #Set Trigger Board 0, Channel 0, Positive Slope at 1V, 0.1V hysteris
        EL.TPC_SetTrigger(deviceId,board,input,EL.tpc_etrgSlope,EL.tpc_etrgCompPositive,EL.tpc_etrgNone, 1,0.1, 0, 0)	
        
# Set recording mode settings
EL.TPC_SetParameter(deviceId,0,0,EL.tpc_parOperationMode, EL.tpc_opModeBlock)   # MultoBlock Mode
EL.TPC_SetParameter(deviceId,0,0,EL.tpc_parBlkNumberOfBlocks, NUMBER_OF_BLOCKS) # Set Number of Blocks 
EL.TPC_SetParameter(deviceId,0,0,EL.tpc_parBlkBlockLength, BLOCK_LENGTH)        # Set Block length to 8kS
EL.TPC_SetParameter(deviceId,0,0,EL.tpc_parSamplingFrequency, 2000000)          # Set Sampling rate to 1MS/s

# Get device Status
status = EL.TPC_DeviceStatus()
EL.TPC_GetDeviceStatus(deviceId, status, sys.getsizeof(status))

# Readout out measurement number for detection a new measurmement
m = status.measurementNumber

# Start Measurement
EL.TPC_ExecuteSystemCommand(EL.tpc_cmdStart)

# Wait until the measurement number has changed
while True:
   EL.TPC_GetDeviceStatus(deviceId, status, sys.getsizeof(status)) 
   if status.measurementNumber != m:
       break

# Send manual trigger command if you don't have a signal triggering the daq
for block in range(0, NUMBER_OF_BLOCKS):
    EL.TPC_ExecuteSystemCommand(EL.tpc_cmdTrigger)
    time.sleep(0.1)


dataset1 = tpc.getPyDataVoltage(deviceId,0,0,0,status.measurementNumber, 0, BLOCK_LENGTH)   # Data in voltage
dataset2 = tpc.getPyDataPhyiscal(deviceId,0,0,0,status.measurementNumber, 0, BLOCK_LENGTH)  # Data scaled to physical unit

# Get time mete data
tMeta = EL.TPC_TMetaData()
size = sys.getsizeof(EL.TPC_TMetaData()) + 10
EL.TPC_GetTMetaData(deviceId,0,0,status.measurementNumber, tMeta, size)

TriggerSample = tMeta.triggerSample
SamplingRate  = tMeta.sampleRate

''' Scale x Axis to ms '''
TimeScale = 1000

startTime = -TriggerSample /SamplingRate * TimeScale
endTime   = (len(dataset1)-TriggerSample)/SamplingRate * TimeScale
x         = np.arange(startTime, endTime, 1/SamplingRate * TimeScale)



# Make Plot 
import matplotlib.pyplot as plt

#x = np.linspace(0, 8191, 8192)

fig, (ax0, ax1) = plt.subplots(nrows=2, sharex=True)

ax0.plot(x, dataset1)
ax0.grid(True, zorder=5)

ax1.plot(x, dataset2)
ax1.grid(True, zorder=5)
plt.show()












