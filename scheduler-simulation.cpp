#include <vector>
#include <iostream>
#include <queue>

using namespace std;

//a class to represent a process 
class Process {
public:
	Process(); //default constructor, does nothing
	Process(vector<int>, int); //constructor to fill the process with the bursts and ID number
	bool isIOcomplete(int); //function to see if done with I/O
	vector<int> bursts; //vector of ints containing CPU and I/O bursts
	int ioTime = 0; //at what time I/O will be finished
	int num; //process ID number, i.e. P1, P4, etc.
	int waitingTime = 0; 
	int turnaroundTime = 0;
	int responseTime = -1; //initialized to -1 to allow for only setting response time once
};

//default constructor, does nothing
Process::Process() {

}

//constructor to fill the process with the bursts and ID number
Process::Process(vector<int> b, int n) {
	bursts = b;
	num = n;
}

//function to see if done with I/O
bool Process::isIOcomplete(int time) {
	if (ioTime <= time) { return true; } //returns true if the process has been in I/O long enough
	else { return false; }
}

void calcResponseTime(Process &a, int time) {
	if (a.responseTime < 0) { a.responseTime = time; } //sets the response time to the time the process is first executed
}

//function for the FCFS implementation
void FCFS(vector<Process> readyQ) { //takes a vector of processes as input, will represent ready queue
	int time = 0; //current timer
	int notUtilized = 0; //counter variable used to calculate CPU utilization later
	vector<Process> IO; //represents processes currently in I/O
	vector<Process> completedP; //completed processes
	Process realTime; //used for the sample of dynamic output
	bool completed = false; //used to exit loop when all processes are completed

	while (!completed) {
		int i = 0;
		//this loop tests all processes in I/O to see if they are done, and puts them at the end of the 
		//ready queue if they are
		while (!IO.empty() && i < IO.size()) {
			if (IO[i].isIOcomplete(time)) { //if current I/O process is done
				readyQ.push_back(IO[i]); //add it to end of ready queue
				swap(IO[i], IO.back()); //swap it with process in back of container
				IO.pop_back(); //delete back process
				i--; //move counter backward to check new process in this position
			}
			i++;
		}
		//a series of nested if statements drives the logic of the program
		if (!readyQ.empty()) { //if the ready queue is not empty
			if (!readyQ.front().bursts.empty()) { //if the first process in ready queue has another burst
				calcResponseTime(readyQ.front(), time); //set the response time
				realTime = readyQ.front(); //store process about to be executed for output
				time += readyQ.front().bursts[0]; //add time equal to the CPU burst to timer
				for (int i = 1; i < readyQ.size(); i++) {
					readyQ[i].waitingTime += readyQ.front().bursts[0]; //add waiting time to all processes in ready queue
				}
				readyQ.front().bursts.erase(readyQ.front().bursts.begin()); //erase the CPU burst that was just done
			}
			if (!readyQ.front().bursts.empty()) { //if process in front of queue has another burst, it is I/O burst
				readyQ.front().ioTime = time + readyQ.front().bursts[0]; //calculate the time process will be in I/O
				readyQ.front().bursts.erase(readyQ.front().bursts.begin()); //erase the I/O burst that was just done
				IO.push_back(readyQ.front()); //add current process to I/O container
			}
			else { //if process has no more bursts, it is completed
				readyQ.front().turnaroundTime = time; //set the turnaround time
				completedP.push_back(readyQ.front()); //add current process to completed container
			}
			readyQ.erase(readyQ.begin()); //remove process from ready queue, its either completed or in I/O
		}
		else { //if ready queue is empty
			if (!IO.empty()) { //and there are processes still in I/O
				time++; //increment timer
				notUtilized++; //increment CPU idle counter
			}
			else { //if ready queue is empty and no processes in I/O
				completed = true; //all processes completed
			}
		}
	
		//sample of dynamic execution
		cout << "Current execution time: " << time << endl;
		cout << "Current running process: P" << realTime.num << endl;
		cout << "Ready queue:\n";
		for (int i = 0; i < readyQ.size(); i++) {
			cout << "P" << readyQ[i].num << " with CPU burst " << readyQ[i].bursts[0] << endl;
		}
		cout << "Processes in I/O:\n";
		for (int i = 0; i < IO.size(); i++) {
			cout << "P" << IO[i].num << " with remaining I/O burst " << IO[i].ioTime - time << endl;
		}
		cout << "Processes that have completed execution: ";
		for (int i = 0; i < completedP.size(); i++) {
			cout << "P" << completedP[i].num << " ";
		}
		cout << endl << "---------------------------------------" << endl;
	}//end of master loop

	//simulation results
	sort(completedP.begin(), completedP.end(), [](const Process &lhs, const Process &rhs) { return lhs.num < rhs.num; }); //sorts the processes by ID number
	int utilized = time - notUtilized; //calculate total time CPU was active
	int sum = 0; //used for averages
	double CPUu = ((double)utilized / (double)time) * 100; //calculate CPU utilization
	
	cout << "Results for FCFS\n";
	cout << "-----------------\n";
	cout << "Total time needed: " << time << " units.\n";
	cout << "CPU utilization: " << CPUu << "%" << endl;

	cout << "\nWaiting time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].waitingTime << endl;
		sum += completedP[i].waitingTime; //add all waiting times together
	}
	cout << "Average waiting time: " << ((double)sum / (double)completedP.size()) << endl; //calculate and display average

	sum = 0;
	cout << "\nTurnaround time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].turnaroundTime << endl;
		sum += completedP[i].turnaroundTime;
	}
	cout << "Average turnaround time: " << ((double)sum / (double)completedP.size()) << endl;

	sum = 0;
	cout << "\nResponse time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].responseTime << endl;
		sum += completedP[i].responseTime;
	}
	cout << "Average response time: " << ((double)sum / (double)completedP.size()) << endl;
	cout << "-----------------\n\n\n";
}

