# Simple but powerful (local only) IRC server written in C++ using Boost
## by SkyWodd

Written for a homework from my computer science school. Not a real project, just a "code and forget" project for school.

Designed to be :
* multi-platform (using Boost libraries)
* single-threaded (I'm too lazy to add multi-threading)
* (at most) errors safe
* RFC1495 compatible, BUT NOT TOTALY (Deadline too close to implement correctly all commands)

---

# Compilation 

## Remarks

This program require Boost with Asio.signal (>= 1_47) to have signals support !
If compiled with an older version of Boost signals support will be automaticaly disabled.

Coded, tested, compiled and debugged on Windows 7 with Boost 1_51 using Eclipse Juno.

## Dependencies

* [cmake](http://www.cmake.org/)
* [Boost](http://www.boost.org)

## Compiling

### On MacOSX or Linux

    cd <path to Boost_IRC_server root>
    mkdir build
    cd build
    cmake ..
    make
    make doc

### On Widows

You need to install Visual Studio 2008

    cd <path to Boost_IRC_server root>
    mkdir build
    cd build
    cmake -G "NMake Makefiles" ..
    nmake
    nmake test

---

# Supported IRC commands

Command    - Support - Comment (if any)
* PASS     - full    - Does not make difference between client and server PASS
* NICK     - half    - No nickname collision check (TODO)
* USER     - full    - Ignore host name and server name (set at connection)
* SERVER   - no      - Always answer with ERR_ALREADYREGISTRED
* OPER     - half    - ERR_NOOPERHOST not supported
* QUIT     - full    - 
* SQUIT    - no      - Always answer with ERR_NOPRIVILEGES 
* JOIN     - half    - No multiple channel / key arguments support
* PART     - full    - 
* MODE     - TODO    - TODO
* TOPIC    - full    - 
* NAMES    - half    - Does not support "all channels" listing
* LIST     - half    - Does not support "all channels" listing
* INVITE   - full    - 
* KICK     - half    - Does not support extended KICK format
* VERSION  - full    -
* STATS    - TODO    - TODO
* LINKS    - no      - Always answer with RPL_ENDOFLINKS
* TIME     - full    - 
* CONNECT  - no      - Always answer with ERR_NOPRIVILEGES
* TRACE    - no      - Do nothing
* ADMIN    - full    - 
* INFO     - full    - 
* WHO      - TODO    - TODO
* WHOIS    - TODO    - TODO
* WHOAS    - no      - Always answer with ERR_WASNOSUCHNICK
* NOTICE   - full    - 
* PRIVMSG  - full    - 
* KILL     - half    - ERR_CANTKILLSERVER not supported
* PING     - full    - Does not answer to request with deamon 2 != local server domain
* PONG     - full    - 
* ERROR    - no      - Do nothing
* AWAY     - full    -
* REHASH   - half    - Supported but no configuration file used yet (TODO)
* RESTART  - full    - Stop server, service must be restarted by an external launcher
* SUMMON   - half    - Always answer with ERR_SUMMONDISABLED
* USERS    - half    - Always answer with ERR_USERSDISABLED
* WALLOPS  - full    - Need to be IRC ops to send wallops (avoid denial of service from regular users)
* USERHOST - full    -
* ISON     - full    -
