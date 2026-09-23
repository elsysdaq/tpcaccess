using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {

    [StructLayout(LayoutKind.Sequential)]
    public struct TPC_AssociatedChannel {

        /// <summary> Board address of the associated channel. </summary>
        public int BoardAddress;

        /// <summary> Input number of the associated channel. </summary>
        public int InputNumber;

        /// <summary>
        /// Create a new instance of the AssociatedChannel structure with the given initial values.
        /// </summary>
        /// <param name="boardAddress">The board address of the associated channel.</param>
        /// <param name="inputNumber">The input number of the associated channel.</param>
        public TPC_AssociatedChannel(int boardAddress, int inputNumber) {
            BoardAddress = boardAddress;
            InputNumber = inputNumber;
        }

    }

}
