package xml;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
/*
 * ClassName EventListCreator
 * 
 * Created On: 03/02/07
 *
 * 
 */
import java.util.ArrayList;
import java.util.List;

import mapper.Mapper;

import org.joda.time.DateTime;

import au.com.bytecode.opencsv.CSVReader;
import data.Dataset;
import data.InputDataType;
import eventlist.Event;

public class EventListCreator {

	public static List<Event> createEventList(String csv, List<InputDataType> dataSet) throws IOException {
		File file = new File(csv);
		if (file == null) {
			return null;
		}

		int num = 1;
		int timestamp = 1;
		List<Event> events = new ArrayList<>();
		String preName = "";
		CSVReader csvReader = new CSVReader(new FileReader(file), ',', '\'', 1);
		List<String[]> records = csvReader.readAll();
		for (String[] record : records) {
			String name = record[1];
			if (!name.equals(preName) && !preName.equals("")) {
				timestamp += 100;
			}

			int datasetNum = 1;
			for (InputDataType idt : dataSet) {
				String oriValue = record[idt.getColumn()];
				double value = 0.0;
				if (oriValue.equals("Yes")) {
					value = 1.0;
				} else if (oriValue.equals("No")) {
					value = 0.0;
				} else {
					value = Double.parseDouble(oriValue);
				}
				
				double split = idt.getSplitValue();
				int tempVal = 0;
				if (value > split)
					tempVal = 2;
				else
					tempVal = 1;
				
				
				String mappedVal = Mapper.mappedValue(datasetNum, tempVal);
				events.add((new Event(Integer.toString(timestamp),mappedVal)));
				
				datasetNum++;
			}
			
			timestamp += 1;
			preName = name;
		}

		csvReader.close();
		return events;
	}
}
