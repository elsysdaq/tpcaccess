# ******************************************************************************************** 
# Python TpcAccess Helper Modules for reading data accessing TpcAccess API                         
# Copyright 2024 Elsys AG      
# ******************************************************************************************** 
import TpcAccess as EL
import sys
from ctypes import *
import numpy as np

def getPyDataVoltage(deviceIx, board, input, block, measurementNr, start, numberOfData):
    """
    Get the measurement data as double value scaled to voltage for DC, AC and IEPE input types 
    or scaled to pC for Charge input types
    """

    data = EL.new_doubleArray(numberOfData)
    EL.TPC_GetData(deviceIx, board, input, block, measurementNr, start, numberOfData, data)

    p = (c_double * numberOfData).from_address(int(data))
    datalistp = np.array(p)

    return datalistp

def getPyDataPhyiscal(deviceIx, board, input, block, measurementNr, start, numberOfData):
    """
    Get the measurement data scaled to a physical unit defined by the constant and factor
    meta data values
    """
     # read out y meta data for getting the scaling values
    ymetaData = EL.TPC_YMetaData()
    EL.TPC_GetYMetaData(deviceIx, board, input, measurementNr, ymetaData, sys.getsizeof(ymetaData)+32);

    const       = ymetaData.voltToPhysicalConstant
    fac         = ymetaData.voltToPhysicalFactor
    
    data = getPyDataVoltage(deviceIx, board, input, block, measurementNr, start, numberOfData)

    data = (data * fac) + const

    datalist = np.array(data)

    return datalist

def getMarkerData(deviceIx, board, input, block, measurementNr, start, numberOfData):
    """
    Get the two bit marker data fromt data stream 
    """
    
    # read out y meta data for getting the scaling values
    ymetaData = EL.TPC_YMetaData()
    EL.TPC_GetYMetaData(deviceIx, board, input, measurementNr, ymetaData, sys.getsizeof(ymetaData)+32);

    markerMask  = ymetaData.markerMask

    data = EL.new_int32_tArray(numberOfData)
    EL.TPC_GetRawData(deviceIx, board, input, block, measurementNr, start, numberOfData, data)

    datalist = []

    # Copy C++ Array to python list
    for sample in range(0, numberOfData):
        s = EL.int32_tArray_getitem(data,sample)
        s = s & markerMask
        datalist.append(s)
    
    EL.delete_int32_tArray

    return datalist