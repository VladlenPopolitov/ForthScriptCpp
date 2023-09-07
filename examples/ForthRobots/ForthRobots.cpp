// ForthRobots.cpp: main() defined here
//
// define FORTHSCRIPTCPP_ENABLE_INTERNALDEBUGGER to activate internal debugger and 
// callback function executed before every step
#define FORTHSCRIPTCPP_ENABLE_INTERNALDEBUGGER 1
#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
#ifdef _WIN32
#include "..\..\ForthScriptCpp.h"
#else
#include "../../ForthScriptCpp.h"
#endif

#include "crobots.h"
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>

///  multyprocess stuff
#include <chrono>
#include <thread>
#include <future>
#include <condition_variable>
#include <atomic>

#include "ForthRobotSamples.h"

void step(cppforth::Forth* ptr, cppforth::Forth::Cell xt, cppforth::Forth::Cell cell);

void InitForthRobot(cppforth::Forth &forth, void(*func)(cppforth::Forth* ptr,
  cppforth::Forth::Cell xt, cppforth::Forth::Cell cell)=step);

enum RobotStatus {
	StatusWaitRun = 1,
	StatusRun = 2,
	StatusPaused = 3,
	StatusKilled = 4,
	StatusStopped = 6,
	StatusException = 0
};

RobotStatus robotStatus[4] = { StatusException, StatusException, StatusException, StatusException };
std::condition_variable condition[4];
std::mutex mutex_for_condition[4];
std::mutex stdout_mutex{};
int condition_value[4] = { 0, 0, 0, 0 };
std::atomic<int> stopAll{};

void robot_wait(int number){
	if (stopAll.load() == 1) throw std::runtime_error("stopped");
	if (robotStatus[number] == StatusRun){
		/*
		{
			std::lock_guard<std::mutex> lg(mutex_for_condition[number]);
			condition_value[number] = 0;
		}
		condition[number].notify_one();
		*/
		robotStatus[number] = StatusPaused;
		std::unique_lock<std::mutex> ul(mutex_for_condition[number]);
		condition[number].wait(ul, [=]{ return condition_value[number] == 2; });
		robotStatus[number] = StatusRun;
	}
	else {
    throw(std::runtime_error("finished"));
	}
	std::lock_guard<std::mutex> lg(mutex_for_condition[number]);
	condition_value[number] = 1;

}



void cycle(int num){
/*	for (int i = num; i < num; ++i){
		{
			std::unique_lock<std::mutex> ul(mutex_for_condition[i]);
			condition[i].wait(ul, [=]{ return condition_value[i] == 0; });
		}
	}
	*/
	for (int i = 0; i < num; ++i){
		{
			std::lock_guard<std::mutex> lg(mutex_for_condition[i]);
			condition_value[i] = 2;
		}
		condition[i].notify_one();
	}
}




std::map<const cppforth::Forth*, int> Forth2Int{};
int ForthToInt(const cppforth::Forth* forth){
	auto it=Forth2Int.find(forth);
	if (it != Forth2Int.end()){
		return it->second;
	}
	else {
		assert(!"wrong forth class instance is used");
		return 2;
	}
}

void InsertForthToInt(const cppforth::Forth* forth, int num){
	Forth2Int[forth] = num;
}


/* rand_pos - randomize the starting robot postions */
/*           dependent on MAXROBOTS <= 4 */
/*            put robots in separate quadrant */

void rand_pos(int n)
{
	int i, k;
	int quad[4];

	for (i = 0; i < 4; i++) {
		quad[i] = 0;
	}

	/* get a new quadrant */
	for (i = 0; i < n; i++) {
		k = rand() % 4;
		if (quad[k] == 0)
			quad[k] = 1;
		else {
			while (quad[k] != 0) {
				if (++k == 4)
					k = 0;
			}
			quad[k] = 1;
		}
		robots[i].org_x = robots[i].x =
			(rand() % (MAX_X * CLICK / 2)) + ((MAX_X * CLICK / 2) * (k % 2));
		robots[i].org_y = robots[i].y =
			(rand() % (MAX_Y * CLICK / 2)) + ((MAX_Y * CLICK / 2) * (k<2));
	}
}

