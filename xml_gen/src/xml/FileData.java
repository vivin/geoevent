package xml;
/*
 * ClassName EventListCreator
 * 
 * Created On: 03/02/07
 *
 * Purpose: To hold data parsed from the config file
 * 
 */
import java.util.ArrayList;
import java.util.List;
import data.InputDataType;

public class FileData {

	private List<InputDataType> anticedentList;
	private List<InputDataType> consequentList;	
	
	private int minFreq;
	private float minConf;
	private int antWidth;
	private int consWidth;
	private int lagWidth;
	private String episodeType;
	private String allowRepeats;
	private int ruleType;
	private String lagType;	
	
	public FileData() {
		
		anticedentList = new ArrayList<InputDataType>();
		consequentList = new ArrayList<InputDataType>();
		
	}

	public List<InputDataType> getAnticedentList() {
		return anticedentList;
	}
	
	public List<InputDataType> getConsequentList() {
		return consequentList;
	}
	
	public int getMinFreq() {
		return minFreq;
	}
	
	public float getMinConf() {
		return minConf;
	}
	
	public int getAntWidth() {
		return antWidth;
	}
	
	public int getConsWidth() {
		return consWidth;
	}
	
	public int getLagWidth() {
		return lagWidth;
	}
	
	public String getEpisodeType() {
		return episodeType;
	}
	
	public String getAllowRepeats() {
		return allowRepeats;
	}
	
	public int getRuleType() {
		return ruleType;
	}
	
	public String getLagType() {
		return lagType;
	}
	
	public void setAnticedentList(List<InputDataType> anticedentList) {
		this.anticedentList = anticedentList;
	}
	
	public void setConsequentList(List<InputDataType> consequentList) {
		this.consequentList = consequentList;
	}
	
	public void setAntWidth(int antWidth) {
		this.antWidth = antWidth;
	}
	
	public void setConsWidth(int consWidth) {
		this.consWidth = consWidth;
	}
	
	public void setMinConf(float minConf) {
		this.minConf = minConf;
	}
	public void setAllowRepeats(String allowRepeats) {
		this.allowRepeats = allowRepeats;
	}
	public void setEpisodeType(String episodeType) {
		this.episodeType = episodeType;
	}
	public void setLagType(String lagType) {
		this.lagType = lagType;
	}
	public void setLagWidth(int lagWidth) {
		this.lagWidth = lagWidth;
	}
	public void setMinFreq(int minFreq) {
		this.minFreq = minFreq;
	}
	public void setRuleType(int ruleType) {
		this.ruleType = ruleType;
	}
	public String toString() {
		return this.anticedentList.toString() + "\n" +
				this.consequentList.toString() + "\n" +
				this.minFreq + "\n" + 
				this.minConf + "\n" +
				this.antWidth + "\n" +
				this.consWidth + "\n" +
				this.lagWidth + "\n" +
				this.episodeType + "\n" +
				this.ruleType + "\n" +
				this.lagType;
	}
}
