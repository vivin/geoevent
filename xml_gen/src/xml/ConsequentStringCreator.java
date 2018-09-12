package xml;
/*
 * ClassName ConsequentStringCreator
 * 
 * Created On: 03/02/07
 *
 * 
 */

import java.util.ArrayList;
import java.util.List;

import data.InputDataType;

import mapper.Mapper;

public class ConsequentStringCreator {
	private static List<InputDataType> consequentList;
	static {
		consequentList = new ArrayList<InputDataType>();
		consequentList.add(InputDataType.RI);
	}
	/** 
	 * @param dataSet
	 * @return
	 * This method takes in a List of InputDataType objects and creates the String for the consequent node in the xml file.
	 * The dataSet list contains all the categories i.e., oceanic and drought categories.
	 * eg: dataSet [SPI3,PDSI,SOI,MEI]
	 * This method goes through the dataSet to retrieve the dataSetNum which is the index+1 of the category in the list.
	 * Since, we are only interested in drought values it only processes it for the drought normalized value 1-3 (event though,
	 * for SPI and PDSI the normalized value returned from firm are from -3 to 3 for consistency with the existing mowcatl 1-7 is
	 * used)
	 */
	 
	public static String consequentStringCreator(List<InputDataType> dataSet) {
		StringBuffer consequentString = null;
		
		if ( dataSet.size() > 0 ) {
			consequentString = new StringBuffer();			
		}
		for( InputDataType idt: dataSet ) {
			if ( !(idt.isAntecedent()) ) {				
				int dataSetNum = dataSet.indexOf(idt) + 1;
				for ( int i = 1 ; i <= 2; i++ ) {					
					consequentString.append(Mapper.mappedValue(dataSetNum, i));
					if ( i == 2 && (!doesContainMoreDroughtCategory(dataSet.indexOf(idt), dataSet) )) {
						break;
					}
					consequentString.append(",");
				}
			}
		}
		return consequentString.toString();
	}
	/**
	 * 
	 * @param index
	 * @param dataSet
	 * @return boolean
	 * Checks if there are more drought categories in the dataset
	 */
	private static boolean doesContainMoreDroughtCategory(int index,List<InputDataType>  dataSet) {
		boolean result = false;
		for (int i = index+1; i < dataSet.size(); i++ ) {
			InputDataType temp = dataSet.get(i);
			if ( consequentList.contains(temp) ) {
				result = true;
				break;
			}
		}
		return result;
		
	}
}
