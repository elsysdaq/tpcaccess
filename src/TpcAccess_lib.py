''' ******************************************************************************************** '''
''' Python TpcAccess Helper Modules for reading accessing TpcAccess API                          '''
''' Copyright 2017 Elsys AG      '''
''' ******************************************************************************************** '''
import TpcAccess as EL
import sys

def getPyDataVoltage(deviceIx, board, input, block, measurementNr, start, numberOfData):

    data = EL.new_doubleArray(numberOfData)
    EL.TPC_GetData(deviceIx, board, input, block, measurementNr, start, numberOfData, data)

    datalist = []

    for sample in range(0, numberOfData):
        datalist.append(EL.doubleArray_getitem(data,sample))
    
    EL.delete_doubleArray

    return datalist

def getPyDataPhyiscal(deviceIx, board, input, block, measurementNr, start, numberOfData):

     # read out y meta data for getting the scaling values
    ymetaData = EL.TPC_YMetaData()
    EL.TPC_GetYMetaData(deviceIx, board, input, measurementNr, ymetaData, sys.getsizeof(ymetaData)+10);

    const       = ymetaData.binToPhysicalConstant
    fac         = ymetaData.binToPhysicalFactor
    analogMask  = ymetaData.analogMask

    data = EL.new_int32_tArray(numberOfData)
    EL.TPC_GetRawData(deviceIx, board, input, block, measurementNr, start, numberOfData, data)

    datalist = []

    # Copy C++ Array to python list
    for sample in range(0, numberOfData):
        s = EL.int32_tArray_getitem(data,sample)
        s = s & analogMask
        datalist.append(s * fac + const )
    
    EL.delete_int32_tArray

    return datalist