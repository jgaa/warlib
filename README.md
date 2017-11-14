# warlib
A small collection of useful C++ classes targeted for high performance servers.

## Logging
Logging is a pain-point in many C++ projects. My log library was written before boost::log was available, and I must admit that i was a bit disappointed over the complexity of boost:log when it arrived. I have written a handful of logger libraries over the years, and used quite a few in different programing languages. Why so complex? Logging is not all that hard, is it?

WarLog gives you control over what you want to log. You can filter on severity and context, and log-statements that won't be forwarded to any logs are not expanded (the log processing stops after evaluating if the log statement is required).

For example:

```C++
  LOG_NOTICE << "If we log at notice level, this line will be processed and logged";

  LOG_DEBUG << "While this will not";
```

This is significant if some of your log statement contains method calls that are slow (like pulling stack frames, or context data from variables scattered around in memory). Method calls in the log statements are only called if the data is required at the current log-level.

I usually add *lots* of log statements in my code, much of which I never want to see when things work as expected. Therefore, I have 4 TRACE levels, so that I can log really low-level and frequently called code, and very easily filter it out while still getting lots of details. In a server, like a HTTP server, I may have several layers processing input from the client. I can log the data received over the wire at TRACE level 4, the decompression at level 3, decoding of chunks at level 2, final raw data at level one, and then nicely show what's going on like "Client at 127.0.0.1 sent a GET request regarding "/bla/bla" with headers *headers dumped here* at debug level. That means that if there ever is a problem with, for example, the chunk-decoding (and be sure - there will be if you wrote it yourself!), it's trivial to deduce from the logs what went wrong. But normally, you will only see the high-level debug messages (and higher).

Log levels:
```C++
/*! Log Level */
enum LogLevel {
    /// Fatal error. The application terminates
    LL_FATAL,
    /// Error
    LL_ERROR,
    /// Warning
    LL_WARNING,
    /// Information. Typically program version, internal modules being enabled etc.
    LL_INFO,
    /// Notice. Typically some action that is started or completed.
    LL_NOTICE,
    /// Debug messages. These provides verbose information regarding the processing
    LL_DEBUG,
    /// Trace message
    LL_TRACE1,
    /// Trace message
    LL_TRACE2,
    /// Trace message
    LL_TRACE3,
    /// Trace message
    LL_TRACE4
};

```

WarLog can also filter on context, based on bits. These are the defined values. You are free to add more yourself in your own code.

```C++
enum LogAbout {
    /// General message
    LA_GENERAL  = 0x00000001,
    /// Security
    LA_SECURITY = 0x00000002,
    /// File transfer
    LA_TRANSFER = 0x00000004,
    /// Authentication
    LA_AUTH = 0x00000008,
    /// IO operations (file system)
    LA_IO = 0x00000010,
    /// Network operations (relevant for debug and trace-levels)
    LA_NETWORK = 0x00000020,
    /// Threads (relevant for debug and trace-levels)
    LA_THREADS = 0x00000040,
    /// Inter process commnication (relevant for debug and trace-levels)
    LA_IPC = 0x00000080,
    /// Statistics updates
    LA_STATS = 0x00000100,
    /// Function called and left
    LA_FUNCTION_CALL = 0x00000200
};
```

WarLog allows you to log at different log-levels and context flags to different logs. A log may be the console, or it may be a file. You can add any number of logs with their own filters. When a log-message is processed, the message traverse trough each logger until all have seen it. There are global variables caching the lowest log-level used by any log. Messages logged at a lower level will be skipped at run-time.

### Some examples

TBD


## Threadpool

TBD
