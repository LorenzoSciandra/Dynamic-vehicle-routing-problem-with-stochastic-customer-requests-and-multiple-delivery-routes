//
// Class Chrono: This file contains functions that can be used to get some
//               timing information about your program.
//
// VERSION 1.1
// 5 - 11 - 2004
//
// AUTHOR : Francois Guertin
//

#ifndef TEMPS_H
#define TEMPS_H

#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <iostream>


//==============================================================
class Chrono
{
public:

  //constructor and destructor
  Chrono();

  virtual ~Chrono() {};

  //Resets cumulated time to 0 and indicate that the timer is stopped.
  virtual void    reset() = 0;

  //start the chrono
  virtual void    start() = 0;

  //stop the chrono. The time elapsed between the stop and the next start
  //will not be accumulated.
  //the stop and the next start will not be accumulated.
  virtual void    stop() = 0;

  //This function returns the current elapsed time.  This function can
  //be used after the clock is started and after is it stopped.
  virtual double  getTime() = 0;

  void setDuration(unsigned int nb_secs){nb_sec_duration = nb_secs;}
  unsigned int getDuration(){return nb_sec_duration;}
  bool hasToEnd()
  {
  	if(getTime() > nb_sec_duration)
  		return true;
  	else
  		return false;
  }

protected:
  double clockTicks;           //number of clock ticks per second
  double clockStart;	       // Last time we consulted the clock
  double clockTotal;	       // Total time elapsed
  bool fStart;		       // Is the clock started or not
  unsigned int nb_sec_duration;
};

//
//  This function initializes the structure so it can be used later.
//  It must be called before using any of the other functions,
//  otherwise they will not produce valid results.

//
inline
Chrono::Chrono()
  : clockTicks(sysconf(_SC_CLK_TCK)),
    clockStart(0.0),
    clockTotal(0.0),
    fStart(false),
    nb_sec_duration(0)
{}

//==============================================================


class ChronoCPU : public Chrono
{
public:

  //constructor and destructor
   ChronoCPU();

  //Resets cumulated time to 0 and indicate that the timer is stopped.
  void    reset();

  //start the chrono
  void    start();

  //stop the chrono. The time elapsed between the stop and the next start
  //will not be accumulated.
  //the stop and the next start will not be accumulated.
  void    stop();

  //This function returns the current elapsed time.  This function can
  //be used after the clock is started and after is it stopped.
  double  getTime();

};

//  This function initializes the structure so it can be used later.
//  It must be called before using any of the other functions,
//  otherwise they will not produce valid results.
inline ChronoCPU::ChronoCPU()
  : Chrono()
{}


// Resets cumulated time to 0 and indicate that the timer is stopped.
inline void ChronoCPU::reset()
{
  clockTotal = 0.0;
  fStart = false;
}


// This function indicates that the clock should be started now.
inline void ChronoCPU::start()
{
  tms tmsStart;
  times(&tmsStart);
  clockStart = tmsStart.tms_utime + tmsStart.tms_stime;
  fStart = true;
}


// This stops the clock.  This means that the time elapsed between
// the stop and the next start will not be accumulated.
inline void ChronoCPU::stop()
{
  if (fStart)
  {
    tms tmsCur;
    times(&tmsCur);
    clockTotal += tmsCur.tms_utime + tmsCur.tms_stime - clockStart;

    fStart = false;
  }
  else
    std::cout <<"start() must be called before using stop()" <<std::endl;
}


// This function returns the current elapsed time.  This function can
// be used after the clock is started and after is it stopped.
inline double ChronoCPU::getTime()
{
  if(fStart)
    {
      tms tmsCur;
      times(&tmsCur);
      clockTotal += tmsCur.tms_utime + tmsCur.tms_stime - clockStart;
      clockStart = tmsCur.tms_utime + tmsCur.tms_stime;
    }
  return clockTotal / clockTicks ;
}

//==============================================================
class  ChronoReal : public Chrono
{
public:

  //constructor and destructor
   ChronoReal();

  //Resets cumulated time to 0 and indicate that the timer is stopped.
  void    reset();

  //start the chrono
  void    start();

  //stop the chrono. The time elapsed between the stop and the next start
  //will not be accumulated.
  //the stop and the next start will not be accumulated.
  void    stop();

  //This function returns the current elapsed time.  This function can
  //be used after the clock is started and after is it stopped.
  double  getTime();

};


//  This function initializes the structure so it can be used later.
//  It must be called before using any of the other functions,
//  otherwise they will not produce valid results.
inline ChronoReal::ChronoReal()
  : Chrono()
{}


// Resets cumulated time to 0 and indicate that the timer is stopped.
inline void ChronoReal::reset()
{
  clockTotal = 0.0;
  fStart = false;
}


