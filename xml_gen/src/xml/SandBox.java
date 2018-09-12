package xml;

import java.util.ArrayList;

import data.InputDataType;

public class SandBox {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		ArrayList<InputDataType> dataSet = new ArrayList<InputDataType>();
		
		dataSet.add(InputDataType.RI);
		dataSet.add(InputDataType.SHRD);
		dataSet.add(InputDataType.SST);
		dataSet.add(InputDataType.POT);
		dataSet.add(InputDataType.RHLO);
		dataSet.add(InputDataType.LAT);
		dataSet.add(InputDataType.LON);
		dataSet.add(InputDataType.PD12);
		dataSet.add(InputDataType.U200);
		dataSet.add(InputDataType.REFC);
		dataSet.add(InputDataType.PSLV);

		String test = AnticedentStringCreator.anticedentStringCreator(dataSet);
		System.out.println(test);
		String testC = ConsequentStringCreator.consequentStringCreator(dataSet);
		System.out.println(testC);
	}
}
