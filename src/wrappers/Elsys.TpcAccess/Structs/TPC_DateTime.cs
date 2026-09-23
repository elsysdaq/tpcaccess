using System;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 4)]
    public struct TPC_DateTime {

        public int year;
        public int month;
        public int day;
        public int hour;
        public int minute;
        public int second;
        public int milliSecond;

        /*public DateTime ToDateTime() {
            try {
                if (year <= 0
                    || month <= 0 || month > 12
                    || day <= 0 || day > 31
                    || hour < 0 || hour > 23
                    || minute < 0 || minute > 59
                    || second < 0 || second > 59) {
                    return DateTime.MinValue;
                }

                return new DateTime(year, month, day, hour, minute, second, milliSecond, DateTimeKind.Local);
            } catch (ArgumentOutOfRangeException) {
                return DateTime.MinValue;
            }
        }*/

    }

}
