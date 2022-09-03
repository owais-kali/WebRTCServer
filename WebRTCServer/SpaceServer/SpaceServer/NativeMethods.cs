
using System.Runtime.InteropServices;

namespace SpaceServer
{
    public static class NativeMethods
    {
        private const string LibPath =
            "/LinuxData/Kickback/Webrtc_Source_Code/webrtc-checkout/src/out/Default/libSpacePlugin.so";
        
        [DllImport(LibPath)]
        public static extern int print();
    }
}