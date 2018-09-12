package data;
/*
 * ClassName Dataset
 * 
 * Created On: 03/02/07
 *
 * Purpose: Since, the mowcatl code requires that the timestamp to be in increasing order for different dataset to appear.  This
 * class is used as holder for the values for the different categories.
 *   
 */
import java.util.ArrayList;
import java.util.List;


public class Dataset {
	private InputDataType dataType;	
	private List<Integer> values; 
	
	public Dataset(InputDataType dataType) {
		this.dataType = dataType;		
		values = new ArrayList<Integer>();
	}
	
	public InputDataType getDataType() {
		return dataType;
	}
	
	public void addValues(List<Integer> values) {
		this.values = values;
	}
	
	public List<Integer> getValues() {
		return values;
	}
}