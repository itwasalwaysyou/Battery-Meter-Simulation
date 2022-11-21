#include "batt.h"

// Uses the two global variables (ports) BATT_VOLTAGE_PORT and
// BATT_STATUS_PORT to set the fields of the parameter 'batt'.  If
// BATT_VOLTAGE_PORT is negative, then battery has been wired wrong;
// no fields of 'batt' are changed and 1 is returned to indicate an
// error.  Otherwise, sets fields of batt based on reading the voltage
// value and converting to precent using the provided formula. Returns
// 0 on a successful execution with no errors. This function DOES NOT
// modify any global variables but may access global variables.
//
// CONSTRAINT: Avoids the use of the division operation as much as
// possible. Makes use of shift operations in place of division where
// possible.
//
// CONSTRAINT: Uses only integer operations. No floating point
// operations are used as the target machine does not have a FPU.
// 
// CONSTRAINT: Limit the complexity of code as much as possible. Do
// not use deeply nested conditional structures. Seek to make the code
// as short, and simple as possible. Code longer than 40 lines may be
// penalized for complexity.



int set_batt_from_ports(batt_t *batt){
   if(BATT_VOLTAGE_PORT<0){
   	return 1;
   }

   batt->mlvolts=BATT_VOLTAGE_PORT>>1;
   if (BATT_VOLTAGE_PORT>=7600){
  	 batt -> percent=100;
   }else if (BATT_VOLTAGE_PORT<=6000){
   	batt -> percent=0;
   }else{
   	batt->percent=((BATT_VOLTAGE_PORT>>1)-3000)>>3;
   }
   
   if (BATT_STATUS_PORT&(1<<4)){
   	batt->mode =1;   //if 1, p
   }else{
   	batt->mode =2;   //if 0, vol
   	}
   
   return 0;

}


int set_display_from_batt(batt_t batt, int *display){

	*display=0;

	int nums[]= {0b0111111,0b0000110,0b1011011,0b1001111,0b1100110,0b1101101,0b1111101,0b0000111,0b1111111,0b1101111,0b0000000};


	if(batt.mode ==1){
		*display =*display|(1<<0);

		if(batt.percent==100){//If mode is percent? 
			*display= *display|(nums[1]<<17)|(nums[0]<<10)|(nums[0]<<3);
		}else if(batt.percent<10){
			*display= *display| (nums[(int)batt.percent]<<3);
		}else{  
			*display= *display| (nums[batt.percent/10]<<10)|(nums[batt.percent%10]<<3);
		}
	}else if(batt.mlvolts%1000%100%10>=5){
		*display= *display|(3<<1);
		*display= *display| (nums[batt.mlvolts/1000]<<17)|(nums[batt.mlvolts%1000/100]<<10)|(nums[(batt.mlvolts%1000%100/10)+1]<<3);
	}else {
		*display= *display|(3<<1);
		*display= *display| (nums[batt.mlvolts/1000]<<17)|(nums[batt.mlvolts%1000/100]<<10)|(nums[batt.mlvolts%1000%100/10]<<3);
	}
	if (batt.percent<=5){
		;
	}else if(batt.percent<=29){
		*display= *display|(1<<24);
	}else if (batt.percent<=49){
		*display= *display|(3<<24);
	}else if (batt.percent<=69){
		*display= *display|(7<<24);
	}else if (batt.percent<=89){
		*display= *display|(15<<24);
	}else{
		*display= *display|(31<<24);
	}
	return 0;
}



int batt_update(){

   batt_t batt = {};
   if(set_batt_from_ports(&batt)!=0){
   	return 1;
   }
   set_display_from_batt(batt, &BATT_DISPLAY_PORT);


   return 0;

}


