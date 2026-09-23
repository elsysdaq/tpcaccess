using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Elsys.TpcAccess.Enumerations
{
    /// <summary>Board Firmware Capabilities</summary>
    [Flags()]
    public enum FirmwareCapabilities {
        /// <summary>Extended Block Sizes Capability</summary>
        ExtendedBlockSizes = 0x01,

    }
}
