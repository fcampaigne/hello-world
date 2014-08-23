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

#include <iostream>
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
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread/thread.hpp"
#include <unistd.h>
#include <string>
//asio
#include <boost/asio.hpp>
//namespace management
using namespace std;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;

src::severity_logger<severity_level> lg;

void startSysLogging(const char* id, int level)
{
	setlogmask(LOG_UPTO(level));
	openlog(id, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
}

void testBoostDateTime()
{
	try
	{
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

		usleep(1000); //microseconds
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		boost::posix_time::ptime t2 =
				boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration diff = t2 - t1; //posix
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
	initLogging();
	testLogging();
	testTimers();
	closelog();
	return 0;
}


