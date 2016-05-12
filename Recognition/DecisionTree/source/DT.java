import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.StringTokenizer;
import java.math.*;

public class DT {
	static Scanner scan = new Scanner(System.in);
	static FileWriter wFile;
	static BufferedWriter wBuffer;
	static FileReader rFile;
	static BufferedReader rBuffer;
	public static List<String> attributes;
	public static List<String> mReadDocuments;
	public static List<Data> DB;
	public List< List<Integer> > ItemSet;
	public static String file_Training;
	public static String file_Testing;
	public static String file_Output;
	public static int NUM_ITERATOR = 3000;
	
	public static String attribute[] = {"buying","maint","doors","persons","lug_boot","safety","labels"};
	public static String classification[] = {"unacc","acc","good","vgood"};
	
	
	// data의 최빈값을 확인. yes가 많으면 NodeYes를, no가 많으면 NodeNo를 반환
	// unacc, acc, good, vgood
	public static Node Mode(List<Data> examples){
		int size_examples = examples.size();
		int count[] = {0,0,0,0};
		for(int i = 0; i < size_examples; i++){
			int index = examples.get(i).getAttribute(6)-1;
			count[index]++;
		}
		int max = 0;
		int max_index = 0;
		for(int i = 0; i < 4; i++){
			if(max < count[i]){
				max = count[i];
				max_index = i;
			}
		}
		return new Node(classification[max_index]);
	}
	
	public static String Choose_Attribute(List<String> attributes, List<Data> examples){
		int size_examples = examples.size();
		
		Double[][][] _attributes = new Double[6][4][4];
		double max = -100.0;
		String choose_attribute = null;
		double remainder = 0.0;
		double IG = 0.0;
		double splitInfo = 0.0;
		double _class[] = {0,0,0,0};
		
		for(int i = 0; i < 6; i++){
			for(int j = 0; j < 4; j++){
				for(int k = 0; k < 4; k++){
					_attributes[i][j][k] = 0.0;
				}
			}
		}
		
		for(int i = 0; i < size_examples; i++){
			Data data = examples.get(i);
			int _classIndex = data.getAttribute(6) - 1;
			for(int j = 0; j < 6; j++){
				_attributes[j][data.getAttribute(j)-1][_classIndex]++;
				_class[_classIndex]++;
			}
		}
		
		IG = I(_class[0],_class[1],_class[2],_class[3]);
		
		for(int i = 0; i < 6; i++){
			double _IG = 0;
			for(int j = 0; j < 4; j++){
				double sum = 0;
				for(int k = 0; k < 4; k++)
					sum += _attributes[i][j][k];
				remainder += (sum/(double)size_examples) * I(_attributes[i][j][0],_attributes[i][j][1],_attributes[i][j][2],_attributes[i][j][3]);
				
				if(sum != 0)
					splitInfo += (-1 * sum * (Math.log(sum/(double)size_examples) / Math.log(2)));
			}
			_IG = (IG - remainder);
			if(_IG != 0)
			_IG /= splitInfo;
			
			if((_IG >= max) && (attributes.contains(attribute[i]))){
				max = _IG;
				choose_attribute = attribute[i];
			}
			splitInfo = 0.0;
			remainder = 0.0;
		}
		
		return choose_attribute;
	}
	public static double I(double p1, double p2, double p3, double p4){
		double result = 0;
		double p[] = {p1,p2,p3,p4};
		double total = p1+p2+p3+p4;

		for(int i = 0; i < 4; i++){
			if(p[i] != 0)
				result += (-1 * (p[i] / total)) * (Math.log(p[i]/total)) / Math.log(2);
		}
		
		return result;
	}
	
	// attribute의 value에 해당하는 examples을 반환
	public static List<Data> Extract_Examples(List<Data> examples, Node node){
		int size_examples = examples.size();
		List<Data> examples_i = new ArrayList<Data>();
		// 1 age, 2 income, 3 student, 4 credit_rating
		int node_attribute = node.attribute;
		for(int i = 0; i < size_examples; i++){
			if(examples.get(i).getAttribute(node_attribute-1)-1 == node.my_value)
				examples_i.add(examples.get(i));
		}
		return examples_i;
	}
	
