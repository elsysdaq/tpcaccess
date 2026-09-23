#define _LINUX

#include "TpcAccess.h"
#include "intTypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

static int CheckError(enum TPC_ErrorCode errorCode) {
    if (tpc_noError != errorCode) {
        (void)printf("%s: errorCode: %d\n", __FUNCTION__, (int)errorCode);
        return 0;
    }
    else {
        return 1;
    }
}

int main() {
    int iSystemID1, iSystemID2, iDevice1, iDevice2, iSystemDeviceID1, iSystemDeviceID2;
    ;
    TPC_DeviceInfo DeviceInfo;
    TPC_DeviceStatus deviceStatus;
    printf("Connect to the first System \n");
    // CheckError(TPC_NewSystem(&iSystemID1));
    // printf("System 1 ID: %d  \n", iSystemID1);

    CheckError(TPC_BeginSystemDefinition());
    iDevice1 = TPC_AddDevice("192.168.0.107:10010");
    printf("Device 1 ID: %d  \n", iDevice1);
    // IMPORTANT
    // iSystemDeviceID1 = iDevice1 + iSystemID1;
    CheckError(TPC_EndSystemDefinition(5000));

    // Get Device Info
    CheckError(TPC_GetDeviceInfo(iDevice1, &DeviceInfo, sizeof(TPC_DeviceInfo)));
    printf("Device ID: %x\n", DeviceInfo.deviceID);
    printf("Software Version: %d\n", DeviceInfo.serverSoftwareVersion);
    printf("Simulation: %d\n", DeviceInfo.simulatedHardware);
    printf("installedBoards %x\n", DeviceInfo.installedBoards);
    printf("Device Name %s\n", DeviceInfo.deviceName);
    printf("Device Description %s\n", DeviceInfo.deviceDescription);
    printf("MAC Address %X\n", DeviceInfo.deviceMACAddress);
    printf("Use Local Time %d\n", DeviceInfo.UseLocalTime);
    printf("Auto Start Measurement: %d\n", DeviceInfo.AutoStartMeasurement);
    printf("Auto Start Autosequence %d\n", DeviceInfo.AutoStartAutoSequence);
    printf("Server Port 1 %d\n", DeviceInfo.ServerPort1);
    printf("Server Port 2 %d\n", DeviceInfo.ServerPort2);
    printf("Target Port %d\n", DeviceInfo.TargetPort);
    printf("TwoInOnepassword %s\n", DeviceInfo.TwoInOnepassword);
    printf("Data File Name %s\n", DeviceInfo.DataFileName);
    printf("Number of Backup Files: %d\n", DeviceInfo.NumberOfBackupFiles);
    printf("HD Flush Interval %d s\n", DeviceInfo.HdFlushInterval);
    printf("Write trough Cache %d\n", DeviceInfo.WriteTroughCache);
    printf("Model Type %d\n", DeviceInfo.ModelType);

    uint64_t freeDiskSpace;
    uint64_t DiskSize;
    CheckError(TPC_GetFreeDiskSpace(iDevice1, &freeDiskSpace, &DiskSize));
    printf("Disk Size: %lld\n", DiskSize);
    printf("Free Disk Size: %lld\n", freeDiskSpace);

    return 0;
}
