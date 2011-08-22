if you want to use with C#, just add following imports. 

[DllImport("cpusnid.dll", CallingConvention=CallingConvention.Cdecl, EntryPoint = "GetProcessorSerialNumber", CharSet = CharSet.Ansi)]
private static extern void GetProcessorSerialNumber([MarshalAs(UnmanagedType.LPStr)] StringBuilder CpuSerial);

[DllImport("cpusnid.dll", CallingConvention=CallingConvention.Cdecl, EntryPoint = "GetProcessorID", CharSet = CharSet.Ansi)]
private static extern void GetProcessorID([MarshalAs(UnmanagedType.LPStr)] StringBuilder CpuId);


And use it!
