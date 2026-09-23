using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Elsys.TpcAccess.Enumerations {
    /// Trigger comperator modes for the \ref TPC_SetTrigger function
    [Flags()]
    public enum TPC_EasyTriggerComperatorMode {
        /// Triggers on positive slope; Triggers when signal goes inside window in Window mode; Triggers when signal above level in State mode 
        tpc_etrgCompPositive = 1,
        /// Triggers on negative slope; Triggers when signal goes outside window in Window mode; Triggers when signal below level in State mode
        tpc_etrgCompNegative = 2,
        /// Triggers on positive and negative slope
        tpc_etrgCompBoth = 3,
    }
}
