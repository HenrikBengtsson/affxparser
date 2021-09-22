////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License 
// (version 2.1) as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
////////////////////////////////////////////////////////////////

/**
 * @file   LogStream.cpp
 * @author Chuck Sugnet
 * @date   Tue Mar 21 10:35:22 PST 2006
 * 
 * @brief  Output verbose messages to log file as we go along.
 */

#include "util/LogStream.h"
//
#include "util/AffxTime.h"
#include "util/Util.h"
//
#ifdef __APPLE__
#include <mach/mach_init.h>
#include <mach/task.h>
#endif
//
#ifdef __linux__
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#endif
//
#ifdef WIN32
#include "windows.h"
#include "psapi.h"
#endif


void printHeader(std::ostream *out, bool profile) {
    if(out != NULL && out->good()) {
        (*out) << "Timestamp\t";
        if(profile) {
            (*out) << "Total RAM\t";
            (*out) << "Free RAM\t";
            (*out) << "Swap Available\t";
            (*out) << "Memory Available\t";
            (*out) << "Memory Available at Start\t";
            (*out) << "Memory Available at Block Start\t";
            (*out) << "Memory Used Since Start\t";
            (*out) << "Memory Used Since Block Start\t";
#ifdef __APPLE__
            (*out) << "Memory Resident Size\t";
            (*out) << "Memory Virtual Size\t";
#endif
#ifdef __linux__
            (*out) << "Memory Resident Size\t";
            (*out) << "Memory Virtual Size\t";
#endif
#ifdef WIN32
            (*out) << "Memory Resident Size\t";
            (*out) << "Memory Virtual Size\t";
#endif
        }
        (*out) << "Message\n";
    }
}

/** Constructor. */
LogStream::LogStream(int verbosity, std::ostream *out, bool profile) :
  m_Verbosity(verbosity), m_Out(out), m_Profile(profile) { 

    printHeader(m_Out, m_Profile);
} 
  
LogStream::~LogStream() {
    m_ProgressTimeStart.clear();
    m_ProgressTotal.clear();
}

void LogStream::setStream(std::ostream *out) { 
    m_Out = out; 
    printHeader(m_Out, m_Profile);
}

void LogStream::unsetStream(std::ostream *out) { 
    m_Out = NULL; 
}

/** 
 * A message to be processed by the stream.
 * 
 * @param verbosity - What level of verbosity is associated with this message, higher number == more verbosity.
 * @param log - Message that is to be processed.
 * @param delimiter - Should a delimiter be emitted as well?
 */
void LogStream::message(int verbosity, const std::string &log, bool delimiter) {
    if(verbosity <= m_Verbosity && m_Out != NULL && m_Out->good()) {

        std::string timeStr = AffxTime::getCurrentTime().getTimeStampString();
        (*m_Out) << timeStr << "\t";
        if(m_Profile)
            (*m_Out) << profileString();
        (*m_Out) << log;
        // For the log file we always dump a newline
        (*m_Out) << std::endl;
        m_Out->flush();
    }
}

/** 
 * Begin a progress reporting. After this initial call the
 * progressStep call will be called N times where N = the parameter
 * total passed in below.
 * 
 * @param verbosity - What level of verbosity is the progress
 * associated with. Higher levels of verbosity produce more
 * messages.
 * @param msg - String message associated with the beginning of this task.
 * @param total - Expected number of times that the progressStep()
 * will be called after this.
 */
void LogStream::progressBegin(int verbosity, const std::string &msg, int total) {

    m_ProgressTimeStart.push_back(time(NULL));
    m_ProgressTotal.push_back(total);

    if(verbosity <= m_Verbosity && m_Out != NULL && m_Out->good()) {

        // flush any C io before doing our IO
        fflush(NULL); 

        std::string timeStr = AffxTime::getCurrentTime().getTimeStampString();
        (*m_Out) << timeStr << "\t";
        if(m_Profile)
            (*m_Out) << profileString();
        (*m_Out) << msg;
        // For the log file we always dump a newline
        (*m_Out) << std::endl;
        m_Out->flush();
    }
}
  
/** 
 * This function is called when one unit of work has been done. In
 * general it is expected that the units of work should be roughly
 * equal in size.
 * 
 * @param verbosity - At what verbosity levell should this step be
 * displayed.
 */
void LogStream::progressStep(int verbosity) {
    if(verbosity <= m_Verbosity && m_Out != NULL && m_Out->good()) {
        // flush any C io before doing our IO
        fflush(NULL); 

        std::string timeStr = AffxTime::getCurrentTime().getTimeStampString();
        (*m_Out) << timeStr << "\t";
        if(m_Profile)
            (*m_Out) << profileString();
        (*m_Out) << "Progress Step";
        // For the log file we always dump a newline
        (*m_Out) << std::endl;
        m_Out->flush();
    }
}

/** 
 * Signals the end of progress report. 
 * 
 * @param verbosity - Level of verbosity associated with this progress report.
 * @param msg - Closing message from calling function.
 */
