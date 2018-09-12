package main;

import java.io.*;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import xml.AnticedentStringCreator;
import xml.ConsequentStringCreator;
import xml.EventListCreator;
import xml.FileData;
import xml.InputXml;
import data.InputDataType;
import eventlist.Event;
import fileparser.FileParser;

public class CreateXML {
	
	public static void main(String[] args) throws IOException {
		
		String configfile = null;
		String outputfile = null;
		String csvfile = null;
		List<InputDataType> dataSet = null;
		File f= null;
		FileData params = null;
		InputXml inputXML = null;
		List<Event> events = null;
		
		csvfile = "..//data//xml_gen//input//SHIPS_MiningRI1982_2013.csv";
		configfile = ".//TemporalInputConfig.txt";
		outputfile = "..//data//xml_gen//output//ships_mocatl_temporal_input.xml";
		
		params = FileParser.parseFile(new File(configfile));
		if ( params != null ) {
			dataSet = new ArrayList<InputDataType>();
			for ( InputDataType idt: params.getConsequentList() ) {
				dataSet.add(idt);
			}
			for ( InputDataType idt: params.getAnticedentList() ) {
				dataSet.add(idt);
			}

			inputXML = new InputXml();
			inputXML.createUserInput(params.getMinFreq(),params.getMinConf(),params.getAntWidth(),params.getConsWidth(),
										params.getLagWidth(),params.getEpisodeType(),Boolean.valueOf(params.getAllowRepeats()));
			String anteString = AnticedentStringCreator.anticedentStringCreator(dataSet);
			String conseString = ConsequentStringCreator.consequentStringCreator(dataSet);

			events = EventListCreator.createEventList(csvfile, dataSet);
			
			inputXML.createAnticedent(anteString);
			inputXML.createConsequent(conseString);
			inputXML.createRuleTypeAndLagType(params.getRuleType(),params.getLagType());
			inputXML.createEventList(events);
			
			f = new File(outputfile); 
			inputXML.createParams(f);
			System.out.println("file created");
		}
	}
}
