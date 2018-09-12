package mapper;
/*
 * Class name: Mapper
 * Purpose: calculate and return mapped values
 */

public class Mapper {
	/**
	 * Method Name: mappedValue
	 * @param datasetNum
	 * @param normalizedValue
	 * @return String
	 * The function takes two integer parameters.  The first integer is the data set number and the second
	 * integer is the normalized value.  Since, the categories are divided into seven indices based on their
	 * normalized value.  The mowcatl algorithm requires a unique set of 7 numbers to distinguish each dataset.
	 * So, this function calculates and returns a unique value for the given dataset and the normalizedvalue.
	 * Eg. for dataset 1 the unique values will be from 1-7
	 *     for dataset 2 the unique values will be from 8-14
	 *     and so on
	 * Even though, for SPI and PDSI the normazlied value returned from FIRM is from -3 to +3, for the purpose of consistency
	 * the normalized value will be from 1-7. This is achieved by adding 4 to the SPI and PDSI normalized value before they are
	 * passed into this method.
	 *   
	 */
	 
	public static String mappedValue(int datasetNum,int normalizedValue) {
		int max_val = 0;
		int mapped_val = 0;
		max_val = datasetNum * 2;		
		mapped_val = max_val - (2 - normalizedValue);
		return Integer.toString(mapped_val);
	}
}