//the implementation of the SJF scheduling algorithm is exactly the same, except
//it sorts the processes by CPU burst at the start of each iteration of the loop
//i am leaving it uncommented because it is exactly the same as the previous function
void SJF(vector<Process> readyQ) {
	int time = 0;
	int notUtilized = 0;
	Process realTime;
	vector<Process> IO;
	vector<Process> completedP;
	bool completed = false;

	while (!completed) {
		int i = 0;
		while (!IO.empty() && i < IO.size()) {
			if (IO[i].isIOcomplete(time)) {
				readyQ.push_back(IO[i]);
				swap(IO[i], IO.back());
				IO.pop_back();
				i--;
			}
			i++;
		}
		if (!readyQ.empty()) {
			if (!readyQ.front().bursts.empty()) {
				sort(readyQ.begin(), readyQ.end(), [](const Process &lhs, const Process &rhs) { return lhs.bursts[0] < rhs.bursts[0]; }); //this is the only new line, it sorts the ready queue by current CPU bursts
				realTime = readyQ.front();
				calcResponseTime(readyQ.front(), time);
				time += readyQ.front().bursts[0];
				for (int i = 1; i < readyQ.size(); i++) {
					readyQ[i].waitingTime += readyQ.front().bursts[0];
				}
				readyQ.front().bursts.erase(readyQ.front().bursts.begin());
			}
			if (!readyQ.front().bursts.empty()) {
				readyQ.front().ioTime = time + readyQ.front().bursts[0];
				readyQ.front().bursts.erase(readyQ.front().bursts.begin());
				IO.push_back(readyQ.front());
			}
			else {
				readyQ.front().turnaroundTime = time;
				completedP.push_back(readyQ.front());
			}
			readyQ.erase(readyQ.begin());
		}
		else {
			if (!IO.empty()) {
				time++;
				notUtilized++;
			}
			else {
				completed = true;
			}
		}
		cout << "Current execution time: " << time << endl;
		cout << "Current running process: P" << realTime.num << endl;
		cout << "Ready queue:\n";
		for (int i = 0; i < readyQ.size(); i++) {
			cout << "P" << readyQ[i].num << " with CPU burst " << readyQ[i].bursts[0] << endl;
		}
		cout << "Processes in I/O:\n";
		for (int i = 0; i < IO.size(); i++) {
			cout << "P" << IO[i].num << " with remaining I/O burst " << IO[i].ioTime - time << endl;
		}
		cout << "Processes that have completed execution: ";
		for (int i = 0; i < completedP.size(); i++) {
			cout << "P" << completedP[i].num << " ";
		}
		cout << endl << "---------------------------------------" << endl;
	}//end master while

	sort(completedP.begin(), completedP.end(), [](const Process &lhs, const Process &rhs) { return lhs.num < rhs.num; });
	int utilized = time - notUtilized;
	int sum = 0;
	double CPUu = ((double)utilized / (double)time) * 100;
	cout << "Results for SJF\n";
	cout << "-----------------\n";
	cout << "Total time needed: " << time << " units.\n";
	cout << "CPU utilization: " << CPUu << "%" << endl;

	cout << "\nWaiting time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].waitingTime << endl;
		sum += completedP[i].waitingTime;
	}
	cout << "Average waiting time: " << ((double)sum / (double)completedP.size()) << endl;

	sum = 0;
	cout << "\nTurnaround time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].turnaroundTime << endl;
		sum += completedP[i].turnaroundTime;
	}
	cout << "Average turnaround time: " << ((double)sum / (double)completedP.size()) << endl;

	sum = 0;
	cout << "\nResponse time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].responseTime << endl;
		sum += completedP[i].responseTime;
	}
	cout << "Average response time: " << ((double)sum / (double)completedP.size()) << endl;
	cout << "-----------------\n\n\n";
}

