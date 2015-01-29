#include <KEIL_VRS1000_regs.h>
#include "LibIO_T3IO.h"
#include "define.h"
#include "calibration_IO.h"
#include "math.h"




// ****************************************************************************************************
// void produce_LUT(void)
// function:	from data entered, tabulate the LUT for calibration
// note: 	- no return, no parameters required
//			- only requires that input_calibration_grid[][] array is set
// ****************************************************************************************************
void produce_LUT(void)
{
	unsigned char input_selection=0;
	unsigned char note, test_index, store_index;
	unsigned int reading_difference, rectified_difference;	// reading is x, rectified is y
	unsigned long store_value, test_value;


// --- trial data ---
input_calibration_grid[0][0] = 0;
input_calibration_grid[0][1] = 0;
input_calibration_grid[0][2] = 300;
input_calibration_grid[0][3] = 350;
input_calibration_grid[0][4] = 700;
input_calibration_grid[0][5] = 685;
input_calibration_grid[0][6] = 1000;
input_calibration_grid[0][7] = 950;


input_calibration_grid[1][0] = 0;
input_calibration_grid[1][1] = 2;
input_calibration_grid[1][2] = 250;
input_calibration_grid[1][3] = 275;
input_calibration_grid[1][4] = 645;
input_calibration_grid[1][5] = 695;
input_calibration_grid[1][6] = 1000;
input_calibration_grid[1][7] = 1000;

input_calibration_grid[2][0] = 250;
input_calibration_grid[2][1] = 275;
input_calibration_grid[2][2] = 645;
input_calibration_grid[2][3] = 695;

input_calibration_grid[2][0] = 0;
input_calibration_grid[2][1] = 75;
input_calibration_grid[2][2] = 250;
input_calibration_grid[2][3] = 275;
input_calibration_grid[2][4] = 645;
input_calibration_grid[2][5] = 695;
// --- end trial data ---




	for(input_selection=0; input_selection<MAX_INPUT_CHANNELS; input_selection++)	// watch out for 32IN card... must test
	{	// first check if calibration points have been entered
		if( (input_calibration_grid[input_selection][0] == 0) && (input_calibration_grid[input_selection][1] == 0)
			&& (input_calibration_grid[input_selection][2] == 0) && (input_calibration_grid[input_selection][3] == 0) )
		{	// in the case no calibration entered, we make a note
			note = 1;
		}
		else
		{	// in the case there is, we produce LUT
			// check if first point is zero
			if (input_calibration_grid[input_selection][0] == 0)
			{	// first point is zero, store data in defines tab and then perform algorithm
				VoltageReading_defines[input_selection][0] = input_calibration_grid[input_selection][1];
				test_index = 2;
				store_index = 1;
				test_value = 100;
			}
			else
			{	// first point is not zero, enter default value and perform first set of algorithm
				VoltageReading_defines[input_selection][0] = 0;
				test_index = 0;
				store_index = 1;
				test_value = 100;

				while( (test_value <= input_calibration_grid[input_selection][0]) && (test_value<=1000) )
				{	// evaluate delta x, theoretical difference compared to default zero
					reading_difference = abs(0 - input_calibration_grid[input_selection][0]);
					// evaluate delta y, desired difference compared to default zero
					rectified_difference = abs(0 - input_calibration_grid[input_selection][1]);
					// evaluate proportional increemnt and store into array
					store_value = rectified_difference*test_value/reading_difference;
					VoltageReading_defines[input_selection][store_index] = store_value;
				
				
					// increment all index ---
					store_index++;
					test_value += 100;
				}
				// while loop below needs to have index started at 2
				test_index = 2;

			}

			// produce remainder of the define tab calibration LUT
			while( (test_value <= input_calibration_grid[input_selection][test_index]) && (test_value<=1000) && (test_index<20) )
			{	// evaluate delta x, theoretical difference
				reading_difference = abs(input_calibration_grid[input_selection][test_index] - input_calibration_grid[input_selection][test_index-2]);
				// evaluate delta y, desired difference
				rectified_difference = abs(input_calibration_grid[input_selection][test_index+1] - input_calibration_grid[input_selection][test_index-1]);
				// evaluate proportional increemnt and store into array, store value = slope(test_value) + offset, y=mx+b
				store_value = rectified_difference*(test_value-input_calibration_grid[input_selection][test_index-2])/reading_difference + input_calibration_grid[input_selection][test_index-1];
				VoltageReading_defines[input_selection][store_index] = store_value;
			
			
				// increment all index ---
				store_index++;
				test_value += 100;
			
				while( (test_value > input_calibration_grid[input_selection][test_index]) && (test_index<20) )
					test_index+=2;
			
			
			}
			

			
			// in the case max value was not entered, we enter default
			if( (test_index >= 20) && (test_value<=1000) )
			{	// we already know last defined tab is 1000
				VoltageReading_defines[input_selection][DEFINE_TABS-1] = 1000;
			
				// search for non zero value
				while(input_calibration_grid[input_selection][test_index] == 0)
					test_index-=2;
				// given we are now using last value entered, test_index does not need to be incremented
			
				// now need to fill in remining components
				while(test_value<=1000)
				{	// evaluate delta x, theoretical difference compared to default 1000
					reading_difference = abs(1000 - input_calibration_grid[input_selection][test_index]);
					// evaluate delta y, desired difference compared to default 1000
					rectified_difference = abs(1000 - input_calibration_grid[input_selection][test_index+1]);
					// evaluate proportional increemnt and store into array, store value = slope(test_value) + offset, y=mx+b
					store_value = rectified_difference*(test_value-input_calibration_grid[input_selection][test_index])/reading_difference + input_calibration_grid[input_selection][test_index+1];
					VoltageReading_defines[input_selection][store_index] = store_value;
				
				
					// increment all index ---
					store_index++;
					test_value += 100;
					// given we are now using last value entered, test_index does not need to be incremented
				}
			
			
			
			}


		}
	
	}// end of 	for(input_selection=0; input_selection<MAX_INPUT_CHANNELS; input_selection++)
	

}









// ****************************************************************************************************
// void rectify_reading(unsigned char register_location, unsigned int pic_reading)
// function:	given a certain LUT, rectify value being read
// parameters:	desired Input or Output, pic reading
// will return rectified value
// ****************************************************************************************************


unsigned int rectify_reading(unsigned char IO_port_select, unsigned int pic_reading)
{
	unsigned char define_index = 5;
	unsigned int rectified_difference, rectified_reading;	// x difference is 100, rectify difference is y

	//find the range of pic reading
	while(pic_reading > define_index*100)
		define_index++;

	while(pic_reading < define_index*100)
		define_index--;
	
	// we now have the lower limit of the range
	// find slope and perform x=(y-b)/m
	rectified_difference = VoltageReading_defines[IO_port_select][define_index+1] - VoltageReading_defines[IO_port_select][define_index];
	rectified_reading = (pic_reading - (define_index*100))*rectified_difference/100 + VoltageReading_defines[IO_port_select][define_index];

	return rectified_reading;
}
























