package eventlist;

/*
 * ClassName Event
 * 
 * Created On: 03/02/07
 *
 * Purpose: Creating an event object for xml file
 *   
 */

public class Event {
	private String value;
	private String timestamp;
	
	public Event(String timestamp,String value) {
		this.timestamp = timestamp;
		this.value = value;
	}
	 
	public String getValue() {
		return value;
	}
	public String getTimestamp() {
		return timestamp;
	}
	
	public String toString() {
		return "timestamp = " + timestamp + " value = " + value;
	}
}