/* init a robot */
void init_robot(int i)
{
	 int j;

	robots[i].status = DEAD;
	robots[i].x = 0;
	robots[i].y = 0;
	robots[i].org_x = 0;
	robots[i].org_y = 0;
	robots[i].range = 0;
	robots[i].last_x = -1;
	robots[i].last_y = -1;
	robots[i].speed = 0;
	robots[i].last_speed = -1;
	robots[i].accel = 0;
	robots[i].d_speed = 0;
	robots[i].heading = 0;
	robots[i].last_heading = -1;
	robots[i].d_heading = 0;
	robots[i].damage = 0;
	robots[i].last_damage = -1;
	robots[i].scan = 0;
	robots[i].last_scan = -1;
	robots[i].reload = 0;
	for (j = 0; j < MIS_ROBOT; j++) {
		missiles[i][j].stat = AVAIL;
		missiles[i][j].last_xx = -1;
		missiles[i][j].last_yy = -1;
	}
}




/* match - run a series of matches */

void match(int matches,
long limit,
int num_robots,
const std::string &command1,
const std::string &command2,
const std::string &command3,
const std::string &command4,
bool outputToStdout=false,
bool outputToFullScreen=false
)
{
	//int num_robots = 0;
	stopAll.store(0);
	int robotsleft{};
	int m_count{};
	int movement{};
	int i{}, j{}, k{};
	int wins[MAXROBOTS];
	int ties[MAXROBOTS];
	long counter{};

	r_debug = 0;  /* turns off full compile info */

	for (i = 0; i < num_robots; i++) {
		wins[i] = 0;
		ties[i] = 0;
	}
	//num_robots=2;
	if (num_robots < 2) {
		if (outputToStdout) fprintf(stderr, "\n\nCannot play without at least 2 robots.\n\n");
		return;
	}

	for (m_count = 1; m_count <= matches; m_count++) {
		cppforth::Forth forth1{};
		InitForthRobot(forth1);
		InsertForthToInt(&forth1, 0);
		cppforth::Forth forth2{};
		InitForthRobot(forth2);
		InsertForthToInt(&forth2, 1);
		cppforth::Forth forth3{};
		InitForthRobot(forth3);
		InsertForthToInt(&forth3, 2);
		cppforth::Forth forth4{};
		InitForthRobot(forth4);
		InsertForthToInt(&forth4, 3);

		std::vector<std::future<void>> allServersThreads;
		allServersThreads.push_back(
			std::async([&forth1, &command1]{
			try{
				robotStatus[0] = StatusRun;
				forth1.ExecuteString(command1);
				robotStatus[0] = StatusException;
			}
			catch (...){
				robotStatus[0] = StatusException;
			}
		})
			);
		allServersThreads.push_back(
			std::async([&forth2, &command2]{
			try{
				robotStatus[1] = StatusRun;
				forth2.ExecuteString(command2);
				robotStatus[1] = StatusException;
			}
			catch (...){
				robotStatus[1] = StatusException;
			}
		})
			);
		allServersThreads.push_back(
			std::async([&forth3, &command3]{
			try{
				robotStatus[2] = StatusRun;
				forth3.ExecuteString(command3);
				robotStatus[2] = StatusException;
			}
			catch (...){
				robotStatus[2] = StatusException;
			}
		}));
		allServersThreads.push_back(
			std::async([&forth4, &command4]{
			try{
				robotStatus[3] = StatusRun;
				forth4.ExecuteString(command4);
				robotStatus[3] = StatusException;
			}
			catch (...){
				robotStatus[3] = StatusException;
			}
		}));
		if (outputToStdout) printf("\nMatch %6d: ", m_count);
for (i = 0; i < num_robots; i++) {
	init_robot(i);
	// robot_go should be here
	robots[i].status = ACTIVE;
}
rand_pos(num_robots);
movement = MOTION_CYCLES;
robotsleft = num_robots;
counter = 0L;
while (robotsleft > 1 && counter < limit) {
	robotsleft = 0;
	for (i = 0; i < num_robots; i++) {
		if (robots[i].status == ACTIVE) {
			robotsleft++;
			cur_robot = &robots[i];
		}
	}
	cycle(num_robots);
	if (outputToFullScreen) update_disp();
	if (outputToFullScreen) std::this_thread::sleep_for(std::chrono::milliseconds(1));

	if (--movement == 0) {
		counter += MOTION_CYCLES;
		movement = MOTION_CYCLES;
		move_robots(0);
		move_miss(0);
#ifdef DOS
		kbhit();  /* check keyboard so ctrl-break can work */
#endif
		for (i = 0; i < num_robots; i++) {
			for (j = 0; j < MIS_ROBOT; j++) {
				if (missiles[i][j].stat == EXPLODING) {
					count_miss(i, j);
				}
			}
		}
	}
}

/* allow any flying missiles to explode */
while (1) {
	k = 0;
	for (i = 0; i < num_robots; i++) {
		for (j = 0; j < MIS_ROBOT; j++) {
			if (missiles[i][j].stat == FLYING) {
				k = 1;
			}
		}
	}
	if (k) {
		move_robots(0);
		move_miss(0);
	}
	else
		break;
}

if (outputToStdout) printf(" cycles = %ld:\n  Survivors:\n", counter);

k = 0;
for (i = 0; i < num_robots; i++) {
	if (robots[i].status == ACTIVE) {
		if (outputToStdout) 
			printf("   (%d)%14s: damage=%% %d  ", i + 1, robots[i].name,
			robots[i].damage);
    if (i == 1) {
      if (outputToStdout) printf("\n");
    }
    else {
      if (outputToStdout) printf("\t");
    }
		k++;
	}
}

if (k == 0) {
	if (outputToStdout) printf("mutual destruction\n");
}
else {
	if (outputToStdout) printf("\n");
}

if (outputToStdout) printf("  Cumulative score:\n");
for (i = 0; i < num_robots; i++) {
	if (robots[i].status == ACTIVE) {
		if (k == 1)
			wins[i]++;
		else
			ties[i]++;
	}
	if (outputToStdout) printf("   (%d)%14s: wins=%d ties=%d  ", i + 1, robots[i].name,
		wins[i], ties[i]);
  if (i == 1) {
    if (outputToStdout) printf("\n");
  }
  else {
    if (outputToStdout) printf("\t");
  }
}
if (outputToStdout) printf("\n");
stopAll.store(1);
for (int i = 0; i < 10000; ++i){
	auto end = true;
	for (j = 0; j < 4; ++j){
		if (robotStatus[j] != StatusException){
			end = false;
		}
	}
	if (end) 
		break;
	cycle(num_robots);
	std::this_thread::yield();
}
		for (auto it = begin(allServersThreads); it != end(allServersThreads); ++it){
			try{
				(*it).get();
			}
			catch (std::exception &){
				;
			}
		}
		stopAll.store(0);
	}
	if (outputToStdout) fprintf(stderr, "\nMatch play finished.\n\n");
	return ;

}