	public static Node DTL(List<Data> examples, List<String> attributes, Node def){
		Node result;
		// examples is empty then return default
		if(examples.isEmpty() == true){
			return def;
		}

		// all attributes have the same classification then return the classification
		{
			int count[] = {0,0,0,0};
			int size_examples = examples.size();
			for (int i = 0; i < size_examples; i++) {
				int index = examples.get(i).getAttribute(6)-1;
					count[index]++;
			}
			
			for(int i = 0; i < classification.length; i++){
				if(count[i] == size_examples){
					result = new Node(classification[i]);
					return result;
				}
			}
		}
				
		// attributus is empty then return Mode(examples)
		if(attributes.isEmpty() == true){
			result = Mode(examples);
			return result;
		}
		
		String best = Choose_Attribute(attributes,examples);
		result = new Node(best);
		
		for(int i = 0; i < result.size_node; i++){
			List<String> _attributes = new ArrayList<String>(attributes);
			_attributes.remove(best);
				
			List<Data> examples_i = Extract_Examples(examples, result.node.get(i));
			result.node.set(i, DTL(examples_i,_attributes,Mode(examples)));
		}
		return result;
	}
	
	public static int decision(Data data, Node root){
		int value = 1;
		
		if(root.attribute != 7)
			value = data.getAttribute(root.attribute-1);
		else{
			value = root.node.get(0).my_value;
			return value-1;
		}
		return decision(data,root.node.get(value-1));
	}
	
