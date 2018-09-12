package data;
/*
 * ClassName InputDataType
 * 
 * Created On: 03/02/07
 *
 * Purpose: This enum class provides enum value for the most frequent antecedent and consequent category used by the user.
 *   
 */
public enum InputDataType {
	
	/*RI("RI",0,false),
	SHRD("SHRD",0,true),
	SST("SST",0,true),
	POT("POT",0,true),
	RHLO("RHLO",0,true),
	LAT("LAT",0,true),
	LON("LON",0,true),
	USTM("USTM",0,true),
	U200("U200",0,true),
	REFC("REFC",0,true),
	PSLV("PSLV",0,true);
	*/
	
	RI("RI",150,0.0,false),
	PD12("PD12",137,4.6,true),
	SHRD("SHRD",69,49.0,true),
	SST("SST",16,284.0,true),
	POT("POT",143,47.6,true),
	RHLO("RHLO",34,69.7,true),
	LAT("LAT",12,197.0,true),
	LON("LON",14,632.0,true),
	USTM("USTM",142,-3.1,true),
	U200("U200",25,-0.6,true),
	REFC("REFC",44,0.9,true),
	PSLV("PSLV",40,583.4,true);

	private String name;
	private int step;
	private boolean anteCedent;
	
	private int columnInCVS;
	private double splitValue;
	
	
	private InputDataType(String name, int step, boolean anticedent) {
		this.name = name;
		this.step = step;
		this.anteCedent = anticedent;
	}
	
	private InputDataType(String name, int column, double value, boolean anticedent) {
		this.name = name;
		this.columnInCVS = column;
		this.splitValue = value;
		this.anteCedent = anticedent;
	}
	
	public int getColumn() {
		return this.columnInCVS;
	}
	
	public double getSplitValue() {
		return this.splitValue;
	}
	
	public String getName() {
		return this.name;
	}
	
	public int getStep() {
		return this.step;
	}
	
	public boolean isAntecedent() {
		return this.anteCedent;
	}
}