/** 

match - run a series of matches without display animation.
Source text is based on 
*/

int match1vs1(int matches,
	long limit,
	const std::string &command1,
	const std::string &command2	)
{
	int num_robots = 2;
	stopAll.store(0);
	int robotsleft;
	int m_count;
	int movement;
	int i, j, k;
	int wins[MAXROBOTS];
	int ties[MAXROBOTS];
	long counter{};

	r_debug = 0;  /* turns off full compile info */

	for (i = 0; i < num_robots; i++) {
		wins[i] = 0;
		ties[i] = 0;
	}

	for (m_count = 1; m_count <= matches; m_count++) {
		cppforth::Forth forth1{};
		InitForthRobot(forth1);
		InsertForthToInt(&forth1, 0);
		cppforth::Forth forth2{};
		InitForthRobot(forth2);
		InsertForthToInt(&forth2, 1);

		std::vector<std::future<void>> allServersThreads;
		allServersThreads.push_back(
			std::async([&forth1, &command1]{
			try{
				robotStatus[0] = StatusRun;
				forth1.ExecuteString(command1);
				robotStatus[0] = StatusException;
			}
			catch (...){
				robotStatus[0] = StatusException;
			}
		})
			);
		allServersThreads.push_back(
			std::async([&forth2, &command2]{
			try{
				robotStatus[1] = StatusRun;
				forth2.ExecuteString(command2);
				robotStatus[1] = StatusException;
			}
			catch (...){
				robotStatus[1] = StatusException;
			}
		})
			);

		for (i = 0; i < num_robots; i++) {
			init_robot(i);
			// robot_go should be here
			robots[i].status = ACTIVE;
		}
		rand_pos(num_robots);
		movement = MOTION_CYCLES;
		robotsleft = num_robots;
		counter = 0L;
		while (robotsleft > 1 && counter < limit) {
			robotsleft = 0;
			for (i = 0; i < num_robots; i++) {
				if (robots[i].status == ACTIVE) {
					robotsleft++;
					cur_robot = &robots[i];
				}
			}
			cycle(num_robots);

			if (--movement == 0) {
				counter += MOTION_CYCLES;
				movement = MOTION_CYCLES;
				move_robots(0);
				move_miss(0);
				for (i = 0; i < num_robots; i++) {
					for (j = 0; j < MIS_ROBOT; j++) {
						if (missiles[i][j].stat == EXPLODING) {
							count_miss(i, j);
						}
					}
				}
			}
		}

		/* allow any flying missiles to explode */
		while (1) {
			k = 0;
			for (i = 0; i < num_robots; i++) {
				for (j = 0; j < MIS_ROBOT; j++) {
					if (missiles[i][j].stat == FLYING) {
						k = 1;
					}
				}
			}
			if (k) {
				move_robots(0);
				move_miss(0);
			}
			else
				break;
		}

		k = 0;
		for (i = 0; i < num_robots; i++) {
			if (robots[i].status == ACTIVE) {
				k++;
			}
		}

		for (i = 0; i < num_robots; i++) {
			if (robots[i].status == ACTIVE) {
				if (k == 1)
					wins[i]++;
				else
					ties[i]++;
			}
		}
		stopAll.store(1);
		for (int i = 0; i < 10000; ++i){
			auto end = true;
			for (j = 0; j < 2; ++j){
				if (robotStatus[j] != StatusException){
					end = false;
				}
			}
			if (end)
				break;
			cycle(num_robots);
			std::this_thread::yield();
		}
		for (auto it = begin(allServersThreads); it != end(allServersThreads); ++it){
			try{
				(*it).get();
			}
			catch (std::exception &){
				;
			}
		}
		stopAll.store(0);
	}
	int result{};
	if (wins[0] > wins[1]) result = 1;
	else if(wins[0] < wins[1]) result = -1;
	return result; 

}