void LogStream::progressEnd(int verbosity, const std::string &msg) {
    time_t timeStart = m_ProgressTimeStart[m_ProgressTimeStart.size()-1];
    m_ProgressTimeStart.pop_back();
    m_ProgressTotal.pop_back();

    if(verbosity <= m_Verbosity && m_Out != NULL && m_Out->good()) {
        time_t timeEnd = time(NULL);
        double dRunTime = (timeEnd - timeStart); // time span in seconds.
        std::string str;
    
        if (dRunTime < 60)
        {
            str = msg + "\t";
            str += ::getDouble(dRunTime, 2, true);
            str += " second run time";
        }
        else if (dRunTime < (60 * 60)) 
        {
            str = msg + "\t";
            str += ::getDouble((dRunTime) / 60, 2, true);
            str += " minute run time"; 
        }
        else
        {
            str = msg + "\t";
            str += ::getDouble((dRunTime) / (60 * 60), 2, true);
            str += " hour run time"; 
        }
    
        // flush any C io before doing our IO
        fflush(NULL); 

        std::string timeStr = AffxTime::getCurrentTime().getTimeStampString();
        (*m_Out) << timeStr << "\t";
        if(m_Profile)
            (*m_Out) << profileString();
        (*m_Out) << str ;
        // For the log file we always dump a newline
        (*m_Out) << std::endl;
        m_Out->flush();
    }
}

/** 
 * What level of verbosity is requested. We force lots of output here.
 * 
 * @param verbosity - Level below which progress messages are printed.
 */
void LogStream::setBaseVerbosity(int verbosity) { // do notthing
}

std::string toMB(int64_t mem) {
  return ToStr(mem/MEGABYTE);
}

#ifdef __APPLE__
void _getProcessMemOSX(uint64_t &rss, uint64_t &vs) {
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if (KERN_SUCCESS != task_info(mach_task_self(),
       TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count)) {
        rss = 0;
        vs = 0;
    } else {
        rss = t_info.resident_size;
        vs  = t_info.virtual_size;
        /* Looks like the os-x docs are wrong. The return values are in bytes, not pages
        vm_size_t page_size;
        if(KERN_SUCCESS != host_page_size(mach_task_self(), &page_size)) {
            rss = 0;
            vs = 0;
        } else {
            rss *= page_size;
            vs *= page_size;
        }
        */
    }
}
#endif

#ifdef __linux__
int parseProcLine(char* line){
    int i = strlen(line);
    while (*line < '0' || *line > '9') line++;
    line[i-3] = '\0';
    i = atoi(line);
    return i;
}
    
void _getProcessMemLinux(uint64_t &rss, uint64_t &vs) {
    FILE* file = fopen("/proc/self/status", "r");
    char line[128];

    rss = 0;
    vs = 0;

    // VmPeak:     3764 kB
    // VmSize:     3764 kB
    // VmLck:         0 kB
    // VmHWM:       376 kB
    // VmRSS:       376 kB
    // VmData:      164 kB
    // VmStk:        88 kB
    // VmExe:        20 kB
    // VmLib:      1408 kB
    // VmPTE:        32 kB

    while (fgets(line, 128, file) != NULL){
            if (strncmp(line, "VmSize:", 7) == 0) vs = parseProcLine(line);
            if (strncmp(line, "VmRSS:", 6) == 0) rss = parseProcLine(line);
    }
    fclose(file);

    vs *= 1024;
    rss *= 1024;
}
#endif

#ifdef WIN32
void _getProcessMemWin32(uint64_t &rss, uint64_t &vs) {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if(GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc))==0) {
        vs = 0;
        rss = 0;
    } else {
        vs = pmc.WorkingSetSize;
        rss = pmc.PrivateUsage;
    }
}
#endif

std::string LogStream::profileString() {
    uint64_t freeRam = 0, totalRam = 0, swapAvail = 0, memAvail = 0;
    Util::memInfo(freeRam, totalRam, swapAvail, memAvail, false);
    uint64_t memFreeAtStart = Util::getMemFreeAtStart();
    uint64_t memFreeAtBlock = Util::getMemFreeAtBlock();

    int64_t memUsedSinceStart = memFreeAtStart;
    memUsedSinceStart -= memAvail;
    int64_t memUsedSinceBlock = memFreeAtBlock;
    memUsedSinceBlock -= memAvail;

    std::string profile = 
            toMB(totalRam) + "\t" +
            toMB(freeRam) + "\t" +
            toMB(swapAvail) + "\t" +
            toMB(memAvail) + "\t" +
            toMB(memFreeAtStart) + "\t" +
            toMB(memFreeAtBlock) + "\t" +
            toMB(memUsedSinceStart) + "\t" +
            toMB(memUsedSinceBlock) + "\t";

#ifdef __APPLE__
    uint64_t rss=0, vs=0;
    _getProcessMemOSX(rss, vs);
    profile += toMB(rss) + "\t" +
               toMB(vs) + "\t";
#endif

#ifdef __linux__
    uint64_t rss=0, vs=0;
    _getProcessMemLinux(rss, vs);
    profile += toMB(rss) + "\t" +
               toMB(vs) + "\t";
#endif

#ifdef WIN32
    uint64_t rss=0, vs=0;
    _getProcessMemWin32(rss, vs);
    profile += toMB(rss) + "\t" +
               toMB(vs) + "\t";
#endif

    return profile;
}

