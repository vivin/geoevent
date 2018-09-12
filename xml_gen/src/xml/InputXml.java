package xml;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eventlist.Event;

public class InputXml {
	private Document doc;
	
	public InputXml() {
		try {
	        DocumentBuilder docBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
			
			doc = docBuilder.newDocument();
			doc.appendChild( doc.createElement("Params") );	
	    } catch (IllegalArgumentException iae) {	    	
	    	iae.printStackTrace();
	    } catch (Exception e) {	    	
	    	e.printStackTrace();
	    }
	}
	/**
	 * Method name:createAnticedent
	 * @param anticedent
	 * Creates the Anticedent node
	 */
	public void createAnticedent(String anticedent) {
		Element root = doc.getDocumentElement();
		Element anticedents = createElement(doc, "Anticedents",anticedent);
		root.appendChild(anticedents);
	}
	/**
	 * Method name: createConsequent
	 * @param consequents
	 * creates the consequent node
	 */
	public void createConsequent(String consequents) {
		Element root = doc.getDocumentElement();
		Element eventList = createElement(doc, "Consequents",consequents);
		root.appendChild(eventList);
	}
	/**
	 * Method name: createEventList
	 * @param events
	 * creates the EventList node
	 */
	public void createEventList(List<Event> events) {
		
		Element root = doc.getDocumentElement();
		
		Element eventList = createElement(doc, "EventList", "");
		root.appendChild(eventList);
		
		for (Event e: events) {
			Element event = createElement(doc, "Event", "");
			eventList.appendChild(event);
		
			Element timeStamp = createElement(doc, "TimeStamp", e.getTimestamp());
			event.appendChild(timeStamp);
		
			Element value = createElement(doc, "Value", e.getValue());
			event.appendChild(value);
		}

	}
	/**
	 * Method Name: createUserInput
	 * @param minfreq
	 * @param minconf
	 * @param antwidth
	 * @param conswidth
	 * @param lagwidth
	 * @param episodetype
	 * @param allowrepeats
	 * @param ruletype
	 * @param lagtype
	 * creates the MinFreq, MinConf, AntWidth, ConsWidth, LagWidth, EpisodeType, AllowRepeats, RuleTYpe and LagType nodes
	 */
	public void createUserInput(int minfreq, float minconf,int antwidth,int conswidth, int lagwidth,String episodetype,
									boolean allowrepeats) {
		Element root = doc.getDocumentElement();
		
		Element minFreq = createElement(doc, "MinFreq",Integer.toString(minfreq));
		root.appendChild(minFreq);				
		
		Element minConf = createElement(doc, "MinConf",Float.toString(minconf));
		root.appendChild(minConf);
		
		Element antWidth = createElement(doc, "AntWidth",Integer.toString(antwidth));
		root.appendChild(antWidth);
		
		Element consWidth = createElement(doc, "ConsWidth",Integer.toString(conswidth));
		root.appendChild(consWidth);
		
		Element lagWidth = createElement(doc, "LagWidth",Integer.toString(lagwidth));
		root.appendChild(lagWidth);
		
		Element episodeType = createElement(doc, "EpisodeType",episodetype);
		root.appendChild(episodeType);
		
		Element allowRepeats = createElement(doc, "AllowRepeats",Boolean.toString(allowrepeats));
		root.appendChild(allowRepeats);
		
	}
	/**
	 * Method:createRuleTypeAndLagType
	 * @param ruletype
	 * @param lagtype
	 * Creates the RuleType and LagType nodes
	 */
	public void createRuleTypeAndLagType(int ruletype, String lagtype) {
		
		Element root = doc.getDocumentElement();
		
		Element ruleType = createElement(doc, "RuleType",Integer.toString(ruletype));
		root.appendChild(ruleType);
		
		Element lagType = createElement(doc, "LagType",lagtype);
		root.appendChild(lagType);
	}
	/**
	 * Method name:createParams
	 * @param f
	 * creates the xml file
	 */
	public void createParams(File f) {
		try {
			Transformer transformer = TransformerFactory.newInstance().newTransformer();
			transformer.transform(new DOMSource(doc), new StreamResult(new FileOutputStream(f)));
		
			doc.getDocumentElement().normalize();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	/**
	 * Method name: createElement
	 * @param doc
	 * @param name
	 * @param value
	 * @return Element
	 * Creates and returns the element for the node
	 */
	public static Element createElement(Document doc, String name, String value) {
		Element e = doc.createElement(name);
		e.setTextContent(value == null ? "" : value);
		return e;
	}
}
