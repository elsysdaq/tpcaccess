' -----------------------------------------------------------------------------------------------------------------
'
' TpcAccess VB Example Program for Continuous Measurement
'
' ELSYS EXPRESSLY DISCLAIMS ALL WARRANITIES Or REPRESENTATIONS WITH RESPECT TO
' THIS SOFTWARE And DOCUMENTATION And ANY SUPPORT Or MAINTENANCE SERVICES THAT
' ELSYS MAY CHOOSE TO PROVIDE IN RELATION THERETO(INCLUDING, WITHOUT
' LIMITATION, ANY WARRANTIES Or CONDITIONS OF TITLE Or THAT THE SOFTWARE : WILL BE
' ERROR - FREE, WILL OPERATE WITHOUT INTERUPTION, WILL Not INFRINGE THE RIGHTS OF A
' THIRD PARTY, Or WILL PRODUCE RESULTS IN CONNECTION WITH ITS USE).FURTHER,
' ELSYS EXPRESSLY DISCLAIMS ALL EXPRESS Or IMPLIED WARRANTIES, INCLUDING,
' WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY And FITNESS FOR A
' PARTICULAR PURPOSE.
'
'
' Copyright 2021 Elsys AG
' Author: Roman Bertschi
' Contact: info@elsys.ch
' Licence: MIT License
' See Licence file for more information about the used licence. 
'
'
' -----------------------------------------------------------------------------------------------------------------
Imports System
Imports System.Runtime.InteropServices
Imports System.IO
Imports System.Threading
Imports Elsys.TpcAccess.Enumerations
Imports Elsys.TpcAccess.Structs

Namespace Elsys.TpcAccess.ContMeasurementExample
    Friend Class Program
        Private Shared m_datacounter As ULong
        Private Shared m_iMeasurementNumber As Integer

        Public Shared Sub Main(ByVal args As String())
            Dim url = "192.168.0.33:10010"          ' Replace the IP address with your device address
            Dim deviceIx As Integer
            Dim err As ErrorCode

#Region "Initialize connection to device"
            Dim systemId As Integer
            NativeMethods.TPC_NewSystem(systemId)
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

            ' Set default parameters
            err = NativeMethods.TPC_ResetConfiguration()

            If CheckError(err) Then
                Return
            End If

            Dim brd = 0    ' board and group parameters must be send always to board 0, input 0
            Dim inp = 0

            ' Set recording mode to continuous
            err = NativeMethods.TPC_SetParameter(deviceIx, brd, inp, Parameter.OperationMode, TimeBaseMode.Continuous)

            If CheckError(err) Then
                Return
            End If

            ' Set Sampling frequency to 1MHz
            err = NativeMethods.TPC_SetParameter(deviceIx, brd, inp, Parameter.SamplingFrequency, 1000000)

            If CheckError(err) Then
                Return
            End If

#End Region

#Region "InitalizeStatusUpdate and start measurement"

            m_datacounter = 0  ' Initialize the data counter to 0

            ' install a callback function for getting the device status as fast as possible
            Call NativeMethods.TPC_SetStatusCallbackSystem(systemId, New TPC_StatusCallbackFunc(AddressOf StatusCall), IntPtr.Zero)

            ' Initialize unmanged memory to hold the struct.
            Dim NativeStatus = Marshal.AllocHGlobal(Marshal.SizeOf(GetType(TPC_DeviceStatus)))
            Dim s = Marshal.SizeOf(GetType(TPC_DeviceStatus))
            NativeMethods.TPC_GetDeviceStatus(deviceIx, NativeStatus, s)
            Dim deviceStatus As TPC_DeviceStatus = Marshal.PtrToStructure(NativeStatus, GetType(TPC_DeviceStatus))

            ' get current measurement number (needet for detecting the start of measurement )

            Dim currentMeasurementNr = deviceStatus.measurementNumber
            m_iMeasurementNumber = deviceStatus.measurementNumber
            Marshal.FreeHGlobal(NativeStatus)

            ' send start recording command (asynchron) 
            NativeMethods.TPC_ExecuteSystemCommandSystem(systemId, TPC_SystemCommand.Start)

            ' wait till recording has started = new measurment number is set in the callback function
            While currentMeasurementNr = m_iMeasurementNumber
                ' ToDO programm abord condition if start measurement failed
                Thread.Sleep(200)
            End While

            Console.WriteLine("Measurement has started")

#End Region

#Region "DataReadout"

            Dim MaxRecordingLenght As ULong = 5 * 1024 * 1024      ' define recording length by the number of samples
            Dim ReadChunkSize = 32 * 1024                   ' read chunk size
            Dim rawData = New Integer(ReadChunkSize - 1) {}      ' data buffer
            Dim gcHandleData = GCHandle.Alloc(rawData, GCHandleType.Pinned)
            Dim dataPtr As IntPtr = gcHandleData.AddrOfPinnedObject()
            Dim ReadDataCounter As ULong = 0    ' actual read data counter
            Dim writer_CH0 As BinaryWriter = New BinaryWriter(File.Open("data_ch0.bin", FileMode.Create))
            Dim writer_CH1 As BinaryWriter = New BinaryWriter(File.Open("data_ch1.bin", FileMode.Create))

            While ReadDataCounter < MaxRecordingLenght
                Dim tempCounter = m_datacounter

                If tempCounter > MaxRecordingLenght Then
                    NativeMethods.TPC_ExecuteSystemCommandSystem(systemId, TPC_SystemCommand.Stop)
                    Console.WriteLine("Measurement has stopped")
                End If

                Dim count As Integer = tempCounter - ReadDataCounter

                While count > 0
                    Dim c = Math.Min(count, ReadChunkSize)
                    Console.Write("Read Data: ")
                    Console.Write(c)
                    Console.Write(vbTab & " from ")
                    Console.WriteLine(ReadDataCounter)


                    ' read channel 0
                    NativeMethods.TPC_GetRawData(deviceIx, brd, 0, 0, m_iMeasurementNumber, ReadDataCounter, c, dataPtr)

                    For i = 0 To ReadChunkSize - 1
                        writer_CH0.Write(rawData(i))
                    Next

                    ' read channel 1
                    NativeMethods.TPC_GetRawData(deviceIx, brd, 1, 0, m_iMeasurementNumber, ReadDataCounter, c, dataPtr)

                    For i = 0 To ReadChunkSize - 1
                        writer_CH1.Write(rawData(i))
                    Next

                    ' update read counters
                    ReadDataCounter += CULng(c)
                    count -= c
                End While
            End While

            gcHandleData.Free()
            writer_CH0.Close()
            writer_CH1.Close()


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

        Private Shared Sub StatusCall(ByVal userData As IntPtr, ByVal deviceIndex As Integer, ByVal nativeStatus As IntPtr)
            Dim deviceStatus As TPC_DeviceStatus = Marshal.PtrToStructure(nativeStatus, GetType(TPC_DeviceStatus))
            m_iMeasurementNumber = deviceStatus.measurementNumber

            ' Check actual recording state of board 0
            Select Case deviceStatus.boards(0).recordingState
                    ' Recording is stopped or abborded (stop was send)
                Case RecordingState.Aborted, RecordingState.Stopped
                Case RecordingState.Starting
                Case RecordingState.Recording
                    m_datacounter = CULng(deviceStatus.boards(0).dataCounter)  ' update current data counter for continuous measurements
                Case RecordingState.StartError
            End Select
        End Sub
    End Class
End Namespace