void forth_loc_x(cppforth::Forth* ptr){
	int num = ForthToInt(ptr);
	if (ptr->forth_available(1)){
		ptr->forth_push(c_loc_x(num));
	}
	else {
		throw std::runtime_error(("forth_loc_x does not have space in Forth stack"));
	}
}
void forth_loc_y(cppforth::Forth* ptr){
	int num = ForthToInt(ptr);
	if (ptr->forth_available(1)){
		ptr->forth_push(c_loc_y(num));
	}
	else {
		throw std::runtime_error(("forth_loc_y does not have space in Forth stack"));
	}
}
void forth_speed(cppforth::Forth* ptr){
	int num = ForthToInt(ptr);
	if (ptr->forth_available(1)){
		ptr->forth_push(c_speed(num));
	}
	else {
		throw std::runtime_error(("forth_loc_y does not have space in Forth stack"));
	}
}
void forth_damage(cppforth::Forth* ptr){
	int num = ForthToInt(ptr);
	if (ptr->forth_available(1)){
		ptr->forth_push(c_damage(num));
	}
	else {
		throw std::runtime_error(("forth_loc_y does not have space in Forth stack"));
	}
}
void forth_drive(cppforth::Forth* ptr){
	int num = ForthToInt(ptr);
	if (ptr->forth_depth()>=2){
		auto speed = ptr->forth_tocell(0); 
		auto degree = ptr->forth_tocell(1); ptr->forth_pop(2);
		c_drive(num, degree, speed);
	}
	else {
		throw std::runtime_error(("forth_drive does not have parameters in Forth stack"));
	}
}
void forth_cannon(cppforth::Forth* ptr){
	int num = ForthToInt(ptr);
	if (ptr->forth_depth() >= 2){
		auto range = ptr->forth_tocell(0);
		auto degree = ptr->forth_tocell(1); ptr->forth_pop(2);
		c_cannon(num, degree, range);
	}
	else {
		throw std::runtime_error(("forth_cannon does not have parameters in Forth stack"));
	}
}
void forth_scan(cppforth::Forth* ptr){
	int num = ForthToInt(ptr);
	if (ptr->forth_depth() >= 2){
		auto resolution = ptr->forth_tocell(0);
		auto degree = ptr->forth_tocell(1); ptr->forth_pop(2);
		ptr->forth_push(c_scan(num, degree, resolution));
	}
	else {
		throw std::runtime_error(("forth_scan does not have parameters in Forth stack"));
	}
}

