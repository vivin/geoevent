package main;
import java.util.*;
import java.io.*;
import javax.xml.parsers.*;
import org.xml.sax.*;
import org.xml.sax.helpers.*;
import station.Station;

public class mowcatl_convert {
	public final int MONTHS_IN_YEAR = 12;
	private static String inputFile = null;
	private static PrintWriter outputFile = null;
	private static BufferedReader dateFile = null;
	private static BufferedReader stationFile = null;
	private static Station station = null;
	
	public static void main(String[] args) throws IOException {	
		String outputFileName="MOWCATLRulesStation", 
		dateFileName=null, stationFileName=null;
		
		inputFile = "D:/MOWCATLApplicationAug2010/mowcatlConvert/MowcatlInput.xml";
		dateFileName = "D:/MOWCATLApplicationAug2010/mowcatlConvert/StartDate.txt";
		stationFileName = "D:/MOWCATLApplicationAug2010/mowcatlConvert/Station.txt";
		
		/*if ( args.length > 0 ) {
			inputFile = args[0];
			dateFileName = args[1];
			stationFileName = args[2];
			
		} else {
			System.err.println("Must provide three file paths");
			System.exit(-1);
		}*/
		
		dateFile =  openDateFile(dateFileName);
		stationFile = openStationFile(stationFileName);
		station = getStation(stationFile);
		outputFileName += station.getID()+".csv";
		outputFile = openOutputFile(outputFileName);
		mowcatl_convert parserDemo = new mowcatl_convert();
		
//		String uri = openInputFile();
//		parserDemo.parseXML(uri);
		parserDemo.parseXML(inputFile);
		
		outputFile.close();
	}
	/**
	 * Creates map of precipitation values
	 * @return precipMap
	 */
	public Map createMap() {
		Map precipMap = new LinkedHashMap(70, 1.00F);
		precipMap.put("1", "3_Month_SPI_Extremely_Dry");
		precipMap.put("2", "3_Month_SPI_Severely_Dry");
		precipMap.put("3", "3_Month_SPI_Moderately_Dry");
		precipMap.put("4", "3_Month_SPI_Normal");
		precipMap.put("5", "3_Month_SPI_Moderately_Wet");
		precipMap.put("6", "3_Month_SPI_Severely_Wet");
		precipMap.put("7", "3_Month_SPI_Extremely_Wet");
		precipMap.put("8", "6_Month_SPI_Extremely_Dry");
		precipMap.put("9", "6_Month_SPI_Severely_Dry");
		precipMap.put("10", "6_Month_SPI_Moderately_Dry");
		precipMap.put("11", "6_Month_SPI_Normal");
		precipMap.put("12", "6_Month_SPI_Moderately_Wet");
		precipMap.put("13", "6_Month_SPI_Severely_Wet");
		precipMap.put("14", "6_Month_SPI_Extremely_Wet");
		precipMap.put("15", "9_Month_SPI_Extremely_Dry");
		precipMap.put("16", "9_Month_SPI_Severely_Dry");
		precipMap.put("17", "9_Month_SPI_Moderately_Dry");
		precipMap.put("18", "9_Month_SPI_Normal");
		precipMap.put("19", "9_Month_SPI_Moderately_Wet");
		precipMap.put("20", "9_Month_SPI_Severely_Wet");
		precipMap.put("21", "9_Month_SPI_Extremely_Wet");
		precipMap.put("22", "12_Month_SPI_Extremely_Dry");
		precipMap.put("23", "12_Month_SPI_Severely_Dry");
		precipMap.put("24", "12_Month_SPI_Moderately_Dry");
		precipMap.put("25", "12_Month_SPI_Normal");
		precipMap.put("26", "12_Month_SPI_Moderately_Wet");
		precipMap.put("27", "12_Month_SPI_Severely_Wet");
		precipMap.put("28", "12_Month_SPI_Extremely_Wet");
		precipMap.put("29", "PDSI_Extremely_Dry");
		precipMap.put("30", "PDSI_Severely_Dry");
		precipMap.put("31", "PDSI_Moderately_Dry");
		precipMap.put("32", "PDSI_Normal");
		precipMap.put("33", "PDSI_Moderately_Wet");
		precipMap.put("34", "PDSI_Severely_Wet");
		precipMap.put("35", "PDSI_Extremely_Wet");
		precipMap.put("36", "SOI_Extremely_Wet");
		precipMap.put("37", "SOI_Severely_Wet");
		precipMap.put("38", "SOI_Moderately_Wet");
		precipMap.put("39", "SOI_Normal");
		precipMap.put("40", "SOI_Moderately_Dry");
		precipMap.put("41", "SOI_Severely_Dry");
		precipMap.put("42", "SOI_Extremely_Dry");
		precipMap.put("43", "MEI_Extremely_Dry");
		precipMap.put("44", "MEI_Severely_Dry");
		precipMap.put("45", "MEI_Moderately_Dry");
		precipMap.put("46", "MEI_Normal");
		precipMap.put("47", "MEI_Moderately_Wet");
		precipMap.put("48", "MEI_Severely_Wet");
		precipMap.put("49", "MEI_Extremely_Wet");
		precipMap.put("50", "NAO_Extremely_Dry");
		precipMap.put("51", "NAO_Severely_Dry");
		precipMap.put("52", "NAO_Moderately_Dry");
		precipMap.put("53", "NAO_Normal");
		precipMap.put("54", "NAO_Moderately_Wet");
		precipMap.put("55", "NAO_Severely_Wet");
		precipMap.put("56", "NAO_Extremely_Wet");
		precipMap.put("57", "PDO_Extremely_Dry");
		precipMap.put("58", "PDO_Severely_Dry");
		precipMap.put("59", "PDO_Moderately_Dry");
		precipMap.put("60", "PDO_Normal");
		precipMap.put("61", "PDO_Moderately_Wet");
		precipMap.put("62", "PDO_Severely_Wet");
		precipMap.put("63", "PDO_Extremely_Wet");
		precipMap.put("64", "PNA_Extremely_Dry");
		precipMap.put("65", "PNA_Severely_Dry");
		precipMap.put("66", "PNA_Moderately_Dry");
		precipMap.put("67", "PNA_Normal");
		precipMap.put("68", "PNA_Moderately_Wet");
		precipMap.put("69", "PNA_Severely_Wet");
		precipMap.put("70", "PNA_Extremely_Wet");
		
		return precipMap;
	}
	