// This function indicates that the clock should be started now.
inline void ChronoReal::start()
{
  timeval start;
  gettimeofday(&start, NULL);
  clockStart = start.tv_sec + start.tv_usec/1.0e6;
  fStart = true;
}


// This stops the clock.  This means that the time elapsed between
// the stop and the next start will not be accumulated.
inline void ChronoReal::stop()
{
  if (fStart)
  {
    timeval cur;
    gettimeofday(&cur, NULL);
    clockTotal += cur.tv_sec + cur.tv_usec/1.0e6 - clockStart;

    fStart = false;
  }
  else
    std::cout <<"start() must be called before using stop()" <<std::endl;
}


// This function returns the current elapsed time.  This function can
// be used after the clock is started and after is it stopped.
inline double ChronoReal::getTime()
{
  if(fStart)
    {
      timeval cur;
      gettimeofday(&cur, NULL);
      clockTotal += cur.tv_sec + cur.tv_usec/1.0e6 - clockStart;
      clockStart = cur.tv_sec + cur.tv_usec/1.0e6;
    }
  return clockTotal;
}




//Special class of the chrono where we don't need to stop the chrono to get the elapsed time
class ChronoCpuNoStop : public Chrono
{
public:

  //constructor and destructor
   ChronoCpuNoStop();

  //Resets cumulated time to 0 and indicate that the timer is stopped.
  void    reset();

  //start the chrono
  void    start();

  //stop the chrono. The time elapsed between the stop and the next start
  //will not be accumulated.
  //the stop and the next start will not be accumulated.
  void    stop();

  //This function returns the current elapsed time since the last start
  double  getTime();

};

//  This function initializes the structure so it can be used later.
//  It must be called before using any of the other functions,
//  otherwise they will not produce valid results.
inline ChronoCpuNoStop::ChronoCpuNoStop()
  : Chrono()
{}


// Resets cumulated time to 0 and indicate that the timer is stopped.
inline void ChronoCpuNoStop::reset()
{
  clockTotal = 0.0;
  fStart = false;
}


// This function indicates that the clock should be started now.
inline void ChronoCpuNoStop::start()
{
  tms tmsStart;
  times(&tmsStart);
  clockStart = tmsStart.tms_utime + tmsStart.tms_stime;
  fStart = true;
}


// This stops the clock.  This means that the time elapsed between
// the stop and the next start will not be accumulated.
inline void ChronoCpuNoStop::stop()
{
  if (fStart)
  {
    tms tmsCur;
    times(&tmsCur);
    clockTotal += tmsCur.tms_utime + tmsCur.tms_stime - clockStart;

    fStart = false;
  }
  else
    std::cout <<"start() must be called before using stop()" <<std::endl;
}


// This function returns the current elapsed time since the start.
inline double ChronoCpuNoStop::getTime()
{
	if(fStart)
	{
		tms tmsCur;
		times(&tmsCur);
		return (tmsCur.tms_utime + tmsCur.tms_stime - clockStart) / clockTicks;
	}
	else
		return clockTotal / clockTicks ;
}






//==============================================================
class  ChronoRealNoStop : public Chrono
{
public:

  //constructor and destructor
   ChronoRealNoStop();

  //Resets cumulated time to 0 and indicate that the timer is stopped.
  void    reset();

  //start the chrono
  void    start();

  //stop the chrono. The time elapsed between the stop and the next start
  //will not be accumulated.
  //the stop and the next start will not be accumulated.
  void    stop();

  //This function returns the current elapsed time.
  double  getTime();

};


//  This function initializes the structure so it can be used later.
//  It must be called before using any of the other functions,
//  otherwise they will not produce valid results.
inline ChronoRealNoStop::ChronoRealNoStop()
  : Chrono()
{}


// Resets cumulated time to 0 and indicate that the timer is stopped.
inline void ChronoRealNoStop::reset()
{
  clockTotal = 0.0;
  fStart = false;
}


// This function indicates that the clock should be started now.
inline void ChronoRealNoStop::start()
{
  timeval start;
  gettimeofday(&start, NULL);
  clockStart = start.tv_sec + start.tv_usec/1.0e6;
  fStart = true;
}


// This stops the clock.  This means that the time elapsed between
// the stop and the next start will not be accumulated.
inline void ChronoRealNoStop::stop()
{
  if (fStart)
  {
    timeval cur;
    gettimeofday(&cur, NULL);
    clockTotal += cur.tv_sec + cur.tv_usec/1.0e6 - clockStart;
    fStart = false;
  }
  else
    std::cout <<"start() must be called before using stop()" <<std::endl;
}


// This function returns the current elapsed time.
inline double ChronoRealNoStop::getTime()
{
  if(fStart)
    {
      timeval cur;
      gettimeofday(&cur, NULL);
      return cur.tv_sec + cur.tv_usec/1.0e6 - clockStart;
    }
  return clockTotal;
}



#endif