//used for attempt at MLFQ
int checkFlags(vector<int> IOtimesq1, Process p, int time) {
	int result = -1;
	for (int i = 0; i < IOtimesq1.size(); i++) {
		if (time + p.bursts[0] > IOtimesq1[i]) result = i;
	}
	return result;
}

//ran out of time, implementation of MLFQ scheduling algorithm not finished
void MLFQ(vector<Process> readyQ) {
	int time = 0;
	int notUtilized = 0;
	vector<int> IOtimesq1;
	vector<int> IOtimesq2;
	vector<Process> completedP;
	vector<Process> q1io;
	vector<Process> q2io;
	vector<Process> q3io;
	vector<Process> q1 = readyQ; //tq = 6 units
	vector<Process> q2; //tq = 11 units
	vector<Process> q3; //fcfs
	bool completed = false;

	while (!completed) {
		if (!q1.empty()) {
			calcResponseTime(q1.front(), time);
			if (q1.front().bursts[0] <= 6 && !q1.front().bursts.empty()) {
				time += q1.front().bursts[0];
				for (int i = 1; i < q1.size(); i++) {
					q1[i].waitingTime += q1.front().bursts[0];
				}
				q1.front().bursts.erase(q1.front().bursts.begin());
				if (!q1.front().bursts.empty()) {
					IOtimesq1.push_back(time + q1.front().bursts[0]);
					q1.front().bursts.erase(q1.front().bursts.begin());
					q1io.push_back(q1.front());
					q1.erase(q1.begin());
				}
			}//end if less than 6
			else {
				if (q1.front().bursts.empty()) {
					q1.front().turnaroundTime = time;
					completedP.push_back(q1.front());
					q1.erase(q1.begin());
				}//end if no more bursts
				else {
					time += 6;
					for (int i = 1; i < q1.size(); i++) {
						q1[i].waitingTime += 6;
					}
					q1.front().bursts[0] -= 6;
					q2.push_back(q1.front());
					q1.erase(q1.begin());
				}//end if greater than six
			}//end if greater than six or no more bursts
		}//end if q1 not empty
		else {
			if (!q2.empty()) {
				int x = checkFlags(IOtimesq1, q2.front(), time);
				if (x == -1) {
					if (q2.front().bursts[0] <= 11 && !q2.front().bursts.empty()) {
						time += q2.front().bursts[0];
						for (int i = 1; i < q2.size(); i++) {
							q2[i].waitingTime += q2.front().bursts[0];
						}
						q2.front().bursts.erase(q1.front().bursts.begin());
						if (!q2.front().bursts.empty()) {
							q2.front().ioTime = time + q2.front().bursts[0];
							q2.front().bursts.erase(q2.front().bursts.begin());
							q2io.push_back(q2.front());
							q2.erase(q1.begin());
						}//end if io burst
					}//end if less than 11
					else {
						if (q2.front().bursts.empty()) {
							q2.front().turnaroundTime = time;
							completedP.push_back(q2.front());
							q2.erase(q2.begin());
						}
						else {
							time += 11;
							for (int i = 1; i < q2.size(); i++) {
								q2[i].waitingTime += 11;
							}
							q2.front().bursts[0] -= 11;
							q3.push_back(q2.front());
							q2.erase(q2.begin());
						}//end if greater than 11
					}//end if greater than 11 or no more bursts
				}//end if no flag
				else {
					time += time - IOtimesq1[x];
					q2.front().bursts[0] -= time - IOtimesq1[x];
				}
			}//end if q2 not empty
			else {
				if (!q3.empty()) {
					if (!q3.front().bursts.empty()) {
						time += q3.front().bursts[0];
						for (int i = 1; i < q2.size(); i++) {
							q3[i].waitingTime += q3.front().bursts[0];
						}
					}
					q3.front().bursts.erase(q3.front().bursts.begin());
					if (!q3.front().bursts.empty()) {
						q3.front().ioTime = time + q3.front().bursts[0];
						q3.front().bursts.erase(q3.front().bursts.begin());
						q3io.push_back(q3.front());
					}//end if io burst
					else {
						q3.front().turnaroundTime = time;
						completedP.push_back(readyQ.front());
					}
					q3.erase(q3.begin());
				}//end if q3 not empty
			}//end if q2 is empty
		}//end if q1 is empty
	}//end master while

	//int i = 0;
	//while (!IO.empty() && i < IO.size()) {
	//	if (IO[i].isIOcomplete(time)) {
	//		readyQ.push_back(IO[i]);
	//		swap(IO[i], IO.back());
	//		IO.pop_back();
	//		i--;
	//	}
	//	i++;
	//}

	sort(completedP.begin(), completedP.end(), [](const Process &lhs, const Process &rhs) { return lhs.num < rhs.num; });
	int utilized = time - notUtilized;
	int sum = 0;
	double CPUu = ((double)utilized / (double)time) * 100;
	cout << "Results for MLFQ\n";
	cout << "-----------------\n";
	cout << "Total time needed: " << time << " units.\n";
	cout << "CPU utilization: " << CPUu << "%" << endl;

	cout << "\nWaiting time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].waitingTime << endl;
		sum += completedP[i].waitingTime;
	}
	cout << "Average waiting time: " << ((double)sum / (double)completedP.size());

	sum = 0;
	cout << "\nTurnaround time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].turnaroundTime << endl;
		sum += completedP[i].turnaroundTime;
	}
	cout << "Average turnaround time: " << ((double)sum / (double)completedP.size());

	sum = 0;
	cout << "\nResponse time for each process:\n";
	for (int i = 0; i < completedP.size(); i++) {
		cout << "P" << completedP[i].num << ": " << completedP[i].responseTime << endl;
		sum += completedP[i].responseTime;
	}
	cout << "Average response time: " << ((double)sum / (double)completedP.size()) << endl;
	cout << "-----------------\n\n\n";
}

