/*
 * hello-world.cpp
 *
 *  Created on: Aug 23, 2014
 *      Author: fred
 */


//============================================================================
// Name        : hello-world.cpp
// Author      : fcampaigne
// Version     :
// Copyright   : gnu
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "ftwTime.h"

#include <iostream>
#include <limits>
//syslog
#include <syslog.h>
#include <unistd.h>
//logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
//date time
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <unistd.h>
#include <string>
//chrono
#include <ctime>
#include <ratio>
#include <chrono>
//asio
#include <boost/asio.hpp>


//namespace management
using namespace std;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;
using namespace ftw;

typedef std::numeric_limits< double > dbl;

src::severity_logger<severity_level> lg;



void testFTWTime()
{
	Time::MsgTimePoint msgTime1 = Time::getMsgTimePoint();
	cout << "message time count since epoch: " << msgTime1.time_since_epoch().count() << endl;


	Time::MsgTimePoint msgTime2 = msgTime1;
	Time::MsgTimePoint msgTime3(msgTime2);

	cout << "= and copy: " << msgTime2.time_since_epoch().count() << " " << msgTime3.time_since_epoch().count() << endl;

	Time::HighResTimePoint hiResTime1 = Time::getHighResTimePoint();

	cout << "HighRes time count since epoch: " << hiResTime1.time_since_epoch().count() << endl;

	cout << typeid(Time::HighResTimePoint::period).name() << endl;
	cout <<  "MsgPrecision: " << Time::MsgPrecision << endl;
	cout <<  "HighResPrecision: " << Time::HighResPrecision << endl;

	streamsize old = cout.precision(Time::MsgPrecision);
	cout <<  fixed << Time::getMsgSeconds() << endl;
	cout.precision(Time::HighResPrecision);
	cout <<  fixed << Time::getHighResSeconds() << endl;

	cout.precision(old);

}