void forth_sin(cppforth::Forth* ptr){
	if (ptr->forth_depth() >= 1){
		auto degree = ptr->forth_tocell(0); ptr->forth_pop(1);
		ptr->forth_push(c_sin(degree));
	}
	else {
		throw std::runtime_error(("forth_sin does not have parameters in Forth stack"));
	}
}

void forth_cos(cppforth::Forth* ptr){
	if (ptr->forth_depth() >= 1){
		auto degree = ptr->forth_tocell(0); ptr->forth_pop(1);
		ptr->forth_push(c_cos(degree));
	}
	else {
		throw std::runtime_error(("forth_cos does not have parameters in Forth stack"));
	}
}
void forth_tan(cppforth::Forth* ptr){
	if (ptr->forth_depth() >= 1){
		auto degree = ptr->forth_tocell(0); ptr->forth_pop(1);
		ptr->forth_push(c_tan(degree));
	}
	else {
		throw std::runtime_error(("forth_tan does not have parameters in Forth stack"));
	}
}
void forth_atan(cppforth::Forth* ptr){
	if (ptr->forth_depth() >= 1){
		auto ratio = ptr->forth_tocell(0); ptr->forth_pop(1);
		ptr->forth_push(c_atan(ratio));
	}
	else {
		throw std::runtime_error(("forth_atan does not have parameters in Forth stack"));
	}
}
void forth_sqrt(cppforth::Forth* ptr){
	if (ptr->forth_depth() >= 1){
		auto value = ptr->forth_tocell(0); ptr->forth_pop(1);
		ptr->forth_push(c_sqrt(value));
	}
	else {
		throw std::runtime_error(("forth_sqrt does not have parameters in Forth stack"));
	}
}
void forth_rand(cppforth::Forth* ptr){
	if (ptr->forth_depth() >= 1){
		auto limit = ptr->forth_tocell(0); ptr->forth_pop(1);
		ptr->forth_push(c_rand(limit));
	}
	else {
		throw std::runtime_error(("forth_sin does not have parameters in Forth stack"));
	}
}


void step(cppforth::Forth* ptr, cppforth::Forth::Cell xt, cppforth::Forth::Cell cell){
	int num = ForthToInt(ptr);
	robot_wait(num);
}

void abortRobot(cppforth::Forth* ptr, cppforth::Forth::Cell xt, cppforth::Forth::Cell cell){
  throw std::runtime_error("compiled,ran and stopped");
}


void InitForthRobot(cppforth::Forth &forth, void (*func)(cppforth::Forth* ptr, 
  cppforth::Forth::Cell xt, cppforth::Forth::Cell cell)){
	forth.forth_setcfunction(forth_loc_y, "loc_y");
	forth.forth_setcfunction(forth_loc_x, "loc_x");
	forth.forth_setcfunction(forth_damage, "damage");
	forth.forth_setcfunction(forth_speed, "speed");
	forth.forth_setcfunction(forth_scan, "scan");
	forth.forth_setcfunction(forth_cannon, "cannon");
	forth.forth_setcfunction(forth_drive, "drive");
	forth.forth_setcfunction(forth_sin, "r_sin");
	forth.forth_setcfunction(forth_cos, "r_cos");
	forth.forth_setcfunction(forth_tan, "r_tan");
	forth.forth_setcfunction(forth_atan, "r_atan");
	forth.forth_setcfunction(forth_sqrt, "r_sqrt");
	forth.forth_setcfunction(forth_rand, "r_rand");
	forth.forth_setstepfunction(func, 10);
}