	/**
	 * Gets and opens output file from argument 2
	 * @return outputFile
	 * @throws IOException
	 */
	public static PrintWriter openOutputFile(String outputFileName) throws IOException
	{
		FileWriter outputFwriter = new FileWriter(new File(outputFileName));
		PrintWriter outputFile = new PrintWriter(outputFwriter);
		return outputFile;
	}
	
	/**
	 * Gets and opens date file
	 * @return dateFile
	 */
	public static BufferedReader openDateFile(String dateFileName) {
		try {
			FileReader fileReader = new FileReader(dateFileName);
			BufferedReader dateFile = new BufferedReader(fileReader);
			return dateFile;
		} catch(FileNotFoundException e){
			System.out.println("Date File not found!");
			return null;
		}
	}

	/**
	 * Gets starting date from file
	 * @param BufferedReader reader
	 * @throws IOException
	 */
	public void getDate(BufferedReader reader) throws IOException {
		String line = reader.readLine();
		StringTokenizer tokenizer = new StringTokenizer(line);
		startMonth = Integer.parseInt(tokenizer.nextToken());
		startYear = Integer.parseInt(tokenizer.nextToken());	
		reader.close();	
	}

	
	/**
	 * Gets and opens station file
	 * @return stationFile
	 */
	public static BufferedReader openStationFile(String stationFileName) {
		try {
			FileReader fileReader = new FileReader(stationFileName);
			BufferedReader stationFile = new BufferedReader(fileReader);
			return stationFile;
		} catch(FileNotFoundException e){
			System.out.println("Station File not found!");
			return null;
		}
	}

	
	/**
	 * Gets stationID & name from file
	 * @param BufferedReader reader
	 * @throws IOException
	 */
	public static Station getStation(BufferedReader reader) throws IOException
	{
		String line = reader.readLine();
		StringTokenizer tokenizer = new StringTokenizer(line,",");
		String ID = tokenizer.nextToken();
		String name = tokenizer.nextToken();	
		reader.close();
		return new Station(ID,name);
	}
	
	/**
	 * Converts a date from XML document to human-readable date
	 * @param String s
	 * @return newDate
	 */
	public String parseDate(String s) {
		int tempYears = Integer.parseInt(s)/MONTHS_IN_YEAR;
		int remainderMonths = Integer.parseInt(s)%MONTHS_IN_YEAR;
		int newMonth = startMonth + remainderMonths;
		int newYear = startYear + tempYears;
		if (newMonth>12)
		{
			newMonth -= 12;
			newYear += 1;
		}
		
		String newDate = newMonth + "-" + newYear;
		return newDate;
	}

	/**
	 * Creates SAX parser and parses XML document
	 * @param String uri
	 */
	public void parseXML(String uri) {
		System.out.println("Parsing XML File: " + uri);

		try {
			// Instantiate a parser
			SAXParserFactory factory = SAXParserFactory.newInstance();
			SAXParser parser = factory.newSAXParser();

			// Parse the document
			parser.parse(uri, handler);
		}catch(IOException e){
		}catch(SAXException e){
		}catch(ParserConfigurationException e){
		}
	}
	