void startSysLogging(const char* id, int level)
{
	setlogmask(LOG_UPTO(level));
	openlog(id, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
}

void testBoostDateTime()
{
	try
	{
		double* x = new double(1.5);
		cout << *x << std::endl;
		delete x;
		cout << "Hello World!!! testing boost date time" << endl;
		// The following date is in ISO 8601 extended format (CCYY-MM-DD)
		string s("2001-10-9"); //2001-October-09
		boost::gregorian::date d(boost::gregorian::from_simple_string(s));
		cout << to_simple_string(d) << endl;

		boost::posix_time::ptime t1 =
				boost::posix_time::microsec_clock::universal_time();
		cout << "time time_of_day: " << t1.time_of_day() << std::endl;
		cout << "time ticks: " << t1.time_of_day().ticks() << std::endl;
		cout << "time ticks_per_second: " << t1.time_of_day().ticks_per_second()
				<< std::endl;
		cout << "time seconds: " << t1.time_of_day().seconds() << std::endl;
		cout << "time minutes: " << t1.time_of_day().minutes() << std::endl;
		cout << "time hours: " << t1.time_of_day().hours() << std::endl;
		cout << "time total_seconds: " << t1.time_of_day().total_seconds()
				<< std::endl;
		cout << "time total_milliseconds: "
				<< t1.time_of_day().total_milliseconds() << std::endl;
		cout << "time total_microseconds: "
				<< t1.time_of_day().total_microseconds() << std::endl;
		cout << "time total_nanoseconds: "
				<< t1.time_of_day().total_nanoseconds() << std::endl;

		boost::posix_time::ptime t2;
		cout << "default time: " << t2.time_of_day() << std::endl;

		usleep(1000); //microseconds
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		boost::posix_time::ptime t3 =
				boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration diff = t3 - t1; //posix
		cout << ((double) diff.total_milliseconds()) / 1000.0
				<< " seconds delayed" << endl;
		//Read ISO Standard(CCYYMMDD) and output ISO Extended
		string ud("20011009"); //2001-Oct-09
		boost::gregorian::date d1(
				boost::gregorian::from_undelimited_string(ud));
		cout << to_iso_extended_string(d1) << endl;

		//Output the parts of the date - Tuesday October 9, 2001
		boost::gregorian::date::ymd_type ymd = d1.year_month_day();
		boost::gregorian::greg_weekday wd = d1.day_of_week();
		cout << wd.as_long_string() << " " << ymd.month.as_long_string() << " "
				<< ymd.day << ", " << ymd.year << endl;

		//Let's send in month 25 by accident and create an exception
		std::string bad_date("20012509"); //2001-??-09
		std::cout << "An expected exception is next: " << endl;
		boost::gregorian::date wont_construct(
				boost::gregorian::from_undelimited_string(bad_date));
		//use wont_construct so compiler doesn't complain, but you wont get here!
		cout << "oh oh, you shouldn't reach this line: "
				<< to_iso_string(wont_construct) << endl;
	} catch (exception& e)
	{
		cout << "  Exception: " << e.what() << endl;
	}
}
void testCPPChronoTime()
{
	  using namespace std::chrono;

	  cout << "high_resolution_clock::is_steady: " << high_resolution_clock::is_steady << endl;
	  high_resolution_clock::time_point tp1 = high_resolution_clock::now();

	  system_clock::time_point tp2 = system_clock::now();
	  steady_clock::time_point tp3 = steady_clock::now();
	  cout << "high_resolution_clock time since epoch: " << tp1.time_since_epoch().count() << endl;
	  cout << "system_clock time since epoch         : " << tp2.time_since_epoch().count() << endl;
	  cout << "steady_clock time since epoch         : " << tp3.time_since_epoch().count() << endl;


	  high_resolution_clock::time_point t1 = high_resolution_clock::now();

	  std::cout << "printing out 1000 stars...\n";
	  for (int i=0; i<1000; ++i) std::cout << "*";
	  std::cout << std::endl;

	  high_resolution_clock::time_point t2 = high_resolution_clock::now();

	  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

	  std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
	  high_resolution_clock::time_point t3;
	  high_resolution_clock::time_point t4 = high_resolution_clock::now();
	  duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
	  std::cout << (long long)time_span2.count() << std::endl;
}


void initLogging()
{
	logging::add_file_log(keywords::file_name = "../../log/hello_%N.log", /*< file name pattern >*/
	keywords::rotation_size = 10 * 1024 * 1024, /*< rotate files every 10 MiB... >*/
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(
					0, 0, 0), /*< ...or at midnight >*/
			keywords::format = "[%TimeStamp% %ThreadID%]:%Message%" /*< log record format >*/
			);

	logging::core::get()->set_filter(
			logging::trivial::severity >= logging::trivial::trace);
	logging::add_common_attributes();
}
string fl(const std::string file, int line)
{
	return string(
			"{" + file + ":" + boost::lexical_cast<std::string>(line) + "}:");
}
void testLogging()
{
	//logging::add_common_attributes();
	src::severity_logger<severity_level> lg;

	BOOST_LOG_SEV(lg, trace)<< fl(__FILE__,__LINE__) << "A trace severity message";
	BOOST_LOG_SEV(lg, debug)<< fl(__FILE__,__LINE__) << "A debug severity message";
	BOOST_LOG_SEV(lg, info)<< fl(__FILE__,__LINE__) << "An informational severity message";
	BOOST_LOG_SEV(lg, warning)<< fl(__FILE__,__LINE__) << "A warning severity message";
	BOOST_LOG_SEV(lg, error)<< fl(__FILE__,__LINE__) << "An error severity message";
	BOOST_LOG_SEV(lg, fatal)<< fl(__FILE__,__LINE__) << "A fatal severity message";

}
void timerHandler(const boost::system::error_code& /*e*/,
		boost::asio::deadline_timer* t, int* count)
{
	BOOST_LOG_SEV(lg, debug)<< "timer fired: " << boost::posix_time::microsec_clock::universal_time().time_of_day();
	if (*count < 5)
	{
		BOOST_LOG_SEV(lg, debug)<< *count;
		++(*count); t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
		t->async_wait(boost::bind(timerHandler,
						boost::asio::placeholders::error, t, count));
	}
}
void testTimers()
{
	//synchronous timer
	boost::asio::io_service io;
	BOOST_LOG_SEV(lg, debug)<< "start synchronous timer: " << boost::posix_time::microsec_clock::universal_time().time_of_day();
	for (int i = 0; i < 3; i++)
	{
		boost::asio::deadline_timer* t = new boost::asio::deadline_timer(io,
				boost::posix_time::seconds(2));
		t->wait();
		BOOST_LOG_SEV(lg, debug)<< "end synchronous timer: " << boost::posix_time::microsec_clock::universal_time().time_of_day();
		delete t;
	}
	//asynchronous callback
	BOOST_LOG_SEV(lg, debug)<< "start asynchronous timer: " << boost::posix_time::microsec_clock::universal_time().time_of_day();
	int count = 0;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
	t.async_wait(
			boost::bind(timerHandler, boost::asio::placeholders::error, &t,
					&count));
	BOOST_LOG_SEV(lg, debug)<< "start asynchronous timer after wait call: " << boost::posix_time::microsec_clock::universal_time().time_of_day();
	io.run();
}

int main()
{
	cout << "Hello World!!! testing syslog...tail -f /var/log/messages" << endl;

	startSysLogging("bbb", LOG_INFO);
	syslog(LOG_INFO, "Hello World!!!");
	syslog(LOG_NOTICE, "Program started by User %d", getuid());
	syslog(LOG_INFO, "A tree falls in a forest");

	testBoostDateTime();
	testCPPChronoTime();
	testFTWTime();
	initLogging();
	testLogging();
	testTimers();

	closelog();
	return 0;
}


