package fileparser;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import xml.FileData;
import data.InputDataType;
/*
 * ClassName FileParser
 * 
 * Created On: 03/02/07
 *
 * Purpose: This class provides the parseFile methot that will parse the config file.
 *   
 */
public class FileParser {
	/**
	 * Method name: parseFile
	 * @param f
	 * @return InputDataBean
	 * This method takes in the config file and parses out the data from the file and returns an FileData object.
	 */
	public static FileData parseFile(File f) {
		FileData parameters = null;
		
		try {
	        BufferedReader in = new BufferedReader(new FileReader(f));
	        parameters = new FileData();
	        String str;
	        while ((str = in.readLine()) != null) {
	        	
	            int indexOf = str.indexOf("=");	            
	            String value = str.substring(indexOf+1, str.length());
	            String header = str.substring(0, indexOf);
	            if ( header.equals("antecedent") ) {
	            	List<InputDataType> list = createList(value);
	            	if ( list != null) {
	            		if ( isListAntecedents(list) ) {
	            			parameters.setAnticedentList(list);
	            		} else {
	            			System.err.println("Check config file: check antecedent categories.  May contain consequent categories");
	            			parameters = null;
	            			break;
	            		}
	            	} else {
	            		System.err.println("Check config file: Must have at least one antecedent category");
	            		parameters = null;
	            		break;
	            	}
	            } else if ( header.equals("consequent") ) {
	            	List<InputDataType> list = createList(value);
	            	if ( list != null) {
	            		if ( isListConseqents(list) ) {
	            			parameters.setConsequentList(list);
	            		} else {
	            			System.err.println("Check config file: check consequent categories.  May contain antecedent categories");
	            			parameters = null;
	            			break;
	            		}
	            	} else {
	            		System.err.println("Check config file: Must have at least one consequent category");
	            		parameters = null;
	            		break;
	            	}
	            } else if ( header.equals("minfreq") ) {
	            	if ( isInt(value) ) {
		            	int tempInt = Integer.parseInt(value);
		            	if ( tempInt > 0 ) {
		            		parameters.setMinFreq(tempInt);
		            	} else {
		            		System.err.println("Minimum frequency must be greater than 0");
		            		parameters = null;
		            		break;
		            	}
	            	} else {
	            		System.err.println("Minimum frequency must be an integer greater than 0");
	            		parameters = null;
	            		break;
	            	}
	            } else if ( header.equals("minconf") ) {
	            	Float tempFloat = 0.0f;
	            	try {
	            		tempFloat = Float.parseFloat(value);
	            	} catch ( Exception e) {
	            		System.err.println("mininum confidence must be a decimal");
	            		parameters = null;
	            		break;
	            	}
	            	if ( tempFloat > 0.0f && tempFloat <= 1.0f ) {
	            		parameters.setMinConf(tempFloat);
	            	} else {
	            		System.err.println("Minimum confidence must be between 0 and 1");
	            		parameters = null;
	            		break;
	            	}
	            } else if ( header.equals("antwidth") ) {
	            	if ( isInt(value) ) { 
	            		int tempInt = Integer.parseInt(value);
	            		if ( tempInt > 0 ) {
	            			parameters.setAntWidth(tempInt);
	            		} else {
	            			System.err.println("Antwidth must be greater than 0");
	            			parameters = null;
	            			break;
	            		}
	            	} else {
	            		System.err.println("antwidth confidence must be an integer");
	            		parameters = null;
	            		break;
	            	}
	            } else if ( header.equals("conswidth") ) {
	            	if ( isInt(value) ) { 
		            	int tempInt = Integer.parseInt(value);
		            	if ( tempInt > 0 ) {
		            		parameters.setConsWidth(tempInt);
		            	} else {
		            		System.err.println("Conswidth must be greater than 0");
		            		parameters = null;
		            		break;
		            	}
	            	} else {
	            		System.err.println("conswidth confidence must be an integer");
	            		parameters = null;
	            		break;
	            	}
	            	
	            } else if ( header.equals("lagwidth") ) {
	            	if ( isInt(value) ) {
	            		int tempInt = Integer.parseInt(value);
	            	
	            		if ( tempInt > 0 ) {
	            			parameters.setLagWidth(tempInt);
	            		} else {
	            			System.err.println("lagwidth must be greater than 0");
	            			parameters = null;
	            			break;
	            		}
	            	} else {
	            		System.err.println("lagwidth must be an integer");
	            		parameters = null;
	            		break;
	            		
	            	}
	            	
	            } else if ( header.equals("episodetype") ) {
	            	if ( value.equals("parallel") || value.equals("serial") ) {
	            		parameters.setEpisodeType(value);
	            	} else {
	            		System.err.println("episode type must be either parallel or serial");
	            		parameters = null;
	            		break;
	            	}
	            	
	            } else if ( header.equals("allowrepeats") ) {
	            	Boolean tempBool = Boolean.parseBoolean(value); 
	            	parameters.setAllowRepeats(value);	            	
	            } else if ( header.equals("ruletype") ) {
	            	int tempInt = Integer.parseInt(value);
	            	if ( tempInt > 0 ) {
	            		parameters.setRuleType(tempInt);
	            	} else {
	            		System.err.println("ruletype must be greater than 0");
	            		parameters = null;
	            		break;
	            	}
	            	
	            } else if ( header.equals("lagtype") ) {
	            	if ( ! value.equals("") ) {
	            		parameters.setLagType(value);
	            	} else {
	            		System.err.println("check file: lagtype required");
	            		parameters = null;
	            		break;
	            	}
	            }
	        }
	        in.close();
	    } catch (IOException ioe) {
	    	ioe.printStackTrace();
	    } catch (Exception e) {
	    	e.printStackTrace();
	    }
		return parameters;
	}
	/**
	 * 
	 * @param category
	 * @return InputDataType
	 * Returns the matching InputDataType object for the String provided
	 */
	private static InputDataType parseCategory(String category) {
		InputDataType idt = null;
		if ( category.equals("SHRD") ) {
			idt =  InputDataType.SHRD;
		} else if ( category.equals("SST") ) {
			idt = InputDataType.SST;
		} else if ( category.equals("POT") ) {
			idt = InputDataType.POT;
		} else if ( category.equals("RHLO") ) {
			idt = InputDataType.RHLO;
		} else if ( category.equals("LAT") ) {
			idt = InputDataType.LAT;
		} else if ( category.equals("LON") ) {
			idt = InputDataType.LON;
		} else if ( category.equals("PD12") ) {
			idt = InputDataType.PD12;
		} else if ( category.equals("USTM") ) {
			idt = InputDataType.USTM;
		} else if ( category.equals("U200") ) {
			idt = InputDataType.U200;
		} else if ( category.equals("REFC") ) {
			idt = InputDataType.REFC;
		} else if ( category.equals("PSLV") ) {
			idt = InputDataType.PSLV;
		} else if ( category.equals("RI") ) {
			idt = InputDataType.RI;
		}
		return idt;
	}
	/**
	 * 
	 * @param categories
	 * @return List<InputDataType>
	 * This method takes in a string (either antecedent string or consequent string) and returns a list of
	 * InputDataType objects
	 */
	private static List<InputDataType> createList(String categories) {
		List<InputDataType> tempList = null;
		if ( categories.length() > 0 ) {
			String[] category = categories.split("\\s*,\\s*");
		
			tempList = new ArrayList<InputDataType>();
			for ( String tmpStr: category ) {
				tempList.add(parseCategory(tmpStr));
			}
		} 
		return tempList;
	}
	/**
	 * 
	 * @param val
	 * @return boolean
	 * Checks if the string value is an integer or not
	 */
	private static boolean isInt(String val) {
		boolean returnVal = true;
		try {
			int temp = Integer.parseInt(val); 
		} catch (Exception e) {
			System.err.println("FileParser class.  Method isInt: This string cannot be converted into a integer.");
			returnVal = false;
		}
		return returnVal;
	}
	/**
	 * 
	 * @param list
	 * @return boolean
	 * Checks if the parsed list contains all antecedent categories or not
	 */
	private static boolean isListAntecedents(List<InputDataType> list) {
		boolean isAntecedent = true;
		for ( InputDataType idt: list ) {
			if ( !idt.isAntecedent() ) {
				isAntecedent = false;
				break;
			}
		}
		return isAntecedent;
	}
	/**
	 * 
	 * @param list
	 * @return boolean
	 * Checks if the parsed list contains all consequent categories or not
	 */
	private static boolean isListConseqents(List<InputDataType> list) {
		boolean isConsequent = true;
		for ( InputDataType idt: list ) {
			if ( idt.isAntecedent() ) {
				isConsequent = false;
				break;
			}
		}
		return isConsequent;
	}
}