	public static void main(String[] args) {
		file_Training = args[0];
		file_Testing = args[1];
		file_Output = args[2];
		Node root[] = new Node[NUM_ITERATOR];
		
		for(int i = 0; i < NUM_ITERATOR; i++){
			ReadFile(file_Training);
			scan("training");
			attributes = new ArrayList<String>();
			for(int j = 0; j < 6; j++){
				attributes.add(attribute[j]);
			}
			root[i] = DTL(DB, attributes, Mode(DB));
		}
		
		
		ReadFile(file_Testing);
		scan("testing");
				
		
		// write association rules in output file
		try {
			WriteFile(file_Output);
			
			for(int i = 0; i < 6; i++){
				wBuffer.write(attribute[i] + "\t");
			}
			wBuffer.write("car_evaluation\n");
			
			for(Data data : DB){
				int count_classification[] = {0,0,0,0};
				for(int j = 0; j < NUM_ITERATOR; j++){
					count_classification[decision(data,root[j])]++;
				}
				int count_max = 0;
				int index_max = 0;
				for(int j = 0; j < 4; j++){
					if(count_classification[j] > count_max){
						count_max = count_classification[j];
						index_max = j;
					}
				}
				data.setS_attribute(6, classification[index_max]);
				wBuffer.write(data.toString());
//				wBuffer.write(decision(data,root));
			
				
			}
			// close buffer and file
			wBuffer.close();
			wFile.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	// open the output file
	public static void 
	WriteFile(String Filename) {
		String filename = new String(Filename);
		try {
			wFile = new FileWriter(filename);
			wBuffer = new BufferedWriter(wFile);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	// open the input file
	public static void ReadFile(String Filename) {
		String filename = new String(Filename);
		try {
			rFile = new FileReader(filename);
			rBuffer = new BufferedReader(rFile);
		} catch (FileNotFoundException e) {
			try {
				rBuffer.close();
				rFile.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
			e.printStackTrace();
		} 
	}
	
	public static void scan(String s){
		try {
			String readLine = null;
			readLine = rBuffer.readLine();
			DB = new ArrayList<Data>();
			
			while (true) {
				readLine = null;
				readLine = rBuffer.readLine();
				if (readLine == null){
					break;
				}

				StringTokenizer s1 = new StringTokenizer(readLine);
				Data data = new Data();
				String token = "";
				for(int i = 0; i < 6; i++){
					token = s1.nextToken("\t");
					data.setS_attribute(i, token);
				}
				if(s.equals("training") == true){
					token = s1.nextToken("\t");
					data.setS_attribute(6, token);
					if(Math.random() < 0.7){
						DB.add(data);
					}
				}
				else
					DB.add(data);	
			}
			closeFile();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static void closeFile(){
		try {
			rBuffer.close();
			rFile.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
	}
}


class Node{
	List<Data> DB;
	/*
	 * 만약, attribute가 7일 때에는 my_value로 잘 판단해야됨...
	 * 
	 */
	public int my_value; // if attribute is 7, 1 unacc, 2 acc, 3 good, 4 vgood ...
	public int attribute = 10; // 1 buying, 2 maint, 3 doors, 4 persons, 5 lug_boot, 6 safety, 7 labels
	public List<Node> node;
	public int size_node;
	public Node(){
		
	}
	public Node(List<Data> db){
		super();
		DB = db;
	}
	public Node(String attribute){
		super();
		int Attribute = 0;
		node = new ArrayList<Node>();
		DB = new ArrayList<Data>();
		
		if(attribute.equals("buying") == true){
			Attribute = 1;
			size_node = 4;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = i;
				node.add(temp);
			}
		}
		else if(attribute.equals("maint") == true){
			Attribute = 2;
			size_node = 4;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = i;
				node.add(temp);
			}
		}
		else if(attribute.equals("doors") == true){
			Attribute = 3;
			size_node = 4;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = i;
				node.add(temp);
			}
		}
		else if(attribute.equals("persons") == true){
			Attribute = 4;
			size_node = 3;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = i;
				node.add(temp);
			}
		}
		else if(attribute.equals("lug_boot") == true){
			Attribute = 5;
			size_node = 3;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = i;
				node.add(temp);
			}
		}
		else if(attribute.equals("safety") == true){
			Attribute = 6;
			size_node = 3;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = i;
				node.add(temp);
			}
		}
		else if(attribute.equals("unacc") == true){
			Attribute = 7;
			size_node = 1;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = 1;
				node.add(temp);
			}
		}
		else if(attribute.equals("acc") == true){
			Attribute = 7;
			size_node = 1;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = 2;
				node.add(temp);
			}
		}
		else if(attribute.equals("good") == true){
			Attribute = 7;
			size_node = 1;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = 3;
				node.add(temp);
			}
		}
		else if(attribute.equals("vgood") == true){
			Attribute = 7;
			size_node = 1;
			for(int i = 0; i < size_node; i++){
				Node temp = new Node();
				temp.attribute = Attribute;
				temp.my_value = 4;
				node.add(temp);
			}
		}
		this.attribute = Attribute;
	}

	public void setSize_node(int size){
		size_node = size;
	}
}

class Data{
	public String s_attribute[] = new String[7];
	public int i_attribute[] = new int[7];
	
	public Data(){
		super();
	}
	
	public void setS_attribute(int i, String s){
		s_attribute[i] = s;
		if(s.equals("low") || s.equals("unacc") || s.equals("small") || s.equals("2"))
			i_attribute[i] = 1;
		else if(s.equals("med") || s.equals("3") || s.equals("acc"))
			i_attribute[i] = 2;
		else if(i == 3 && s.equals("4"))
			i_attribute[i] = 2;
		else if(s.equals("vhigh") || s.equals("vgood") || s.equals("5more"))
			i_attribute[i] = 4;
		else
			i_attribute[i] = 3;
	}	
	public int getAttribute(int i){
		return i_attribute[i];
	}
	
	public String toString(){
		String s = "";
		for(int i = 0; i < 6; i++){
			s += s_attribute[i] + "\t";
		}
		s += s_attribute[6] + "\n";
		return s;
	}
}