//main function that populates the processes and calls the functions for FCFS and SJF
int main()
{
	vector<Process> processes;

	vector<int> P1 = { 4,24,5,73,3,31,5,27,4,33,6,43,4,64,5,19,2 };
	Process p1(P1, 1);
	processes.push_back(p1);

	vector<int> P2 = { 18,31,19,35,11,42,18,43,19,47,18,43,17,51,19,32,10 };
	Process p2(P2, 2);
	processes.push_back(p2);

	vector<int> P3 = { 6,18,4,21,7,19,4,16,5,29,7,21,8,22,6,24,5 };
	Process p3(P3, 3);
	processes.push_back(p3);

	vector<int> P4 = { 17,42,19,55,20,54,17,52,15,67,12,72,15,66,14 };
	Process p4(P4, 4);
	processes.push_back(p4);

	vector<int> P5 = { 5,81,4,82,5,71,3,61,5,62,4,51,3,77,4,61,3,42,5 };
	Process p5(P5, 5);
	processes.push_back(p5);

	vector<int> P6 = { 10,35,12,41,14,33,11,32,15,41,13,29,11 };
	Process p6(P6, 6);
	processes.push_back(p6);

	vector<int> P7 = { 21,51,23,53,24,61,22,31,21,43,20 };
	Process p7(P7, 7);
	processes.push_back(p7);

	vector<int> P8 = { 11,52,14,42,15,31,17,21,16,43,12,31,13,32,15 };
	Process p8(P8, 8);
	processes.push_back(p8);

	FCFS(processes);
	SJF(processes);
	return 0;
}