void tournament(const std::vector<std::string> robots, std::vector<int> &result){
	size_t robots_num = robots.size();
	result.resize(robots_num);
	for (size_t i = 0; i < robots_num; ++i) result[i] = 0;
	for (size_t i = 0; i < robots_num; ++i){
		for (size_t j = i+1; j < robots_num; ++j){
			auto res = match1vs1(10, 500000, robots[i], robots[j]);
			result[i] += res;
			result[j] -= res;
		}
	}
	return;
}


struct retArgs {
	size_t loops{500000}, matches{};
	bool tournament{};
	bool showHelp{};
  bool compileAndRun{};
	std::vector<std::string> filenames{};
};
/*************

 Parse arguments and fill retArgs struct

**************/
struct retArgs getArgs(int argc, char**argv){
	struct retArgs retValue;
	for (int i = 1; i < argc; ++i){
		if (argv[i][0] == '-'){
			switch (argv[i][1]){
			case 'l' :
				retValue.loops = atol(argv[i] + 2);
				break;
			case 'm':
				retValue.matches = atol(argv[i] + 2);
				break;
			case 't':
				retValue.tournament = true;
				break;
			case 'h':
				retValue.showHelp = true;
				break;
      case 'c':
        retValue.compileAndRun = true;
        break;
      default:
				break;
			}
		}
		else {
			retValue.filenames.push_back(argv[i]);
		}
	}
	return retValue;
}
/*****

Read the content of file `fileName` to std::string and return it
  
*****/
std::string readFile(const std::string &fileName){
	std::string content{};
	std::ifstream is{};
	is.open(fileName, std::ios_base::in);
	if (is.is_open() && !is.bad()){
		std::stringstream inStrStream{};
		inStrStream << is.rdbuf();//read the file
		is.close();
		return inStrStream.str();//
	}
	return "";
}

int main(int argc, char* argv[])
{
	try{
		auto tim = time(nullptr);
		int seed = tim % 0x7fffffff;
		srand(seed);
		retArgs arg = getArgs(argc, argv);
		if (arg.showHelp){
			std::cout << "ForthRobots [-m9999] [-l999999] [-t] [-c] filename1 filename2 ..." << std::endl
				<< " -m quantity of matches " << std::endl
				<< " -l quantity of command cycles (Forth commands) per match" << std::endl
        << " -c compile filename1 and run it" << std::endl
        << " -t run tournaments for ForthRobots from filename1, filename2 ... files. "
				"2,3 or 4 robots must be specified" << std::endl;
		}
		if (arg.tournament){
	if (arg.filenames.size() > 0){
		std::vector<int> result{};
	std::vector<std::string> tour{};
	for (auto fileName : arg.filenames){
		tour.push_back(readFile(fileName));
	}
		tournament(tour, result);
		for (size_t i = 0; i < tour.size(); ++i){
			std::cout << " robot " << i << " " << arg.filenames.at(i) << ": points " << result.at(i) << std::endl;
		}
	}
	else {
		std::cout << "Error: no robots specified in command line" << std::endl;
	}
		}

		if (arg.matches){
	    if (arg.filenames.size() > 1){
		    init_disp();
        match(arg.matches, arg.loops, 
          arg.filenames.size()>4 ? 4 : arg.filenames.size(),
			    arg.filenames.size()>0 ? readFile(arg.filenames.at(0)) : "", 
			    arg.filenames.size()>1 ? readFile(arg.filenames.at(1)) : "",
			    arg.filenames.size()>2 ? readFile(arg.filenames.at(2)) : "",
			    arg.filenames.size()>3 ? readFile(arg.filenames.at(3)) : "",
			    false, true);
	    }
	    else {
		    std::cout << "Error: AT least 2 robots must be specified in command line" << std::endl;
	    }
		}
    if (arg.compileAndRun && arg.filenames.size() > 0){
      try{
        std::string command = readFile(arg.filenames.at(0));
        cppforth::Forth forth1{};
        InitForthRobot(forth1, abortRobot);
        InsertForthToInt(&forth1, 0);
        forth1.ExecuteString(command);
      }
      catch (std::exception &ex){
        const char *message = ex.what();
        std::cout << "Compiled"<<std::endl<<message << std::endl;
      }
    }
	}
	catch (cppforth::Forth::AbortException &ex){
		const char *aaa = ex.what();
	}
	catch (std::exception &ex){
		const char *aaa = ex.what();		
	}
	catch (...){
		;
	}
	return 0;
}
