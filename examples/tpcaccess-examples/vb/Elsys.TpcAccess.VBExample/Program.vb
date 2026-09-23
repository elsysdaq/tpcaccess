Imports System
Imports System.Runtime.InteropServices
Imports System.Threading
Imports Elsys.TpcAccess.Enumerations
Imports Elsys.TpcAccess.Structs

Namespace Elsys.TpcAccess.Example

    Friend Class Program

        Public Shared Sub Main(ByVal args As String())
            'Dim url = "10.0.0.20:10010"
            Dim url = "127.0.0.1:10030"
            Dim deviceIx As Integer
            Dim err As ErrorCode

#Region "Define system and install status callback"

            Dim systemId As Integer
            NativeMethods.TPC_NewSystem(systemId)
            err = NativeMethods.TPC_SetStatusCallbackSystem(systemId, New TPC_StatusCallbackFunc(AddressOf StatusCallbackSystem), IntPtr.Zero)

#End Region

#Region "Initialize connection to device"

            Console.WriteLine($"Initialize connection to device with Url '{url}'...")

            ' Initialize the connection to the instrument
            NativeMethods.TPC_BeginSystemDefinitionSystem(systemId)
            err = NativeMethods.TPC_AddDeviceSystemEx(systemId, url, 20, 60, deviceIx)

            If CheckError(err) Then
                Return
            End If

            ' Set new device Index
            deviceIx = systemId + deviceIx
            err = NativeMethods.TPC_EndSystemDefinitionSystem(systemId, 1000)                    ' 1s timeout till connection failed

            If CheckError(err) Then
                Return
            End If

#End Region

#Region "Configure the device"

            Console.WriteLine($"Configure the device...")

            ' Set default parameters
            err = NativeMethods.TPC_ResetConfiguration()

            If CheckError(err) Then
                Return
            End If

            ' Set recording mode to scope
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.OperationMode, 0)

            If CheckError(err) Then
                Return
            End If

            ' Set single shot and turn on auto trigger
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.ScopeAutoTrigger, 1)

            If CheckError(err) Then
                Return
            End If

            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.ScopeSingleShot, 1)

            If CheckError(err) Then
                Return
            End If

            ' Set Sampling frequency to 10MHz
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.SamplingFrequency, 10000000)

            If CheckError(err) Then
                Return
            End If

            ' Set trigger delay to -50%
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.ScopeTriggerDelay, -50)

            If CheckError(err) Then
                Return
            End If

            ' Set Block Size to 16k Samples
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.ScopeBlockLength, 4096)

            If CheckError(err) Then
                Return
            End If

            ' Set Board 0, Channel 0, Input Range to 5V, 0% Offset
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.Range, 5)

            If CheckError(err) Then
                Return
            End If

            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.Offset, 0)

            If CheckError(err) Then
                Return
            End If

            ' Set Trigger Board 0, Channel 0, Positive Slope at 1V, 0.1V hysteris
            err = NativeMethods.TPC_SetTrigger(deviceIx, 0, 0, TPC_EasyTriggerMode.tpc_etrgSlope, TPC_EasyTriggerComparatorMode.tpc_etrgCompPositive, TPC_EasyTriggerFlags.tpc_etrgNone, 1, 0.1, 0, 0)

#End Region

#Region "Start measurement and readout scaling information"

            Console.WriteLine($"Start measurement and readout scaling information...")
            Dim measurementNumber As Integer
            MakeMeasurement(systemId, deviceIx, measurementNumber)

            ' readout time meta data
            Dim tMetaData As TPC_TMetaData
            err = NativeMethods.TPC_GetTMetaData(deviceIx, 0, 0, measurementNumber, tMetaData, Marshal.SizeOf(GetType(TPC_TMetaData)))

            If CheckError(err) Then
                Return
            End If

            Dim startTime = ConvertDateTime(tMetaData.startTime)
            Console.WriteLine("Meas. Start Time: {0}", startTime.ToString())
            Console.WriteLine("Trigger Sample: {0}", tMetaData.triggerSample)

            ' read out y meta data
            Dim yMetaData As TPC_YMetaData
            err = NativeMethods.TPC_GetYMetaData(deviceIx, 0, 0, measurementNumber, yMetaData, Marshal.SizeOf(GetType(TPC_YMetaData)))

            If CheckError(err) Then
                Return
            End If

            Dim analogMask = yMetaData.analogMask
            Dim markerMask = yMetaData.markerMask
            Dim bintoVoltFactor = yMetaData.binToVoltFactor
            Dim bintoVoltConstant = yMetaData.binToVoltConstant

#End Region

