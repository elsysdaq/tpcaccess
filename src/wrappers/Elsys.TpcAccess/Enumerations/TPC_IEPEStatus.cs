namespace Elsys.TpcAccess.Enumerations {

    /// IEPE Connection State
    public enum TPC_IEPEStatus {
        /// IEPE off
        tpc_iepe_off = 0,
        /// IEPE on, but no sensor connected
        tpc_iepe_nosensor = 1,
        /// IEPE on, sensor connected
        tpc_iepe_ok = 2,
    }

}
