using System;

namespace DhtTinyMonitor.Common
{
    public class ExceptionEvent
    {
        public static void Publish(Exception ex)
        {
            // ***
            // ** This can be logged internally to a storag device
            // *** or sent to the cloud.
            // ***
        }
    }
}
