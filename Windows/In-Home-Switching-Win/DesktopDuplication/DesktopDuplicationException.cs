using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InHomeSwitching
{
    public class DesktopDuplicationException : Exception
    {
        public DesktopDuplicationException(string message)
            : base(message) { }
    }
}
