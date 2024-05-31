RealTimeOS Scheduling Algorithms README
=========================================

This is the README file for our RealTime OS SCheduling algorithms Implementation code file.

Here we have 2 Structs to store the Processes info and Scheduling Metrics info.


generateRandomProcessesToFile Method Implementation
---------------------------------------------------

Parameter n is passed inside the function.
Creates the new file and writes the data into it, with the Processes information. 
A for loop is used and run till n times and Generates the 5 random numbers each time.


readProcessesFromFile Method Implementation
-------------------------------------------

Opens and reads the generated random file, and stores the randonly generated numbers data as Process Name, Burst Time, Priority, Deadline, Period, Share for each process.


checkprio, checkdeadline, checkperiod, checkshare
-------------------------------------------------

Verifies the priorities between any 2 processes according to the piority, deadlines, periods, shares respectively.


calAverageTat, calAverageWaittime, calAverageResponseTime 
---------------------------------------------------------

Calculates the Average TurnAround Time, Average Waiting Time, Average Response Time respectively.


runPriorityScheduling, runEDFScheduling, runRMScheduling, runProportionalScheduling
-----------------------------------------------------------------------------------

Schedules the Processes according to the related scheduling algorithms, and calculates the TurnAround Time, Waiting Time, Response Time for each Process.



Main Function
-------------

Main function is passed with 2 arguments, to get the info of number of Processes to execute, from the user.
Calls "generateRandomProcessesToFile" Method and writes the data to the "random_processes.txt" file.
Calls "readProcessesFromFile" Method to read the generated data and store it in struct Process.
Calls "runPriorityScheduling" Method to run the Priority SCheduling Algorithm and prints the results of it i.e., TurnAround Time, Waiting Time, Response Time for each Process.
Same repeats for "runEDFScheduling", "runRMScheduling", "runProportionalScheduling" scheduling Methods and prints the same for each Scheduling algorithm.
Calculates the Scheduling Metrics i.e., Average TurnAround Time, Average Waiting Time, Average Response Time for all the processes for each of the 4 scheduling algorithms, by calling "calAverageTat", "calAverageWaittime", "calAverageResponseTime" respectively and storing them in their respective scheduling metric variables.
Prints each metric value for each scheduling algorithm.

Conclusion
==========

Compares the Average TurnAround Time and Average Waiting Time for all 4 Scheduling algorithms and prints the Best Scheduling Algorithm, which have the lower values of the metrics.

Also, here as we are taking the number of processes value from the user each time we run the code, we can try with different number of processes and analyze the output which gives as the Best Scheduling algorithm, when number of processes varies too.


Execution
=========

After implementing all these methods in the code file, we have to move all this file to the one of the UNT Servers and execute them in PUTTY, by running the following commands.

To compile, run
$ gcc -o RealTimeOS_Scheduling RealTimeOS_Scheduling.c

To test your implementation providing the value of Number of Process to consider, run
$ ./RealTimeOS_Scheduling 5

You can store the output in a file so that you can look at it carefully:
./RealTimeOS_Scheduling 5 > output.txt

You can even store the output each time in the same file and can append it to the all old outputs, run
./RealTimeOS_Scheduling 10 >> output.txt


