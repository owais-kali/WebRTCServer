using System;

namespace SpaceServer
{
    class SpaceServer
    {
        static int Main(String[] args)
        {
            int val = NativeMethods.print();
            return 0;
        }
    }
}