/*
void MLFQ(vector<Process> readyQ) {
	int time = 0;
	int notUtilized = 0;
					   
					   
	vector<Process> completedP;
	vector<Process> q1io;
	vector<Process> q2io;
	vector<Process> q3io;
	vector<Process> q1 = readyQ; //tq = 6 units
	vector<Process> q2; //tq = 11 units
	vector<Process> q3; //fcfs
	bool completed = false;

	while (!completed) {
		if (!q1.empty()) {
			calcResponseTime(q1.front(), time);
			if (q1.front().bursts[0] <= 6 && !q1.front().bursts.empty()) {
				time += q1.front().bursts[0];
				for (int i = 1; i < q1.size(); i++) {
					q1[i].waitingTime += q1.front().bursts[0];
				}
				q1.front().bursts.erase(q1.front().bursts.begin());
				if (!q1.front().bursts.empty()) {
					q1.front().ioTime = time + q1.front().bursts[0];
					q1.front().bursts.erase(q1.front().bursts.begin());
					q1io.push_back(q1.front());
					q1.erase(q1.begin());
				}
			}//end if less than 6
			else {
				if (q1.front().bursts.empty()) {
					q1.front().turnaroundTime = time;
					completedP.push_back(q1.front());
					q1.erase(q1.begin());
				}
				else {
					time += 6;
					for (int i = 1; i < q1.size(); i++) {
						q1[i].waitingTime += 6;
					}
					q1.front().bursts[0] -= 6;
					q2.push_back(q1.front());
					q1.erase(q1.begin());
				}//end if greater than six
			}//end if greater than six or no more bursts
		}//end if q1 not empty
		else {
			if (!q2.empty()) {
				if (q2.front().bursts[0] <= 11 && !q2.front().bursts.empty()) {
					time += q2.front().bursts[0];
					for (int i = 1; i < q2.size(); i++) {
						q2[i].waitingTime += q2.front().bursts[0];
					}
					q2.front().bursts.erase(q1.front().bursts.begin());
					if (!q2.front().bursts.empty()) {
						q2.front().ioTime = time + q2.front().bursts[0];
						q2.front().bursts.erase(q2.front().bursts.begin());
						q2io.push_back(q2.front());
						q2.erase(q1.begin());
					}//end if io burst
				}//end if less than 11
				else {
					if (q2.front().bursts.empty()) {
						q2.front().turnaroundTime = time;
						completedP.push_back(q2.front());
						q2.erase(q2.begin());
					}
					else {
						time += 11;
						for (int i = 1; i < q2.size(); i++) {
							q2[i].waitingTime += 11;
						}
						q2.front().bursts[0] -= 11;
									   
													   
														  
								  
							
						
						   
		   
									  
										
										
							
	   
			
				  
											
								
		
								  
						q3.push_back(q2.front());
						q2.erase(q2.begin());
					}//end if greater than 11
				}//end if greater than 11 or no more bursts
					 
		  
								 
												 
	 
			}//end if q2 not empty
			else {
				if (!q3.empty()) {
					if (!q3.front().bursts.empty()) {
						time += q3.front().bursts[0];
						for (int i = 1; i < q2.size(); i++) {
							q3[i].waitingTime += q3.front().bursts[0];
						}
					}
					q3.front().bursts.erase(q3.front().bursts.begin());
					if (!q3.front().bursts.empty()) {
						q3.front().ioTime = time + q3.front().bursts[0];
						q3.front().bursts.erase(q3.front().bursts.begin());
						q3io.push_back(q3.front());
					}//end if io burst
					else {
						q3.front().turnaroundTime = time;
						completedP.push_back(readyQ.front());
					}
					q3.erase(q3.begin());
				}//end if q3 not empty
			}//end if q2 is empty
		}//end if q1 is empty
	}//end master while

	 //int i = 0;
	 //while (!IO.empty() && i < IO.size()) {
	 //	if (IO[i].isIOcomplete(time)) {
	 //		readyQ.push_back(IO[i]);
	 //		swap(IO[i], IO.back());
	 //		IO.pop_back();
	 //		i--;
	 //	}
	 //	i++;
	 //}

*/
