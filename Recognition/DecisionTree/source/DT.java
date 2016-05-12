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
	Scanner scan = new Scanner(System.in);
	FileWriter wFile;
	BufferedWriter wBuffer;
	static FileReader rFile;
	static BufferedReader rBuffer;
	public List<String> attributes;
	public List<String> mReadDocuments;
	public List<List<Integer>> ItemSet;
	public String file_Training;
	public String file_Testing;
	public String file_Output;
	public static int NUM_ITERATOR = 1000;
	public static int NUM_ATTRIBUTES;
	public static int NUM_CLASS;
	public static int MAX_VALUE;
	public double MINIMUM = -100.0;
	public static double BEGGING_RATIO = 0.7;
	public String NAME_OUTPUT_FILE = "dt_result.txt";
	public static ClassAttribute[] metaAttribute;
	public String classification[] = { "unacc", "acc", "good", "vgood" };


	public static List<String> List_Attributes;
	
	public static void main(String[] args) {
		// 한 번 db 스캔을 하면서, 어떠한 attribute와 class가 있는지 확인. 그 개수도 확인.
		// 그 다음, 확인한 것들을 MAIN class에 인자로 넘겨 실행.
		readParameter(args[0]);
		
		MAIN a = new MAIN(args, metaAttribute);
	}

	public static void readParameter(String file){
		ReadFile(file);
		scan();
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

	
	
	public static void scan() {
		List_Attributes = new ArrayList<String>();
		try {
			String readLine = null;
			readLine = rBuffer.readLine();
			StringTokenizer s1 = new StringTokenizer(readLine);
			while (s1.hasMoreTokens()) {
				String token = s1.nextToken("\t");
				List_Attributes.add(token);
			}
			
			NUM_ATTRIBUTES = List_Attributes.size() - 1;
			metaAttribute = new ClassAttribute[NUM_ATTRIBUTES+1];
			for(int i = 0; i < NUM_ATTRIBUTES+1; i++){
				metaAttribute[i] = new ClassAttribute();
				metaAttribute[i].name = List_Attributes.get(i);
				metaAttribute[i].index = i+1;
				metaAttribute[i].size_value = 0;
				metaAttribute[i].value = new ArrayList<String>();
			}
			
			while (true) {
				readLine = null;
				readLine = rBuffer.readLine();
				if (readLine == null) {
					break;
				}
				StringTokenizer s2 = new StringTokenizer(readLine);
				for (int i = 0; i < NUM_ATTRIBUTES; i++) {
					String token = s2.nextToken("\t");
					if(metaAttribute[i].value.contains(token) == false){
						metaAttribute[i].value.add(token);
						metaAttribute[i].size_value++;
					}
				}
				String token = s2.nextToken("\t");
				if(metaAttribute[NUM_ATTRIBUTES].value.contains(token) == false){
					metaAttribute[NUM_ATTRIBUTES].value.add(token);
					metaAttribute[NUM_ATTRIBUTES].size_value++;
				}
			}
			
			NUM_CLASS = metaAttribute[NUM_ATTRIBUTES].size_value;
			int max_value = 0;
			for(int i = 0; i < NUM_ATTRIBUTES; i++){
				if(metaAttribute[i].size_value >= max_value){
					max_value = metaAttribute[i].size_value;
				}
			}
			MAX_VALUE = max_value;
			closeFile();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void closeFile() {
		try {
			rBuffer.close();
			rFile.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
	}
	
	
	static class MAIN {
		FileWriter wFile;
		BufferedWriter wBuffer;
		FileReader rFile;
		BufferedReader rBuffer;
		public List<String> attributes;
		public List<String> mReadDocuments;
		public List<Data> DB;
		public List<List<Integer>> ItemSet;
		public String file_Training;
		public String file_Testing;
		public String file_Output;
		public int NUM_ITERATOR;
		public int NUM_ATTRIBUTES;
		public int NUM_CLASS;
		public int MAX_VALUE;
		public double MINIMUM = -100.0;
		public double BEGGING_RATIO;
		public String NAME_OUTPUT_FILE = "dt_result.txt";
		public ClassAttribute[] metaAttribute;
		
		public void func(Node root){
			if(root.attribute == NUM_ATTRIBUTES+1)
				System.out.println(metaAttribute[NUM_ATTRIBUTES].value.get(root.node.get(0).my_value-1));
			else{
				System.out.println(metaAttribute[root.attribute-1].name);
				for(int i = 0; i < root.size_node; i++){
					func(root.node.get(i));
				}
			}
		}
		
		public MAIN(String[] args, ClassAttribute[] att) {
			
			file_Training = args[0];
			file_Testing = args[1];
			metaAttribute = att;
			NUM_ITERATOR = DT.NUM_ITERATOR;
			NUM_ATTRIBUTES = DT.NUM_ATTRIBUTES;
			NUM_CLASS = DT.NUM_CLASS;
			MAX_VALUE = DT.MAX_VALUE;
			BEGGING_RATIO = DT.BEGGING_RATIO;
			file_Output = NAME_OUTPUT_FILE;
			Node root[] = new Node[NUM_ITERATOR];

			for (int i = 0; i < NUM_ITERATOR; i++) {
				ReadFile(file_Training);
				scan("training");
				attributes = new ArrayList<String>();
				for (int j = 0; j < NUM_ATTRIBUTES; j++) {
					attributes.add(metaAttribute[j].name);
				}
				root[i] = DTL(DB, attributes, Mode(DB));
			}

//			func(root[0]);
			
			ReadFile(file_Testing);
			scan("testing");

			
			// write association rules in output file
			try {
				WriteFile(file_Output);

				for (int i = 0; i < NUM_ATTRIBUTES; i++) {
					wBuffer.write(metaAttribute[i].name + "\t");
				}
				wBuffer.write(metaAttribute[NUM_ATTRIBUTES].name + "\n");

				for (Data data : DB) {
					int count_classification[] = new int[NUM_CLASS];
					for (int i = 0; i < NUM_CLASS; i++)
						count_classification[i] = 0;
					for (int j = 0; j < NUM_ITERATOR; j++) {
						count_classification[decision(data, root[j])]++;
					}
					int count_max = 0;
					int index_max = 0;
					for (int j = 0; j < NUM_CLASS; j++) {
						if (count_classification[j] > count_max) {
							count_max = count_classification[j];
							index_max = j;
						}
					}
					data.setS_attribute(NUM_ATTRIBUTES,metaAttribute[NUM_ATTRIBUTES].value.get(index_max));
					wBuffer.write(data.toString());
				}
				// close buffer and file
				wBuffer.close();
				wFile.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		// get the MODE value about class
		public Node Mode(List<Data> examples) {
			int size_examples = examples.size();
			int count[] = new int[NUM_CLASS];
			for (int i = 0; i < NUM_CLASS; i++)
				count[i] = 0;

			
			for (int i = 0; i < size_examples; i++) {
				int index = examples.get(i).getAttribute(NUM_ATTRIBUTES) - 1; // get the classification's value
				count[index]++;
			}
			// get the index about the MODE value
			int max = 0;
			int max_index = 0;
			for (int i = 0; i < NUM_CLASS; i++) {
				if (max < count[i]) {
					max = count[i];
					max_index = i;
				}
			}
			// return the class's value
			return new Node(metaAttribute[NUM_ATTRIBUTES].value.get(max_index));
		}

		public String Choose_Attribute(List<String> attributes,
				List<Data> examples) {
			int size_examples = examples.size();

			Double[][][] _attributes = new Double[NUM_ATTRIBUTES][MAX_VALUE][NUM_CLASS];
			double max = MINIMUM;
			String choose_attribute = null;
			double remainder = 0.0;
			double IG = 0.0;
			double splitInfo = 0.0;
			Double _class[] = new Double[NUM_CLASS];
			for (int i = 0; i < NUM_CLASS; i++)
				_class[i] = 0.0;

			for (int i = 0; i < NUM_ATTRIBUTES; i++) {
				for (int j = 0; j < MAX_VALUE; j++) {
					for (int k = 0; k < NUM_CLASS; k++) {
						_attributes[i][j][k] = 0.0;
					}
				}
			}

			for (int i = 0; i < size_examples; i++) {
				Data data = examples.get(i);
				int _classIndex = data.getAttribute(NUM_ATTRIBUTES) - 1;
				for (int j = 0; j < NUM_ATTRIBUTES; j++) {
					_attributes[j][data.getAttribute(j) - 1][_classIndex]++;
					_class[_classIndex]++;
				}
			}

			IG = I(_class);

			for (int i = 0; i < NUM_ATTRIBUTES; i++) {
				double _IG = 0;
				for (int j = 0; j < MAX_VALUE; j++) {
					double sum = 0;
					for (int k = 0; k < NUM_CLASS; k++)
						sum += _attributes[i][j][k];
					remainder += (sum / (double) size_examples)
							* I(_attributes[i][j]);

					if (sum != 0)
						splitInfo += (-1 * sum * (Math.log(sum
								/ (double) size_examples) / Math.log(2)));
				}
				_IG = (IG - remainder);
				if (_IG != 0)
					_IG /= splitInfo;

				if ((_IG >= max) && (attributes.contains(metaAttribute[i].name))) {
					max = _IG;
					choose_attribute = metaAttribute[i].name;
				}
				splitInfo = 0.0;
				remainder = 0.0;
			}

			return choose_attribute;
		}

		public double I(Double[] _attributes) {
			double result = 0;
			double total = 0.0;
			for (int i = 0; i < _attributes.length; i++)
				total += _attributes[i];

			for (int i = 0; i < _attributes.length; i++) {
				if (_attributes[i] != 0)
					result += (-1 * (_attributes[i] / total))
							* (Math.log(_attributes[i] / total)) / Math.log(2);
			}

			return result;
		}

		// return examples corresponding to attribute's value
		public List<Data> Extract_Examples(List<Data> examples, Node node) {
			int size_examples = examples.size();
			List<Data> examples_i = new ArrayList<Data>();
			int node_attribute = node.attribute;
			for (int i = 0; i < size_examples; i++) {
				if (examples.get(i).getAttribute(node_attribute - 1) - 1 == node.my_value)
					examples_i.add(examples.get(i));
			}
			return examples_i;
		}

		// core function
		public Node DTL(List<Data> examples, List<String> attributes, Node def) {
			Node result;
			// examples is empty then return default
			if (examples.isEmpty() == true) {
				return def;
			}

			// all attributes have the same classification then return the
			// classification
			{
				int count[] = new int[NUM_CLASS];
				for (int i = 0; i < NUM_CLASS; i++) {
					count[i] = 0;
				}
				int size_examples = examples.size();
				for (int i = 0; i < size_examples; i++) {
					int index = examples.get(i).getAttribute(NUM_ATTRIBUTES) - 1;
					count[index]++;
				}

				for (int i = 0; i < NUM_CLASS; i++) {
					if (count[i] == size_examples) {
						result = new Node(metaAttribute[NUM_ATTRIBUTES].value.get(i));
						return result;
					}
				}
			}

			// attributus is empty then return Mode(examples)
			if (attributes.isEmpty() == true) {
				result = Mode(examples);
				return result;
			}

			String best = Choose_Attribute(attributes, examples);
			result = new Node(best);

			for (int i = 0; i < result.size_node; i++) {
				List<String> _attributes = new ArrayList<String>(attributes);
				_attributes.remove(best);
				List<Data> examples_i = Extract_Examples(examples,result.node.get(i));
				result.node.set(i, DTL(examples_i, _attributes, Mode(examples)));
			}
			return result;
		}

		// use the decision trees and then return class about data
		public int decision(Data data, Node root) {
			int value = 1;

			if (root.attribute != NUM_ATTRIBUTES + 1) // if it's not class
														// attribute
				value = data.getAttribute(root.attribute - 1); // allocate the
																// attribute's
																// value to
																// 'value'
			else {
				value = root.node.get(0).my_value; // otherwise, allocate the
													// class's value to 'value'
				return value - 1;
			}
			return decision(data, root.node.get(value - 1)); // do that
																// recursively
		}

		// open the output file
		public void WriteFile(String Filename) {
			String filename = new String(Filename);
			try {
				wFile = new FileWriter(filename);
				wBuffer = new BufferedWriter(wFile);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		// open the input file
		public void ReadFile(String Filename) {
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

		public void scan(String s) {
			try {
				String readLine = null;
				readLine = rBuffer.readLine();
				DB = new ArrayList<Data>();

				while (true) {
					readLine = null;
					readLine = rBuffer.readLine();
					if (readLine == null) {
						break;
					}

					StringTokenizer s1 = new StringTokenizer(readLine);
					Data data = new Data();
					String token = "";
					for (int i = 0; i < NUM_ATTRIBUTES; i++) {
						token = s1.nextToken("\t");
						data.setS_attribute(i, token);
					}
					if (s.equals("training") == true) {
						token = s1.nextToken("\t");
						data.setS_attribute(NUM_ATTRIBUTES, token);
						if (Math.random() < BEGGING_RATIO) {
							DB.add(data);
						}
					} else
						DB.add(data);
				}
				closeFile();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		public void closeFile() {
			try {
				rBuffer.close();
				rFile.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}

		class Node {
			List<Data> DB;
			/*
			 * 만약, attribute가 7일 때에는 my_value로 잘 판단해야됨...
			 */
			public int my_value; // if attribute is 7, 1 unacc, 2 acc, 3 good, 4
									// vgood ...
			public int attribute = 10; // 1 buying, 2 maint, 3 doors, 4 persons,
										// 5 lug_boot, 6 safety, 7 labels
			public List<Node> node;
			public int size_node;

			public Node() {

			}

			public Node(List<Data> db) {
				super();
				DB = db;
			}

			public Node(String attribute) {
				super();
				int Attribute = 0;
				node = new ArrayList<Node>();
				DB = new ArrayList<Data>();

				for(int i = 0; i < NUM_ATTRIBUTES; i++){
					if(attribute.equals(MAIN.this.metaAttribute[i].name)){
						Attribute = i+1;
						size_node = MAIN.this.metaAttribute[i].size_value;
						for(int j = 0; j < size_node; j++){
							Node temp = new Node();
							temp.attribute = Attribute;
							temp.my_value = j;
							node.add(temp);
						}
					}
				}
				for(int i = 0; i < NUM_CLASS; i++){
					if(attribute.equals(MAIN.this.metaAttribute[NUM_ATTRIBUTES].value.get(i))){
						Attribute = NUM_ATTRIBUTES+1;
						size_node = 1;
						for(int j = 0; j < size_node; j++){
							Node temp = new Node();
							temp.attribute = Attribute;
							temp.my_value = i+1;
							node.add(temp);
						}
					}
				}
				
				this.attribute = Attribute;
			}

			public void setSize_node(int size) {
				size_node = size;
			}
		}

		class Data {
			public String s_attribute[] = new String[NUM_ATTRIBUTES+1];
			public int i_attribute[] = new int[NUM_ATTRIBUTES+1];

			public Data() {
				super();
			}

			public void setS_attribute(int i, String s) {
				s_attribute[i] = s;
				for(int k = 0; k < MAIN.this.metaAttribute[i].size_value; k++){
					if(s.equals(MAIN.this.metaAttribute[i].value.get(k))){
						i_attribute[i] = k+1;
					}
				}
			}

			public int getAttribute(int i) {
				return i_attribute[i];
			}

			public String toString() {
				String s = "";
				for (int i = 0; i < NUM_ATTRIBUTES; i++) {
					s += s_attribute[i] + "\t";
				}
				s += s_attribute[NUM_ATTRIBUTES] + "\n";
				return s;
			}
		}
	}
}

class ClassAttribute{
	public List<String> value;
	public String name;
	public int index;
	public int size_value;
}