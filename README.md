# STM32_Ultrasound
Ultrasound module connected to an STM32 board bare metal code

Rochester Institute of Technology
SWEN 563/CMPE 663/EEEE 663 
Project G - Ultrasound

Overview of the Graduate Student Project:
Building upon your prior work with the bare metal development using the STM32 Discovery board design and implement an ultrasonic range measurement system. This is the same type of system used in many of today’s generation of cars to sense and locate nearby objects.

Design Requirements:
•	The distance is measured at a rate of 10 times per second.
•	The distances shall be stored in integer units of millimeters.
•	Create a simple command line interface with these features:
o	Perform the POST as described in the POST section that follows.
o	Do a single measurement and immediately display the distance in mm (integer). This measurement is not stored.
o	Start measurement of 100 readings. No output display during the reading is permitted. Store 100 valid values in an array. Discard values that are out of range.
o	Display most recent set of 100 measurements as integers to the virtual comm port. The purpose of this will be to capture that set of 100 readings into a log file which you will then analyze and include with your report as a CSV file. See Report Additional Section for details.
o	While doing 100 measurements stop the measurement if the user presses any key on the virtual terminal. Verify that this check for a key press does not impact the accuracy of your measurements. If the user stops the measurement display the number of valid readings that were saved when the user stopped the measurements.
•	Out of range occurs when the nearest object is closer than 50 mm and further than 1000 mm. This is intentionally less than the sensor’s capabilities. On a single out of range reading the distance is displayed as three asterisks. When doing a set of 100 measurements simply discard out of range values.
•	After the set of 100 measurements is complete display the maximum and minimum distances measured.

Technical Notes:
To measure distance with the ultrasound sensor, generate a positive “ping” pulse that remains high for at least 10 microseconds. The output from the ultrasound sensor returns a positive pulse, the duration of which will be from 100 microseconds to 18 milliseconds. However, you can ignore all readings that are further than 1000 mm.

The duration of the return pulse equals the round-trip propagation time of the “ping” pulse between the sensor and the target. The distance is determined by measuring the duration of the return pulse between the rising and falling edges. Calculate the distance based on the speed of sound in air at room temperature (346 meters/second). Please refer to the technical data sheet for the ultrasound sensor for more information.

Dr. Zhu’s book has a section on ultrasonic sensors that you may find helpful. I put a PDF of that section in the Reference Materials folder in myCourses Content.

POST:
Perform a Power On Self Test (POST) when the program starts.
•	Using the virtual terminal instruct the user to place a large object about 100 mm from the sensor.
•	When the user presses the Enter key in the virtual terminal start the POST.
•	Your POST software will issue a single ping.
•	If an echo is received in the range of 300 to 1000 microseconds the POST passes.
•	If an echo is received before 300 microseconds instruct the user that the object is too close.
•	If an echo is received between 1000 to 5000 microseconds instruct the user that the object is too far.
•	If the object is too close or too far give the user the option to try again or the exit POST with a failure.
•	If the POST is successful proceed to the normal user interface operation described above.

Report Additional Section (CSV file):
The report is required to contain a statistical analysis of two separate sets of measurements at distances of approximately 100 mm and 300 mm. Details:
•	Submit this section as a separate CSV text file. Do NOT include it in a zip file with the other file.
•	The set of 100 measurements at 100 mm must be listed first in 100 lines. The first column must be the measurement number (numbers from 1 to 100 in ascending order). The second column must the integer number of mm. Example:
o	1,115
o	2,93
o	(continue for the remaining measurements)
•	The next five lines must have the statistic name in the first column and the value in the second column. Use the whole number of millimeters (no fraction) except for the Stddev where you use two digits after the decimal point as shown below. Use the order and statistic name as shown in the example below:
o	Minimum,89
o	Maximum,123
o	Mean,102
o	Median,104
o	Stddev,5.07
•	Follow this by a second section by 100 lines with the measurements at 300 mm followed by the same four statistics for the 300 mm measurements. 
Submission:
Submit your report (in Word or PDF format only), CSV file, and source code files as separate files to the Project 5 Dropbox in myCourses. Do not combine in a zip file.

Refer to the Report Specification for the format and contents of your report. Be sure to include the additional CSV report.

This project will be demonstrated in class to the instructor or Course Assistant. 