	String outputString = "", name;
	boolean firstAnt = false, firstCons = false;
	Map precipMap = createMap();
	private static int startMonth, startYear;
	
	
	/**
	 * Handler for SAX parser
	 */
	DefaultHandler handler = new DefaultHandler()
	{
		private Locator locator;

		public void setDocumentLocator(Locator locator) {
//			System.out.println("   * setDocumentLocator() called");
			this.locator = locator;
		}

		public void startDocument() throws SAXException {
			System.out.println("Parsing begins...");
			outputString += "Station ID,"+station.getID()+"\n";
			outputString += "Station Name,"+station.getName()+"\n";
			try {
				getDate(dateFile);
				outputString += "Starting Date for Data,"+startMonth+"-"+startYear+"\n";
			} catch(IOException e){}
		}

		public void endDocument() throws SAXException {
			outputFile.print(outputString);
			System.out.println("...Parsing completed!");
		}

		public void processingInstruction(String target, String data) throws SAXException {
//			System.out.println("PI: Target: " + target + " and Data: " + data);
		}

		public void startPrefixMapping(String prefix, String uri) {
//			System.out.println("Mapping starts for prefix " + prefix + " mapped to URI " + uri);
		}

		public void endPrefixMapping(String prefix) {
//			System.out.println("Mapping ends for prefix " + prefix);
		}

		public void startElement(String namespaceURI, String localName, String rawName, Attributes atts) throws SAXException {
			name = rawName;
			if (name.equals("AntList"))
			{
				outputString += "\n\"";
				firstAnt = true;
			}
			else if (name.equals("ConsList"))
			{
				outputString += "\",\"";
				firstCons = true;
			}
//			System.out.println("name: " + name);
		}

		public void endElement(String namespaceURI, String localName, String rawName) throws SAXException {
//			System.out.println("endElement: " + localName + "\n");
		}

		public void characters(char[] ch, int start, int length) throws SAXException {
			String s = new String(ch, start, length);
//			System.out.println(name + ": " + s);

			if (name.equals("Anticedent"))
			{
				if (!firstAnt)
					outputString += ", ";
				outputString += precipMap.get(s);		// MAP LOOKUP - s
				firstAnt = false;
			}
			else if (name.equals("Consequent"))
			{
				if (!firstCons)
					outputString += ", ";
				outputString += precipMap.get(s);		// MAP LOOKUP - s
				firstCons = false;
			}
			else if (name.equals("Start"))
			{
				outputString += "\",";
				outputString += parseDate(s);
			}
			else if (name.equals("Stop"))
			{
				outputString += ",";
				outputString += parseDate(s);
			}
			else if (name.equals("Confidence")||
					 name.equals("Support")||name.equals("Coverage")|
					 name.equals("ConsequentSupport")||name.equals("JMeasure"))
				outputString += "," + s;
			else if (name.equals("Anticedents"))
			{
				boolean flag = true;
				outputString += "Anticedents,,\"";
				
				StringTokenizer tokenizer = new StringTokenizer(s,",");
				while (tokenizer.hasMoreTokens())
				{
					if (!flag)
						outputString += ", ";
					else flag = false;
					outputString += precipMap.get(tokenizer.nextToken());		// MAP LOOKUP - tokenizer.nextToken()
				}
				outputString += "\"\n";
			}
			else if (name.equals("Consequents"))
			{
				boolean flag = true;
				outputString += "Consequents,,\"";
				
				StringTokenizer tokenizer = new StringTokenizer(s,",");
				while (tokenizer.hasMoreTokens())
				{
					if (!flag)
						outputString += ", ";
					else flag = false;
					outputString += precipMap.get(tokenizer.nextToken());		// MAP LOOKUP - tokenizer.nextToken()
				}
				outputString += "\"\n\n";
				outputString += "Ant List,Cons List,Start,Stop,Confidence,Support,Coverage,Consequent Support,JMeasure";
			}
			else if (name.equals("MinFreq")||name.equals("MinConf")||
					 name.equals("AntWidth")||name.equals("ConsWidth")||
					 name.equals("LagWidth")||name.equals("EpisodeType")||
					 name.equals("AllowRepeats"))
				outputString += name + "," + s + "\n";
	//		else
		//		System.out.println("Unexpected element: " + name + " " + s);
		}

		public void ignorableWhitespace(char[] ch, int start, int length) throws SAXException {
			String s = new String(ch, start, length);
			System.out.println("ignorableWhitespace: [" + s + "]");
		}

		public void skippedEntity(String name) throws SAXException {
//			System.out.println("Skipping entity " + name);
		}	
	};
}
