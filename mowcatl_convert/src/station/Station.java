package station;

public class Station {
	private String stationID;	
	private String stationName; 
	
	public Station(String id, String name) {
		this.stationID = id;		
		this.stationName = name;
	}
	
	public String getID() {
		return stationID;
	}
	
	public String getName() {
		return stationName;
	}

	public String toString (){
		return stationID + ","+ stationName;
	}
}
