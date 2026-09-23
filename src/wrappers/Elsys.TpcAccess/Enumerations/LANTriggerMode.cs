using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Elsys.TpcAccess.Enumerations {

    [Flags]
    public enum LANTriggerMode {
        None = 0,
        Generate = 1,
        Receive = 2,
    }

}