#Region "Get measurement data"

            Console.WriteLine($"Get measurement data...")

            ' get raw data
            Dim blockLength = 4096
            Dim rawData = New Integer(blockLength - 1) {}
            Dim gcHandleData = GCHandle.Alloc(rawData, GCHandleType.Pinned)

            Try
                Dim dataPtr As IntPtr = gcHandleData.AddrOfPinnedObject()
                err = NativeMethods.TPC_GetRawData(deviceIx, 0, 0, 0, measurementNumber, 0, blockLength, dataPtr)
            Finally
                gcHandleData.Free()
            End Try

            If CheckError(err) Then
                Return
            End If

            Dim voltageData = New Double(blockLength - 1) {}
            Dim analogData = New Integer(blockLength - 1) {}
            Dim markerData = New Integer(blockLength - 1) {}

            ' get analog an marker data
            For i = 0 To blockLength - 1
                analogData(i) = rawData(i) And analogMask ' Mask digital marker data from the analog data
                markerData(i) = rawData(i) And markerMask ' Extract the digital marker signals

                ' scale to voltage (this gives the same data as the TPC_GetData function)
                voltageData(i) = analogData(i) * bintoVoltFactor + bintoVoltConstant
            Next

#End Region

            Console.WriteLine($"Press any key to continue...")
            Console.ReadLine()

#Region "Dispose"

            NativeMethods.TPC_SetStatusCallbackSystem(systemId, Nothing, IntPtr.Zero)
            NativeMethods.TPC_DeleteSystem(systemId)

#End Region

        End Sub

        Private Shared Function CheckError(ByVal errCode As ErrorCode) As Boolean
            If errCode <> ErrorCode.NoError Then
                Console.WriteLine("Error: {0}", errCode.ToString())
                Console.ReadLine()
                Return True
            Else
                Return False
            End If
        End Function

        Private Shared Sub StatusCallbackSystem(ByVal userData As IntPtr, ByVal deviceIndex As Integer, ByVal nativeStatus As IntPtr)
            Dim deviceStatus As TPC_DeviceStatus = Marshal.PtrToStructure(nativeStatus, GetType(TPC_DeviceStatus))
        End Sub

        Private Shared Sub MakeMeasurement(ByVal systemId As Integer, ByVal deviceIndex As Integer, <Out> ByRef measurementNumber As Integer)
            Dim status = GetDeviceStatus(deviceIndex)
            Dim measurementNumberBefore = status.measurementNumber
            Dim errorCode = NativeMethods.TPC_ExecuteSystemCommandSystem(systemId, TPC_SystemCommand.Start)
            measurementNumber = measurementNumberBefore

            ' Wait until measurement started
            While measurementNumber = measurementNumberBefore
                status = GetDeviceStatus(deviceIndex)
                measurementNumber = status.measurementNumber
                Thread.Sleep(10)
            End While

            While IsRecording(deviceIndex)
                Thread.Sleep(200)
            End While
        End Sub

        Private Shared Function GetDeviceStatus(ByVal deviceIndex As Integer) As TPC_DeviceStatus
            Dim deviceStatus As TPC_DeviceStatus = New TPC_DeviceStatus()
            Dim size = Marshal.SizeOf(GetType(TPC_DeviceStatus))
            Dim ptr = Marshal.AllocCoTaskMem(size)

            If ptr = IntPtr.Zero Then
                Throw New OutOfMemoryException()
            End If

            Try
                NativeMethods.TPC_GetDeviceStatus(deviceIndex, ptr, size)
                deviceStatus = CType(Marshal.PtrToStructure(ptr, GetType(TPC_DeviceStatus)), TPC_DeviceStatus)
            Catch __unusedException1__ As Exception
            Finally
                Marshal.FreeCoTaskMem(ptr)
            End Try

            Return deviceStatus
        End Function

        Private Shared Function IsRecording(ByVal deviceIndex As Integer) As Boolean
            Dim status = GetDeviceStatus(deviceIndex)
            Dim lIsRecording = False

            For Each boardStatus In status.boards
                lIsRecording = lIsRecording Or boardStatus.recordingState = RecordingState.Starting OrElse boardStatus.recordingState = RecordingState.Recording
            Next

            Return lIsRecording
        End Function

        Private Shared Function ConvertDateTime(ByVal dateTime As TPC_DateTime) As Date
            Try

                If dateTime.year <= 0 OrElse dateTime.month <= 0 OrElse dateTime.month > 12 OrElse dateTime.day <= 0 OrElse dateTime.day > 31 OrElse dateTime.hour < 0 OrElse dateTime.hour > 23 OrElse dateTime.minute < 0 OrElse dateTime.minute > 59 OrElse dateTime.second < 0 OrElse dateTime.second > 59 Then
                    Return Date.MinValue
                End If

                Return New DateTime(dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.minute, dateTime.second, dateTime.milliSecond, DateTimeKind.Local)
            Catch __unusedArgumentOutOfRangeException1__ As ArgumentOutOfRangeException
                Return Date.MinValue
            End Try
        End Function
    End Class

End Namespace
